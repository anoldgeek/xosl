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

#ifndef __dosfile__
#define __dosfile__

#include <newdefs.h>

int __cdecl DosCreate(const char *FileName);
void __cdecl DosClose(int FileHandle);
void __cdecl DosWrite(int FileHandle, const void *Data, unsigned short Size);

#endif
