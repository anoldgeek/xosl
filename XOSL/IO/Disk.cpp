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
 * - Use proper casting to get rid of Warning! W389: integral value may be 
 *   truncated during assignment or initialization
 *     Sector = RSector % DrvSectorCount + 1;
 *     Head = RSector % DrvHeadCount;
 *     Cylinder = RSector / DrvHeadCount;
 * - Comment-out unused variables to get rid of Warning! W014: no reference to symbol
 *
 */

#include <disk.h>
#include <mem.h>
#include <transfer.h>

CDisk::CDisk()
{
}

CDisk::~CDisk()
{
}

int CDisk::DriveCount(int Fixed)
{
	if (Fixed)
		return DiskAccess.DriveCount(0x80);
	return DiskAccess.DriveCount(0x00);
}

int CDisk::Map(int Drive, unsigned long long StartSector)
{
	//int Status;

	this->Drive = Drive;
	this->StartSector = StartSector;
	if (Drive >= 0x80 && DiskAccess.LBAAccessAvail(Drive) == 0)
		UseLBA = 1;
	else {
		if (DiskAccess.GetDriveInfo(Drive,DrvHeadCount,DrvSectorCount) == -1) {
			DiskMapped = 0;
			return -1;
		}
		UseLBA = 0;
	}
	DiskMapped = 1;
	return 0;
}

int CDisk::Read(unsigned long long Sector, void *Buffer, int Count)
{
	int Status;

	if (!DiskMapped)
		return -1;
	Status = Transfer(DISK_READ,Sector,Buffer,Count);
//	Status = Transfer(DISK_READ,Sector,Scratchpad,Count);
//	DiskAccess.CopyFromScratchpad(Buffer,Count);
	return Status;
}


int CDisk::Write(unsigned long long Sector, void *Buffer, int Count)
{
	if (!DiskMapped)
		return -1;
//	DiskAccess.CopyToScratchpad(Buffer,Count);
//	return Transfer(DISK_WRITE,Sector,Scratchpad,Count);
	return Transfer(DISK_WRITE,Sector,Buffer,Count);
}

int CDisk::Verify(unsigned long long Sector, int Count)
{
	if (!DiskMapped)
		return -1;
	return Transfer(DISK_VERIFY,Sector,NULL,Count);
}

int CDisk::Transfer(int Action, unsigned long long Sector, void *Buffer, int Count)
{
	TLBAPacket LBAPacket;
	unsigned short SectCyl, DrvHead;

	if (UseLBA) {
		LBAPacket.PacketSize = 0x0010;
		LBAPacket.SectorCount = Count;
		LBAPacket.TransferBuffer = Buffer;
//		LBAPacket.SectorLow = Sector + StartSector;
//		LBAPacket.SectorHigh = 0;
		LBAPacket.Sector = Sector + StartSector;

		return DiskAccess.LBATransfer(Action,Drive,LBAPacket);
	}
	if(Sector >> 32 != 0){
		// /2TB Can't use CHS on drives > 2TB
		return -1;
	}
	Sector2CHS((unsigned long)Sector,SectCyl,DrvHead);
	return DiskAccess.Transfer(Action,SectCyl,DrvHead,Buffer,Count);
}


void CDisk::Sector2CHS(unsigned long RSector, unsigned short &SectCyl, unsigned short &DrvHead)
{
	int Cylinder;
	int Sector;
	int Head;

	RSector += (unsigned long)StartSector;

	Sector = (int)(RSector % DrvSectorCount + 1);
	RSector /= DrvSectorCount;
	Head = (int)(RSector % DrvHeadCount);
	Cylinder = (int)(RSector / DrvHeadCount);

	DrvHead = Drive | (Head << 8);
	SectCyl = Sector | ((Cylinder & 0xff) << 8) | ((Cylinder >> 8) << 6);
}
