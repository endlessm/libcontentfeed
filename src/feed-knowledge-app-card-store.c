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

#include "feed-enums.h"
#include "feed-knowledge-app-card-store.h"
#include "feed-sizes.h"

struct _ContentFeedKnowledgeAppCardStore
{
  GObject parent_instance;
};

typedef struct _ContentFeedKnowledgeAppCardStorePrivate
{
  gchar                          *title;
  gchar                          *uri;
  gchar                          *synopsis;
  gchar                          *bus_name;
  gchar                          *knowledge_search_object_path;
  gchar                          *knowledge_app_id;
  GInputStream                   *thumbnail;
  gchar                          *thumbnail_uri;
  ContentFeedCardLayoutDirection  layout_direction;
  guint                           thumbnail_size;
  gchar                          *content_type;
} ContentFeedKnowledgeAppCardStorePrivate;

static void base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface);

G_DEFINE_TYPE_WITH_CODE (ContentFeedKnowledgeAppCardStore,
                         content_feed_knowledge_app_card_store,
                         CONTENT_FEED_TYPE_APP_CARD_STORE,
                         G_IMPLEMENT_INTERFACE (CONTENT_FEED_TYPE_BASE_CARD_STORE,
                                                base_card_store_iface_init)
                         G_ADD_PRIVATE (ContentFeedKnowledgeAppCardStore))

enum {
  PROP_0,
  PROP_TITLE,
  PROP_URI,
  PROP_SYNOPSIS,
  PROP_THUMBNAIL,
  PROP_BUS_NAME,
  PROP_KNOWLEDGE_SEARCH_OBJECT_PATH,
  PROP_KNOWLEDGE_APP_ID,
  PROP_LAYOUT_DIRECTION,
  PROP_THUMBNAIL_SIZE,
  PROP_THUMBNAIL_URI,
  PROP_CONTENT_TYPE,
  PROP_TYPE,
  NPROPS
};

static GParamSpec *content_feed_knowledge_app_card_store_props [NPROPS] = { NULL, };

