#ifndef yyHEADER_H
#define yyHEADER_H 1
#define yyIN_HEADER 1

/*
     ZZZZZZZZZZZZZZZZZZZZ    8888888888888       00000000000
   ZZZZZZZZZZZZZZZZZZZZ    88888888888888888    0000000000000
                ZZZZZ      888           888  0000         0000
              ZZZZZ        88888888888888888  0000         0000
            ZZZZZ            8888888888888    0000         0000       AAAAAA         SSSSSSSSSSS   MMMM       MMMM
          ZZZZZ            88888888888888888  0000         0000      AAAAAAAA      SSSS            MMMMMM   MMMMMM
        ZZZZZ              8888         8888  0000         0000     AAAA  AAAA     SSSSSSSSSSS     MMMMMMMMMMMMMMM
      ZZZZZ                8888         8888  0000         0000    AAAAAAAAAAAA      SSSSSSSSSSS   MMMM MMMMM MMMM
    ZZZZZZZZZZZZZZZZZZZZZ  88888888888888888    0000000000000     AAAA      AAAA           SSSSS   MMMM       MMMM
  ZZZZZZZZZZZZZZZZZZZZZ      8888888888888       00000000000     AAAA        AAAA  SSSSSSSSSSS     MMMM       MMMM

Copyright (C) Paulo Custodio, 2011-2013

Scanner - to be processed by: flex -L scan.l
*/

/* $Header: /home/dom/z88dk-git/cvs/z88dk/src/z80asm/scan.h,v 1.2 2013-03-31 18:28:30 pauloscustodio Exp $ */
/* $Log: scan.h,v $
/* Revision 1.2  2013-03-31 18:28:30  pauloscustodio
/* New TOK_LABEL for a label definition, i.e. ". NAME" or "NAME :"
/*
/* Revision 1.1  2013/03/29 23:53:08  pauloscustodio
/* Added GNU Flex-based scanner. Not yet integrated into assembler.
/*
/*
/* */

#include "memalloc.h"	/* before any other include */

#define YY_NO_UNISTD_H	1		/* don't include unistd */

#ifndef SCAN_H
#define SCAN_H

#include "class.h"
#include "classlist.h"
#include "dynstr.h"

/*-----------------------------------------------------------------------------
*   Token Type
*----------------------------------------------------------------------------*/
typedef enum TokType
{
	/* mark end of input */
	TOK_NULL			= 0,
	
    /* single character tokens */
    TOK_NEWLINE         = '\n',
	
    TOK_EXCLAM          = '!',
    TOK_HASH            = '#',
    TOK_DOLLAR          = '$',
    TOK_PERCENT         = '%',
    TOK_AMPERSAND       = '&',
    TOK_LPAREN          = '(',
    TOK_RPAREN          = ')',
    TOK_ASTERISK        = '*',
    TOK_PLUS            = '+',
    TOK_COMMA           = ',',
    TOK_HYPHEN          = '-',
    TOK_PERIOD          = '.',
    TOK_SLASH           = '/',
    TOK_COLON           = ':',
    TOK_LESS            = '<',
    TOK_EQUAL           = '=',
    TOK_GREATER         = '>',
    TOK_QUESTION        = '?',
    TOK_ATSIGN          = '@',
    TOK_LSQUARE         = '[',
    TOK_BACKSLASH       = '\\',
    TOK_RSQUARE         = ']',
    TOK_CARET           = '^',
    TOK_BACKQUOTE       = '`',
    TOK_LCURLY          = '{',
    TOK_VBAR            = '|',
    TOK_RCURLY          = '}',
    TOK_TILDE           = '~',
	
    /* multi-character tokens */
    TOK_EQUAL_EQUAL     = 0x100,/* "==" */
    TOK_LESS_GREATER,           /* "<>" */
    TOK_NOT_EQUAL,              /* "!=" */
    TOK_LESS_EQUAL,             /* "<=" */
    TOK_GREATER_EQUAL,          /* ">=" */
    TOK_DBL_VBAR,               /* "||" */
    TOK_DBL_AMPERSAND,          /* "&&" */
    TOK_DBL_LESS,               /* "<<" */
    TOK_DBL_GREATER,            /* ">>" */
    TOK_DBL_ASTERISK,           /* "**" */

    /* language tokens */
    TOK_NAME,                   /* any identifier */
    TOK_LABEL,                  /* label definition, i.e. ". NAME" or "NAME :" */
    TOK_NUMBER,
    TOK_STRING,                 /* single- or double-quoted string */
    TOK_PREPROC,                /* preprocessor command */

} TokType;

/*-----------------------------------------------------------------------------
*   Token and list of tokens
*----------------------------------------------------------------------------*/
CLASS(Token)
	TokType	tok_type;			/* type of token */
	long	num_value;			/* numeric value, if any */
	Str	   *str_value;			/* string value, if any */
	char   *filename;			/* file name - kept in strpool */
	int 	line_nr;			/* input line number */
END_CLASS;

CLASS_LIST(Token);

/*-----------------------------------------------------------------------------
*   API - uses srcfile.h singleton API
*----------------------------------------------------------------------------*/

/* Start reading file / text at current position */
extern void scan_file( char *filename );
extern void scan_text( char *text );

/* scan input for next token, return token type; 
   use scan_xxx() to last returned token attributes, only valid until next
   scan_get()/scan_unget() call */
extern TokType scan_get( void );
extern TokType scan_tok_type( void );
extern long	   scan_num_value( void );
extern char   *scan_str_value( void );
extern char   *scan_filename( void );
extern int 	   scan_line_nr( void );

/* push back token to input stream */
extern void scan_unget( TokType tok_type, long num_value, char *str_value,
						char *filename, int line_nr );

