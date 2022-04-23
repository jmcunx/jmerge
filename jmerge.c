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

#ifdef OpenBSD
#include <err.h>
#endif
#ifdef __FreeBSD_version
#include <err.h>
#endif
#ifdef __NetBSD_Version__
#include <err.h>
#endif

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jmerge.h"

/*
 * main()
 */
int main(int argc, char **argv)

{
  struct s_work w;
  struct s_file_data d1;
  struct s_file_data d2;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath",NULL) == -1)
    err(1,"pledge\n");
#endif

  init(argc, argv, &w, &d1, &d2);

  compare_d(&d1, &d2, w.first_headg);

  /* display data here */
  switch (w.list_mode)
    {
      case LIST_MODE_1:
	w.out.io += list_match(w.out.fp, w.show_changed, w.only_match, d1.r, w.delim_in, d2.max_fields);
	break;
      case LIST_MODE_2:
	w.out.io += list_match(w.out.fp, w.show_changed, w.only_match, d2.r, w.delim_in, d1.max_fields);
	break;
    }

  /*** DONE ***/
  if (w.verbose)
    {
      fprintf(w.err.fp, MSG_INFO_I003, d1.finfo.io,  0L, 
              (strcmp(d1.finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d1.finfo.fname));
      fprintf(w.err.fp, MSG_INFO_I003, d2.finfo.io,  0L, 
              (strcmp(d2.finfo.fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : d2.finfo.fname));
      fprintf(w.err.fp, MSG_INFO_I003, 0L, w.out.io, 
              (strcmp(w.out.fname,FILE_NAME_STDOUT) == 0 ? LIT_STDOUT : w.out.fname));
    }
  if (w.debug)
    {
      list_in_raw(w.err.fp, w.show_changed, d1.r, d1.finfo.fname, d2.finfo.fname);
      list_in_raw(w.err.fp, w.show_changed, d2.r, d2.finfo.fname, d1.finfo.fname);
    }

  close_out(&(w.out));
  close_out(&(w.err));
  exit(EXIT_SUCCESS);

}  /* main() */
