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

#ifndef __transfer__
#define __transfer__

#define Scratchpad ( (void *)0x90008000 )
//#define Scratchpad ( (void *)0x00008000 )

#define DISK_READ   0x0200
#define DISK_WRITE  0x0300
#define DISK_VERIFY 0x0400

#define LOCK_STATUS_LOCKED   0x00
#define LOCK_STATUS_UNLOCKED 0x01

typedef struct {
	int PacketSize;
	int SectorCount;
	void *TransferBuffer;
//	unsigned long SectorLow;
//	unsigned long SectorHigh;
	unsigned long long Sector;
} TLBAPacket;

class CDiskAccess {
	public:
		CDiskAccess();
		~CDiskAccess();

		// conventional stuff
		int __cdecl DriveCount(int Fixed);
		int __cdecl Transfer(int Action, unsigned short SectCyl,
						     unsigned short DrvHead, void *Buffer, int Count);
		int __cdecl GetDriveInfo(int Drive, int &Heads, int &Sectors);
//		int __cdecl CopyFromScratchpad(void *Buffer, int Sectors);
//		int __cdecl CopyToScratchpad(const void *Buffer, int Sectors);

		// lba stuff
		int __cdecl LBAAccessAvail(int Drive);
		int __cdecl LBATransfer(int Action, int Drive, const TLBAPacket &LBAPacket);
		void __cdecl SetLockStatus(int Drive, int Status);
};

#endif
