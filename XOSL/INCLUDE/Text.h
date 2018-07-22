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

#ifndef __text__
#define __text__

#include <newdefs.h>

/*
void __cdecl gotoxy(int x, int y);
void __cdecl wherexy(int *x, int *y);
void __cdecl puts(const char far *str);
void __cdecl putch(int ch);
*/
void gotoxy(int x, int y);
void wherexy(int *x, int *y);
void puts(const char far *str);
void putch(int ch);


#endif