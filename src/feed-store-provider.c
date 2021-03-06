/* Copyright 2018 Endless Mobile, Inc.
 *
 * libcontentfeed is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * libcontentfeed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libcontentfeed.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <math.h>
#include <string.h>

#include <gio/gio.h>

#include <eos-shard/eos-shard-blob.h>
#include <eos-shard/eos-shard-shard-file.h>
#include <eos-shard/eos-shard-record.h>

#include <libsoup/soup.h>

#include "feed-all-async-tasks-private.h"
#include "feed-base-card-store.h"
#include "feed-card-layout-direction.h"
#include "feed-knowledge-app-artwork-card-store.h"
#include "feed-knowledge-app-card-store.h"
#include "feed-knowledge-app-news-card-store.h"
#include "feed-knowledge-app-proxy.h"
#include "feed-knowledge-app-video-card-store.h"
#include "feed-orderable-model.h"
#include "feed-quote-card-store.h"
#include "feed-sizes.h"
#include "feed-store-provider.h"
#include "feed-text-sanitization.h"
#include "feed-word-card-store.h"
#include "feed-word-quote-card-store.h"

typedef GSList * (*ModelsFromResultsAndShardsFunc) (const char * const *shards,
                                                    GPtrArray          *model_props_variants,
                                                    gpointer            user_data);
typedef GObject * (*ModelFromResultFunc) (GVariant *model_variant,
                                          gpointer  user_data);

static gboolean
call_dbus_proxy_and_construct_from_models_and_shards (GDBusProxy                      *proxy,
                                                      const gchar                     *method_name,
                                                      ModelsFromResultsAndShardsFunc   marshal_func,
                                                      gpointer                         marshal_data,
                                                      GCancellable                    *cancellable,
                                                      gpointer                        *out_result,
                                                      GError                         **error)
{
  g_autoptr(GVariant) result = NULL;
  g_autoptr(GVariant) shards_variant = NULL;
  g_autoptr(GVariant) models_variant = NULL;
  g_auto(GStrv) shards_strv = NULL;
  g_autoptr(GPtrArray) model_props_variants = NULL;
  GVariantIter iter;
  GVariant *model_variant = NULL;

  g_return_val_if_fail (out_result != NULL, FALSE);

  result = g_dbus_proxy_call_sync (proxy,
                                   method_name,
                                   NULL,
                                   G_DBUS_CALL_FLAGS_NONE,
                                   -1,
                                   cancellable,
                                   error);

  if (result == NULL)
    return FALSE;

  g_variant_get (result, "(^as@aa{ss})", &shards_strv, &models_variant);

  model_props_variants = g_ptr_array_new_full (g_variant_n_children (models_variant),
                                               NULL);

  g_variant_iter_init (&iter, models_variant);
  while (g_variant_iter_loop (&iter, "@a{ss}", &model_variant))
    g_ptr_array_add (model_props_variants,
                     model_variant);

  /* Now that we have the models and shards, we can marshal them into
   * a GSList containing the discovery-feed models */
  *out_result = marshal_func ((const gchar * const *) shards_strv,
                              model_props_variants,
                              marshal_data);
  return TRUE;
}

static gboolean
call_dbus_proxy_and_construct_from_model (GDBusProxy           *proxy,
                                          const gchar          *method_name,
                                          ModelFromResultFunc   marshal_func,
                                          gpointer              marshal_data,
                                          GCancellable         *cancellable,
                                          gpointer             *out_result,
                                          GError              **error)
{
  g_autoptr(GVariant) result = NULL;
  g_autoptr(GVariant) model_variant = NULL;

  g_return_val_if_fail (out_result != NULL, FALSE);

  result = g_dbus_proxy_call_sync (proxy,
                                   method_name,
                                   NULL,
                                   G_DBUS_CALL_FLAGS_NONE,
                                   -1,
                                   cancellable,
                                   error);

  if (result == NULL)
    return FALSE;

  g_variant_get (result, "(@a{ss})", &model_variant);

  /* Now that we have the model, we can marshal it into a
   * GObject */
  *out_result = marshal_func (model_variant, marshal_data);
  return TRUE;
}

static gchar *
strip_leading_slashes (const gchar *str)
{
  const gchar *strp = str;

  while (*strp == '/')
    ++strp;

  return g_strdup (strp);
}

