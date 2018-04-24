/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * Open Watcom Migration
 * Copyright (c) 2010 by Mario Looijkens:
 * - Use __cdecl calling convention
 *
 */

#ifndef __STRING__
#define __STRING__

#include <newdefs.h>

#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16

#ifdef __cplusplus
extern "C" {
#endif

int far __cdecl strlen(const char far *str);
char *far __cdecl strcpy(char far *dest, const char far *src);
char *far __cdecl strcat(char far *dest, const char far *src);
char *far __cdecl strchr(const char far *str, int c);
char *far __cdecl itoa(long value, char far *str, int base);
int far __cdecl strcmp(const char far *s1, const char far *s2);
int far __cdecl stricmp(const char far *s1, const char far *s2);
long far __cdecl atol(const char far *str);
char far __cdecl *ultoa(unsigned long Value, char *Str, int Base);
char  far __cdecl*ltoa(long Value, char *Str, int Base);

#ifdef __cplusplus
};
#endif

#endif