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
/*
 * jmerge_d.c -- Process data that is not sorted
 *               This can be very slow
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "jmerge.h"

/*
 * read_a_line() -- get a lin
 */
ssize_t read_a_line(struct s_file_info *inf, struct s_buffer *buf_in)
{
  ssize_t r = (ssize_t) 0;

  r = getline(&(buf_in->buf), &(buf_in->bsiz), inf->fp);

  if (r > -1)
    {
      inf->lines_read++;
      j2_rtw(buf_in->buf);
    }
  else
    memset(buf_in->buf, 0, buf_in->bsiz);

  return(r);

} /* read_a_line() */

/*
 * search_randomf() -- process a single non-sorted file
 */
int search_randomf(struct s_work *w,
                   struct s_buffer *buf_in,
                   struct s_buffer *buf_key,
                   struct s_key_data *raw)
{
  struct s_key_data *found = NULL_KEY_DATA;
  char *dchar = (char *) NULL;;

  /*** reallocate match key if needed ***/
  if (buf_in->ksiz <= buf_in->bsiz)
    {
      if (buf_in->match_key != (char *) NULL)
	free(buf_in->match_key);
      buf_in->ksiz = buf_in->bsiz + 20;
      buf_in->match_key = (char *) calloc(buf_in->ksiz, sizeof(char));
      if (buf_in->buf == (char *) NULL)
	{
	  fprintf(w->err.fp, MSG_ERR_E080, strerror(errno));
	  exit(EXIT_FAILURE);
	}
    }

  /*** set key value ***/
  strncpy(buf_in->match_key, buf_in->buf, buf_in->bsiz);
  dchar = strchr(buf_in->match_key, w->arg_delm);
  if (dchar != (char *) NULL)
    (*dchar) = JLIB2_CHAR_NULL;

  found = key_search(buf_in->match_key, raw);

  if (found == NULL_KEY_DATA)
    {
      w->out.lines_writes++;
      fprintf(w->out.fp, "NO MATCH: %s\n", buf_in->buf);
      return(FALSE);
    }
 
  if (fseek(w->key.fp, found->pos, SEEK_SET) == -1)
    {
      fprintf(w->err.fp, MSG_ERR_E092, found->pos,
              strerror(errno), w->key.fname);
      exit(EXIT_FAILURE);
    }
  if (getline(&(buf_key->buf), &(buf_key->bsiz), w->key.fp) < 0)
    {
      fprintf(w->err.fp, MSG_ERR_E093, found->pos,
              strerror(errno), w->key.fname);
      exit(EXIT_FAILURE);
    }
  j2_rtw(buf_key->buf);
  w->key.lines_read++;
  w->key.count_match++;
  w->out.lines_writes++;
  fprintf(w->out.fp, "MATCHED:  %s%c%s\n", buf_key->buf, w->arg_delm, buf_in->buf);
  return(TRUE);

} /* search_random() */

/*
 * process_a_randomf() -- process a single non-sorted file
 */
long int process_a_randomf(struct s_work *w, char *fname,
                           struct s_buffer *buf_in,
                           struct s_buffer *buf_key,
                           struct s_key_data *raw)

{

  struct s_file_info inf;
  ssize_t cbytes = (ssize_t) 0;
  long int reads = 0L;
  
  if (w->arg_verbose > 0)
    show_file_heading(&(w->out), fname);
 
  init_finfo(&inf);
  if (fname != (char *) NULL)
    {
      if (strcmp(fname, FILE_NAME_STDIN) != 0)
	inf.fname = strdup(fname);
    }
  if ( ! open_in(w->err.fp, &inf, w->prog_name) )
    return(0L);

  key_clear_counts(raw);

  /*** get first record ***/
  if (w->arg_heading == TRUE)
    cbytes = read_a_line(&inf, buf_in);

  if (cbytes > -1)
    {
      while (read_a_line(&inf, buf_in) > (ssize_t) -1)
	{
	  search_randomf(w, buf_in, buf_key, raw);
	  if (w->arg_pause_reads > 0)
	    {
	      if ((inf.lines_read % w->arg_pause_reads) == 0)
		j2_sleepm(w->arg_milliseconds);
	    }
	}
    }
  
  /*** complete ***/  
  if ((w->arg_verbose > 0) && (w->stats.fname != (char *) NULL))
    {
      show_file_heading(&(w->stats), fname);
      w->stats.lines_writes += key_print(w->stats.fp, raw);
    }
  reads=inf.lines_read;
  close_in(&(inf));
  return(reads);

} /* process_a_randomf() */

/*
 * process_random() -- Process data that is not sorted
 */
void process_random(int argc, char **argv, struct s_work *w)
{
  int i;
  long int total_reads = 0L;
  struct s_buffer buf_in;
  struct s_buffer buf_key;
  struct s_key_data *raw = NULL_KEY_DATA;

  init_buf(w->err.fp, &buf_in,   w->arg_delm);
  init_buf(w->err.fp, &buf_key,  w->arg_delm);

  raw = key_load(w->err.fp, &(w->key), w->arg_delm, w->arg_verbose);

  for (i = optind; i < argc; i++)
    total_reads += process_a_randomf(w, argv[i], &buf_in, &buf_key, raw);
  if (i == optind)
    total_reads += process_a_randomf(w, FILE_NAME_STDIN, &buf_in, &buf_key, raw);

  /* DONE */
  key_free(&raw);

  if (w->arg_verbose > 1)
    {
      fprintf(w->err.fp, MSG_INFO_I072, w->key.lines_read,
              (w->key.fname == (char *) NULL ? LIT_STDOUT : w->key.fname));
      fprintf(w->err.fp, MSG_INFO_I072, total_reads, "All Input Files");
      fprintf(w->err.fp, MSG_INFO_I080, w->out.lines_writes,
              (w->out.fname == (char *) NULL ? LIT_STDOUT : w->out.fname));
      if (w->stats.fname != (char *) NULL)
	fprintf(w->err.fp, MSG_INFO_I080, w->stats.lines_writes,w->stats.fname);
    }
  
  free_buf(&buf_in);
  free_buf(&buf_key);

}  /* process_random() */

