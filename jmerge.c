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
 * jmerge.c -- Main routines
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

#include "jmerge.h"

/*
 * show_file_heading() -- Show run stats
 */
void show_file_heading(struct s_file_info *f, char *in_file)

{
  
  fprintf(f->fp, "%s\n", LIT_C80);
  f->lines_writes++;

  if (in_file == (char *) NULL)
    {
      f->lines_writes++;
      fprintf(f->fp, "%s\n", LIT_STDIN);
    }
  else
    {
      f->lines_writes++;
      if (strcmp(in_file, FILE_NAME_STDIN) == 0)
	fprintf(f->fp, "%s\n", LIT_STDIN);
      else
	fprintf(f->fp, "%s\n", in_file);
    }
  
  f->lines_writes++;
  fprintf(f->fp, "%s\n", LIT_C80);
  
} /* show_file_heading() */

/*
 * main()
 */
int main(int argc, char **argv)

{
  struct s_work w;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath flock",NULL) == -1)
    err(1,"pledge\n");
#endif

  init(argc, argv, &w);

  if (w.arg_sorted == TRUE)
    process_sorted(argc, argv, &w);
  else
    process_random(argc, argv, &w);

  close_in(&(w.key));
  close_out(&(w.out));
  close_out(&(w.err));
  close_out(&(w.stats));
  if (w.prog_name != (char *) NULL)
    free(w.prog_name);
  exit(EXIT_SUCCESS);

}  /* main() */
