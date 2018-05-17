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
#include "feed-orderable-model.h"

typedef struct _ContentFeedOrderableModel {
  GObject object;
} EosDiscoeryFeedOrderableModel;

typedef struct _ContentFeedOrderableModelPrivate
{
  ContentFeedBaseCardStore *model;
  ContentFeedCardStoreType  type;
  gchar                    *source;
} ContentFeedOrderableModelPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (ContentFeedOrderableModel,
                            content_feed_orderable_model,
                            G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_MODEL,
  PROP_TYPE,
  PROP_SOURCE,
  NPROPS
};

static GParamSpec *content_feed_orderable_model_props [NPROPS] = { NULL, };

/**
 * content_feed_orderable_model_get_source:
 * @model: An #ContentFeedOrderableModel
 *
 * Returns: (transfer none): The source of this model or %NULL if no source was
 *                           registered at construction.
 */
const gchar *
content_feed_orderable_model_get_source (ContentFeedOrderableModel *model)
{
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (model);

  return priv->source;
}

/**
 * content_feed_orderable_model_get_card_store_type:
 * @model: An #ContentFeedOrderableModel
 *
 * Returns: The #ContentFeedCardStoreType of this model.
 */
ContentFeedCardStoreType
content_feed_orderable_model_get_card_store_type (ContentFeedOrderableModel *model)
{
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (model);

  return priv->type;
}

/**
 * content_feed_orderable_model_get_model:
 * @model: An #ContentFeedOrderableModel
 *
 * Returns: (transfer none): The #ContentFeedCardBaseCardStore of this model.
 */
ContentFeedBaseCardStore *
content_feed_orderable_model_get_model (ContentFeedOrderableModel *model)
{
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (model);

  return priv->model;
}

static void
content_feed_orderable_model_init (ContentFeedOrderableModel *model G_GNUC_UNUSED)
{
}

static void
content_feed_orderable_model_set_property (GObject      *object,
                                           guint         prop_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
  ContentFeedOrderableModel *store = CONTENT_FEED_ORDERABLE_MODEL (object);
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_MODEL:
      priv->model = g_value_dup_object (value);
      break;
    case PROP_TYPE:
      priv->type = g_value_get_enum (value);
      break;
    case PROP_SOURCE:
      priv->source = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_orderable_model_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
  ContentFeedOrderableModel *store = CONTENT_FEED_ORDERABLE_MODEL (object);
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_MODEL:
      g_value_set_object (value, priv->model);
      break;
    case PROP_TYPE:
      g_value_set_enum (value, priv->type);
      break;
    case PROP_SOURCE:
      g_value_set_string (value, priv->source);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_orderable_model_finalize (GObject *object)
{
  ContentFeedOrderableModel *store = CONTENT_FEED_ORDERABLE_MODEL (object);
  ContentFeedOrderableModelPrivate *priv = content_feed_orderable_model_get_instance_private (store);

  g_clear_pointer (&priv->source, g_free);
  g_clear_object (&priv->model);

  G_OBJECT_CLASS (content_feed_orderable_model_parent_class)->finalize (object);
}

static void
content_feed_orderable_model_class_init (ContentFeedOrderableModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = content_feed_orderable_model_get_property;
  object_class->set_property = content_feed_orderable_model_set_property;
  object_class->finalize = content_feed_orderable_model_finalize;

  content_feed_orderable_model_props[PROP_MODEL] =
    g_param_spec_object ("model",
                         "Model",
                         "The ContentFeedBaseCardStore for this orderable model",
                         CONTENT_FEED_TYPE_BASE_CARD_STORE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_orderable_model_props[PROP_TYPE] =
    g_param_spec_enum ("type",
                       "Type",
                       "The ContentFeedCardStoreType for this orderable model",
                       CONTENT_FEED_TYPE_CONTENT_FEED_CARD_STORE_TYPE,
                       CONTENT_FEED_CARD_STORE_TYPE_UNSET,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_orderable_model_props[PROP_SOURCE] =
    g_param_spec_string ("source",
                         "Source",
                         "A string indicating where this model came from",
                         "",
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     content_feed_orderable_model_props);
}

ContentFeedOrderableModel *
content_feed_orderable_model_new (ContentFeedBaseCardStore *model,
                                  ContentFeedCardStoreType  type,
                                  const char               *source)
{
  return g_object_new (CONTENT_FEED_TYPE_ORDERABLE_MODEL,
                       "model", model,
                       "type", type,
                       "source", source,
                       NULL);
}

