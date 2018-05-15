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

#define CONTENT_FEED_TYPE_WORD_CARD_STORE content_feed_word_card_store_get_type ()
G_DECLARE_FINAL_TYPE (ContentFeedWordCardStore, content_feed_word_card_store, CONTENT_FEED, WORD_CARD_STORE, GObject)

ContentFeedWordCardStore * content_feed_word_card_store_new (const gchar *word,
                                                             const gchar *part_of_speech,
                                                             const gchar *definition);

G_END_DECLS