static gchar *
remove_uri_prefix (const gchar *uri)
{
  g_autoptr(SoupURI) soup_uri = soup_uri_new (uri);
  const gchar *path = NULL;

  if (soup_uri == NULL)
    {
      g_message ("Expected %s to be a URI, but it was not, continuing anyway", uri);
      return strip_leading_slashes (uri);
    }

  path = soup_uri_get_path (soup_uri);
  return strip_leading_slashes (path);
}

static GInputStream *
find_thumbnail_stream_in_shards (const gchar * const  *shards_strv,
                                 const gchar          *thumbnail_uri)
{
  const gchar * const *iter = shards_strv;
  g_autofree gchar *normalized = remove_uri_prefix (thumbnail_uri);

  for (; *iter != NULL; ++iter) {
    g_autoptr(EosShardShardFile) shard_file = NULL;
    g_autoptr(EosShardRecord) record = NULL;
    g_autoptr(GError) local_error = NULL;

    /* XXX: This should probably be done asynchronously if possible */
    shard_file = g_initable_new (EOS_SHARD_TYPE_SHARD_FILE,
                                 NULL,
                                 &local_error,
                                 "path",
                                 *iter,
                                 NULL);

    if (shard_file == NULL)
      {
        g_message ("Failed to load shard file %s: %s. Skipping.",
                   *iter,
                   local_error->message);
        continue;
      }

    record = eos_shard_shard_file_find_record_by_hex_name (shard_file,
                                                           normalized);

    if (record == NULL || record->data == NULL)
      continue;

    return eos_shard_blob_get_stream (record->data);
  }

  return NULL;
}

typedef ContentFeedKnowledgeAppCardStore * (*ContentFeedKnowledgeAppCardStoreFactoryFunc) (const gchar                    *title,
                                                                                           const gchar                    *uri,
                                                                                           const gchar                    *synopsis,
                                                                                           GInputStream                   *thumbnail,
                                                                                           const gchar                    *desktop_id,
                                                                                           const gchar                    *bus_name,
                                                                                           const gchar                    *knowledge_search_object_path,
                                                                                           const gchar                    *knowledge_app_id,
                                                                                           ContentFeedCardLayoutDirection  layout_direction,
                                                                                           guint                           thumbnail_size,
                                                                                           const gchar                    *thumbnail_uri,
                                                                                           const gchar                    *content_type);


typedef struct _ArticleCardsFromShardsAndItemsData
{
  ContentFeedKnowledgeAppProxy                *ka_proxy;
  ContentFeedCardLayoutDirection               direction;
  ContentFeedCardStoreType                     type;
  guint                                        thumbnail_size;
  ContentFeedKnowledgeAppCardStoreFactoryFunc  factory;
} ArticleCardsFromShardsAndItemsData;

static ArticleCardsFromShardsAndItemsData *
article_cards_from_shards_and_items_data_new (ContentFeedKnowledgeAppProxy                *ka_proxy,
                                              ContentFeedCardLayoutDirection               direction,
                                              ContentFeedCardStoreType                     type,
                                              guint                                        thumbnail_size,
                                              ContentFeedKnowledgeAppCardStoreFactoryFunc  factory)
{
  ArticleCardsFromShardsAndItemsData *data = g_new0 (ArticleCardsFromShardsAndItemsData, 1);

  data->ka_proxy = g_object_ref (ka_proxy);
  data->direction = direction;
  data->type = type;
  data->thumbnail_size = thumbnail_size;
  data->factory = factory;

  return data;
}

