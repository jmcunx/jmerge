/*
 * Copyright (c) 2023 2024
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
 * jmerge_s.c -- Process Sorted Files
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifdef OpenBSD
#include <err.h>
#endif
#ifdef __FreeBSD_version
#include <err.h>
#endif
#ifdef __NetBSD_Version__
#include <err.h>
#endif

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jmerge.h"

/*
 * process_a_next_key()
 */
int process_sort_rec(struct s_work *w, 
		     struct s_buffer *buf_in, 
		     struct s_file_info *inf,
		     struct s_buffer *buf_key,
		     struct s_file_info *keyf,
		     struct s_file_info *statf)
{

  /* get next in record */
  if (read_buf(w->err.fp, buf_in, inf) == FALSE)
    return(-1); /* no more data */

  if (strncmp(buf_in->last_match_key, buf_in->match_key, buf_in->bsiz) > 0)
    {
      fprintf(w->err.fp, "ERROR: Keys %s -- %s\n", buf_in->last_match_key, buf_key->match_key);
      fprintf(w->err.fp, "ERROR: Seq error on Rec %ld from %s\n", inf->lines_read, inf->fname);
      exit(EXIT_FAILURE);
    }

  /* get next key rec (if needed) */
  while (strcmp(buf_in->match_key, buf_key->match_key) != 0)
    {
      if (strcmp(buf_in->match_key, buf_key->match_key) < 0)
	  return(0);
      if ((statf->fname != (char *) NULL) && (strlen(buf_key->match_key) > 0))
	{
	  fprintf(statf->fp, "%11ld %s\n", keyf->count_match, buf_key->match_key);
	  statf->lines_writes++;
	}
      if (read_buf(w->err.fp, buf_key, keyf) == FALSE)
	break;
      keyf->count_match = 0;
      if (strncmp(buf_key->last_match_key, buf_key->match_key, buf_in->bsiz) > 0)
	{
	  fprintf(w->err.fp, "ERROR: Keys %s -- %s\n", buf_key->last_match_key, buf_key->match_key);
	  fprintf(w->err.fp, "ERROR: Seq error on Rec %ld from %s\n", keyf->lines_read, keyf->fname);
	  exit(EXIT_FAILURE);
	}
    }

  /* Returns: 1 for match found, 0 for no match */ 
  if (strcmp(buf_in->match_key, buf_key->match_key) == 0)
    {
      inf->count_match++;
      keyf->count_match++;
      return(1);
    }
  else
    return(0);

} /* process_sort_rec() */

/*
 * process_a_sort_file()
 */
long int process_a_sort_file(struct s_work *w, char *fname,
                             struct s_buffer *buf_in,
                             struct s_buffer *buf_key)

{

  long int lines_matched  = 0L;
  long int lines_no_match = 0L;
  long int r              = 0L;
  struct s_file_info inf;
  char *sdlm = (char *) NULL;
  
  if (w->arg_verbose > 0)
    {
      show_file_heading(&(w->out), fname);
      if (w->stats.fname != (char *) NULL)
	show_file_heading(&(w->stats), fname);
    }
 
  init_finfo(&inf);
  if (fname != (char *) NULL)
    {
      if (strcmp(fname, FILE_NAME_STDIN) != 0)
	inf.fname = strdup(fname);
    }
  if ( ! open_in(w->err.fp, &inf, w->prog_name) )
    return(0L);

  rewind(w->key.fp);
  w->key.count_match = 0;

  /*** get first record ***/
  if (w->arg_heading == TRUE)
    {
      read_buf(w->err.fp, buf_in, &inf);
      if (w->arg_pause_reads > 0)
	{
	  if ((inf.lines_read % w->arg_pause_reads) == 0)
	    j2_sleepm(w->arg_milliseconds);
	}
      read_buf(w->err.fp, buf_key, &(w->key));
    }

  /*** process data ***/
  while (buf_in->stat_read > (ssize_t) -1)
    {
      switch (process_sort_rec(w, buf_in, &inf, buf_key, &(w->key), &(w->stats)))
	{
	  case 0:
	    lines_no_match++;
	    w->out.lines_writes++;
	    fprintf(w->out.fp, "NO MATCH: %s\n", buf_in->buf);
	    break;
	  case 1:
	    lines_matched++;
	    w->out.lines_writes++;
	    fprintf(w->out.fp, "MATCHED:  %s%c%s\n", buf_key->buf, w->arg_delm, buf_in->buf);
	    break;
	  default:
	    /* no more data */
	    break;
	}
      if (w->arg_milliseconds > 0L)
	{
	  r = inf.lines_read % w->arg_pause_reads;
	  if (r == 0)
	    j2_sleepm(w->arg_milliseconds);
	}
    }

  /* show remaining stats */
  if (w->stats.fname != (char *) NULL)
    {
      fprintf(w->stats.fp, "%11ld %s\n", w->key.count_match, buf_key->match_key);
      w->stats.lines_writes++;
      while (j2_getline(&(buf_key->buf), &(buf_key->bsiz), w->key.fp) > -1)
	{
	  sdlm = strchr(buf_key->buf, w->arg_delm);
	  if (sdlm != (char *) NULL)
	    (*sdlm) = JLIB2_CHAR_NULL;
	  fprintf(w->stats.fp, "%11d %s\n", 0, buf_key->buf);
	  w->stats.lines_writes++;
	}
    }

  if (w->arg_verbose > 1)
    {
      fprintf(w->err.fp, MSG_INFO_I109, lines_no_match,
	      (inf.fname == (char *) NULL ? LIT_STDIN : inf.fname));
      fprintf(w->err.fp, MSG_INFO_I108, lines_matched,
	      (inf.fname == (char *) NULL ? LIT_STDIN : inf.fname));
      fprintf(w->err.fp, MSG_INFO_I072, inf.lines_read,
	      (inf.fname == (char *) NULL ? LIT_STDIN : inf.fname));
    }
  
  /*** complete ***/  
  r = inf.lines_read;
  close_in(&(inf));
  return(r);

} /* process_a_sort_file() */

/*
 * process_sorted() -- Process all sorted files
 */
void process_sorted(int argc, char **argv, struct s_work *w)

{
  int i;
  long int total_reads = 0L;
  struct s_buffer buf_in;
  struct s_buffer buf_key;

  init_buf(w->err.fp, &buf_in,  w->arg_delm);
  init_buf(w->err.fp, &buf_key, w->arg_delm);

  /* process files */
  for (i = optind; i < argc; i++)
    total_reads += process_a_sort_file(w, argv[i], &buf_in, &buf_key);

  if (i == optind)
    total_reads += process_a_sort_file(w, FILE_NAME_STDIN, &buf_in, &buf_key);

  if (w->arg_verbose > 1)
    {
      fprintf(w->err.fp, MSG_INFO_I072, total_reads, "All Input Files");
      fprintf(w->err.fp, MSG_INFO_I072, w->key.lines_read,
              (w->out.fname == (char *) NULL ? LIT_STDOUT : w->key.fname));
      fprintf(w->err.fp, MSG_INFO_I080, w->out.lines_writes,
              (w->out.fname == (char *) NULL ? LIT_STDOUT : w->out.fname));
      if (w->stats.fname != (char *) NULL)
	fprintf(w->err.fp, MSG_INFO_I080, w->stats.lines_writes,w->stats.fname);
    }
  
  free_buf(&buf_in);
  free_buf(&buf_key);

}  /* process_sorted() */
