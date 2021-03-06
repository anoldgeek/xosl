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
 * - Modify code to get rid of Error! E592: left operand must be an 
 *   lvalue (cast produces rvalue)
 * - Use proper casting to get rid of Warning! W389: integral value may be 
 *   truncated during assignment or initialization
 * - Comment-out unused variables to get rid of Warning! W014: 
 *   no reference to symbol
 * - Modify code to get rid of Warning! W919: delete of a pointer to void
 *
 */

#include <fat16.h>
#include <cstring.h>
#include <disk.h>
#include <mem.h>

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
	delete FAT;
}

int CFAT16::Mount(int Drive, unsigned long long StartSector)
{
	int Status;

	Status = CFileSystem::Mount(Drive,StartSector);
	if (Status != -1) {
		Disk->Read(0,&BootSector,1);
		if (memcmp(BootSector.FSID,"FAT16   ",8) != 0)
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
	TFAT16DirEntry Entry;
	void *ClusterData;
	unsigned long SizeLeft;

	if (Locate(FileName,Entry) == -1)
		return 0;
	if (Entry.FileSize) {
		ClusterData = new char[ClusterSize];
		SizeLeft = Entry.FileSize;
		for (Cluster = Entry.StartCluster; Cluster != 0xffff; GetNextCluster(Cluster)) {
			ReadCluster(Cluster,ClusterData);
			memcpy(Buffer,ClusterData,SizeLeft > ClusterSize ? ClusterSize : (unsigned short)SizeLeft);
			Buffer = (char *)Buffer + ClusterSize;
			SizeLeft -= ClusterSize;
		}
		delete (char*)ClusterData;
	}
	return (unsigned short)Entry.FileSize;
}

void CFAT16::UpdateFileDateTime(const char *FileName)
{
	TFAT16DirEntry Root[INMEMORY_ENTRIES];
	unsigned short ABSIndex;
	int Index;
	unsigned short FatDate;
	unsigned short FatTime;


	Index = INMEMORY_ENTRIES;
	for (ABSIndex = 0; ABSIndex < BootSector.RootEntries; ++Index, ++ABSIndex) {
		if (Index == INMEMORY_ENTRIES) {
			Index = 0;
			ReadDirectory(ABSIndex,Root);
		}
		if (!Root[Index].FileName[0])
			return;
		if (*Root[Index].FileName != 0xe5){
			if (memcmp(Root[Index].FileName,FileName,8) == 0 &&
				 memcmp(Root[Index].Extension,FileName + 8,3) == 0) {

				GetCurFatDateTime(&FatDate, &FatTime);
				Root[Index].Date = FatDate;
				Root[Index].Time = FatTime;
				// Now write the update to disk
				WriteDirectory(ABSIndex, Root);
			 }
		}
	}
}

int CFAT16::WriteFile(const char *FileName, void *Buffer)
{
	unsigned short Cluster;
	TFAT16DirEntry Entry;

	if (Locate(FileName,Entry) == -1)
		return -1;
	if (Entry.FileSize) {
		for (Cluster = Entry.StartCluster; Cluster != 0xffff; GetNextCluster(Cluster)) {
			WriteCluster(Cluster,Buffer);
			Buffer = (char *)Buffer + ClusterSize;
		}
		UpdateFileDateTime(FileName);
	}
	return 0;
}

void CFAT16::ReadFAT(unsigned short Cluster)
{
	unsigned long Sector;

	Sector = (Cluster / INMEMORY_CLUSTERS) * FAT_SECTOR_COUNT + FATStart;
	Disk->Read(Sector,FAT,FAT_SECTOR_COUNT);

	FirstCluster = (Cluster / INMEMORY_CLUSTERS) * INMEMORY_CLUSTERS;
	LastCluster = FirstCluster + 4095;
}

void CFAT16::ReadDirectory(unsigned short Index, TFAT16DirEntry *Root)
{
	unsigned long Sector;

	Sector = DirStart + (Index / INMEMORY_ENTRIES) * DIR_SECTOR_COUNT;
	Disk->Read(Sector,Root,DIR_SECTOR_COUNT);
}

void CFAT16::WriteDirectory(unsigned short Index, TFAT16DirEntry *Root)
{
	unsigned long Sector;

	Sector = DirStart + (Index / INMEMORY_ENTRIES) * DIR_SECTOR_COUNT;
	Disk->Write(Sector,Root,DIR_SECTOR_COUNT);
}



int CFAT16::Locate(const char *FileName, TFAT16DirEntry &Entry)
{
	TFAT16DirEntry Root[INMEMORY_ENTRIES];
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
			if (memcmp(Root[Index].FileName,FileName,8) == 0 &&
				 memcmp(Root[Index].Extension,FileName + 8,3) == 0) {
				memcpy(&Entry,&Root[Index],sizeof (TFAT16DirEntry));
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
	Disk->Read(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT16::WriteCluster(unsigned short Cluster, void *Buffer)
{
	unsigned long Sector;

	Sector = DataStart + (long)(Cluster - 2) * (long)BootSector.ClusterSize;
	Disk->Write(Sector,Buffer,BootSector.ClusterSize);
}

void CFAT16::GetCurFatDateTime(unsigned short *pfatdate,unsigned short *pfattime)
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