static void
article_cards_from_shards_and_items_data_free (ArticleCardsFromShardsAndItemsData *data)
{
  g_clear_object (&data->ka_proxy);

  g_free (data);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC (ArticleCardsFromShardsAndItemsData,
                               article_cards_from_shards_and_items_data_free)

/* Given a variant of type a{ss}, look up a string for a corresponding key,
 * note that this is currently done with a linear scan and is transfer-none */
static const gchar *
lookup_string_in_dict_variant (GVariant *variant, const gchar *key)
{
  const gchar *str;

  if (!g_variant_lookup (variant, key, "&s", &str, NULL))
    str = NULL;

  return str;
}

static GSList *
article_cards_from_shards_and_items (const char * const *shards_strv,
                                     GPtrArray          *model_props_variants,
                                     gpointer            user_data)
{
  ArticleCardsFromShardsAndItemsData *data = user_data;
  GSList *orderable_stores = NULL;
  guint i = 0;

  for (; i < model_props_variants->len; ++i)
    {
      GVariant *model_props = g_ptr_array_index (model_props_variants, i);
      const gchar *unsanitized_snopsis = lookup_string_in_dict_variant (model_props,
                                                                        "synopsis");
      const gchar *synopsis = content_feed_sanitize_synopsis (unsanitized_snopsis);
      const gchar *title = lookup_string_in_dict_variant (model_props, "title");
      const gchar *ekn_id = lookup_string_in_dict_variant (model_props, "ekn_id");
      const gchar *thumbnail_uri = lookup_string_in_dict_variant (model_props,
                                                                  "thumbnail_uri");
      const gchar *content_type = lookup_string_in_dict_variant (model_props,
                                                                 "content_type");
      g_autoptr(GInputStream) thumbnail_stream =
        find_thumbnail_stream_in_shards (shards_strv, thumbnail_uri);
      GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (data->ka_proxy);

      ContentFeedKnowledgeAppCardStore *store =
        data->factory (title,
                       ekn_id,
                       synopsis,
                       thumbnail_stream,
                       content_feed_knowledge_app_proxy_get_desktop_id (data->ka_proxy),
                       g_dbus_proxy_get_name (dbus_proxy),
                       content_feed_knowledge_app_proxy_get_knowledge_search_object_path (data->ka_proxy),
                       content_feed_knowledge_app_proxy_get_knowledge_app_id (data->ka_proxy),
                       data->direction ? data->direction : CONTENT_FEED_CARD_LAYOUT_DIRECTION_IMAGE_FIRST,
                       data->thumbnail_size,
                       thumbnail_uri,
                       content_type);
      orderable_stores = g_slist_prepend (orderable_stores,
                                          content_feed_orderable_model_new (CONTENT_FEED_BASE_CARD_STORE (store),
                                                                            data->type,
                                                                            content_feed_knowledge_app_proxy_get_desktop_id (data->ka_proxy)));
    }

    return g_steal_pointer (&orderable_stores);
}

typedef struct _AppendDiscoveryFeedContentFromProxyData
{
  gchar                                       *method;
  ContentFeedCardStoreType                     type;
  ContentFeedCardLayoutDirection               direction;
  ContentFeedKnowledgeAppCardStoreFactoryFunc  factory;
  guint                                        thumbnail_size;
} AppendDiscoveryFeedContentFromProxyData;

static AppendDiscoveryFeedContentFromProxyData *
append_discovery_feed_content_from_proxy_data_new (const gchar                                *method,
                                                   ContentFeedCardStoreType                    type,
                                                   ContentFeedCardLayoutDirection              direction,
                                                   ContentFeedKnowledgeAppCardStoreFactoryFunc factory,
                                                   guint                                       thumbnail_size)

{
  AppendDiscoveryFeedContentFromProxyData *data = g_new0 (AppendDiscoveryFeedContentFromProxyData, 1);

  data->method = g_strdup (method);
  data->type = type;
  data->direction = direction;
  data->factory = factory;
  data->thumbnail_size = thumbnail_size;

  return data;
}

static void
append_discovery_feed_content_from_proxy_data_free (AppendDiscoveryFeedContentFromProxyData *data)
{
  g_clear_pointer (&data->method, g_free);

  g_free (data);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC (AppendDiscoveryFeedContentFromProxyData,
                               append_discovery_feed_content_from_proxy_data_free)

static gboolean
append_discovery_feed_content_from_proxy (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                          gpointer                       proxy_data,
                                          GCancellable                  *cancellable,
                                          gpointer                      *out_result,
                                          GError                       **error)
{
  g_autoptr(AppendDiscoveryFeedContentFromProxyData) data = proxy_data;
  g_autoptr(ArticleCardsFromShardsAndItemsData) marshal_data = article_cards_from_shards_and_items_data_new (ka_proxy,
                                                                                                             data->direction,
                                                                                                             data->type,
                                                                                                             data->thumbnail_size,
                                                                                                             data->factory);
  GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

  return call_dbus_proxy_and_construct_from_models_and_shards (dbus_proxy,
                                                               data->method,
                                                               article_cards_from_shards_and_items,
                                                               marshal_data,
                                                               cancellable,
                                                               out_result,
                                                               error);
}

static gboolean
parse_int64_with_limits (const gchar  *str,
                         guint         base,
                         gint64        min,
                         gint64        max,
                         gint64       *out,
                         GError      **error)
{
  gint64 ret = 0;

  g_return_val_if_fail (out != NULL, FALSE);

  /* Clear errno first before calling g_ascii_stroll */
  errno = 0;

  ret = g_ascii_strtoll (str, NULL, base);

  if (errno != 0)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   g_io_error_from_errno (errno),
                   "Parsing of integer failed with %s", strerror (errno));
      return FALSE;
    }

  if (ret < min || ret > max)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_FAILED,
                   "Parsing of integer failed, %li is not "
                   "in the range of %li - %li",
                   ret,
                   min,
                   max);
      return FALSE;
    }

  *out = ret;
  return TRUE;
}

