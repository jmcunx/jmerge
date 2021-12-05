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

#ifndef JMERGE_H

#define JMERGE_H "$Id: jmerge.h,v 2.2 2020/06/16 00:17:49 jmccue Exp $"

#define PROG_NAME  "jmerge"
#define LIST_MODE_1   1
#define LIST_MODE_2   2
#define FILTER_NO     0
#define FILTER_YES    1
#define FILTER_INVERT 2
#define KEY_MAX_IDX   255

/*** structures ***/
struct s_file_info
  {
    FILE *fp;
    char *fname;
    long int io;
  } ;

struct s_key
  {
    char *key;
    struct s_key *next;  /* next node in this list  */
  } ;

struct s_key_data
  {
    int filter;  /* FILTER_NO, FILTER_YES, FILTER_INVERSE */
    struct s_file_info finfo;
    struct s_key *keys;
    struct s_key *kidx[KEY_MAX_IDX]; /* index to first char of each entry */
  } ;

struct s_file_recs
  {
    char *key;
    char *other_fields;
    int match_count;
    struct s_file_recs *match; /* pointer to matched data */
    struct s_file_recs *next;  /* next node in this list  */
  } ;

struct s_file_data
  {
    int max_fields;
    int need_uniq;             /* TRUE or FALSE -- need unique key */
    int file_num;              /* only for error reporting         */
    struct s_file_info finfo;  /* File information                 */
    struct s_file_recs *r;
  } ;

struct s_work
  {
    struct s_file_info out;         /* default stdout        */
    struct s_file_info err;         /* default stderr        */
    char delim_in;                  /* delimiter             */
    int num_files;                  /* # of files to process */
    int list_mode;                  /* how to list data      */
    int invert;                     /* TRUE or FALSE         */
    int only_match;                 /* TRUE or FALSE         */
    int verbose;                    /* TRUE or FALSE         */
    int debug;                      /* TRUE or FALSE         */
    int first_headg;                /* TRUE or FALSE         */
    int force;                      /* TRUE or FALSE         */
    int show_changed;               /* TRUE or FALSE         */
  } ;


/*** prototypes ***/
void init(int, char **, struct s_work *, struct s_file_data *, 
          struct s_file_data *);
void init_finfo(struct s_file_info *);
int  show_brief_help(FILE *, char *);
int  show_rev(FILE *, char *);
void process_arg(int, char **, struct s_work *, struct s_file_data *, 
                 struct s_file_data *, struct s_key_data *);
int  open_out(FILE *, struct s_file_info *, char *, int);
void close_out(struct s_file_info *);
int  open_in(FILE **, char *, FILE *);
void close_in(FILE **, char *);
void load_link(FILE *, struct s_file_data *, struct s_key_data *, char, int);
void compare_d(struct s_file_data *, struct s_file_data *, int);
void list_in_raw(FILE *, int, struct s_file_recs *, char *, char *);
int  list_match(FILE *, int, int, struct s_file_recs *, char, int);
void load_keys(FILE *efp, struct s_key_data *, int);
void show_keys(FILE *, struct s_key_data *);
void free_key_data(struct s_key *);

#endif /* JMERGE_H */

/* END: jmerge.h */
