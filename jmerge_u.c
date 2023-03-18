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
 * jmerge_u.c -- common routines
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <ctype.h>

#include "jmerge.h"

/*
 * init_finfo() -- initialize out file structure
 */
void init_finfo(struct s_file_info *f)

{

  f->fp    = (FILE *) NULL;
  f->fname = (char *) NULL;
  f->lock_succeeded = FALSE;
  f->abort_on_lock  = FALSE;
  f->lines_read     = 0L;
  f->lines_writes   = 0L;
  f->count_match    = 0L;

} /* init_finfo() */

/*
 * open_in() -- open in file and lock the file
 */
int open_in(FILE *wfp, struct s_file_info *f, char *prog_name)

{

  f->lock_succeeded = FALSE;

  if (f->fname == (char *) NULL)
    {
      f->fp = stdin;
      return(TRUE);
    }
  if (strcmp(f->fname, FILE_NAME_STDIN) == 0)
    {
      f->fp = stdin;
      return(TRUE);
    }

  f->fp = fopen(f->fname, "r");

  if (f->fp == (FILE *) NULL)
    {
      fprintf(wfp, MSG_WARN_W002, f->fname, strerror(errno));
      return(FALSE);
    }

  if (flock(fileno(f->fp), LOCK_EX | LOCK_NB) == -1)
    {
      if (f->abort_on_lock == TRUE)
	{
	  fprintf(wfp, MSG_ERR_E091, f->fname, strerror(errno));
	  fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  fclose(f->fp);
	  close_in(f);
	  exit(EXIT_FAILURE);
	}
      else
	fprintf(wfp, MSG_WARN_W031, f->fname, strerror(errno));
      close_in(f);
      return(FALSE);
    }
  else
    f->lock_succeeded = TRUE;

  return(TRUE);

} /* open_in() */

/*
 * open_out() -- save the file anem and check status
 */
int open_out(FILE *wfp, struct s_file_info *f, char *prog_name, int force)

{

  if (f->fname == (char *) NULL)
    return((int) FALSE);
  if (strcmp(f->fname, FILE_NAME_STDOUT) == 0)
    return((int) FALSE);

  if (force == (int) FALSE)
    {
      if ( j2_f_exist(f->fname) )
	{
	  fprintf(wfp, MSG_ERR_E025, f->fname);
	  fprintf(wfp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  f->fp = fopen(f->fname, "w");
  if (f->fp == (FILE *) NULL)
    {
      fprintf(wfp, MSG_ERR_E002, f->fname);
      fprintf(wfp, "\t%s\n", strerror(errno));
      fprintf(wfp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  return(TRUE);

} /* open_out() */

/*
 * close_in() -- Close an input file
 */
void close_in(struct s_file_info *f)

{

  if (f->fname == (char *) NULL)
    return;
  if (strcmp(f->fname, FILE_NAME_STDIN) == 0)
    return;

  if (f->fp != (FILE *) NULL)
    {
      if (f->lock_succeeded == TRUE)
	flock(fileno(f->fp), LOCK_UN);
      fclose(f->fp);
      free(f->fname);
      f->fp = stdin;
    }

  init_finfo(f);

} /* close_in() */

/*
 * close_out() -- close output
 */
void close_out(struct s_file_info *f)

{
  if (f->fname == (char *) NULL)
    return;
  if (strcmp(f->fname, FILE_NAME_STDOUT) == 0)
    return;

  fclose(f->fp);
  free(f->fname);
  f->fname = (char *) NULL;
  init_finfo(f);
  
} /* close_out() */

/*
 * buf_match_mem() -- allocate memory for match key
 */
void buf_match_mem(FILE *efp, struct s_buffer *b)
{
  
  if (b->ksiz > b->bsiz)
    return;

  b->ksiz = b->bsiz + 20;

  if (b->match_key != (char *) NULL)
    {
#ifdef OpenBSD
    freezero(b->match_key, b->ksiz);
#else
    free(b->match_key);
#endif
    }

  b->match_key = (char *) calloc(b->ksiz, sizeof(char));
  if (b->match_key == (char *) NULL)
    {
      fprintf(efp, MSG_ERR_E080, strerror(errno));
      exit(EXIT_FAILURE);
    }
  b->last_match_key = (char *) calloc(b->ksiz, sizeof(char));
  if (b->last_match_key == (char *) NULL)
    {
      fprintf(efp, MSG_ERR_E080, strerror(errno));
      exit(EXIT_FAILURE);
    }

} /* buf_match_mem() */

/*
 * init_buf() -- initialize read buffer and allocate initial size
 */
void init_buf(FILE *efp, struct s_buffer *b, char arg_delm)
{
  b->buf = (char *) NULL;
  b->match_key = (char *) NULL;
  b->bsiz = (size_t) 256;
  b->ksiz = (size_t) 0;
  b->stat_read = (ssize_t) 0;
  b->delm = arg_delm;

  b->buf = (char *) calloc(b->bsiz, sizeof(char));
  if (b->buf == (char *) NULL)
    {
      fprintf(efp, MSG_ERR_E080, strerror(errno));
      exit(EXIT_FAILURE);
    }
  buf_match_mem(efp, b);

} /* init_buf() */

/*
 * read_buf() -- read a record
 */
int read_buf(FILE *efp, struct s_buffer *b, struct s_file_info *f)
{
  char *s = (char *) NULL;

  if (b->stat_read < (ssize_t) 0L)
    return(FALSE);

  b->stat_read = getline(&(b->buf), &(b->bsiz), f->fp);

  if (b->stat_read < (ssize_t) 0)
    return(FALSE);

  f->lines_read++;
  j2_rtw(b->buf);

  /*** load key value ***/
  buf_match_mem(efp, b);

  if (strncmp(b->last_match_key, b->match_key, b->bsiz) != 0)
    strncpy(b->last_match_key, b->match_key, b->bsiz);

  memset(b->match_key, '\0', b->ksiz);
  strncpy(b->match_key, b->buf, b->bsiz);

  s = strchr(b->match_key, b->delm);
  if (s != (char *) NULL)
    (*s) = JLIB2_CHAR_NULL;

  return(TRUE);

} /* read_buf() */

/*
 * free_buf() -- free memory for the buffer
 */
void free_buf(struct s_buffer *b)
{
  if (b->buf != (char *) NULL)
    {
#ifdef OpenBSD
    freezero(b->buf, b->bsiz);
#else
    free(b->buf);
#endif
    }
  if (b->match_key != (char *) NULL)
    {
#ifdef OpenBSD
    freezero(b->match_key, b->ksiz);
#else
    free(b->match_key);
#endif
    }
  if (b->last_match_key != (char *) NULL)
    {
#ifdef OpenBSD
    freezero(b->last_match_key, b->ksiz);
#else
    free(b->last_match_key);
#endif
    }

  b->buf = (char *) NULL;
  b->match_key = (char *) NULL;
  b->bsiz = (size_t) 0;
  b->ksiz = (size_t) 0;
  b->stat_read = (ssize_t) 0L;
  b->delm = JLIB2_CHAR_NULL;

} /* free_buf() */