static gchar *
parse_duration (const gchar  *duration,
                GError      **error)
{
  gint64 total_seconds = 0;
  gint64 hours = 0;
  gint64 minutes = 0;
  gint64 seconds = 0;

  if (!parse_int64_with_limits (duration,
                                10,
                                G_MININT64,
                                G_MAXINT64,
                                &total_seconds,
                                error))
    return NULL;

  hours = floor (total_seconds / 3600);
  minutes = ((gint64) floor (total_seconds / 60)) % 60;
  seconds = floor (((gint64) total_seconds) % 60);

  if (hours > 0)
    return g_strdup_printf ("%li:%02li:%02li", hours, minutes, seconds);

  return g_strdup_printf ("%li:%02li", minutes, seconds);
}

static GSList *
video_cards_from_shards_and_items (const char * const *shards_strv,
                                   GPtrArray          *model_props_variants,
                                   gpointer            user_data)
{
  ContentFeedKnowledgeAppProxy *ka_proxy = user_data;
  GSList *orderable_stores = NULL;
  guint i = 0;

  for (; i < model_props_variants->len; ++i)
    {
      GVariant *model_props = g_ptr_array_index (model_props_variants, i);
      g_autoptr(GError) local_error = NULL;
      const gchar *in_duration = lookup_string_in_dict_variant (model_props,
                                                                "duration");
      const gchar *thumbnail_uri = lookup_string_in_dict_variant (model_props,
                                                                  "thumbnail_uri");
      const gchar *title = lookup_string_in_dict_variant (model_props, "title");
      const gchar *ekn_id = lookup_string_in_dict_variant (model_props, "ekn_id");
      const gchar *content_type = lookup_string_in_dict_variant (model_props,
                                                                 "content_type");
      g_autofree gchar *duration = parse_duration (in_duration, &local_error);
      g_autoptr(GInputStream) thumbnail_stream = NULL;
      ContentFeedKnowledgeAppVideoCardStore *store = NULL;
      GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

      if (duration == NULL)
        {
          g_message ("Failed to parse duration %s: %s",
                     in_duration,
                     local_error->message);
          continue;
        }

      thumbnail_stream = find_thumbnail_stream_in_shards (shards_strv, thumbnail_uri);

      store = content_feed_knowledge_app_video_card_store_new (title,
                                                               ekn_id,
                                                               duration,
                                                               thumbnail_stream,
                                                               content_feed_knowledge_app_proxy_get_desktop_id (ka_proxy),
                                                               g_dbus_proxy_get_name (dbus_proxy),
                                                               content_feed_knowledge_app_proxy_get_knowledge_search_object_path (ka_proxy),
                                                               content_feed_knowledge_app_proxy_get_knowledge_app_id (ka_proxy),
                                                               thumbnail_uri,
                                                               content_type);
      orderable_stores = g_slist_prepend (orderable_stores,
                                          content_feed_orderable_model_new (CONTENT_FEED_BASE_CARD_STORE (store),
                                                                            CONTENT_FEED_CARD_STORE_TYPE_VIDEO_CARD,
                                                                            content_feed_knowledge_app_proxy_get_desktop_id (ka_proxy)));
    }

    return g_steal_pointer (&orderable_stores);
}

static gboolean
append_discovery_feed_video_from_proxy (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                        gpointer                       proxy_data G_GNUC_UNUSED,
                                        GCancellable                  *cancellable,
                                        gpointer                      *out_result,
                                        GError                       **error)
{
  GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

  return call_dbus_proxy_and_construct_from_models_and_shards (dbus_proxy,
                                                               "GetVideos",
                                                               video_cards_from_shards_and_items,
                                                               ka_proxy,
                                                               cancellable,
                                                               out_result,
                                                               error);
}

