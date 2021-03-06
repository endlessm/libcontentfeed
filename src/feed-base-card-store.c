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
#include "feed-enums.h"

G_DEFINE_INTERFACE (ContentFeedBaseCardStore,
                    content_feed_base_card_store,
                    G_TYPE_OBJECT)

static void
content_feed_base_card_store_default_init (ContentFeedBaseCardStoreInterface *iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_enum ("type",
                                                          "Card Type",
                                                          "The type of card",
                                                          CONTENT_FEED_TYPE_CONTENT_FEED_CARD_STORE_TYPE,
                                                          CONTENT_FEED_CARD_STORE_TYPE_UNSET,
                                                          G_PARAM_READABLE));
}
