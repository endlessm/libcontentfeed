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

#include <string.h>

#include <glib.h>

#include "feed-text-sanitization.h"

#include "config.h"

/* Don't take any more sentences if they would cause the number of
 * lines to go above roughly three. However, we always take the first
 * sentence, see below. */
#define CHARACTER_COUNT_THRESHOLD 160

static gchar *
regex_replace (const gchar *exp, const gchar *str, const gchar *replacement)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GRegex) regex = g_regex_new (exp, 0, 0, &error);
  g_autofree gchar *replaced = NULL;

  if (regex == NULL)
    {
      g_error ("Fatal error in compiling regular expression: %s", error->message);
      return NULL;
    }

  replaced = g_regex_replace (regex, str, -1, 0, replacement, 0, &error);

  if (replaced == NULL)
    {
      g_error ("Fatal error in performing string replacement: %s", error->message);
      return NULL;
    }

  return g_steal_pointer (&replaced);
}

static gchar *
remove_square_brackets (const gchar *str)
{
  return regex_replace ("\\[\\d+\\]", str, "");
}

static gchar *
remove_parens (const gchar *str)
{
  return regex_replace ("\\(.*?\\)", str, "");
}

static gchar *
trim_to_first_n_sentences_under_threshold (const gchar *str,
                                           guint        character_threshold)
{
  g_autoptr(GPtrArray) sentences = g_ptr_array_new ();
  g_auto(GStrv) all_sentences = g_strsplit (str, ".", -1);
  GStrv iter = all_sentences;
  guint character_count = 0;

  for (; *iter != NULL; ++iter)
    {
      gsize sentence_length = strlen (*iter);
      gsize sentence_length_with_period = sentence_length + 1;

      if (sentence_length == 0)
        continue;

      /* Even if the sentence would be longer than the character threshold
       * we should always take it. We'll ellipsize it later at a word boundary. */
      if ((character_count + sentence_length_with_period) > character_threshold &&
          sentences->len > 0)
        break;

      g_ptr_array_add (sentences, *iter);
      character_count += sentence_length_with_period;
    }

  /* Null-terminate */
  g_ptr_array_add (sentences, NULL);
  return g_strjoinv (".", (GStrv) sentences->pdata);
}

static gchar *
normalize_whitespace (const gchar *str)
{
  return regex_replace ("\\s+", str, " ");
}

static gchar *
strip (const gchar *str)
{
  gchar *copy = g_strdup (str);
  g_strstrip (copy);
  return copy;
}

static gchar *
add_ending_period (const gchar *str)
{
  guint len = strlen (str);

  /* Don't add ending period if the string had no length.
   *
   * This might be the case if the model had no synopsis, like those
   * having hook titles. */
  if (len > 0)
    {
      g_autofree gchar *ending_period = g_new0 (gchar, len + 2);

      strcpy (ending_period, str);
      ending_period[len] = '.';
      ending_period[len + 1] = '\0';

      return g_steal_pointer (&ending_period);
    }

  return g_strdup (str);
}

gchar *
content_feed_sanitize_synopsis (const gchar *synopsis)
{
  g_autofree gchar *stripped = strip (synopsis);
  g_autofree gchar *square_brackets_removed = remove_square_brackets (stripped);
  g_autofree gchar *parens_removed = remove_parens (square_brackets_removed);
  g_autofree gchar *first_two_sentences =
    trim_to_first_n_sentences_under_threshold (parens_removed,
                                               CHARACTER_COUNT_THRESHOLD);
  g_autofree gchar *normalized_whitespace = normalize_whitespace (first_two_sentences);
  g_autofree gchar *with_ending_period = add_ending_period (normalized_whitespace);

  return g_steal_pointer (&with_ending_period);
}
