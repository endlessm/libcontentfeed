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

#include "feed-word-quote-card-store.h"

struct _ContentFeedWordQuoteCardStore
{
  GObject parent_instance;
};

typedef struct _ContentFeedWordQuoteCardStorePrivate
{
  ContentFeedWordCardStore *word;
  ContentFeedQuoteCardStore *quote;
} ContentFeedWordQuoteCardStorePrivate;

static void base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface);

G_DEFINE_TYPE_WITH_CODE (ContentFeedWordQuoteCardStore,
                         content_feed_word_quote_card_store,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (CONTENT_FEED_TYPE_BASE_CARD_STORE,
                                                base_card_store_iface_init)
                         G_ADD_PRIVATE (ContentFeedWordQuoteCardStore))

enum {
  PROP_0,
  PROP_WORD,
  PROP_QUOTE,
  PROP_TYPE,
  NPROPS
};

static GParamSpec *content_feed_word_quote_card_store_props [NPROPS] = { NULL, };

static void
content_feed_word_quote_card_store_set_property (GObject      *object,
                                                 guint         prop_id,
                                                 const GValue *value,
                                                 GParamSpec   *pspec)
{
  ContentFeedWordQuoteCardStore *store = CONTENT_FEED_WORD_QUOTE_CARD_STORE (object);
  ContentFeedWordQuoteCardStorePrivate *priv = content_feed_word_quote_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_WORD:
      priv->word = g_value_dup_object (value);
      break;
    case PROP_QUOTE:
      priv->quote = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_word_quote_card_store_get_property (GObject    *object,
                                                 guint       prop_id,
                                                 GValue     *value,
                                                 GParamSpec *pspec)
{
  ContentFeedWordQuoteCardStore *store = CONTENT_FEED_WORD_QUOTE_CARD_STORE (object);
  ContentFeedWordQuoteCardStorePrivate *priv = content_feed_word_quote_card_store_get_instance_private (store);

  switch (prop_id)
    {
    case PROP_TYPE:
      g_value_set_enum (value, CONTENT_FEED_CARD_STORE_TYPE_WORD_QUOTE_CARD);
      break;
    case PROP_WORD:
      g_value_set_object (value, priv->word);
      break;
    case PROP_QUOTE:
      g_value_set_object (value, priv->quote);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
content_feed_word_quote_card_store_dispose (GObject *object)
{
  ContentFeedWordQuoteCardStore *store = CONTENT_FEED_WORD_QUOTE_CARD_STORE (object);
  ContentFeedWordQuoteCardStorePrivate *priv = content_feed_word_quote_card_store_get_instance_private (store);

  g_clear_object (&priv->word);
  g_clear_object (&priv->quote);

  G_OBJECT_CLASS (content_feed_word_quote_card_store_parent_class)->dispose (object);
}

static void
content_feed_word_quote_card_store_init (ContentFeedWordQuoteCardStore *store G_GNUC_UNUSED)
{
}

static void
base_card_store_iface_init (ContentFeedBaseCardStoreInterface *iface G_GNUC_UNUSED)
{
}

static void
content_feed_word_quote_card_store_class_init (ContentFeedWordQuoteCardStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = content_feed_word_quote_card_store_get_property;
  object_class->set_property = content_feed_word_quote_card_store_set_property;
  object_class->dispose = content_feed_word_quote_card_store_dispose;

  content_feed_word_quote_card_store_props[PROP_WORD] =
    g_param_spec_object ("word",
                         "Word Store",
                         "The Word Store part of this WordQuoteStore",
                         CONTENT_FEED_TYPE_WORD_CARD_STORE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  content_feed_word_quote_card_store_props[PROP_QUOTE] =
    g_param_spec_object ("quote",
                         "Quote Store",
                         "The Quote Store part of this WordQuoteStore",
                         CONTENT_FEED_TYPE_QUOTE_CARD_STORE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     PROP_TYPE,
                                     content_feed_word_quote_card_store_props);

  g_object_class_override_property (object_class,
                                    PROP_TYPE,
                                    "type");
}

ContentFeedWordQuoteCardStore *
content_feed_word_quote_card_store_new (ContentFeedWordCardStore *word,
                                              ContentFeedQuoteCardStore *quote)
{
  return g_object_new (CONTENT_FEED_TYPE_WORD_QUOTE_CARD_STORE,
                       "word", word,
                       "quote", quote,
                       NULL);
}
