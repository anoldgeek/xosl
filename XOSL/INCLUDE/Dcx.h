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
 * - Add line at end of file to get rid of WCC Compiler Warning! W138: 
 *   No newline at end of file.
 *
 */

#ifndef __dcx__
#define __dcx__

#include <newdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl CreateCursor(unsigned long CursorFGnd);
void far __cdecl InvertCursor(int Invert);

#ifdef __cplusplus
};
#endif

#endif