static GSList *
artwork_cards_from_shards_and_items (const char * const *shards_strv,
                                     GPtrArray          *model_props_variants,
                                     gpointer            user_data)
{
  ContentFeedKnowledgeAppProxy *ka_proxy = user_data;
  GSList *orderable_stores = NULL;
  guint i = 0;

  for (; i < model_props_variants->len; ++i)
    {
      GVariant *model_props = g_ptr_array_index (model_props_variants, i);
      const gchar *first_date = lookup_string_in_dict_variant (model_props, "first_date");
      const gchar *thumbnail_uri = lookup_string_in_dict_variant (model_props, "thumbnail_uri");
      const gchar *title = lookup_string_in_dict_variant (model_props, "title");
      const gchar *ekn_id = lookup_string_in_dict_variant (model_props, "ekn_id");
      const gchar *author = lookup_string_in_dict_variant (model_props, "author");
      const gchar *content_type = lookup_string_in_dict_variant (model_props, "content_type");
      g_autoptr(GInputStream) thumbnail_stream =
        find_thumbnail_stream_in_shards (shards_strv, thumbnail_uri);
      GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

      ContentFeedKnowledgeAppArtworkCardStore *store =
        content_feed_knowledge_app_artwork_card_store_new (title,
                                                           ekn_id,
                                                           author,
                                                           first_date != NULL ? first_date : "",
                                                           thumbnail_stream,
                                                           content_feed_knowledge_app_proxy_get_desktop_id (ka_proxy),
                                                           g_dbus_proxy_get_name (dbus_proxy),
                                                           content_feed_knowledge_app_proxy_get_knowledge_search_object_path (ka_proxy),
                                                           content_feed_knowledge_app_proxy_get_knowledge_app_id (ka_proxy),
                                                           CONTENT_FEED_CARD_LAYOUT_DIRECTION_IMAGE_FIRST,
                                                           CONTENT_FEED_THUMBNAIL_SIZE_ARTWORK,
                                                           thumbnail_uri,
                                                           content_type);
      orderable_stores = g_slist_prepend (orderable_stores,
                                          content_feed_orderable_model_new (CONTENT_FEED_BASE_CARD_STORE (store),
                                                                            CONTENT_FEED_CARD_STORE_TYPE_ARTWORK_CARD,
                                                                            content_feed_knowledge_app_proxy_get_desktop_id (ka_proxy)));
    }

    return g_steal_pointer (&orderable_stores);
}

static gboolean
append_discovery_feed_artwork_from_proxy (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                          gpointer                       proxy_data G_GNUC_UNUSED,
                                          GCancellable                  *cancellable,
                                          gpointer                      *out_result,
                                          GError                       **error)
{
  GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

  return call_dbus_proxy_and_construct_from_models_and_shards (dbus_proxy,
                                                               "ArtworkCardDescriptions",
                                                               artwork_cards_from_shards_and_items,
                                                               ka_proxy,
                                                               cancellable,
                                                               out_result,
                                                               error);
}

typedef gboolean (*AppendStoresFromProxyFunc) (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                               gpointer                       proxy_data,
                                               GCancellable                  *cancellable,
                                               gpointer                      *out_result,
                                               GError                       **error);

typedef struct _AppendStoresTaskData
{
  ContentFeedKnowledgeAppProxy *ka_proxy;
  AppendStoresFromProxyFunc     proxy_func;
  GDestroyNotify                proxy_return_destroy;
  gpointer                      proxy_data;
} AppendStoresTaskData;

static AppendStoresTaskData *
append_stores_task_data_new (ContentFeedKnowledgeAppProxy *ka_proxy,
                             AppendStoresFromProxyFunc     proxy_func,
                             GDestroyNotify                proxy_return_destroy,
                             gpointer                      proxy_data)
{
  AppendStoresTaskData *data = g_new0 (AppendStoresTaskData, 1);

  data->ka_proxy = g_object_ref (ka_proxy);
  data->proxy_func = proxy_func;
  data->proxy_return_destroy = proxy_return_destroy;
  data->proxy_data = proxy_data;

  return data;
}

