//
//  maketree.h
//  RomanDev
//
//  Created by 岡崎康浩 on 2026/10/22.
//

#ifndef devnag_h
#define devnag_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#define MAX_DIR     180
#define CODE_NUM    280
#define SHORT_A      2
#define VOWEL_IN     4
#define VOWEL_DE     3
#define INDEPEND     0
#define CONSONANT    1
#define PERIOD       9
#define SPACE        5
#define COMMA        8
#define OPEN         7
#define CLOSE        6
#define NON_ROMANIZE    12

//char    cdir_name[MAX_DIR];
unsigned char   ori_buf[4096];
unsigned char   conv_buf[4096];
static unsigned char    VIRAMA[] = "\xe0\xa5\x8d";
/* The structure of each entry in the conversion table. */
typedef struct dvcode{
    char          internal_sign;
    unsigned char dv[5];
    unsigned char rm[8];
    int           dvmode;
    int           rm_length;
    int           dv_length;
    int           read;
    struct dvcode *byte[6];
    struct dvcode *allograph;
    unsigned char *comm;
} dvcode;
dvcode          *internal_buf[2048];
dvcode          *root_dvcode;
dvcode          *deeper_search( unsigned char *lit_seq, dvcode *mother, int d);
dvcode          *maketree( void );
dvcode          *make_dvcode(unsigned char *s, unsigned char *p, int mode);
int             read_buf(int ip);
int             read_list(const char *cdir );
int             one_character(unsigned char str);
unsigned char   wide_to_lower( unsigned char in_char );
int             write_devnag(int i_ip);
int             maintree( void );
int             subtree(int start, int end, int d);
int             prepare(const char* path);
unsigned char   *convert_text(const unsigned char *src);
void            free_text( unsigned char *p);


struct dvcode*  devnag[CODE_NUM];
int             temp_order[CODE_NUM];
static int      inbuf;
unsigned char   *write_ip;
unsigned char   tmp_char[8];
struct dvcode   *root;
int             con_flag;
int             code_num_real;
int             char_flag;
int             read_ip;
#endif // !devnag_h
