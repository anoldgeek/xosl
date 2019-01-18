/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * Open Watcom Migration
 * Copyright (c) 2010 by Mario Looijkens:
 * - Modify code to get rid of Error! E592: left operand must be an lvalue (cast produces rvalue)
 * - Use proper casting to get rid of Warning! W389: integral value may be 
 *   truncated during assignment or initialization
 * - Comment-out unused variables to get rid of Warning! W014: no reference to symbol
 * - Modify code to get rid of Warning! W919: delete of a pointer to void
 *
 */

#include <fat32.h>
#include <cstring.h>
#include <disk.h>
#include <mem.h>

#define INMEMORY_CLUSTERS 4096
#define FAT_SECTOR_COUNT  (INMEMORY_CLUSTERS / 128) // 128 clusters/sector

#define FILE_DELETED 0xe5

CFAT32::CFAT32(): CFileSystem()
{
	FAT = new long[INMEMORY_CLUSTERS];
}

CFAT32::~CFAT32()
{
	delete FAT;
}

int CFAT32::Mount(int Drive, unsigned long long StartSector)
{
	int Status;
	long FATSize;

	Status = CFileSystem::Mount(Drive,StartSector);
	if (Status != -1) {
		Disk->Read(0,&BootSector,1);
		if (memcmp(BootSector.FSID,"FAT32   ",8) != 0)
			Status = -1;
		else {
			ClusterSize = (unsigned short)BootSector.ClusterSize << 9;

			FATSize = BootSector.BigFATSize ? BootSector.BigFATSize : BootSector.FATSize;

			FATStart = BootSector.ReservedSectors;
			DataStart = FATStart + FATSize * (long)BootSector.FATCopies;
			LastCluster = 0;
		}
	}
	return Status;
}

unsigned short CFAT32::ReadFile(const char *FileName, void *Buffer)
{
	long Cluster;
	TFAT32DirEntry Entry;
	char *ClusterData;
	unsigned long SizeLeft;

	if (Locate(FileName,Entry) == -1)
		return 0;
	if (Entry.FileSize) {
		ClusterData = new char[ClusterSize];
		SizeLeft = Entry.FileSize;
		Cluster = (long)Entry.StartClusterL + ((long)Entry.StartClusterH << 16);
		for (; Cluster != 0x0fffffff; GetNextCluster(Cluster)) {
			ReadCluster(Cluster,ClusterData);
			memcpy(Buffer,ClusterData,SizeLeft > ClusterSize ? ClusterSize : (unsigned short) SizeLeft);
			Buffer = (char *)Buffer + ClusterSize;
			SizeLeft -= ClusterSize;
		}
		delete ClusterData;
	}
	return (unsigned short) Entry.FileSize;
}

int CFAT32::WriteFile(const char *FileName, void *Buffer)
{
	long Cluster;
	TFAT32DirEntry Entry;

	if (Locate(FileName,Entry) == -1)
		return -1;
	if (Entry.FileSize) {
		Cluster = (long)Entry.StartClusterL + ((long)Entry.StartClusterH << 16);
		for (; Cluster != 0x0fffffff; GetNextCluster(Cluster)) {
			WriteCluster(Cluster,Buffer);
			Buffer = (char *)Buffer + ClusterSize;
		}
		// Update the Date/Time Stamp
		UpdateFileDateTime(FileName);
	}
	return 0;
}

void CFAT32::ReadFAT(long Cluster)
{
	unsigned long Sector;

	Sector = (Cluster / INMEMORY_CLUSTERS) * FAT_SECTOR_COUNT + FATStart;
	Disk->Read(Sector,FAT,FAT_SECTOR_COUNT);

	FirstCluster = (Cluster / INMEMORY_CLUSTERS) * INMEMORY_CLUSTERS;
	LastCluster = FirstCluster + 4095;
}

