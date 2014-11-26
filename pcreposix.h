/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* Copyright (c) 1997-2003 University of Cambridge */

#ifndef _PCREPOSIX_H
#define _PCREPOSIX_H


#ifdef _CH_
#pragma package <chpcre>
#include <chdl.h>
LOAD_CHDL_CODE(pcre, Pcre);
#endif


/* This is the header for the POSIX wrapper interface to the PCRE Perl-
Compatible Regular Expression library. It defines the things POSIX says should
be there. I hope. */

/* Have to include stdlib.h in order to ensure that size_t is defined. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
 extern "C" {
#endif

/* Options defined by POSIX. */

#define REG_ICASE     0x01
#define REG_NEWLINE   0x02
#define REG_NOTBOL    0x04
#define REG_NOTEOL    0x08

/* These are not used by PCRE, but by defining them we make it easier
to slot PCRE into existing programs that make POSIX calls. */

#define REG_EXTENDED  0
#define REG_NOSUB     0

/* Error values. Not all these are relevant or used by the wrapper. */

enum {
  REG_ASSERT = 1,  /* internal error ? */
  REG_BADBR,       /* invalid repeat counts in {} */
  REG_BADPAT,      /* pattern error */
  REG_BADRPT,      /* ? * + invalid */
  REG_EBRACE,      /* unbalanced {} */
  REG_EBRACK,      /* unbalanced [] */
  REG_ECOLLATE,    /* collation error - not relevant */
  REG_ECTYPE,      /* bad class */
  REG_EESCAPE,     /* bad escape sequence */
  REG_EMPTY,       /* empty expression */
  REG_EPAREN,      /* unbalanced () */
  REG_ERANGE,      /* bad range inside [] */
  REG_ESIZE,       /* expression too big */
  REG_ESPACE,      /* failed to get memory */
  REG_ESUBREG,     /* bad back reference */
  REG_INVARG,      /* bad argument */
  REG_NOMATCH      /* match failed */
};


/* The structure representing a compiled regular expression. */

typedef struct {
  void *re_pcre;
  size_t re_nsub;
  size_t re_erroffset;
} regex_t;

/* The structure in which a captured offset is returned. */

typedef int regoff_t;

typedef struct {
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;

/* The functions */

extern int regcomp(regex_t *preg, const char *pattern, int cflags);
extern int regexec(regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);
extern size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
extern void regfree(regex_t *preg);


#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* End of pcreposix.h */
