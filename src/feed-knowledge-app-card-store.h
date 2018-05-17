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

#include <gio/gio.h>
#include <glib-object.h>

#include <feed-app-card-store.h>
#include <feed-card-layout-direction.h>

G_BEGIN_DECLS

#define CONTENT_FEED_TYPE_KNOWLEDGE_APP_CARD_STORE content_feed_knowledge_app_card_store_get_type ()
G_DECLARE_FINAL_TYPE (ContentFeedKnowledgeAppCardStore, content_feed_knowledge_app_card_store, CONTENT_FEED, KNOWLEDGE_APP_CARD_STORE, ContentFeedAppCardStore)

ContentFeedKnowledgeAppCardStore * content_feed_knowledge_app_card_store_new (const gchar                    *title,
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

G_END_DECLS
