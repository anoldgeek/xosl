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
 * - Use extern "C" to force C language name mangling for variables referenced
 *   in assembly source files
 * - Explicitly declare pointers to be far
 *
 */

#ifndef __transfer__
#define __transfer__


void GetDriveInfo(int Drive);
void ConvRead(unsigned short SectCyl,unsigned short DrvHead, void far *Buffer, int Count);
void Sector2CHS(unsigned long RSector, unsigned short &SectCyl, unsigned short &DrvHead);


extern long DrvHeadCount;
extern unsigned long DrvSectorCount;
extern int Drive;
extern unsigned long long StartSector;


#endif
