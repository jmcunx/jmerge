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
 * jmerge_h.c -- Show help/rev routines
 */
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jmerge.h"

#define MSG_HELP_11  "Merge files based upon a Key File"

/*
 * show_brief_help()
 */
void show_brief_help(FILE *fp, char *pname)

{

  fprintf(fp, USG_MSG_USAGE, pname);
  fprintf(fp, "\t%s\n", MSG_HELP_11);
  fprintf(fp, USG_MSG_OPTIONS);
  fprintf(fp, USG_MSG_ARG_DELM,        SWITCH_CHAR, ARG_DELM);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_DELM_B);
  fprintf(fp, USG_MSG_ARG_ERR,         SWITCH_CHAR, ARG_ERR);
  fprintf(fp, USG_MSG_ARG_FIRST_HEADG, SWITCH_CHAR, ARG_FIRST_HEADG);
  fprintf(fp, USG_MSG_ARG_FORCE,       SWITCH_CHAR, ARG_FORCE);
  fprintf(fp, USG_MSG_ARG_HELP,        SWITCH_CHAR, ARG_HELP);
  fprintf(fp, USG_MSG_ARG_INPUT_3,     SWITCH_CHAR, ARG_INPUT);
  fprintf(fp, USG_MSG_ARG_KEY_STATS,   SWITCH_CHAR, ARG_KEY_STATS);
  fprintf(fp, USG_MSG_ARG_OUT,         SWITCH_CHAR, ARG_OUT);
  fprintf(fp, USG_MSG_ARG_READS,       SWITCH_CHAR, ARG_READS);
  fprintf(fp, USG_MSG_ARG_PAUSE_1,     SWITCH_CHAR, ARG_PAUSE, PAUSE_RECS);
  fprintf(fp, USG_MSG_ARG_UNSORTED,    SWITCH_CHAR, ARG_UNSORTED);
  fprintf(fp, USG_MSG_ARG_VERSION,     SWITCH_CHAR, ARG_VERSION);
  fprintf(fp, USG_MSG_ARG_VERBOSE_8,   SWITCH_CHAR, ARG_VERBOSE);

  exit(EXIT_FAILURE);

}  /* show_brief_help() */

/*
 * show_rev()
 */
void show_rev(FILE *fp, char *pname)

{

  fprintf(fp,"%s %s:\n", pname, LIT_REV);

#ifdef J_LIB2_H
  fprintf(fp, "\t%s %s\n", LIT_INFO_02, j2_get_build());
#endif

#ifdef OSTYPE
  fprintf(fp,"\t%s\n",OSTYPE);
#endif  /* OSTYPE  */
#ifdef PRETTY_NAME
  fprintf(fp,"\t%s\n",PRETTY_NAME);
#endif  /* OSTYPE  */
  fprintf(fp, LIT_INFO_01, __DATE__, __TIME__);

  exit(EXIT_FAILURE);

}  /* show_rev() */
