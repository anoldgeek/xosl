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
 * - Explicitly declare pointers to be far
 *
 */

#ifndef MemH
#define MemH

int MemCompare(const void far *Mem1, const void far *Mem2, int ByteCount);

#endif