static void
append_stores_task_data_free (AppendStoresTaskData *data)
{
  g_clear_object (&data->ka_proxy);

  g_free (data);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC (AppendStoresTaskData,
                               append_stores_task_data_free)

static void
append_stores_task_from_proxy_thread (GTask        *task,
                                      gpointer      source G_GNUC_UNUSED,
                                      gpointer      task_data,
                                      GCancellable *cancellable)
{
  AppendStoresTaskData *data = task_data;
  g_autoptr(GError) local_error = NULL;
  gpointer results = NULL;

  if (!data->proxy_func (data->ka_proxy,
                         data->proxy_data,
                         cancellable,
                         &results,
                         &local_error))
    {
      g_task_return_error (task, g_steal_pointer (&local_error));
      return;
    }

  g_task_return_pointer (task, g_steal_pointer (&results), data->proxy_return_destroy);
}

static void
append_stores_task_from_proxy (ContentFeedKnowledgeAppProxy *ka_proxy,
                               AppendStoresFromProxyFunc     proxy_func,
                               GDestroyNotify                proxy_return_destroy,
                               gpointer                      proxy_func_data,
                               GCancellable                 *cancellable,
                               GAsyncReadyCallback           callback,
                               gpointer                      user_data)
{
  g_autoptr(GTask) task = g_task_new (NULL, cancellable, callback, user_data);
  g_autoptr(AppendStoresTaskData) data = append_stores_task_data_new (ka_proxy,
                                                                      proxy_func,
                                                                      proxy_return_destroy,
                                                                      proxy_func_data);

  g_task_set_return_on_cancel (task, TRUE);
  g_task_set_task_data (task,
                        g_steal_pointer (&data),
                        (GDestroyNotify) append_stores_task_data_free);
  g_task_run_in_thread (task, append_stores_task_from_proxy_thread);
}

static void
object_slist_free (GSList *slist)
{
  g_slist_free_full (slist, g_object_unref);
}

static void
marshal_word_quote_into_store (GObject      *source G_GNUC_UNUSED,
                               GAsyncResult *result,
                               gpointer      user_data)
{
  g_autoptr(GTask) task = user_data;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GPtrArray) word_quote_results = g_task_propagate_pointer (G_TASK (result),
                                                                      &local_error);
  GSList *word_quote_card_results = NULL;
  ContentFeedWordCardStore *word_store = NULL;
  ContentFeedQuoteCardStore *quote_store = NULL;
  ContentFeedWordQuoteCardStore *store = NULL;

  if (word_quote_results == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&local_error));
      return;
    }

  /* Unpack the word/quote results and concatenate the pointer arrays */
  if (word_quote_results->len != 2)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_FAILED,
                               "Expected exactly two results for word/quote query.");
      return;
    }

  word_store = g_task_propagate_pointer (G_TASK (g_ptr_array_index (word_quote_results, 0)),
                                         &local_error);

  if (word_store == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&local_error));
      return;
    }

  quote_store = g_task_propagate_pointer (G_TASK (g_ptr_array_index (word_quote_results, 1)),
                                          &local_error);

  if (quote_store == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&local_error));
      return;
    }

  store = content_feed_word_quote_card_store_new (word_store, quote_store);

  /* Return a list with one element for consistency with everything else,
   * so that we can flat-map everything together in the end */
  word_quote_card_results = g_slist_prepend (word_quote_card_results,
                                             content_feed_orderable_model_new (CONTENT_FEED_BASE_CARD_STORE (store),
                                                                               CONTENT_FEED_CARD_STORE_TYPE_WORD_QUOTE_CARD,
                                                                               "word-quote"));

  g_task_return_pointer (task,
                         g_steal_pointer (&word_quote_card_results),
                         (GDestroyNotify) object_slist_free);
}

static GObject *
word_card_from_item (GVariant *model_props,
                     gpointer  user_data G_GNUC_UNUSED)
{
  const gchar *word = lookup_string_in_dict_variant (model_props, "word");
  const gchar *part_of_speech = lookup_string_in_dict_variant (model_props, "part_of_speech");
  const gchar *definition = lookup_string_in_dict_variant (model_props, "definition");

  return G_OBJECT (content_feed_word_card_store_new (word,
                                                     part_of_speech,
                                                     definition));
}

static gboolean
append_discovery_feed_word_from_proxy (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                       gpointer                       proxy_data G_GNUC_UNUSED,
                                       GCancellable                  *cancellable,
                                       gpointer                      *out_result,
                                       GError                       **error)
{
  GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);
  return call_dbus_proxy_and_construct_from_model (dbus_proxy,
                                                   "GetWordOfTheDay",
                                                   word_card_from_item,
                                                   NULL,
                                                   cancellable,
                                                   out_result,
                                                   error);
}

static GObject *
quote_card_from_item (GVariant *model_props,
                      gpointer  user_data G_GNUC_UNUSED)
{
  const gchar *title = lookup_string_in_dict_variant (model_props, "title");
  const gchar *author = lookup_string_in_dict_variant (model_props, "author");

  return G_OBJECT (content_feed_quote_card_store_new (title, author));
}

