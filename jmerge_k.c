/*
 * Copyright (c) 2013 ... 2021 2022
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

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

/*
 * add_key_node() -- add an entry
 */
void add_key_node(FILE *efp, struct s_key_data *k, char *buf, int *reset)

{
  static struct s_key *now = (struct s_key *) NULL;
  static char *last_key = (char *) NULL;
  int c;

  if ((*reset) == (int) TRUE)
    {
      now = (struct s_key *) NULL;
      last_key = (char *) NULL;
      (*reset) = (int) FALSE;
    }
   
  if (buf == (char *) NULL)
    return;
   
  j2_bye_nl(buf);
  j2_bye_ctlm(buf);
   
  /*** save the record ***/
  if (k->keys == (struct s_key *) NULL)
    {
      k->keys = (struct s_key *) malloc(sizeof(struct s_key));
      now = k->keys;
    }
  else
    {
      now->next = (struct s_key *) malloc(sizeof(struct s_key));
      now = now->next;
    }
  now->key = strdup(buf);

  /*** add to index, if need be ***/
  c = (*now->key);
  if (k->kidx[c] == (struct s_key *) NULL)
    k->kidx[c] = now;

  /* validate key to ensure it is OK */
  if (last_key == (char *) NULL)
    last_key = now->key;
  else
    {
      c = strcmp(last_key,now->key);
      if (c == 0)
	{
	  fprintf(efp, MSG_ERR_E060, k->finfo.io);
	  fprintf(efp, MSG_ERR_FNAMN,
		  (strcmp(k->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : k->finfo.fname));
	  exit(EXIT_FAILURE);
	}
      else
	{
	  if (c > 0)
	    {
	      fprintf(efp, MSG_ERR_E061, k->finfo.io);
	      fprintf(efp, MSG_ERR_FNAMN,
		      (strcmp(k->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : k->finfo.fname));
	      exit(EXIT_FAILURE);
	    }
	}
      last_key = now->key;
    }

} /* add_key_node() */

/*
 * load_keys() -- load key filter into a linked list
 */
void load_keys(FILE *efp, struct s_key_data *k, int match_invert)

{
  char *buf  = (char *) NULL;
  size_t  bsize = (size_t) 0;
  int reset = (int) TRUE;
  FILE *fp;

  if (k->finfo.fname == (char *) NULL)
    return;

  buf  = (char *) NULL;

  if ( ! open_in(&fp, k->finfo.fname, efp) )
    {
      fprintf(efp, MSG_ERR_E010, k->finfo.fname, "r");
      exit(EXIT_FAILURE);
    }

  while (j2_getline(&buf, &bsize, fp) > (ssize_t) -1)
    {
      (k->finfo.io)++;
      add_key_node(efp, k, buf, &reset);
    }

  close_in(&fp, k->finfo.fname);
  if (buf != (char *) NULL)
    free(buf);

  if (k->finfo.io > 0)
    {
      if (match_invert )
	k->filter = FILTER_INVERT;
      else
	k->filter = FILTER_YES;
    }

}  /* load_keys() */

/*
 * show_keys() -- list key_data
 */
void show_keys(FILE *fp, struct s_key_data *k)
{
  struct s_key *now = k->keys;	
  int i;

  fprintf(fp, "%s\n", LIT_DLINE_80);

  if (k->finfo.fname != (char *) NULL)
    {
      fprintf(fp, "%s:\n", LIT_FILTER_F);
      fprintf(fp, "\t%s\n",
	      (strcmp(k->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : k->finfo.fname));
    }
  switch (k->filter)
    {
      case FILTER_NO:
	fprintf(fp, "%s: %s\n", LIT_FILTER_T, LIT_FILTER_N);
	break;
      case FILTER_YES:
	fprintf(fp, "%s: %s\n", LIT_FILTER_T, LIT_FILTER_P);
	break;
      case FILTER_INVERT:
	fprintf(fp, "%s: %s\n", LIT_FILTER_T, LIT_FILTER_D);
	break;
      default:
	fprintf(fp, "%s: %s\n", LIT_FILTER_T, LIT_UNDEFINED);
    }

  while (now != (struct s_key *) NULL)
    {
      fprintf(fp, "    %s\n", now->key);
      now = now->next;
    }

  /*** show filter index ***/
  fprintf(fp, "%s\n", LIT_ULINE_80);

  fprintf(fp, "%s:\n", "Filter Index:");
  for (i = 0; i < KEY_MAX_IDX; i++)
    {
      now = k->kidx[i];
      if (now != (struct s_key *) NULL)
	fprintf(fp, "    %d - %s\n", i, now->key);
    }


  /*** DONE ***/
  fprintf(fp, "%s\n", LIT_DLINE_80);

} /* show_keys() */

/*
 * free_key_data() -- release memory for key data
 */
void free_key_data(struct s_key *k)
{

  if (k == (struct s_key *) NULL)
    return;

  free_key_data(k->next);

  if (k->key != (char *) NULL)
    free(k->key);

  free(k);

} /* free_key_data() */
