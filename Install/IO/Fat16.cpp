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
 * - Rename header file from "memory.h" to "memory_x.h" to make sure that
 *   the XOSL header file is used and not the Open Watcon header file.
 * - To get rid of Warning! W389:  integral value may be truncated during 
 *   assignment or initialization use proper casting in:
 *      MemCopy(Buffer,ClusterData,SizeLeft > ClusterSize ? ClusterSize : SizeLeft)
 *   return Entry.FileSize
 * - To get rid of Error! E592: left operand must be an lvalue (cast produces rvalue)
 *   change (char *)Buffer += ClusterSize
 *   into   Buffer = (char *)Buffer + ClusterSize
 * - To get rid of Warning! W919: delete of a pointer to void
 *   use proper casting in:
 *   delete ClusterData;
 *
 */

#include <fat16.h>
#include <disk.h>
#include <memory_x.h>
#include <string.h>
#include <ctype.h>

#define INMEMORY_CLUSTERS 4096
#define FAT_SECTOR_COUNT  (INMEMORY_CLUSTERS / 256) // 256 clusters/sector

#define INMEMORY_ENTRIES 32
#define DIR_SECTOR_COUNT (INMEMORY_ENTRIES / 16)

#define FILE_DELETED 0xe5


CFAT16::CFAT16(): CFileSystem()
{
	FAT = new unsigned short[INMEMORY_CLUSTERS];
}

CFAT16::~CFAT16()
{
	delete[] FAT;
}

int CFAT16::Mount(int Drive, unsigned long long StartSector)
{
	int Status;

	Status = CFileSystem::Mount(Drive,StartSector);
	if (Status != -1) {
		Disk.Read(0,&BootSector,1);
		if (MemCompare(BootSector.FSID,"FAT16   ",8) != 0)
			Status = -1;
		else {
			ClusterSize = (unsigned short)BootSector.ClusterSize << 9;
			FATStart = BootSector.ReservedSectors;
			DirStart = FATStart + BootSector.FATSize * (int)BootSector.FATCopies;
			DataStart = DirStart + (BootSector.RootEntries >> 4);
			LastCluster = 0;
		}
	}
	return Status;
}

unsigned short CFAT16::ReadFile(const char *FileName, void *Buffer)
{
	unsigned short Cluster;
	CFAT16DirEntry Entry;
	void *ClusterData;
	unsigned long SizeLeft;

	if (Locate(FileName,Entry) == -1)
		return 0;

	if (Entry.FileSize) {
		ClusterData = new char[ClusterSize];
		SizeLeft = Entry.FileSize;
		for (Cluster = Entry.StartCluster; Cluster != 0xffff; GetNextCluster(Cluster)) {
			ReadCluster(Cluster,ClusterData);
			MemCopy(Buffer,ClusterData,SizeLeft > ClusterSize ? ClusterSize : (unsigned short)SizeLeft);
			Buffer = (char *)Buffer + ClusterSize;

			SizeLeft -= ClusterSize;
		}
		delete (char*) ClusterData;
	}
	return (unsigned short) Entry.FileSize;
}

int CFAT16::WriteFile(const char *FileName, const void *Buffer)
{
	unsigned short Cluster;
	CFAT16DirEntry Entry;

	if (Locate(FileName,Entry) == -1)
		return -1;
	if (Entry.FileSize) {
		for (Cluster = Entry.StartCluster; Cluster != 0xffff; GetNextCluster(Cluster)) {
			WriteCluster(Cluster,Buffer);
			Buffer = (const char *)Buffer + ClusterSize;
		}
	}
	return 0;
}

void CFAT16::ReadFAT(unsigned short Cluster)
{
	unsigned long Sector;

	Sector = (Cluster / INMEMORY_CLUSTERS) * FAT_SECTOR_COUNT + FATStart;
	Disk.Read(Sector,FAT,FAT_SECTOR_COUNT);

	FirstCluster = (Cluster / INMEMORY_CLUSTERS) * INMEMORY_CLUSTERS;
	LastCluster = FirstCluster + 4095;
}

void CFAT16::ReadDirectory(unsigned short Index, CFAT16DirEntry *Root)
{
	unsigned long Sector;

	Sector = DirStart + (Index / INMEMORY_ENTRIES) * DIR_SECTOR_COUNT;
	Disk.Read(Sector,Root,DIR_SECTOR_COUNT);
}

int CFAT16::Locate(const char *FileName, CFAT16DirEntry &Entry)
{
	CFAT16DirEntry Root[INMEMORY_ENTRIES];
	unsigned short ABSIndex;
	int Index;

	Index = INMEMORY_ENTRIES;
	for (ABSIndex = 0; ABSIndex < BootSector.RootEntries; ++Index, ++ABSIndex) {
		if (Index == INMEMORY_ENTRIES) {
			Index = 0;
			ReadDirectory(ABSIndex,Root);
		}
		if (!Root[Index].FileName[0])
			return -1;
		if (*Root[Index].FileName != 0xe5)
			if (MemCompare(Root[Index].FileName,FileName,8) == 0 &&
				 MemCompare(Root[Index].Extension,FileName + 8,3) == 0) {
				MemCopy(&Entry,&Root[Index],sizeof (CFAT16DirEntry));
				return 0;
			 }
	}
	return -1;
}


void CFAT16::GetNextCluster(unsigned short &Cluster)
{
	if (Cluster < FirstCluster || Cluster > LastCluster)
		ReadFAT(Cluster);
	Cluster = FAT[Cluster - FirstCluster];
}

void CFAT16::ReadCluster(unsigned short Cluster, void *Buffer)
{
	unsigned long Sector;

	Sector = DataStart + (long)(Cluster - 2) * (long)BootSector.ClusterSize;
	Disk.Read(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT16::WriteCluster(unsigned short Cluster, const void *Buffer)
{
	unsigned long Sector;

	Sector = DataStart + (long)(Cluster - 2) * (long)BootSector.ClusterSize;
	Disk.Write(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT16::DosFileToRawFile(char *RawFile, const char *DosFile)
{
	const char *DotPtr;
	int DotIndex;
	int Index;

	MemSet(RawFile,' ',11);
	RawFile[11] = 0;

	DotPtr = strchr(DosFile,'.');
	DotIndex = DotPtr - DosFile;

	for (Index = 0; Index < DotIndex; ++Index) {
		RawFile[Index] = toupper(DosFile[Index]);
	}
	++DotIndex;
	for (Index = 0; DosFile[Index + DotIndex]; ++Index) {
		RawFile[Index + 8] = toupper(DosFile[Index + DotIndex]);
	}		
}