static gboolean
append_discovery_feed_quote_from_proxy (ContentFeedKnowledgeAppProxy  *ka_proxy,
                                        gpointer                       proxy_data G_GNUC_UNUSED,
                                        GCancellable                  *cancellable,
                                        gpointer                      *out_result,
                                        GError                       **error)
{
  GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);

  return call_dbus_proxy_and_construct_from_model (dbus_proxy,
                                                   "GetQuoteOfTheDay",
                                                   quote_card_from_item,
                                                   NULL,
                                                   cancellable,
                                                   out_result,
                                                   error);
}

static void
append_discovery_feed_word_quote_from_proxies (ContentFeedKnowledgeAppProxy *word_ka_proxy,
                                               ContentFeedKnowledgeAppProxy *quote_ka_proxy,
                                               GCancellable                 *cancellable,
                                               GAsyncReadyCallback           callback,
                                               gpointer                      user_data)
{
  g_autoptr(GTask) task = g_task_new (NULL, cancellable, callback, user_data);
  AllTasksResultsClosure *all_tasks_closure = all_tasks_results_closure_new (g_object_unref,
                                                                             marshal_word_quote_into_store,
                                                                             g_steal_pointer (&task));

  /* Ignoring the return values here, recall that the task's lifecycle owns
   * the task */
  append_stores_task_from_proxy (word_ka_proxy,
                                 append_discovery_feed_word_from_proxy,
                                 g_object_unref,
                                 NULL,
                                 cancellable,
                                 individual_task_result_completed,
                                 individual_task_result_closure_new (all_tasks_closure));

  append_stores_task_from_proxy (quote_ka_proxy,
                                 append_discovery_feed_quote_from_proxy,
                                 g_object_unref,
                                 NULL,
                                 cancellable,
                                 individual_task_result_completed,
                                 individual_task_result_closure_new (all_tasks_closure));

  if (!all_tasks_results_has_tasks_remaining (all_tasks_closure))
    all_tasks_results_return_now (all_tasks_closure);
}

static void
unordered_card_arrays_from_queries (GPtrArray           *ka_proxies,
                                    GCancellable        *cancellable,
                                    GAsyncReadyCallback  callback,
                                    gpointer             user_data)
{
  AllTasksResultsClosure *all_tasks_closure = all_tasks_results_closure_new (g_object_unref,
                                                                             callback,
                                                                             user_data);
  guint i = 0;
  g_autoptr(GPtrArray) word_proxies = g_ptr_array_new ();
  g_autoptr(GPtrArray) quote_proxies = g_ptr_array_new ();

  for (i = 0; i < ka_proxies->len; ++i)
    {
      ContentFeedKnowledgeAppProxy *ka_proxy = g_ptr_array_index (ka_proxies, i);
      GDBusProxy *dbus_proxy = content_feed_knowledge_app_proxy_get_dbus_proxy (ka_proxy);
      const gchar *interface_name = g_dbus_proxy_get_interface_name (dbus_proxy);

      if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedContent") == 0)
        append_stores_task_from_proxy (ka_proxy,
                                       append_discovery_feed_content_from_proxy,
                                       (GDestroyNotify) object_slist_free,
                                       append_discovery_feed_content_from_proxy_data_new ("ArticleCardDescriptions",
                                                                                          CONTENT_FEED_CARD_STORE_TYPE_ARTICLE_CARD,
                                                                                          CONTENT_FEED_CARD_LAYOUT_DIRECTION_IMAGE_FIRST,
                                                                                          content_feed_knowledge_app_card_store_new,
                                                                                          CONTENT_FEED_THUMBNAIL_SIZE_ARTICLE),
                                       cancellable,
                                       individual_task_result_completed,
                                       individual_task_result_closure_new (all_tasks_closure));
      else if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedNews") == 0)
        append_stores_task_from_proxy (ka_proxy,
                                       append_discovery_feed_content_from_proxy,
                                       (GDestroyNotify) object_slist_free,
                                       append_discovery_feed_content_from_proxy_data_new ("GetRecentNews",
                                                                                          CONTENT_FEED_CARD_STORE_TYPE_ARTICLE_CARD,
                                                                                          CONTENT_FEED_CARD_LAYOUT_DIRECTION_IMAGE_LAST,
                                                                                          (ContentFeedKnowledgeAppCardStoreFactoryFunc) content_feed_knowledge_app_news_card_store_new,
                                                                                          CONTENT_FEED_THUMBNAIL_SIZE_NEWS),
                                       cancellable,
                                       individual_task_result_completed,
                                       individual_task_result_closure_new (all_tasks_closure));
      else if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedVideo") == 0)
        append_stores_task_from_proxy (ka_proxy,
                                       append_discovery_feed_video_from_proxy,
                                       (GDestroyNotify) object_slist_free,
                                       NULL,
                                       cancellable,
                                       individual_task_result_completed,
                                       individual_task_result_closure_new (all_tasks_closure));
      else if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedArtwork") == 0)
        append_stores_task_from_proxy (ka_proxy,
                                       append_discovery_feed_artwork_from_proxy,
                                       (GDestroyNotify) object_slist_free,
                                       NULL,
                                       cancellable,
                                       individual_task_result_completed,
                                       individual_task_result_closure_new (all_tasks_closure));
      else if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedWord") == 0)
        g_ptr_array_add (word_proxies, ka_proxy);
      else if (g_strcmp0 (interface_name, "com.endlessm.DiscoveryFeedQuote") == 0)
        g_ptr_array_add (quote_proxies, ka_proxy);
    }

  for (i = 0; i < MIN (word_proxies->len, quote_proxies->len); ++i)
    {
      append_discovery_feed_word_quote_from_proxies (g_ptr_array_index (word_proxies, i),
                                                     g_ptr_array_index (quote_proxies, i),
                                                     cancellable,
                                                     individual_task_result_completed,
                                                     individual_task_result_closure_new (all_tasks_closure));
    }

  if (!all_tasks_results_has_tasks_remaining (all_tasks_closure))
    all_tasks_results_return_now (all_tasks_closure);
}