static void
content_feed_knowledge_app_card_store_set_property (GObject      *object,
                                                    guint         prop_id,
                                                    const GValue *value,
                                                    GParamSpec   *pspec)
{
  ContentFeedKnowledgeAppCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_CARD_STORE (object);
  ContentFeedKnowledgeAppCardStorePrivate *priv = content_feed_knowledge_app_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_TITLE:
      priv->title = g_value_dup_string (value);
      break;
    case PROP_URI:
      priv->uri = g_value_dup_string (value);
      break;
    case PROP_SYNOPSIS:
      priv->synopsis = g_value_dup_string (value);
      break;
    case PROP_THUMBNAIL:
      priv->thumbnail = g_value_dup_object (value);
      break;
    case PROP_BUS_NAME:
      priv->bus_name = g_value_dup_string (value);
      break;
    case PROP_KNOWLEDGE_SEARCH_OBJECT_PATH:
      priv->knowledge_search_object_path = g_value_dup_string (value);
      break;
    case PROP_KNOWLEDGE_APP_ID:
      priv->knowledge_app_id = g_value_dup_string (value);
      break;
    case PROP_LAYOUT_DIRECTION:
      priv->layout_direction = g_value_get_enum (value);
      break;
    case PROP_THUMBNAIL_SIZE:
      priv->thumbnail_size = g_value_get_uint (value);
      break;
    case PROP_THUMBNAIL_URI:
      priv->thumbnail_uri = g_value_dup_string (value);
      break;
    case PROP_CONTENT_TYPE:
      priv->content_type = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_card_store_get_property (GObject    *object,
                                                    guint       prop_id,
                                                    GValue     *value,
                                                    GParamSpec *pspec)
{
  ContentFeedKnowledgeAppCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_CARD_STORE (object);
  ContentFeedKnowledgeAppCardStorePrivate *priv = content_feed_knowledge_app_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_TYPE:
      g_value_set_enum (value, CONTENT_FEED_CARD_STORE_TYPE_ARTICLE_CARD);
      break;
    case PROP_TITLE:
      g_value_set_string (value, priv->title);
      break;
    case PROP_URI:
      g_value_set_string (value, priv->uri);
      break;
    case PROP_SYNOPSIS:
      g_value_set_string (value, priv->synopsis);
      break;
    case PROP_THUMBNAIL:
      g_value_set_object (value, priv->thumbnail);
      break;
    case PROP_BUS_NAME:
      g_value_set_string (value, priv->bus_name);
      break;
    case PROP_KNOWLEDGE_SEARCH_OBJECT_PATH:
      g_value_set_string (value, priv->knowledge_search_object_path);
      break;
    case PROP_KNOWLEDGE_APP_ID:
      g_value_set_string (value, priv->knowledge_app_id);
      break;
    case PROP_LAYOUT_DIRECTION:
      g_value_set_enum (value, priv->layout_direction);
      break;
    case PROP_THUMBNAIL_SIZE:
      g_value_set_uint (value, priv->thumbnail_size);
      break;
    case PROP_THUMBNAIL_URI:
      g_value_set_string (value, priv->thumbnail_uri);
      break;
    case PROP_CONTENT_TYPE:
      g_value_set_string (value, priv->content_type);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_card_store_finalize (GObject *object)
{
  ContentFeedKnowledgeAppCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_CARD_STORE (object);
  ContentFeedKnowledgeAppCardStorePrivate *priv = content_feed_knowledge_app_card_store_get_instance_private (store);

  g_clear_pointer (&priv->title, g_free);
  g_clear_pointer (&priv->uri, g_free);
  g_clear_pointer (&priv->synopsis, g_free);
  g_clear_pointer (&priv->bus_name, g_free);
  g_clear_pointer (&priv->knowledge_search_object_path, g_free);
  g_clear_pointer (&priv->knowledge_app_id, g_free);
  g_clear_pointer (&priv->content_type, g_free);

  G_OBJECT_CLASS (content_feed_knowledge_app_card_store_parent_class)->finalize (object);
}

static void
content_feed_knowledge_app_card_store_dispose (GObject *object)
{
  ContentFeedKnowledgeAppCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_CARD_STORE (object);
  ContentFeedKnowledgeAppCardStorePrivate *priv = content_feed_knowledge_app_card_store_get_instance_private (store);

  g_clear_object (&priv->thumbnail);

  G_OBJECT_CLASS (content_feed_knowledge_app_card_store_parent_class)->dispose (object);
}

static void
content_feed_knowledge_app_card_store_init (ContentFeedKnowledgeAppCardStore *store G_GNUC_UNUSED)
{
}

static void
base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface G_GNUC_UNUSED)
{
}

static void
content_feed_knowledge_app_card_store_class_init (ContentFeedKnowledgeAppCardStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = content_feed_knowledge_app_card_store_get_property;
  object_class->set_property = content_feed_knowledge_app_card_store_set_property;
  object_class->dispose = content_feed_knowledge_app_card_store_dispose;
  object_class->finalize = content_feed_knowledge_app_card_store_finalize;

  content_feed_knowledge_app_card_store_props[PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title of Card",
                         "The title of the card",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_URI] =
    g_param_spec_string ("uri",
                         "URI of Content",
                         "The unique URI for the content in the app",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_SYNOPSIS] =
    g_param_spec_string ("synopsis",
                         "Synopsis of Content",
                         "Brief description of content",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_THUMBNAIL] =
    g_param_spec_object ("thumbnail",
                         "Content Thumbnail Stream",
                         "An input stream containing thumbnail image data",
                         G_TYPE_INPUT_STREAM,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_BUS_NAME] =
    g_param_spec_string ("bus-name",
                         "Bus Name",
                         "The bus name for the app",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_KNOWLEDGE_SEARCH_OBJECT_PATH] =
    g_param_spec_string ("knowledge-search-object-path",
                         "Knowledge Search Object Path",
                         "Object path for search object",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_KNOWLEDGE_APP_ID] =
    g_param_spec_string ("knowledge-app-id",
                         "Knowledge App ID",
                         "App ID for corresponding Knowledge App",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_LAYOUT_DIRECTION] =
    g_param_spec_enum ("layout-direction",
                       "Layout Direction",
                       "The direction in which to layout the card content",
                       CONTENT_FEED_TYPE_CONTENT_FEED_CARD_LAYOUT_DIRECTION,
                       CONTENT_FEED_CARD_LAYOUT_DIRECTION_IMAGE_FIRST,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_THUMBNAIL_SIZE] =
    g_param_spec_uint ("thumbnail-size",
                       "Thumbnail Size",
                       "How big, in square pixels, the thumbnails are",
                       CONTENT_FEED_THUMBNAIL_SIZE_APP_STORE,
                       CONTENT_FEED_THUMBNAIL_SIZE_ARTWORK,
                       CONTENT_FEED_THUMBNAIL_SIZE_ARTICLE,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_THUMBNAIL_URI] =
    g_param_spec_string ("thumbnail-uri",
                         "Thumbmnail URI",
                         "Optional URI to the thumbnail",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_card_store_props[PROP_CONTENT_TYPE] =
    g_param_spec_string ("content-type",
                         "Content Type",
                         "MIME type for the underlying content",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     PROP_TYPE,
                                     content_feed_knowledge_app_card_store_props);

  g_object_class_override_property (object_class,
                                    PROP_TYPE,
                                    "type");
}

ContentFeedKnowledgeAppCardStore *
content_feed_knowledge_app_card_store_new (const gchar                    *title,
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
                                           const gchar                    *content_type)
{
  return g_object_new (CONTENT_FEED_TYPE_KNOWLEDGE_APP_CARD_STORE,
                       "title", title,
                       "uri", uri,
                       "synopsis", synopsis,
                       "thumbnail", thumbnail,
                       "desktop-id", desktop_id,
                       "bus-name", bus_name,
                       "knowledge-search-object-path", knowledge_search_object_path,
                       "knowledge-app-id", knowledge_app_id,
                       "layout-direction", layout_direction,
                       "thumbnail-size", thumbnail_size,
                       "thumbnail-uri", thumbnail_uri,
                       "content-type", content_type,
                       NULL);
}
