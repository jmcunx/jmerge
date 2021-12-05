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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

/*
 * is_different() -- determines if field data is different
 */
int is_different(struct s_file_recs *r1, struct s_file_recs *r2)
{

  /*** if order is important ***/

  if ((r1 == (struct s_file_recs *) NULL) &&
      (r2 == (struct s_file_recs *) NULL) )
    return((int) FALSE);  /* data agrees */

  if (r1 == (struct s_file_recs *) NULL)
    return((int) TRUE);   /* data different */
  if (r2 == (struct s_file_recs *) NULL)
    return((int) TRUE);   /* data different */

  if ((r1->other_fields == (char *) NULL) &&
      (r2->other_fields == (char *) NULL) )
    return((int) FALSE);  /* data agrees */

  if (r1->other_fields == (char *) NULL)
    return((int) TRUE);   /* data different */
  if (r2->other_fields == (char *) NULL)
    return((int) TRUE);   /* data different */

  if (strcmp(r1->other_fields, r2->other_fields) == 0)
    return((int) FALSE);  /* data agrees */
  else
    return((int) TRUE);   /* data different */

} /* is_different() */

/*
 * compare_d() -- compare data
 */
void compare_d(struct s_file_data *d1, struct s_file_data *d2, int headg)
{
  struct s_file_recs *d1_now = d1->r; /* expect a unique key */
  struct s_file_recs *d2_now = d2->r; /* duplicates allowed  */
  int c;

  if ((d1_now == (struct s_file_recs *) NULL) ||
      (d2_now == (struct s_file_recs *) NULL) )
    return;

  /*** process heading ***/
  if ( headg )
    {
      d1_now->match = d2_now;
      d2_now->match = d1_now;
      d1_now = d1->r->next;
      d2_now = d2->r->next;
    }

  /*** normal match processing ***/
  while ((d1_now != (struct s_file_recs *) NULL) &&
         (d2_now != (struct s_file_recs *) NULL) )
    {
      c = strcmp(d1_now->key, d2_now->key);
      if (c == 0)
	{
	  if (d1_now->match == (struct s_file_recs *) NULL)
	    {
	      (d1_now->match_count)++;
	      d1_now->match = d2_now;
	    }
	  (d2_now->match_count)++;
	  d2_now->match = d1_now;
	  d2_now = d2_now->next;
	}
      else
	{
	  if (c < 0)
	    d1_now = d1_now->next;
	  else
	    d2_now = d2_now->next;
	}
    }

} /* compare_data() */

/*
 * list_in_raw() -- list all data in raw mode, good for debugging
 */
void list_in_raw(FILE *fp, int chg, struct s_file_recs *r, char *dr, char *m)

{
  struct s_file_recs *now = r;

  fprintf(fp, "%s\n", LIT_DLINE_80);
  fprintf(fp, MSG_INFO_I011, (strcmp(dr,FILE_NAME_STDIN) == 0 ? LIT_STDIN : dr));
  fprintf(fp, MSG_INFO_I012, (strcmp(m,FILE_NAME_STDIN) == 0 ? LIT_STDIN : m));
  fprintf(fp, "%s\n", LIT_ULINE_80);

  while (now != (struct s_file_recs *) NULL)
    {
      fprintf(fp, "%s: :%s: %s\n", LIT_MAIN, now->key, now->other_fields);
      if (now->match != (struct s_file_recs *) NULL)
	fprintf(fp, "      :%s: %s\n", now->match->key, now->match->other_fields);
      else
	fprintf(fp, "      :%s\n", LIT_NO_MATCH);
      if ( chg )
	{
	  if ( is_different(now, now->match) )
	    fprintf(fp, "      :%s\n", LIT_DIFFERENT);
	  else
	    fprintf(fp, "      :%s\n", LIT_SAME);
	}
      fprintf(fp, "\n");
      now = now->next;
    }

  fprintf(fp, "%s\n", LIT_DLINE_80);

} /* list_in_raw() */

/*
 * list_match() -- list matched data
 *                 returns # of records written
 */
int list_match(FILE *fp, int chg, int only_match, 
               struct s_file_recs *r, char delm, int max)

{
  struct s_file_recs *now = r;
  int i;
  int writes = 0;
  int show_rec = (int) TRUE;

  while (now != (struct s_file_recs *) NULL)
    {
      if ( (only_match) && (now->match == (struct s_file_recs *) NULL))
	{
	  now = now->next;
	  continue;
	}
      if ( chg ) /* show only differences ? */
	show_rec = is_different(now, now->match);
      if ( show_rec )
	{
	  fprintf(fp, "%s%c%s", now->key, delm, now->other_fields);
	  if (now->match != (struct s_file_recs *) NULL)
	    fprintf(fp, "%s", now->match->other_fields);
	  else
	    {
	      for (i=1; i < max; i++)
		fprintf(fp,"%c", delm);
	    }
	  fprintf(fp,"\n");
	  writes++;
	}
      now = now->next;
    }

  return(writes);

} /* list_match() */
