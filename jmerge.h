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

#ifndef JMERGE_H

#define JMERGE_H "INCLUDED"

#define PROG_NAME  "jmerge"
#define PAUSE_RECS 100

#ifndef NULL
#define NULL '\0'
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef JLIB2_CHAR_NULL
#define NO_JLIB 1
#define JLIB2_CHAR_NULL  ((char) '\0')
#ifdef _MSDOS
#define SSIZE_T unsigned long int
#else
#define SSIZE_T ssize_t
#endif
#endif

/*** structures ***/
struct s_file_info
{
  FILE *fp;
  char *fname;
  int fileno;
  long int lines_read;
  long int lines_writes;
  long int count_match;
} ;

struct s_buffer
{
  char *buf;
  char *match_key;
  char *last_match_key;
  size_t bsiz;
  size_t ksiz;
  ssize_t stat_read;
  char delm;                  /* field delimiter       */
} ;
struct s_key_data
{
  struct s_key_data *next;
  long int count_match;
  long int pos;
  char *k;
};

#define NULL_KEY_DATA ((struct s_key_data *) NULL)

struct s_work
{
  struct s_file_info key;         /* main 'key' file       */
  struct s_file_info out;         /* default stdout        */
  struct s_file_info err;         /* default stderr        */
  struct s_file_info stats;       /* write main file stats */
  char *prog_name;                /* real program name     */
  int arg_verbose;                /* TRUE or FALSE         */
  int arg_heading;                /* TRUE or FALSE         */
  int arg_force;                  /* TRUE or FALSE         */
  int arg_sorted;                 /* TRUE of FALSE         */
  long int arg_milliseconds;      /* milliseconds to pause         */
  long int arg_pause_reads;       /* pause after this many reads   */
  char arg_delm;                  /* field delimiter       */
} ;

