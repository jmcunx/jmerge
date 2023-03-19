/*
 * Copyright (c) 2013 ... 2023 2024
 *     John McCue <jmccue@jmcunx.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jmerge.h"

#define REC_SIZE_INIT 512

/*
 * key_init() -- Initialize rec info
 */
void key_init(struct s_key_data *r)
{

  r->pos         = 0;
  r->count_match = 0;
  r->k           = (char *) NULL;
  r->next        = NULL_KEY_DATA;

} /* key_init() */

/*
 * key_clear_counts() -- Clear match Count
 */
void key_clear_counts(struct s_key_data *raw)
{
  struct s_key_data *now   = raw;

  while (now != NULL_KEY_DATA)
    {
      now->count_match = 0L;
      now = now->next;
    }

} /* END: key_clear_counts() */

/*
 * key_print() -- Print data
 */
long int key_print(FILE *fp, struct s_key_data *raw)
{
  struct s_key_data *now   = raw;
  long int r = 0L;

  while (now != NULL_KEY_DATA)
    {
      fprintf(fp, "%11ld %s\n", now->count_match, now->k);
      r++;
      now = now->next;
    }

  return(r);

} /* END: key_print() */

/*
 * key_search() -- search for key data
 */
struct s_key_data *key_search(char *buf, struct s_key_data *raw)
{
  struct s_key_data *now   = raw;

  while (now != NULL_KEY_DATA)
    {
      if (strcmp(now->k,buf) == 0)
	{
	  now->count_match++;
	  return(now);
	}
      now = now->next;
    }
  return(NULL_KEY_DATA);

} /* END: key_search() */

/*
 * key_free() -- free memory
 *               This logic avoids a recursion stack overflow.
 *               Freeing some large lists can cause a stack
 *               overflow  on some Operating Systems.
 */
void key_free(struct s_key_data **raw)
{
  struct s_key_data *now   = (*raw);
  struct s_key_data *prior = (*raw);

  while (now != NULL_KEY_DATA)
    {
      prior = now;
      now = now->next;
      if (prior->k != (char *) NULL)
	free(prior->k);
      free(prior);
      prior = NULL_KEY_DATA;
    }

  (*raw) = NULL_KEY_DATA;

} /* END: key_free() */

/*
 * key_save() -- get memory and save data
 */
struct s_key_data *key_save(char *buf, long int pos_key, long int rcount, char delm)
{
  struct s_key_data *raw = NULL_KEY_DATA;
  char *cdlm = (char *) NULL;

  raw = calloc(1, sizeof (struct s_key_data));
  key_init(raw);

  if (buf != (char *) NULL)
    {
      cdlm = strchr(buf, delm);
      if (cdlm != (char *) NULL)
	(*cdlm) = JLIB2_CHAR_NULL;
      raw->k  = strdup(buf);
      raw->pos = pos_key;
    }

  return(raw);

} /* key_save() */

/*
 * key_load() -- load key data into a linked list
 */
struct s_key_data *key_load(FILE *errfp,
                            struct s_file_info *f, 
                            int delm, int verbose)
{
  size_t bsize = (size_t) 0;
  char *buf = (char *) NULL;
  struct s_key_data *raw, *now, *new;
  long int pos_key = 0L;

  raw = now = new = NULL_KEY_DATA;

  /* allocate initial memory (optional) */
  bsize = REC_SIZE_INIT;
  buf = (char *) calloc(REC_SIZE_INIT, sizeof(char));
  if (buf == (char *) NULL)
    {
      fprintf(errfp, MSG_ERR_E003,
             (f->fname == (char *) NULL ? LIT_STDIN : f->fname));
      fprintf(errfp, "\t%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

  while (getline(&buf, &bsize, f->fp) >= (ssize_t) 0)
    {
      f->lines_read++;
      j2_rtw(buf);
      new = key_save(buf, pos_key, f->lines_read, delm);
      if (raw == NULL_KEY_DATA)
	raw = new;
      else
	now->next = new;
      now = new;
      memset(buf, '\0', bsize);
      pos_key = ftell(f->fp);
    }

  if (buf != (char *) NULL)
    free(buf);

  return(raw);

} /* key_load() */
