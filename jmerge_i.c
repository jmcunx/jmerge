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
 * jmerge_i.c -- Initialization Routines
 */
#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "jmerge.h"

#define SCKARG 80

/*
 * init_w() -- initialize work area
 */
void init_w(struct s_work *w, char *a)

{
  init_finfo(&(w->key));
  init_finfo(&(w->out));
  init_finfo(&(w->err));
  init_finfo(&(w->stats));
  w->err.fp    = stderr;
  w->out.fp    = stdout;
  w->key.abort_on_lock = TRUE;

  w->prog_name        = j2_get_prgname(a, PROG_NAME);
  w->arg_verbose      = 0;
  w->arg_force        = FALSE;
  w->arg_sorted       = TRUE;
  w->arg_heading      = FALSE;
  w->arg_delm         = JLIB2_CHAR_NULL;
  w->arg_pause_reads  = PAUSE_RECS;
  w->arg_milliseconds = 0;

}  /* init_w() */

/*
 * save_fname() -- Check and Save File Name
 */
void save_fname(struct s_file_info *f, char *afname, char *pname, char arg_val)
{
  if (f->fname == (char *) NULL)
    f->fname = strdup(afname);
  else
    {
      fprintf(stderr, MSG_ERR_E074, SWITCH_CHAR, arg_val);
      fprintf(stderr, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

} /* save_fname() */

/*
 * init_get_delm() -- translate a string into a delimiter
 */
char init_get_delm(FILE *fp, char *prog_name, char *s)
{
  int d;
  int c = JLIB2_CHAR_NULL;

  if (s == (char *) NULL)
    return(c);

  if (strlen(s) == 1)
    {
      if ( ((*s) > 47)  && ((*s) < 58) ) /* 0 -- 9 */
	c = (*s) - 48;
      else
	c = (*s);
    }
  else
    {
      if (j2_is_numr(s) == (int) TRUE)
	{
	  d = atoi(s);
	  if ((d < 256) && (d > 0))
	    c = (char) d;
	  else
	    {
	      fprintf(fp, MSG_ERR_E049, s);
	      fprintf(fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  fprintf(fp, MSG_ERR_E049, s);
	  fprintf(fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  return(c);

} /* init_get_delm() */

/*
 * init_get_num() -- verify/convert a positive numeric item
 */
int init_get_num(FILE *fp, char *prog_name, int val, char *s, char arg)
{
  int new = 0;

  if (s == (char *) NULL)
    {
      fprintf(stderr, MSG_ERR_E022, SWITCH_CHAR, arg);
      fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  if (val > 0)
    {
      fprintf(stderr, MSG_ERR_E074, SWITCH_CHAR, arg);
      fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  if (strlen(s) > 1)
    {
      if (((*s) == '-') && (j2_is_numr(&(s[1])) == TRUE) )
	{
	  new = atol(optarg);
	  fprintf(stderr, MSG_ERR_E066, new, SWITCH_CHAR, arg, 0);
	  fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }
  
  if (j2_is_numr(s) == FALSE)
    {
      fprintf(stderr, MSG_ERR_E008, optarg, SWITCH_CHAR, arg);
      fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }
  
  new = atol(optarg);
  
  if (new < 1)
    {
      fprintf(stderr, MSG_ERR_E066, new, SWITCH_CHAR, arg, 0);
      fprintf(stderr, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  return(new);
  
} /* init_get_num() */

/*
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w)

{
  char ckarg[SCKARG];
  int opt = 0;
  int i = 0;
  int num_files = 0;

  memset(ckarg, '\0', SCKARG);
  snprintf(ckarg, (SCKARG - 1), "%c%c%c%c%c%c%c:%c:%c:%c:%c:%c:%c:",
	   ARG_FORCE, ARG_HELP, ARG_VERBOSE, ARG_VERSION, ARG_UNSORTED,
	   ARG_FIRST_HEADG,
	   ARG_INPUT, ARG_ERR,  ARG_OUT, ARG_DELM, ARG_KEY_STATS,
	   ARG_PAUSE, ARG_READS);

  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	case ARG_FORCE:
	  w->arg_force = TRUE;
	  break;
	case ARG_UNSORTED:
	  w->arg_sorted = FALSE;
	  break;
	case ARG_FIRST_HEADG:
	  w->arg_heading = TRUE;
	  break;
	case ARG_DELM:
	  w->arg_delm  = init_get_delm(stderr, w->prog_name, optarg);
	  break;
	case ARG_HELP:
	  show_brief_help(stderr, w->prog_name);
	  break;
	case ARG_VERBOSE:
	  w->arg_verbose++;
	  break;
	case ARG_VERSION:
	  show_rev(stderr, w->prog_name);
	  break;
	case ARG_INPUT:
	  save_fname(&(w->key), optarg, w->prog_name, ARG_INPUT);
	  break;
	case ARG_KEY_STATS:
	  save_fname(&(w->stats), optarg, w->prog_name, ARG_KEY_STATS);
	  break;
	case ARG_ERR:
	  save_fname(&(w->err), optarg, w->prog_name, ARG_ERR);
	  break;
	case ARG_OUT:
	  save_fname(&(w->out), optarg, w->prog_name, ARG_OUT);
	  break;
	case ARG_PAUSE:
	  if (j2_is_numr(optarg) == (int) TRUE)
	    w->arg_milliseconds = atol(optarg);
	  else
	    {
	      fprintf(stderr, MSG_ERR_E008, optarg, SWITCH_CHAR, ARG_PAUSE);
	      exit(EXIT_FAILURE);
	    }
	  break;
	case ARG_READS:
	  if (j2_is_numr(optarg) == (int) TRUE)
	    w->arg_pause_reads = atol(optarg);
	  else
	    {
	      fprintf(stderr, MSG_ERR_E008, optarg, SWITCH_CHAR, ARG_READS);
	      exit(EXIT_FAILURE);
	    }
	  break;
	default:
	  fprintf(stderr, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	  break;
	}
    }

  /*** Count number of files to process */
  for (i = optind; i < argc; i++)
    (num_files)++;
  if (num_files == 0)
    (num_files)++;  /* stdin when no files */

} /* END process_arg() */

/*
 * init() -- initialize
 */
void init(int argc, char **argv, struct s_work *w)

{

  init_w(w, argv[0]);

  process_arg(argc, argv, w);

  /*** validate and process some arguments ***/
  if (w->key.fname == (char *) NULL)
    {
      fprintf(w->err.fp, MSG_ERR_E022, SWITCH_CHAR, ARG_INPUT);
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  if (w->arg_delm == JLIB2_CHAR_NULL)
    w->arg_delm = '|';

  if (open_in(w->err.fp, &(w->key), w->prog_name) == FALSE)
    {
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** open 'out' files ***/
  if ( ! open_out(stderr, &(w->err), w->prog_name, w->arg_force))
    w->err.fp = stderr;
  if ( ! open_out(w->err.fp, &(w->out), w->prog_name, w->arg_force) )
    w->out.fp = stdout;
  if (w->stats.fname != (char *) NULL)
    open_out(w->err.fp, &(w->stats), w->prog_name, w->arg_force);

  /*** load key file data ***/
  if (w->arg_sorted == FALSE)
    {
      /* load key data into linked list */
    }
  
}  /* end: init() */
