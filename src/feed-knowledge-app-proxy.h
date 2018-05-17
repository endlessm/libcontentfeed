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

#pragma once

#include <feed-base-card-store.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define CONTENT_FEED_TYPE_KNOWLEDGE_APP_PROXY content_feed_knowledge_app_proxy_get_type ()
G_DECLARE_FINAL_TYPE (ContentFeedKnowledgeAppProxy, content_feed_knowledge_app_proxy, CONTENT_FEED, KNOWLEDGE_APP_PROXY, GObject)

ContentFeedKnowledgeAppProxy * content_feed_knowledge_app_proxy_new (GDBusProxy  *dbus_proxy,
                                                                     const gchar *desktop_id,
                                                                     const gchar *knowledge_search_object_path,
                                                                     const gchar *knowledge_app_id);

GDBusProxy * content_feed_knowledge_app_proxy_get_dbus_proxy (ContentFeedKnowledgeAppProxy *proxy);

const gchar * content_feed_knowledge_app_proxy_get_desktop_id (ContentFeedKnowledgeAppProxy *proxy);
const gchar * content_feed_knowledge_app_proxy_get_knowledge_search_object_path (ContentFeedKnowledgeAppProxy *proxy);
const gchar * content_feed_knowledge_app_proxy_get_knowledge_app_id (ContentFeedKnowledgeAppProxy *proxy);

G_END_DECLS
