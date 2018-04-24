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
 * - Explicitely declare function argument pointers of type far so
 *   that they match the assembly language function implementations
 * - Add lseek declaration. 
 *
 */

#ifndef __io__
#define __io__

#include <defs.h>

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define S_IRDONLY 0x0001
#define S_IHIDDEN 0x0002
#define S_ISYSTEM 0x0004
#define S_IATTRIB 0x0020

_extern int __cdecl creat(const char far *path);             //ML const char far
_extern int __cdecl open(const char far *path, int access);  //ML const char far
_extern void __cdecl close(int handle);
_extern unsigned short __cdecl read(int handle, void far *buf, unsigned short len);
_extern unsigned short __cdecl write(int handle, void far *buf, unsigned short len);
_extern int __cdecl chmod(const char far *path, int amode);
_extern int __cdecl lseek(int handle,unsigned short origin, int offset);

#endif
