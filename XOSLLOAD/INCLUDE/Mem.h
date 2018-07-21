/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __MEMORY__
#define __MEMORY__

#include <defs.h>

//#define MK_FP(s,o) ((void _seg *)(s) + (void near *)(o))
//#define peekb(s,o) (*(char far *)MK_FP(s,o))

//#define FP_SEG(ptr) ((unsigned short)(void _seg *)(void far *)(ptr))
//#define FP_OFS(ptr) ((unsigned short)(ptr))

//Open Watcom C/C++:
#define MK_FP(__s,__o) (((unsigned short)(__s)):>((void __near *)(__o)))
#define peekb(s,o) (*(char far *)MK_FP(s,o))

#define FP_SEG(__p) ((unsigned)((unsigned long)(void __far*)(__p) >> 16))
#define FP_OFS(__p) ((unsigned)(__p))


#define PhysAddr(lAddr) ( ((long)FP_SEG(lAddr) << 4) + (long)FP_OFS(lAddr) )

void memset(void *dest, int value, unsigned short count);
void memcpy(void far *dest, const void far *src, unsigned short size);
int memcmp(const void far *s1, const void far *s2, unsigned short count);

#endif