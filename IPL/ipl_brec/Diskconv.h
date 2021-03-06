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

/*
 * CDisk
 * Map a hard disk on sector level
 * Uses LBA access when available (necessary for 8GB+ disks)
 */

#ifndef __disk__
#define __disk__

#include <transfer.h>

// ipls
void DiskMap(int DriveToUse, unsigned long StartSectorToUse);
void DiskRead(unsigned long Sector, void far *Buffer, int Count);

#endif