/*** messages ***/
#ifdef NO_JLIB
#define SWITCH_CHAR       '-'
#define FILE_NAME_STDIN   "-"
#define LIT_STDIN         "(standard input)"
#define LIT_C80           "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
#define LIT_INFO_01       "\tBuild: %s %s\n"
#define LIT_REV           "Revision"
#define LIT_STDOUT        "(standard output)"
#define ARG_DELM          'd'  /* Field Delimiter                    */
#define ARG_ERR           'e'  /* Output Error File                  */
#define ARG_FIRST_HEADG   'F'  /* First Record is a Heading          */
#define ARG_FORCE         'f'  /* force create files                 */
#define ARG_HELP          'h'  /* Show Help                      */
#define ARG_INPUT         'i'  /* Input File                         */
#define ARG_KEY_STATS     'K'  /* Write Key Stats to a file          */
#define ARG_OUT           'o'  /* Output File                        */
#define ARG_PAUSE         's'  /* Pause for for microseconds         */
#define ARG_READS         'r'  /* after this many reads, do something*/
#define ARG_UNSORTED      'u'  /* list is unsorted                   */
#define ARG_VERBOSE       'v'  /* Verbose                            */
#define ARG_VERSION       'V'  /* Show Version Information           */
#define FILE_NAME_STDOUT  "-"
#define FILE_NAME_STDOUT  "-"
#define MSG_ERR_E000                     "Try '%s %c%c' for more information\n"
#define MSG_ERR_E002                     "ERROR E002: Cannot open '%s' for write, processing aborted\n"
#define MSG_ERR_E003                     "ERROR E003: Cannot allocate memory when processing file %s, processing aborted\n"
#define MSG_ERR_E008                     "ERROR E008: '%s' is an invalid value for %c%c, must be numeric\n"
#define MSG_ERR_E022                     "ERROR E022: Required Argument '%c%c' missing\n"
#define MSG_ERR_E025                     "ERROR E025: File %s cannot be created, already exists\n"
#define MSG_ERR_E049                     "ERROR E049: delimiter '%s' invalid\n"
#define MSG_ERR_E066                     "ERROR E066: '%d' is an invalid value for %c%c, must be > %d\n"
#define MSG_ERR_E074                     "ERROR E074: 'Too many Arguments specified for %c%c\n"
#define MSG_ERR_E080                     "ERROR E080: cannot allocate initial memory : %s\n"
#define MSG_ERR_E091                     "E091: cannot lock file '%s' : %s\n"
#define MSG_ERR_E092                     "E092: cannot position read to %ld (%s) on file\n    '%s'\n"
#define MSG_ERR_E093                     "E093: no data found after seek to %ld (%s) on file\n    '%s'\n"
#define MSG_INFO_I072                    "I072:    Lines Reads:  %9ld - File %s\n"
#define MSG_INFO_I080                    "I080:   Lines Writes:  %9ld - File %s\n"
#define MSG_INFO_I108                    "I108:  Lines Matched:  %9ld - File %s\n"
#define MSG_INFO_I109                    "I109:     No Matches:  %9ld - File %s\n"
#define MSG_INFO_I109                    "I109:     No Matches:  %9ld - File %s\n"
#define MSG_WARN_W002                    "W002: Open Error Bypass File '%s' : %s\n"
#define MSG_WARN_W031                    "W031: Bypassed, cannot lock file '%s' : %s\n"
#define USG_MSG_ARG_DELM                 "\t%c%c c\t\t: Field delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_A               "\t\t\t  the decimal # representing the character.\n"
#define USG_MSG_ARG_DELM_B               "\t\t\t  Ex: 7 = '^G', 9 = TAB, 124 = '|' or just '|'.\n"
#define USG_MSG_ARG_ERR                  "\t%c%c file\t\t: Write errors to file 'file', default stderr\n"
#define USG_MSG_ARG_FIRST_HEADG          "\t%c%c\t\t: First Record is a Heading\n"
#define USG_MSG_ARG_FORCE                "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_FORCE                "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_HELP                 "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_INPUT_3              "\t%c%c file\t\t: Input File for Merge Driver File\n"
#define USG_MSG_ARG_KEY_STATS            "\t%c%c file\t\t: Write Match Key Stats to 'file'\n"
#define USG_MSG_ARG_OUT                  "\t%c%c file\t\t: Write output to file 'file', default stdout\n"
#define USG_MSG_ARG_PAUSE_1              "\t%c%c n\t\t: Sleep 'n' microseconds based upon records processed\n"
#define USG_MSG_ARG_READS                "\t%c%c n\t\t: Optional, after 'n' reads, pause\n"
#define USG_MSG_ARG_UNSORTED             "\t%c%c\t\t: Data is unsorted, use slow option\n"
#define USG_MSG_ARG_VERBOSE_8            "\t%c%c\t\t: verbose level, each time specified level increases\n"
#define USG_MSG_ARG_VERSION              "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_OPTIONS                  "Options\n"
#define USG_MSG_USAGE                    "usage:\t%s [OPTIONS] [FILES ...]\n"
#endif /* NO_JLIB */

/*** prototypes ***/
void init(int, char **, struct s_work *);
void init_finfo(struct s_file_info *);
void show_brief_help(FILE *, char *);
void show_rev(FILE *, char *);
void process_arg(int, char **, struct s_work *);
int  open_out(FILE *, struct s_file_info *, char *, int);
void close_out(struct s_file_info *);
int open_in(FILE *, struct s_file_info *, char *);
void close_in(struct s_file_info *);
void key_free(struct s_key_data **);
void process_sorted(int, char **, struct s_work *);
void show_file_heading(struct s_file_info *, char *);
void init_buf(FILE *, struct s_buffer *, char);
void free_buf(struct s_buffer *);
int read_buf(FILE *, struct s_buffer *, struct s_file_info *);
struct s_key_data *key_load(FILE *, struct s_file_info *, int, int);
long int key_print(FILE *, struct s_key_data *);
void key_free(struct s_key_data **);
void key_clear_counts(struct s_key_data *);
struct s_key_data *key_search(char *, struct s_key_data *);
void process_random(int, char **, struct s_work *);

#ifdef NO_JLIB
char *j2_get_prgname(char *, char *);
int j2_is_numr(char *);
long int j2_rtw(char *);
int j2_f_exist(char *);
void j2_sleepm(long int);
SSIZE_T j2_getline(char **buf, size_t *n, FILE *fp);
long int j2_clr_str(char *s, char c, int size);
#endif /* NO_JLIB */

#endif /*  JMERGE_H  */

/* END: jmerge.h */
