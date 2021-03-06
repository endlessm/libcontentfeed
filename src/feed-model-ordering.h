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

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum _ContentFeedArrangeOrderableModelsFlags {
  CONTENT_FEED_ARRANGE_ORDERABLE_MODEL_FLAGS_INCLUDE_INSTALLABLE_APPS = (1 << 0)
} ContentFeedArrangeOrderableModelsFlags;

GPtrArray * content_feed_arrange_orderable_models (GPtrArray                              *unordered_orderable_models,
                                                   ContentFeedArrangeOrderableModelsFlags  flags);


G_END_DECLS
