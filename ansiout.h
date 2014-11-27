#ifndef ANSIOUT_H
#define ANSIOUT_H

#include "struct.h"

void ansi_fg ( int fg );
void ansi_bg ( int bg );
void pipe2ansi ( PASSING *pass, char* szString );
void ansi_getxy ( PASSING *pass );
void ansiPrintf ( PASSING *pass, char *filename );

#endif
