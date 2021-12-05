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
#include <errno.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

#define MAXARG 80

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
	  if (d < 256)
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
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w, struct s_file_data *d1, struct s_file_data *d2, struct s_key_data *k)

{
  char ckarg[MAXARG];
  int opt;
  int ok;
  int i;
  char *kf   = (char *) NULL; 
  char *fout = (char *) NULL; 
  char *ferr = (char *) NULL; 
  int display_help = (int) FALSE;
  int display_rev  = (int) FALSE;

  snprintf(ckarg, MAXARG, "%c%c%c%c%c%c%c%c%c%c:%c:%c:%c:%c:", 
		  ARG_FIRST_HEADG, ARG_VERSION, ARG_INVERT,    ARG_HELP, 
		  ARG_DEBUG_MODE,  ARG_FORCE,   ARG_CHANGED,   ARG_QUIET,
		  ARG_MATCH_ONLY,
		  ARG_ERR,         ARG_OUT,     ARG_LIST_MODE, ARG_DELM,
                  ARG_PATERN_FILE);

  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	  case ARG_FORCE:
	    w->force = (int) TRUE;
	    break;
	  case ARG_HELP:
	    display_help = (int) TRUE;
	    break;
	  case ARG_CHANGED:
	    w->show_changed = (int) TRUE;
	    break;
	  case ARG_FIRST_HEADG:
	    w->first_headg = (int) TRUE;
	    break;
	  case ARG_DEBUG_MODE:
	    w->debug = (int) TRUE;
	    break;
	  case ARG_MATCH_ONLY:
	    w->only_match = (int) TRUE;
	    break;
	  case ARG_DELM:
	    w->delim_in  = init_get_delm(w->err.fp, PROG_NAME, optarg);
	    break;
	  case ARG_INVERT:
	    w->invert = (int) TRUE;
	    break;
	  case ARG_QUIET:
	    w->verbose = (int) FALSE;
	    break;
	  case ARG_VERSION:
	    display_rev = (int) TRUE;
	    break;
	  case ARG_ERR:
	    ferr = optarg;
	    break;
	  case ARG_OUT:
	    fout = optarg;
	    break;
	  case ARG_PATERN_FILE:
	    kf = optarg;
	    break;
	  case ARG_LIST_MODE:
	    if ( ! j2_is_numr(optarg) )
	      {
		fprintf(w->err.fp, MSG_ERR_E008, optarg, SWITCH_CHAR, ARG_LIST_MODE);
		fprintf(w->err.fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
		exit(EXIT_FAILURE);
	      }
	    w->list_mode = atoi(optarg);
	    break;
	  default:
	    fprintf(w->err.fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
	    exit(EXIT_FAILURE);
	    break;
	}
    }

  /* adjust file names for display */
  if (fout == (char *) NULL)
    w->out.fname = strdup(FILE_NAME_STDOUT);
  else
    w->out.fname = strdup(fout);

  if (kf != (char *) NULL)
    k->finfo.fname = strdup(kf);

  /*** if necessary - save stdout/err files and open ***/
  ok = open_out(stderr, &(w->err), ferr, w->force);
  if (ok == EXIT_SUCCESS)
    ok = open_out(w->err.fp, &(w->out), fout, w->force);
  if (ok != EXIT_SUCCESS)
    {
      fprintf(w->err.fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** show help/rev and exit ? ***/
  ok = EXIT_SUCCESS;
  if (display_help)
    ok = show_brief_help(w->err.fp, PROG_NAME);
  if (display_rev)
    ok = show_rev(w->out.fp, PROG_NAME);
  if (ok != EXIT_SUCCESS)
    {
      close_out(&(w->out));
      close_out(&(w->err));
      exit(EXIT_FAILURE);
    }

  /*** Validate files to be processed, we expect ***/
  /*   2 files and they cannot be the same file    */
  for (i = optind; i < argc; i++)
    {
      (w->num_files)++;
      if (d1->finfo.fname == (char *) NULL)
	d1->finfo.fname = strdup(argv[i]);
      else
	{
	  if (d2->finfo.fname == (char *) NULL)
	    d2->finfo.fname = strdup(argv[i]);
	}
    }
  if (w->num_files != 2)
    {
      fprintf(w->err.fp, MSG_ERR_E058, 2, w->num_files);
      ok = EXIT_FAILURE;
    }
  if ((d1->finfo.fname != (char *) NULL) &&
      (d2->finfo.fname != (char *) NULL) )
    {
      if (strcmp(d1->finfo.fname, d2->finfo.fname) == 0)
	{
	  fprintf(w->err.fp, MSG_ERR_E059);
	  fprintf(w->err.fp, MSG_ERR_FNAME, 1, 
		 (strcmp(d1->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d1->finfo.fname));
	  fprintf(w->err.fp, MSG_ERR_FNAME, 2,
		 (strcmp(d2->finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d2->finfo.fname));
	  ok = EXIT_FAILURE;
	}
    }
 
  /*** check value of list mode ***/
  switch (w->list_mode)
    {
      case LIST_MODE_1:
      case LIST_MODE_2:
	break;
      default:
	fprintf(w->err.fp, MSG_ERR_E004R, w->list_mode, SWITCH_CHAR, ARG_LIST_MODE);
	ok = EXIT_FAILURE;
    }

  if (ok != EXIT_SUCCESS)
    {
      fprintf(w->err.fp, MSG_ERR_E000, PROG_NAME, SWITCH_CHAR, ARG_HELP);
      close_out(&(w->out));
      close_out(&(w->err));
      exit(EXIT_FAILURE);
    }

} /* process_arg() */
