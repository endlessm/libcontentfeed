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

#include "feed-base-card-store.h"
#include "feed-knowledge-app-proxy.h"

typedef struct _ContentFeedKnowledgeAppProxy {
  GObject object;
} EosDiscoeryFeedKnowledgeAppProxy;

typedef struct _ContentFeedKnowledgeAppProxyPrivate
{
  GDBusProxy *dbus_proxy;
  gchar      *desktop_id;
  gchar      *knowledge_search_object_path;
  gchar      *knowledge_app_id;
} ContentFeedKnowledgeAppProxyPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (ContentFeedKnowledgeAppProxy,
                            content_feed_knowledge_app_proxy,
                            G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_DBUS_PROXY,
  PROP_DESKTOP_ID,
  PROP_KNOWLEDGE_SEARCH_OBJECT_PATH,
  PROP_KNOWLEDGE_APP_ID,
  NPROPS
};

static GParamSpec *content_feed_knowledge_app_proxy_props [NPROPS] = { NULL, };

/**
 * content_feed_knowledge_app_proxy_get_dbus_proxy:
 * @proxy: A #ContentFeedKnowledgeAppProxy.
 *
 * Get the underlying #GDBusProxy for this #ContentFeedKnowledgeAppProxy.
 *
 * Returns: (transfer none): The #GDBusProxy for this #ContentFeedKnowledgeAppProxy
 */
GDBusProxy *
content_feed_knowledge_app_proxy_get_dbus_proxy (ContentFeedKnowledgeAppProxy *proxy)
{
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (proxy);

  return priv->dbus_proxy;
}

const gchar *
content_feed_knowledge_app_proxy_get_desktop_id (ContentFeedKnowledgeAppProxy *proxy)
{
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (proxy);

  return priv->desktop_id;
}

const gchar *
content_feed_knowledge_app_proxy_get_knowledge_search_object_path (ContentFeedKnowledgeAppProxy *proxy)
{
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (proxy);

  return priv->knowledge_search_object_path;
}

const gchar *
content_feed_knowledge_app_proxy_get_knowledge_app_id (ContentFeedKnowledgeAppProxy *proxy)
{
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (proxy);

  return priv->knowledge_app_id;
}

static void
content_feed_knowledge_app_proxy_init (ContentFeedKnowledgeAppProxy *model G_GNUC_UNUSED)
{
}

static void
content_feed_knowledge_app_proxy_set_property (GObject      *object,
                                               guint         prop_id,
                                               const GValue *value,
                                               GParamSpec   *pspec)
{
  ContentFeedKnowledgeAppProxy *store = CONTENT_FEED_KNOWLEDGE_APP_PROXY (object);
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_DBUS_PROXY:
      priv->dbus_proxy = g_value_dup_object (value);
      break;
    case PROP_DESKTOP_ID:
      priv->desktop_id = g_value_dup_string (value);
      break;
    case PROP_KNOWLEDGE_SEARCH_OBJECT_PATH:
      priv->knowledge_search_object_path = g_value_dup_string (value);
      break;
    case PROP_KNOWLEDGE_APP_ID:
      priv->knowledge_app_id = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_proxy_get_property (GObject    *object,
                                               guint       prop_id,
                                               GValue     *value,
                                               GParamSpec *pspec)
{
  ContentFeedKnowledgeAppProxy *store = CONTENT_FEED_KNOWLEDGE_APP_PROXY (object);
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_DBUS_PROXY:
      g_value_set_object (value, priv->dbus_proxy);
      break;
    case PROP_DESKTOP_ID:
      g_value_set_string (value, priv->desktop_id);
      break;
    case PROP_KNOWLEDGE_SEARCH_OBJECT_PATH:
      g_value_set_string (value, priv->knowledge_search_object_path);
      break;
    case PROP_KNOWLEDGE_APP_ID:
      g_value_set_string (value, priv->knowledge_app_id);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_knowledge_app_proxy_finalize (GObject *object)
{
  ContentFeedKnowledgeAppProxy *store = CONTENT_FEED_KNOWLEDGE_APP_PROXY (object);
  ContentFeedKnowledgeAppProxyPrivate *priv = content_feed_knowledge_app_proxy_get_instance_private (store);

  g_clear_object (&priv->dbus_proxy);
  g_clear_pointer (&priv->desktop_id, g_free);
  g_clear_pointer (&priv->knowledge_search_object_path, g_free);
  g_clear_pointer (&priv->knowledge_app_id, g_free);

  G_OBJECT_CLASS (content_feed_knowledge_app_proxy_parent_class)->finalize (object);
}

static void
content_feed_knowledge_app_proxy_class_init (ContentFeedKnowledgeAppProxyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = content_feed_knowledge_app_proxy_get_property;
  object_class->set_property = content_feed_knowledge_app_proxy_set_property;
  object_class->finalize = content_feed_knowledge_app_proxy_finalize;

  content_feed_knowledge_app_proxy_props[PROP_DBUS_PROXY] =
    g_param_spec_object ("dbus-proxy",
                         "DBus Proxy",
                         "The GDBusProxy for this ContentFeedKnowledgeAppProxy",
                         G_TYPE_DBUS_PROXY,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_proxy_props[PROP_DESKTOP_ID] =
    g_param_spec_string ("desktop-id",
                         "Desktop ID",
                         "The Desktop ID corresponding to where this proxy came from",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_proxy_props[PROP_KNOWLEDGE_SEARCH_OBJECT_PATH] =
    g_param_spec_string ("knowledge-search-object-path",
                         "Knowledge Search Object Path",
                         "The object path to access the knowledge-search interface",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_knowledge_app_proxy_props[PROP_KNOWLEDGE_APP_ID] =
    g_param_spec_string ("knowledge-app-id",
                         "Knowledge App ID",
                         "The App ID for the corresponding knowledge-app, if any",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     content_feed_knowledge_app_proxy_props);
}

/**
 * content_feed_knowledge_app_proxy_new:
 * @dbus_proxy: A #GDBusProxy connected to the provider
 * @desktop_id: The desktop-id uniquely identifying the source of this provider
 * @knowledge_search_object_path: (nullable): The object path for the knowledge-search interface
 * @knowledge_app_id: (nullable): The App ID for the corresponding knowledge-app, if any
 *
 * Create a new #ContentFeedKnowledgeAppProxy.
 *
 * Returns: A new #ContentFeedKnowledgeAppProxy.
 */
ContentFeedKnowledgeAppProxy *
content_feed_knowledge_app_proxy_new (GDBusProxy  *dbus_proxy,
                                      const gchar *desktop_id,
                                      const gchar *knowledge_search_object_path,
                                      const gchar *knowledge_app_id)
{
  return g_object_new (CONTENT_FEED_TYPE_KNOWLEDGE_APP_PROXY,
                       "dbus-proxy", dbus_proxy,
                       "desktop-id", desktop_id,
                       "knowledge-search-object-path", knowledge_search_object_path,
                       "knowledge-app-id", knowledge_app_id,
                       NULL);
}

