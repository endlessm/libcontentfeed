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
#include <glib-object.h>

G_BEGIN_DECLS

#define CONTENT_FEED_TYPE_PROVIDER_INFO content_feed_provider_info_get_type ()
G_DECLARE_FINAL_TYPE (ContentFeedProviderInfo, content_feed_provider_info, CONTENT_FEED, PROVIDER_INFO, GObject)

const gchar * content_feed_provider_info_get_object_path (ContentFeedProviderInfo *provider_info);
const gchar * content_feed_provider_info_get_bus_name (ContentFeedProviderInfo *provider_info);
const gchar * const * content_feed_provider_info_get_interfaces (ContentFeedProviderInfo *provider_info);
const gchar * content_feed_provider_info_get_knowledge_app_id (ContentFeedProviderInfo *provider_info);
const gchar * content_feed_provider_info_get_desktop_file_id (ContentFeedProviderInfo *provider_info);
const gchar * content_feed_provider_info_get_knowledge_search_object_path (ContentFeedProviderInfo *provider_info);

ContentFeedProviderInfo * content_feed_provider_info_new (const gchar         *path,
                                                          const gchar         *bus_name,
                                                          const gchar * const *interfaces,
                                                          const gchar         *knowledge_app_id,
                                                          const gchar         *desktop_file_id,
                                                          const gchar         *knowledge_search_object_path);

G_END_DECLS
