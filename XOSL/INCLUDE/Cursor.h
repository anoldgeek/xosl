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
 * - Pull-in variables into extern C block (use C name mangling)
 *     extern int __cdecl CursorX;
 *     extern int __cdecl CursorY;
 *     extern int __cdecl CursorVisible;
 * - Add line at end of file to get rid of WCC Compiler Warning! W138: 
 *   No newline at end of file.
 *
 */

#ifndef __cursor__
#define __cursor__

#include <newdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl DrawCursor(void);
void __cdecl ClearCursor(void);

extern int __cdecl CursorX;
extern int __cdecl CursorY;
extern int __cdecl CursorVisible;

#ifdef __cplusplus
};
#endif

//extern int __cdecl CursorX;
//extern int __cdecl CursorY;
//extern int __cdecl CursorVisible;

#endif
