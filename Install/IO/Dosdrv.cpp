/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#include <dosdrv.h>
#include <bootrec.h>
#include <disk.h>
#include <fatfix.h>
#include <main.h>
#include <data.h>

#define GetFATType(FSType) \
	(FSType == 0x06 || FSType == 0x0e ? FATTYPE_FAT16 : FATTYPE_FAT32)

CDosDriveList::CDosDriveList(CPartList &PartListToUse):
	PartList(PartListToUse)
{
}

CDosDriveList::~CDosDriveList()
{
}

int CDosDriveList::LocateDrive(int Drive, CDosDrive &DosDrive)
{
	int Index;
	int PartCount;
	unsigned long DosSerialNo;
	unsigned long BRecSerialNo;
	const TPartition *Partition;

	DosDrive.DriveChar = Drive + 'C';
	if ((DosSerialNo = GetDosSerialNo(Drive)) == (unsigned long)-1)
		return -1;
	PartCount = PartList.GetCount();
	for (Index = 0; Index < PartCount; ++Index)
		if ((BRecSerialNo = GetBRecSerialNo(Index)) != (unsigned long)-1 &&
			 BRecSerialNo == DosSerialNo) {
			Partition = PartList.GetPartition(Index);
			DosDrive.FATType = GetFATType(Partition->FSType);
			DosDrive.Drive = Partition->Drive;
			DosDrive.StartSector = Partition->StartSector;
			DosDrive.DosDriveSector0 = Partition->MbrHDSector0;
			return 0;
		}
	return -1;
}

void CDosDriveList::UpdateDosDriveMbrHDSector0(CDosDrive &DosDrive, unsigned char MbrHDSector0)
{
	int Index;
	int PartCount;
	unsigned long DosSerialNo;
	unsigned long BRecSerialNo;
	const TPartition *Partition;
	int Drive;

	Drive = DosDrive.DriveChar - 'C';
	if ((DosSerialNo = GetDosSerialNo(Drive)) == (unsigned long)-1)
		return;
	PartCount = PartList.GetCount();
	for (Index = 0; Index < PartCount; ++Index){
		if ((BRecSerialNo = GetBRecSerialNo(Index)) != (unsigned long)-1 &&
				BRecSerialNo == DosSerialNo) {
			PartList.UpdatePartitionMbrHDSector0(Index,MbrHDSector0);
			return;
		}
	}
}


int CDosDriveList::GetDosDriveSector0(int Drive)
{
	CDosDrive DosDrive;

	if(LocateDrive(Drive, DosDrive) != -1 )
		return DosDrive.DosDriveSector0;
	return -1;
}

unsigned long CDosDriveList::GetBRecSerialNo(int Index)
{
	TBootRecord BootRecord;
	const TPartition *Partition;
	CDisk Disk;

	Partition = PartList.GetPartition(Index);
	if (Disk.Map(Partition->Drive,Partition->StartSector) == -1)
		return (unsigned long)-1;
	if (Disk.Read(0,&BootRecord,1) == -1)
		return (unsigned long)-1;
	switch (Partition->FSType) {
		case 0x06: // FAT16
		case 0x0e: // FAT16 LBA
			return BootRecord.BootFAT16.SerialNo;
		case 0x0b: // FAT32
		case 0x0c: // FAT32 LBA
			return BootRecord.BootFAT32.SerialNo;
		default:
			return (unsigned long) -1;
	}
}

int CDosDriveList::GetDriveIndex(TPartition *Partition, TBootRecord &BootRecord)
{
	unsigned long BRecSerialNo;
	unsigned long DosSerialNo;
	int DriveCount;

	if(Partition->Type == PART_GPT)
		return -1;
	// Get BRecSerialNo
	switch (Partition->FSType) {
		case 0x06: // FAT16
		case 0x0e: // FAT16 LBA
			BRecSerialNo = BootRecord.BootFAT16.SerialNo;
			break;
		case 0x0b: // FAT32
		case 0x0c: // FAT32 LBA
			BRecSerialNo = BootRecord.BootFAT32.SerialNo;
			break;
		default:
			BRecSerialNo = (unsigned long) -1;
			return -1;
	}

	// Get drive index
	DriveCount = CData::GetDriveCount() - ( 'C' - 'A'); 
	for(int Drive = 0; Drive <= DriveCount ; ++Drive){
		if ((DosSerialNo = GetDosSerialNo(Drive)) == (unsigned long)-1)
			continue;
		if ( DosSerialNo == BRecSerialNo )
			return Drive;
	}
	return -1;
}


