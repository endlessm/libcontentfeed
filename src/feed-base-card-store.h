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

#include <glib-object.h>

G_BEGIN_DECLS

#define CONTENT_FEED_TYPE_BASE_CARD_STORE content_feed_base_card_store_get_type ()
G_DECLARE_INTERFACE (ContentFeedBaseCardStore,
                     content_feed_base_card_store,
                     CONTENT_FEED,
                     BASE_CARD_STORE,
                     GObject)

typedef enum
{
  CONTENT_FEED_CARD_STORE_TYPE_UNSET,
  CONTENT_FEED_CARD_STORE_TYPE_ARTICLE_CARD,
  CONTENT_FEED_CARD_STORE_TYPE_WORD_QUOTE_CARD,
  CONTENT_FEED_CARD_STORE_TYPE_ARTWORK_CARD,
  CONTENT_FEED_CARD_STORE_TYPE_AVAILABLE_APPS,
  CONTENT_FEED_CARD_STORE_TYPE_VIDEO_CARD,
  CONTENT_FEED_CARD_STORE_TYPE_NEWS_CARD,
  CONTENT_FEED_CARD_STORE_TYPE_N_CARD_TYPES
} ContentFeedCardStoreType;

struct _ContentFeedBaseCardStoreInterface {
  GTypeInterface parent_iface;
};

G_END_DECLS
