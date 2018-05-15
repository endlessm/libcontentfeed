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

#pragma once

#include <feed-base-card-store.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define CONTENT_FEED_TYPE_ORDERABLE_MODEL content_feed_orderable_model_get_type ()
G_DECLARE_FINAL_TYPE (ContentFeedOrderableModel, content_feed_orderable_model, CONTENT_FEED, ORDERABLE_MODEL, GObject)

const gchar * content_feed_orderable_model_get_source (ContentFeedOrderableModel *model);
ContentFeedCardStoreType content_feed_orderable_model_get_card_store_type (ContentFeedOrderableModel *model);
ContentFeedBaseCardStore * content_feed_orderable_model_get_model (ContentFeedOrderableModel *model);

ContentFeedOrderableModel * content_feed_orderable_model_new (ContentFeedBaseCardStore *model,
                                                              ContentFeedCardStoreType  type,
                                                              const char                    *source);

G_END_DECLS