static void
received_all_unordered_card_array_results_from_queries (GObject      *source G_GNUC_UNUSED,
                                                        GAsyncResult *result,
                                                        gpointer      user_data)
{
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GPtrArray) results = g_task_propagate_pointer (G_TASK (result),
                                                           &local_error);
  GSList *all_unordered_elements = NULL;
  g_autoptr(GTask) task = user_data;
  guint i = 0;

  /* This basically shouldn't happen, but handle it anyway */
  if (results == NULL)
    {
      g_message ("Error getting all unordered card results: %s", local_error->message);
      g_task_return_error (task, g_steal_pointer (&local_error));
      return;
    }

  /* Go through each of the results, complain about the ones that failed
   * but keep the ones that yielded some models. We'll flat-map them all
   * together later */
  for (i = 0; i < results->len; ++i)
    {
      GSList *result_list = g_task_propagate_pointer (g_ptr_array_index (results, i),
                                                      &local_error);

      /* Need to check the result of local_error as result_list can be NULL */
      if (local_error != NULL)
        {
          g_message ("Query failed: %s", local_error->message);
          g_clear_error (&local_error);
          continue;
        }

      all_unordered_elements = g_slist_concat (all_unordered_elements, result_list);
    }

  g_task_return_pointer (task,
                         all_unordered_elements,
                         (GDestroyNotify) object_slist_free);
}

/**
 * content_feed_unordered_results_from_queries_finish:
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Complete the call to content_feed_unordered_results_from_queries.
 *
 * Note that a %NULL return value here does not necessarily mean that
 * the task finished with an error, since NULL is a valid value for
 * an empty list, the caller should check the @error outparam to check
 * if an error occurred.
 *
 * Returns: (transfer container) (element-type ContentFeedBaseCardStore):
 *          A #GSList of #ContentFeedBaseCardStore. Note that the list will
 *          be in reversed order internally for efficiency but in any event
 *          are not guaranteed to be in a user-friendly order. You should
 *          use content_feed_arrange_orderable_models to ensure that the
 *          models are in the correct order for presentation to the user.
 */
GSList *
content_feed_unordered_results_from_queries_finish (GAsyncResult  *result,
                                                    GError       **error)
{
  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * content_feed_unordered_results_from_queries:
 * @ka_proxies: (element-type ContentFeedKnowledgeAppProxy): An array of #ContentFeedKnowledgeAppProxy
 * @cancellable: A #GCancellable
 * @callback: Callback function
 * @user_data: Closure for @callback
 *
 * Query all proxies in @ka_proxies and pass a #GPtrArray of non-ordered
 * model results to @callback.
 */
void
content_feed_unordered_results_from_queries (GPtrArray           *ka_proxies,
                                             GCancellable        *cancellable,
                                             GAsyncReadyCallback  callback,
                                             gpointer             user_data)
{
  GTask *task = g_task_new (NULL, cancellable, callback, user_data);

  g_task_set_return_on_cancel (task, TRUE);
  unordered_card_arrays_from_queries (ka_proxies,
                                      cancellable,
                                      received_all_unordered_card_array_results_from_queries,
                                      task);
}