void CFAT32::UpdateFileDateTime(const char *FileName)
{
	TFAT32DirEntry *Entries;
	int EntryCount;
	int Index;
	long Cluster;
	unsigned short FatDate;
	unsigned short FatTime;

	EntryCount = ClusterSize / sizeof (TFAT32DirEntry);
	Entries = new TFAT32DirEntry[EntryCount];
	for (Cluster = BootSector.RootCluster; Cluster != 0x0fffffff; GetNextCluster(Cluster)) {
		ReadCluster(Cluster,Entries);
		for (Index = 0; Index < EntryCount; ++Index) {
			if (!Entries[Index].FileName[0]) {
				delete Entries;
				return ;
			}
			if (*Entries[Index].FileName != 0xe5)
				if (memcmp(Entries[Index].FileName,FileName,8) == 0 &&
					 memcmp(Entries[Index].Extension,FileName + 8,3) == 0) {
					
					GetCurFatDateTime(&FatDate, &FatTime);
					Entries[Index].Date = FatDate;
					Entries[Index].Time = FatTime;
					// Now write the update to disk
					WriteCluster(Cluster,Entries);
					return;
				}
		}
	}
	delete Entries;
}


int CFAT32::Locate(const char *FileName, TFAT32DirEntry &Entry)
{
	TFAT32DirEntry *Entries;
	int EntryCount;
	int Index;
	long Cluster;

	EntryCount = ClusterSize / sizeof (TFAT32DirEntry);
	Entries = new TFAT32DirEntry[EntryCount];
	for (Cluster = BootSector.RootCluster; Cluster != 0x0fffffff; GetNextCluster(Cluster)) {
		ReadCluster(Cluster,Entries);
		for (Index = 0; Index < EntryCount; ++Index) {
			if (!Entries[Index].FileName[0]) {
				delete Entries;
				return -1;
			}
			if (*Entries[Index].FileName != 0xe5)
				if (memcmp(Entries[Index].FileName,FileName,8) == 0 &&
					 memcmp(Entries[Index].Extension,FileName + 8,3) == 0) {
					memcpy(&Entry,&Entries[Index],sizeof (TFAT32DirEntry));
					delete Entries;
					return 0;
				}
		}
	}
	delete Entries;
	return -1;
}


void CFAT32::GetNextCluster(long &Cluster)
{
	if (Cluster < FirstCluster || Cluster > LastCluster)
		ReadFAT(Cluster);
	Cluster = FAT[Cluster - FirstCluster];
}

void CFAT32::ReadCluster(long Cluster, void *Buffer)
{
	unsigned long Sector;

	Sector = DataStart + (long)(Cluster - 2) * (long)BootSector.ClusterSize;
	Disk->Read(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT32::WriteCluster(long Cluster, void *Buffer)
{
	unsigned long Sector;

	Sector = DataStart + (long)(Cluster - 2) * (long)BootSector.ClusterSize;
	Disk->Write(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT32::GetCurFatDateTime(unsigned short *pfatdate,unsigned short *pfattime)
{
	unsigned short bcdhrmin;
	unsigned char bcdsec;

	unsigned short bcdcentyr;
	unsigned short bcdmonday;

	unsigned short fatdate;
	unsigned short fattime;

	int temp;

	_asm{
		mov	ah,2 //; get rtc time 
		int 1ah
		mov	bcdhrmin,cx
		mov	bcdsec,dh

		mov	ah,4 //; get rtc date
		int	1ah
		mov	bcdcentyr,cx
		mov	bcdmonday,dx
	}
	fattime = ((bcdsec & 0xf) + ((( bcdsec >> 4)  & 0xf ) * 10) >> 1); // convert bcd sec ( 2 sec resolotion)

	fattime = fattime + (( (bcdhrmin & 0xf) + (( bcdhrmin >> 4)  & 0xf ) * 10) << 5); // convert bcd min
	fattime = fattime + (( ((bcdhrmin >> 8 ) & 0xf) + (( bcdhrmin >> 12)  & 0xf ) * 10) << 11); // convert bcd hr


	fatdate = (bcdmonday & 0xf)  + (( bcdmonday >> 4) & 0xf) * 10; // convert bcd day
	fatdate = fatdate + (( ( (bcdmonday >> 8) & 0xf)  + (( bcdmonday >> 12) & 0xf) * 10) << 5);  // convert bcd month

	temp = (bcdcentyr & 0xf)  + (( bcdcentyr >> 4) & 0xf) * 10; // convert bcd year
	temp = temp +  ( ((bcdcentyr >> 8 ) & 0xf) * 100  + (( bcdcentyr >> 12 ) & 0xf) * 1000 ); // convert bcd centry
	temp = temp - 1980; // Convert to DOS epoch date
	fatdate = fatdate + (temp << 9);
	
	*pfatdate = fatdate;
	*pfattime = fattime;
}
