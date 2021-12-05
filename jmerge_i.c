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

char *jmerge_i_c="$Id: jmerge_i.c,v 2.3 2021/02/21 20:37:58 jmccue Exp $";

/*
 * init_d() -- initialize link list
 */
void init_d(struct s_file_data *d, int file_num, int need_uniq)
{

    d->max_fields  = 0;
    d->file_num    = file_num;
    d->need_uniq   = need_uniq;
    d->r           = (struct s_file_recs *) NULL;

    init_finfo(&(d->finfo));

} /* init_d() */

/*
 * init_k() -- initialize key list
 */
void init_k(struct s_key_data *k)

{
  int i;

    k->filter   = FILTER_NO;
    k->keys     = (struct s_key *) NULL;

  for (i = 0; i < KEY_MAX_IDX; i++)
    {
      k->kidx[i] = (struct s_key *) NULL;
    }

    init_finfo(&(k->finfo));

} /* init_k() */

/*
 * init_w() -- initialize work area
 */
void init_w(struct s_work *w, char *a)

{
  init_finfo(&(w->out));
  init_finfo(&(w->err));
  w->err.fp    = stderr;
  w->out.fp    = stdout;

  w->num_files        = 0;
  w->invert           = (int) FALSE;
  w->verbose          = (int) TRUE;
  w->force            = (int) FALSE;
  w->debug            = (int) FALSE;
  w->first_headg      = (int) FALSE;
  w->show_changed     = (int) FALSE;
  w->only_match       = (int) FALSE;
  w->delim_in         = '|';
  w->list_mode        = 1;

}  /* init_w() */

/*
 * init() -- initialize
 */
void init(int argc, char **argv, 
         struct s_work *w, 
         struct s_file_data *d1, 
         struct s_file_data *d2)

{
  struct s_key_data k;

  init_w(w, argv[0]);
  init_d(d1, 1, (int) TRUE);
  init_d(d2, 2, (int) FALSE);
  init_k(&k);

  process_arg(argc, argv, w, d1, d2, &k);

  /*** load data ***/

  load_keys(w->err.fp, &k, w->invert);
  load_link(w->err.fp, d1, &k, w->delim_in, w->first_headg);
  load_link(w->err.fp, d2, &k, w->delim_in, w->first_headg);

  if ((w->verbose) && (k.finfo.fname != (char *) NULL))
    fprintf(w->err.fp, MSG_INFO_I003, k.finfo.io,  0L, 
	    (strcmp(k.finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : k.finfo.fname));
  if (w->debug)
    show_keys(w->err.fp, &k);

  /*** release memory no longer needed ***/
  free_key_data(k.keys);
  if ( k.finfo.fname != (char *) NULL)
    free(k.finfo.fname);

}  /* init() */

/* END: jmerge_i.c */
