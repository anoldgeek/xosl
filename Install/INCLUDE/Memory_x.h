/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __MEMORY_X__
#define __MEMORY_X__

#include <defs.h>

//#define MK_FP(s,o) ((void _seg *)(s) + (void near *)(o))
//#define peekb(s,o) (*(char far *)MK_FP(s,o))

//#define FP_SEG(ptr) ((unsigned short)(void _seg *)(void far *)(ptr))
//#define FP_OFS(ptr) ((unsigned short)(ptr))

//#define PhysAddr(lAddr) ( ((long)FP_SEG(lAddr) << 4) + (long)FP_OFS(lAddr) )

extern "C" {

void __cdecl MemSet(void *dest, int value, unsigned short count);
void __cdecl MemCopy(void *dest, const void *src, unsigned short size);
int __cdecl MemCompare(const void *s1, const void *s2, unsigned short count);

}

#endif