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
#include <main.h>
#include <bootrec.h>

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
			unsigned char DosDriveSector0;
		};


		CDosDriveList(CPartList &PartListToUse);
		~CDosDriveList();
		int LocateDrive(int DriveNum, CDosDrive &DosDrive);
		int GetDriveIndex(TPartition *Partition, TBootRecord &BootRecord);
		int GetDosDriveSector0(int Drive);
		void UpdateDosDriveMbrHDSector0(CDosDrive &DosDrive, unsigned char MbrHDSector0);
	private:
		CPartList &PartList;

		unsigned long __cdecl GetDosSerialNo(int DriveNum);
		unsigned long GetBRecSerialNo(int Index);
};

#endif
