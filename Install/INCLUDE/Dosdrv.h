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

#ifndef __dosdrv__
#define __dosdrv__

#include <ptab.h>
#include <defs.h>

#define FATTYPE_FAT16 16
#define FATTYPE_FAT32 32

class CDosDriveList {
	public:
		class CDosDrive {
		public:
			int DriveChar; // 'C', 'D', ...
			int FATType;
			int Drive;
			unsigned long long StartSector;
		};


		CDosDriveList(CPartList &PartListToUse);
		~CDosDriveList();
		int LocateDrive(int DriveNum, CDosDrive &DosDrive);
	private:
		CPartList &PartList;

		unsigned long __cdecl GetDosSerialNo(int DriveNum);
		unsigned long GetBRecSerialNo(int Index);

};

#endif
