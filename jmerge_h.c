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
#include <ctype.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

char *jmerge_h_c="$Id: jmerge_h.c,v 2.4 2021/02/21 20:37:58 jmccue Exp $";

extern char *jmerge_c;
extern char *jmerge_a_c;
extern char *jmerge_d_c;
extern char *jmerge_h_c;
extern char *jmerge_i_c;
extern char *jmerge_k_c;
extern char *jmerge_u_c;
extern char *jmerge_r_c;

/*** Messages ***/
#define MSG_HELP_11  "Record Merge Utility"

/*
 * show_brief_help()
 */
int show_brief_help(FILE *fp, char *pname)

{

  fprintf(fp, USG_MSG_USAGE_2, pname);
  fprintf(fp, "\t%s\n", MSG_HELP_11);
  fprintf(fp, USG_MSG_OPTIONS);
  fprintf(fp, USG_MSG_ARG_CHANGED,     SWITCH_CHAR, ARG_CHANGED);
  fprintf(fp, USG_MSG_ARG_DELM,        SWITCH_CHAR, ARG_DELM);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_DELM_B);
  fprintf(fp, USG_MSG_ARG_ERR,         SWITCH_CHAR, ARG_ERR);
  fprintf(fp, USG_MSG_ARG_FIRST_HEADG, SWITCH_CHAR, ARG_FIRST_HEADG);
  fprintf(fp, USG_MSG_ARG_FORCE,       SWITCH_CHAR, ARG_FORCE);

  fprintf(fp, USG_MSG_ARG_HELP,        SWITCH_CHAR, ARG_HELP);
  fprintf(fp, USG_MSG_ARG_LIST_MODE,   SWITCH_CHAR, ARG_LIST_MODE);
  fprintf(fp, USG_MSG_ARG_MATCH_ONLY,  SWITCH_CHAR, ARG_MATCH_ONLY);
  fprintf(fp, USG_MSG_ARG_OUT,         SWITCH_CHAR, ARG_OUT);
  fprintf(fp, USG_MSG_ARG_PATERN_FIL1, SWITCH_CHAR, ARG_PATERN_FILE);
  fprintf(fp, USG_MSG_ARG_QUIET_1,     SWITCH_CHAR, ARG_QUIET);
  fprintf(fp, USG_MSG_ARG_VERSION,     SWITCH_CHAR, ARG_VERSION);
  fprintf(fp, USG_MSG_ARG_INVERT,      SWITCH_CHAR, ARG_INVERT);
  fprintf(fp, USG_MSG_ARG_DEBUG_MODE,  SWITCH_CHAR, ARG_DEBUG_MODE);

  return(EXIT_FAILURE);

}  /* show_brief_help() */

/*
 * show_rev()
 */
int show_rev(FILE *fp, char *pname)

{

  fprintf(fp,"%s %s:\n", pname, LIT_REV);
  fprintf(fp,"\t%s\n", JMERGE_H);
  fprintf(fp,"\t%s\n", jmerge_c);
  fprintf(fp,"\t%s\n", jmerge_a_c);
  fprintf(fp,"\t%s\n", jmerge_d_c);
  fprintf(fp,"\t%s\n", jmerge_h_c);
  fprintf(fp,"\t%s\n", jmerge_i_c);
  fprintf(fp,"\t%s\n", jmerge_k_c);
  fprintf(fp,"\t%s\n", jmerge_r_c);
  fprintf(fp,"\t%s\n", jmerge_u_c);

#ifdef J_LIB2M_H
  fprintf(fp, "\t%s\n", J_LIB2M_H);
#endif
#ifdef J_LIB2_H
  fprintf(fp, "\t%s\n", J_LIB2_H);
  fprintf(fp,"\t     %s %s\n", LIT_INFO_02, j2_get_build());
#endif

#ifdef OSTYPE
  fprintf(fp,"\t%s\n",OSTYPE);
#endif  /* OSTYPE  */
#ifdef PRETTY_NAME
  fprintf(fp,"\t%s\n",PRETTY_NAME);
#endif  /* OSTYPE  */
  fprintf(fp, LIT_INFO_01, __DATE__, __TIME__);

  return(EXIT_FAILURE);

}  /* show_rev() */

/* END: jmerge_h.c */
