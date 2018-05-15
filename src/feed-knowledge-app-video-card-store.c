/* Copyright 2018 Endless Mobile, Inc.
 *
 * eos-discovery-feed is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * eos-discovery-feed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with eos-discovery-feed.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "feed-base-card-store.h"
#include "feed-knowledge-app-video-card-store.h"
#include "feed-sizes.h"

struct _ContentFeedKnowledgeAppVideoCardStore
{
  GObject parent_instance;
};

typedef struct _ContentFeedKnowledgeAppVideoCardStorePrivate
{
  gchar *duration;
} ContentFeedKnowledgeAppVideoCardStorePrivate;

static void base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface);

G_DEFINE_TYPE_WITH_CODE (ContentFeedKnowledgeAppVideoCardStore,
                         content_feed_knowledge_app_video_card_store,
                         CONTENT_FEED_TYPE_KNOWLEDGE_APP_CARD_STORE,
                         G_IMPLEMENT_INTERFACE (CONTENT_FEED_TYPE_BASE_CARD_STORE,
                                                base_card_store_iface_init)
                         G_ADD_PRIVATE (ContentFeedKnowledgeAppVideoCardStore))

enum {
  PROP_0,
  PROP_DURATION,
  PROP_TYPE,
  NPROPS
};

static GParamSpec *content_feed_knowledge_app_video_card_store_props [NPROPS] = { NULL, };

static void
content_feed_knowledge_app_video_card_store_set_property (GObject      *object,
                                                          guint         prop_id,
                                                          const GValue *value,
                                                          GParamSpec   *pspec)
{
  ContentFeedKnowledgeAppVideoCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_VIDEO_CARD_STORE (object);
  ContentFeedKnowledgeAppVideoCardStorePrivate *priv = content_feed_knowledge_app_video_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_DURATION:
      priv->duration = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_video_card_store_get_property (GObject    *object,
                                                          guint       prop_id,
                                                          GValue     *value,
                                                          GParamSpec *pspec)
{
  ContentFeedKnowledgeAppVideoCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_VIDEO_CARD_STORE (object);
  ContentFeedKnowledgeAppVideoCardStorePrivate *priv = content_feed_knowledge_app_video_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_TYPE:
      g_value_set_enum (value, CONTENT_FEED_CARD_STORE_TYPE_VIDEO_CARD);
      break;
    case PROP_DURATION:
      g_value_set_string (value, priv->duration);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_video_card_store_finalize (GObject *object)
{
  ContentFeedKnowledgeAppVideoCardStore *store = CONTENT_FEED_KNOWLEDGE_APP_VIDEO_CARD_STORE (object);
  ContentFeedKnowledgeAppVideoCardStorePrivate *priv = content_feed_knowledge_app_video_card_store_get_instance_private (store);

  g_clear_pointer (&priv->duration, g_free);

  G_OBJECT_CLASS (content_feed_knowledge_app_video_card_store_parent_class)->finalize (object);
}

static void
content_feed_knowledge_app_video_card_store_init (ContentFeedKnowledgeAppVideoCardStore *store G_GNUC_UNUSED)
{
}

static void
base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface G_GNUC_UNUSED)
{
}

static void
content_feed_knowledge_app_video_card_store_class_init (ContentFeedKnowledgeAppVideoCardStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = content_feed_knowledge_app_video_card_store_get_property;
  object_class->set_property = content_feed_knowledge_app_video_card_store_set_property;
  object_class->finalize = content_feed_knowledge_app_video_card_store_finalize;

  content_feed_knowledge_app_video_card_store_props[PROP_DURATION] =
    g_param_spec_string ("duration",
                         "Duration of Video",
                         "The duration of the video in no particular format",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     PROP_TYPE,
                                     content_feed_knowledge_app_video_card_store_props);

  g_object_class_override_property (object_class,
                                    PROP_TYPE,
                                    "type");
}

ContentFeedKnowledgeAppVideoCardStore *
content_feed_knowledge_app_video_card_store_new (const gchar  *title,
                                                 const gchar  *uri,
                                                 const gchar  *duration,
                                                 GInputStream *thumbnail,
                                                 const gchar  *desktop_id,
                                                 const gchar  *bus_name,
                                                 const gchar  *knowledge_search_object_path,
                                                 const gchar  *knowledge_app_id,
                                                 const gchar  *thumbnail_uri,
                                                 const gchar  *content_type)
{
  return g_object_new (CONTENT_FEED_TYPE_KNOWLEDGE_APP_VIDEO_CARD_STORE,
                       "title", title,
                       "uri", uri,
                       "duration", duration,
                       "thumbnail", thumbnail,
                       "desktop-id", desktop_id,
                       "bus-name", bus_name,
                       "knowledge-search-object-path", knowledge_search_object_path,
                       "knowledge-app-id", knowledge_app_id,
                       "thumbnail-uri", thumbnail_uri,
                       "content-type", content_type,
                       NULL);
}