/* stack of nested constructs, i.e. IF / ELSE / ENDIF
   local to each input file, stack must be empty at the end of file */
extern void scan_push_struct( int id, int value );
extern int  scan_top_id( void );						
extern int  scan_top_value( void );
extern void scan_replace_struct( int id, int value );
extern void scan_pop_struct( int id );		/* syntaxt error if id != top_id */

/* forward declaration for YY_EXTRA_TYPE */
struct Context;

#endif /* ndef SCAN_H */





#define  YY_INT_ALIGNED long int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5
#define YY_FLEX_SUBMINOR_VERSION 35
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif

/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* end standard C headers. */

/* flex integer type definitions */

#ifndef FLEXINT_H
#define FLEXINT_H

/* C99 systems have <inttypes.h>. Non-C99 systems may or may not. */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

/* C99 says to define __STDC_LIMIT_MACROS before including stdint.h,
 * if you want the limit (max/min) macros for int types. 
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;

/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#endif /* ! C99 */

#endif /* ! FLEXINT_H */

#ifdef __cplusplus

/* The "const" storage-class-modifier is valid. */
#define YY_USE_CONST

#else	/* ! __cplusplus */

/* C99 requires __STDC__ to be defined as 1. */
#if defined (__STDC__)

#define YY_USE_CONST

#endif	/* defined (__STDC__) */
#endif	/* ! __cplusplus */

#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst
#endif

/* An opaque pointer. */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

/* For convenience, these vars (plus the bison vars far below)
   are macros in the reentrant scanner. */
#define yyin yyg->yyin_r
#define yyout yyg->yyout_r
#define yyextra yyg->yyextra_r
#define yyleng yyg->yyleng_r
#define yytext yyg->yytext_r
#define yylineno (YY_CURRENT_BUFFER_LVALUE->yy_bs_lineno)
#define yycolumn (YY_CURRENT_BUFFER_LVALUE->yy_bs_column)
#define yy_flex_debug yyg->yy_flex_debug_r

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */
    
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

void yyrestart (FILE *input_file ,yyscan_t yyscanner );
void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer ,yyscan_t yyscanner );
YY_BUFFER_STATE yy_create_buffer (FILE *file,int size ,yyscan_t yyscanner );
void yy_delete_buffer (YY_BUFFER_STATE b ,yyscan_t yyscanner );
void yy_flush_buffer (YY_BUFFER_STATE b ,yyscan_t yyscanner );
void yypush_buffer_state (YY_BUFFER_STATE new_buffer ,yyscan_t yyscanner );
void yypop_buffer_state (yyscan_t yyscanner );

YY_BUFFER_STATE yy_scan_buffer (char *base,yy_size_t size ,yyscan_t yyscanner );
YY_BUFFER_STATE yy_scan_string (yyconst char *yy_str ,yyscan_t yyscanner );
YY_BUFFER_STATE yy_scan_bytes (yyconst char *bytes,int len ,yyscan_t yyscanner );

void *yyalloc (yy_size_t ,yyscan_t yyscanner );
void *yyrealloc (void *,yy_size_t ,yyscan_t yyscanner );
void yyfree (void * ,yyscan_t yyscanner );

/* Begin user sect3 */

#define yywrap(n) 1
#define YY_SKIP_YYWRAP

#define yytext_ptr yytext_r

#ifdef YY_HEADER_EXPORT_START_CONDITIONS
#define INITIAL 0
#define READ_LINE 1

#endif

#ifndef YY_NO_UNISTD_H
/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 */
#include <unistd.h>
#endif

#define YY_EXTRA_TYPE struct Context *

int yylex_init (yyscan_t* scanner);

int yylex_init_extra (YY_EXTRA_TYPE user_defined,yyscan_t* scanner);

/* Accessor methods to globals.
   These are made visible to non-reentrant scanners for convenience. */

int yylex_destroy (yyscan_t yyscanner );

int yyget_debug (yyscan_t yyscanner );

void yyset_debug (int debug_flag ,yyscan_t yyscanner );

YY_EXTRA_TYPE yyget_extra (yyscan_t yyscanner );

void yyset_extra (YY_EXTRA_TYPE user_defined ,yyscan_t yyscanner );

FILE *yyget_in (yyscan_t yyscanner );

void yyset_in  (FILE * in_str ,yyscan_t yyscanner );

FILE *yyget_out (yyscan_t yyscanner );

void yyset_out  (FILE * out_str ,yyscan_t yyscanner );

int yyget_leng (yyscan_t yyscanner );

char *yyget_text (yyscan_t yyscanner );

int yyget_lineno (yyscan_t yyscanner );

void yyset_lineno (int line_number ,yyscan_t yyscanner );

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap (yyscan_t yyscanner );
#else
extern int yywrap (yyscan_t yyscanner );
#endif
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy (char *,yyconst char *,int ,yyscan_t yyscanner);
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * ,yyscan_t yyscanner);
#endif

#ifndef YY_NO_INPUT

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k */
#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif /* __ia64__ */
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1

extern int yylex (yyscan_t yyscanner);

#define YY_DECL int yylex (yyscan_t yyscanner)
#endif /* !YY_DECL */

/* yy_get_previous_state - get the state just before the EOB char was reached */

#undef YY_NEW_FILE
#undef YY_FLUSH_BUFFER
#undef yy_set_bol
#undef yy_new_buffer
#undef yy_set_interactive
#undef YY_DO_BEFORE_ACTION

#ifdef YY_DECL_IS_OURS
#undef YY_DECL_IS_OURS
#undef YY_DECL
#endif

#undef yyIN_HEADER
#endif /* yyHEADER_H */
