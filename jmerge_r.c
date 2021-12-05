/*
 * Copyright (c) 2013 ... 2020 2021
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

#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

char *jmerge_r_c="$Id: jmerge_r.c,v 2.4 2021/02/21 20:37:58 jmccue Exp $";

/*
 * check_key() -- determine of the entry can be saved
 */
int check_key(struct s_key_data *k, char *key)
{
  struct s_key *now = k->keys;
  int found_key, c, n;

  switch (k->filter)
    {
      case FILTER_NO:
	return((int) TRUE);
	break;
      case FILTER_YES:
	break;
      case FILTER_INVERT:
	break;
      default:
	return((int) FALSE);
    }

  /* set starting point and idx character */
  c = (*key);
  if (k->kidx[c] != (struct s_key *) NULL)
    now = k->kidx[c];

  /* look for entry */
  found_key = (int) FALSE;
  while (now != (struct s_key *) NULL)
    {
      n = (*now->key);
      if (c != n)
	break;
      if (now->key != (char *) NULL)
	{
	  if (strcmp(now->key,key) == 0)
	    {
	      found_key = (int) TRUE;
	      break;
	    }
	}
      now = now->next;
    }

  if (k->filter == FILTER_YES)
    return(found_key);

  /* inverse match, flip return */
  if (found_key == (int) TRUE)
    return((int) FALSE);
  else
    return((int) TRUE);

} /* check_key() */

/*
 * parse_in_rec() -- load data into a record structure
 */
void parse_in_rec(struct s_file_recs *r, char *buf, char delm)
{
  char *wbuf = (char *) NULL;
  char *others;
  size_t ksize;

  r->key           = (char *) NULL;
  r->other_fields  = (char *) NULL;
  r->match_count   = 0;
  r->next          = (struct s_file_recs *) NULL;
  r->match         = (struct s_file_recs *) NULL;

  if (buf == (char *) NULL)
    return;

  j2_fix_delm(delm, &(wbuf), buf);

  /*** Find non key fields ***/
  for (others = wbuf, ksize = (size_t) 0; (*others) != JLIB2_CHAR_NULL; others++)
    {
      if ((*others) == delm)
	{
	  others++;
	  break;
	}
      else
	ksize++;
    }

  /*** save data ***/
  if ((*others) == JLIB2_CHAR_NULL)
    r->other_fields  = strdup(" ");
  else
    r->other_fields  = strdup(others);

  if (ksize == (size_t) 0)
    r->key = strdup(" ");
  else
    r->key = strndup(buf,ksize);

  j2_rtw(r->key);

  if (wbuf != (char *) NULL)
    free(wbuf);

} /* parse_in_rec() */

/*
 * add_node() -- add an entry
 */
void add_node(FILE *efp, struct s_file_data *d, 
              struct s_key_data *k, char *buf, 
              int *reset, int headg, char delm)
{
  static struct s_file_recs *now = (struct s_file_recs *) NULL;
  static char *last_key = (char *) NULL;
  struct s_file_recs knew;
  int c;

  if ((*reset) == (int) TRUE)
    {
      now = (struct s_file_recs *) NULL;
      last_key = (char *) NULL;
      (*reset) = (int) FALSE;
    }

  if (buf == (char *) NULL)
    return;

  /*** parse the record and determine if we keep it ***/
  j2_bye_nl(buf);
  j2_bye_ctlm(buf);
  parse_in_rec(&knew, buf, delm);

  if ( ! check_key(k, knew.key) )
    {
      free(knew.key);
      free(knew.other_fields);
      return;
    }

  /*** we can add the entry ***/
  if (d->r == (struct s_file_recs *) NULL)
    {
      d->r = (struct s_file_recs *) malloc(sizeof(struct s_file_recs));
      now = d->r;
    }
  else
    {
      now->next = (struct s_file_recs *) malloc(sizeof(struct s_file_recs));
      now = now->next;
    }
  now->key          = strdup(knew.key);
  now->other_fields = strdup(knew.other_fields);
  now->match_count  = knew.match_count;
  now->match        = knew.match;
  now->next         = knew.next;
  free(knew.key);
  free(knew.other_fields);

  /*** count fields ***/
  c = j2_count_c(delm, buf);
  if (d->max_fields == 0)
    d->max_fields = c;
  else
    {
      if (c != d->max_fields)
	{
	  fprintf(efp, MSG_ERR_E062, d->finfo.io, d->max_fields, c);
	  fprintf(efp, MSG_ERR_FNAME, d->file_num,
		  (strcmp(d->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d->finfo.fname));
	  exit(EXIT_FAILURE);
	}
    }

  /*** from this point down we check to ensure the ***/
  /*   the sort order is correct.  So if  record 1   */
  /*   is a heading, exit function.                  */
  if ((d->finfo.io == 1) && (headg))
    return;

  /* validate key to ensure it is OK */
  if (last_key == (char *) NULL)
    last_key = now->key;
  else
    {
      c = strcmp(last_key,now->key);
      if (c == 0)
	{
	  if (d->need_uniq)
	    {
	      fprintf(efp, MSG_ERR_E060, d->finfo.io);
	      fprintf(efp, MSG_ERR_FNAME, d->file_num,
		      (strcmp(d->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d->finfo.fname));
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  if (c > 0)
	    {
	      fprintf(efp, MSG_ERR_E061, d->finfo.io);
	      fprintf(efp, MSG_ERR_FNAME, d->file_num,
		      (strcmp(d->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d->finfo.fname));
	      exit(EXIT_FAILURE);
	    }
	}
      last_key = now->key;
    }

} /* add_node() */

/*
 * load_link() -- load data into a linked list
 */
void load_link(FILE *efp, 
               struct s_file_data *d, 
               struct s_key_data *k, 
               char delm, int headg)

{
  char *buf  = (char *) NULL;
  size_t  bsize = (size_t) 0;
  int reset = (int) TRUE;
  FILE *fp;

  if ( ! open_in(&fp, d->finfo.fname, efp) )
    {
      fprintf(efp, MSG_ERR_E010, d->finfo.fname, "r");
      exit(EXIT_FAILURE);
    }

  while (getline(&buf, &bsize, fp) > (ssize_t) -1)
    {
      (d->finfo.io)++;
      add_node(efp, d, k, buf, &reset, headg, delm);
    }

  close_in(&fp, d->finfo.fname);
  if (buf != (char *) NULL)
    free(buf);

}  /* load_link() */

/* END: jmerge_r.c */
