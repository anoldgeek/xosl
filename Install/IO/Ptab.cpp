/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#include <ptab.h>
#include <transfer.h>
#include <disk.h>

#include <fat16.h>
#include <bootrec.h>
#include <items.h>
//#include <mem.h>
#include <memory.h>
#include <dosio.h>
#include <files.h>
#include <xosldata.h>

#define FSTYPE_EXTENDED 0x05
#define FSTYPE_EXTENDEDLBA 0x0f
#define FSTYPE_LINUXEXT 0x85
#define FSTYPE_HIDDENEXTENDED 0x1f


CPartList::CPartList()
{
	AllowActiveHD = 0;
}

CPartList::~CPartList()
{
}

void CPartList::ReadStructure()
{
	int DrvCount, Index;
	TMBRNode *MBRList;
	CDiskAccess DiskAccess;

	MBRList = &this->MBRList;
	DrvCount = DiskAccess.DriveCount(0x80);
	for (Index = 0; Index < DrvCount; ++Index)
		MBRList = AddDrive(Index | 0x80,0,0,PART_PRIMARY,MBRList);
	MBRList->Next = NULL;
	CreatePartList(DiskAccess.DriveCount(0x00));
	// Create PartList Look-up Table
	CreatePLUP();
}

TMBRNode *CPartList::AddDrive(int Drive, unsigned long StartSector, unsigned long ExtStart, int Type, TMBRNode *MBRList)
{
	CDisk Disk;
	TPartTable *PartTable;
	const TPartEntry *Entries;
	int Index;
	unsigned long NewStartSector;

	if (Disk.Map(Drive,StartSector) == -1)
		return MBRList;
	PartTable = new TPartTable;
	if (Disk.Read(0,PartTable,1) == -1 || PartTable->MagicNumber != 0xaa55) {
		delete PartTable;
		return MBRList;
	}
	MBRList = MBRList->Next = new TMBRNode;
	MBRList->AbsoluteSector = StartSector;
	MBRList->Drive = Drive;
	MBRList->Type = Type;
	MBRList->Table = PartTable;
	Entries = MBRList->Table->Entries;
	for (Index = 0; Index < 4; ++Index)
		switch (Entries[Index].FSType) {
			case FSTYPE_EXTENDED:
			case FSTYPE_EXTENDEDLBA:
			case FSTYPE_LINUXEXT:
			case FSTYPE_HIDDENEXTENDED:
				if (Type != PART_LOGICAL) {
					ExtStart = Entries[Index].RelativeSector;
					NewStartSector = ExtStart;
				}
				else
					NewStartSector = ExtStart + Entries[Index].RelativeSector;
				MBRList = AddDrive(Drive,NewStartSector,ExtStart,PART_LOGICAL,MBRList);
				break;
			default:
				break;
		}
	return MBRList;
}



void CPartList::CreatePartList(int FloppyCount)
{
	TPartNode *PartList;
	TMBRNode *MBRNode;
	int Index, Drive;
	const TPartEntry *Entries;

	Count = 0;
	PartList = &this->PartList;
	if (MBRList.Next)
		Drive = MBRList.Next->Drive;
	for (MBRNode = MBRList.Next; MBRNode; MBRNode = MBRNode->Next) {
		if (Drive != MBRNode->Drive) {
			Drive = MBRNode->Drive;
			PartList = PartList->Next = CreateNonPartNode(Drive);
			++Count;
		}
		for (Index = 0; Index < 4; ++Index) {
			Entries = &MBRNode->Table->Entries[Index];
			if (Entries->RelativeSector && ((Entries->FSType != FSTYPE_EXTENDED &&
				Entries->FSType != FSTYPE_EXTENDEDLBA && Entries->FSType != FSTYPE_LINUXEXT) ||
				MBRNode->Type != PART_LOGICAL)) {
				// ( RelativeSector && (( ! EXTENDED && ! EXTENDEDLBA && ! LINUXEXT ) || ! PART_LOGICAL )  )
				PartList = PartList->Next = CreatePartNode(MBRNode,Index);
				++Count;
			}
		}
	}
	for (Index = 0; Index < FloppyCount; ++Index)
		PartList = PartList->Next = CreateNonPartNode(Index);
	Count += FloppyCount;
	PartList->Next = NULL;
}


TPartNode *CPartList::CreateNonPartNode(int Drive)
{
	TPartNode *PartNode;
	TPartition *Partition;

	PartNode = new TPartNode;
	Partition = PartNode->Partition = new TPartition;
	PartNode->Entry = NULL;

	Partition->Drive = Drive;
	Partition->StartSector = 0;
	Partition->SectorCount = 0;
	if (Drive >= 0x80) {
		Partition->FSName = "Master Boot Record";
		Partition->FSType = -1;
		Partition->Type = PART_MBR;
	}
	else {
		Partition->FSName = "Boot floppy";
		Partition->FSType = -1;
		Partition->Type = PART_FLOPPY;
	}
	return PartNode;
}

TPartNode *CPartList::CreatePartNode(const TMBRNode *MBRNode, int Index)
{
	TPartNode *PartNode;
	TPartition *Partition;
	const TPartEntry *PartEntry;

	PartNode = new TPartNode;
	Partition = PartNode->Partition = new TPartition;
	PartEntry = PartNode->Entry = &MBRNode->Table->Entries[Index];

	Partition->Drive = MBRNode->Drive;
	Partition->StartSector = MBRNode->AbsoluteSector + PartEntry->RelativeSector;
	Partition->SectorCount = PartEntry->SectorCount;
	Partition->FSName = GetFSName(PartEntry->FSType);
	Partition->FSType = PartEntry->FSType;
	Partition->Type = MBRNode->Type;
	GetPartMbrHDSector0(Partition);
	return PartNode;
}



const char *CPartList::GetFSName(int FSID)
{
	int Index;

	for (Index = 0; FSNameList[Index].FSID != FSID && FSNameList[Index].FSID != 0xff; ++Index);
	return FSNameList[Index].FSName;
}

void CPartList::CreatePLUP()
{
	int Index;
	TPartNode *PartList;

	PartList = this->PartList.Next;
	PLUP = new TPartNode *[Count];
	for (Index = 0; Index < Count; ++Index, PartList = PartList->Next)
		PLUP[Index] = PartList;
}

void CPartList::WriteStructure()
{
	CDisk Disk;
	TMBRNode *MBRList;

	for (MBRList = this->MBRList.Next; MBRList; MBRList = MBRList->Next) {
		Disk.Map(MBRList->Drive,MBRList->AbsoluteSector);
		Disk.Write(0,MBRList->Table,1);
	}
}

const TPartition *CPartList::GetPartition(int Index)
{
	return PLUP[Index]->Partition;
}

int CPartList::Locate(int Drive, unsigned long StartSector)
{
	int Index;
	const TPartition *Partition;

	for (Index = 0; Index < Count; ++Index) {
		Partition = PLUP[Index]->Partition;
		if (Partition->Drive == Drive && Partition->StartSector == StartSector)
			return Index;
	}
	return -1;
}

int CPartList::GetCount()
{
	return Count;
}

int CPartList::CanHide(int Index)
{
	switch (PLUP[Index]->Entry->FSType & 0xef) {
		case 0x01:
		case 0x04:
		case 0x06:
		case 0x07:
		case 0x0b:
		case 0x0c:
		case 0x0e:
		case 0x0f:
			return 1;
		default:
			return 0;
	}
}

void CPartList::Hide(int Index)
{
	if (CanHide(Index))
		PLUP[Index]->Entry->FSType |= 0x10;
}

void CPartList::Reveal(int Index)
{
	if (CanHide(Index))
		PLUP[Index]->Entry->FSType &= 0xef;
}

int CPartList::CanActivate(int Index)
{
	int Type;

	Type = PLUP[Index]->Partition->Type;
	return Type != PART_MBR && Type != PART_FLOPPY;
}

void CPartList::SetAllowActiveHD(int Status)
{
	AllowActiveHD = Status;
}

void CPartList::ClearActive(int Drive)
{
	int Index;

	if (!AllowActiveHD)
		// clear active flag for all partitions
		for (Index = 0; Index < Count; ++Index)
			PLUP[Index]->Entry->Activated = 0x00;
	else
		// clear active flag only for all partitions on same drive
		for (Index = 0; Index < Count; ++Index)
			if (PLUP[Index]->Partition->Drive == Drive)
				PLUP[Index]->Entry->Activated = 0x00;
}

void CPartList::SetActive(int Index)
{
	ClearActive(PLUP[Index]->Partition->Drive);
	PLUP[Index]->Entry->Activated = 0x80;
}

void CPartList::SetFsType(int Index, int FsType)
{
	PLUP[Index]->Entry->FSType = FsType;
}


CPartList::TFSNameEntry CPartList::FSNameList[] = {
	{0x01,"Microsoft FAT12"},
	{0x04,"Microsoft FAT16"},
	{0x05,"Extended"},
	{0x06,"Microsoft FAT16"},
	{0x07,"HPFS or NTFS"},
	{0x0a,"OS/2 Boot Manager"},
	{0x0b,"Microsoft FAT32"},
	{0x0c,"Microsoft FAT32 LBA"},
	{0x0e,"Microsoft FAT16 LBA"},
	{0x0f,"Extended LBA"},
	{0x11,"Hidden FAT12"},
	{0x14,"Hidden FAT16"},
	{0x16,"Hidden FAT16"},
	{0x17,"Hidden NTFS"},
	{0x1b,"Hidden FAT32"},
	{0x1c,"Hidden FAT32 LBA"},
	{0x1e,"Hidden FAT16 LBA"},
	{0x1f,"Hidden Extended LBA"},
	{0x63,"Unix SysV/386"},
	{0x78,"XOSL FS"},
	{0x82,"Linux Swap"},
	{0x83,"Linux Native"},
	{0x85,"Linux Extended"},
	{0xa5,"FreeBSD, BSD/386"},
	{0xeb,"BeOS"},
	{0xff,"Unknown"}
};
/*
typedef	struct {
		unsigned char Jump[3];			// jmp short + nop
		unsigned char OEM_ID[8];		// XOSLINST
		unsigned short SectorSize;		// 512
		unsigned char ClusterSize;		// 16 (8192 byte)
		unsigned short ReservedSectors;	// 1
		unsigned char FATCopies;		// 1
		unsigned short RootEntries;		// 32
		unsigned short SectorCount;		// ?
		unsigned char MediaDescriptor;	// 0xF8 (?) 
		unsigned short FATSize;			// 1
		unsigned short TrackSize;		// ? (sectors per head)
		unsigned short HeadCount;		// ?
		unsigned long HiddenSectors;	// ? (partition offset)
		unsigned long BigSectorCount;	// 0 (total sectors < 65536)
		unsigned short Drive;			// ?
		unsigned char Signature;		// 0x29 (?)
		unsigned long SerialNo;			// 0x4c534f58 (don't really care)
		unsigned char Label[11];		// XOSL110
		unsigned char FSID[8];			// FAT16
		unsigned char Loader[448];		// IPL
		unsigned short MagicNumber;		// 0x534f (used by XOSLLOAD)
	} TBootRecord;
*/

//int memcmp(const void far *s1, const void far *s2, size_t count);
int far memcmp(const void *s1, const void *s2, unsigned short count);
#define BOOTITEM_FILESIZE 4096

void CPartList::GetPartMbrHDSector0(TPartition *Partition)
{
	CDisk Disk;
	CFAT16 *FileSystem = new CFAT16;
	TBootRecord BootRecord;
	CBootItemFile *BootItemData = new CBootItemFile;
	
	// Does this partition have sep XOSL installed
	if(Disk.Map(Partition->Drive, Partition->StartSector) != -1)
		if(Disk.Read(0, &BootRecord,1) != -1 )
			if(MemCompare(BootRecord.BootFAT16.OEM_ID,"XOSLINST",8) == 0 )
				if(FileSystem->Mount(Partition->Drive,Partition->StartSector) != -1 )
					if (FileSystem->ReadFile("BOOTITEMXDF",BootItemData) == BOOTITEM_FILESIZE )
						Partition->MbrHDSector0 = BootItemData->MbrHDSector0;
	delete FileSystem;
	delete BootItemData;
}

int CPartList::UpgradeXoslBootItem(const TPartition *Partition,unsigned char MbrHDSector0)
{
	CDisk Disk;
	CDosFile DosFile;
	CXoslFiles XoslFiles;
	CFAT16 *FileSystem = new CFAT16;
	TBootRecord BootRecord;
	CBootItemFile *BootItemData = new CBootItemFile;
	int fh;
	char XoslFsFileName[13];
	
	ConvertDOS2XoslFsName(XoslFiles.GetBootItemName(),XoslFsFileName);
	// Does this partition have sep XOSL installed
	if(Disk.Map(Partition->Drive, Partition->StartSector) != -1){
		if(Disk.Read(0, &BootRecord,1) != -1 ){
			if(MemCompare(BootRecord.BootFAT16.OEM_ID,"XOSLINST",8) == 0 ){
				if(FileSystem->Mount(Partition->Drive,Partition->StartSector) != -1 ){
					if (FileSystem->ReadFile(XoslFsFileName,BootItemData) == BOOTITEM_FILESIZE ){
						BootItemData->MbrHDSector0 = MbrHDSector0;
						fh = DosFile.Create(XoslFiles.GetBootItemName());
						if( fh != -1 ) {
							if(DosFile.Write(fh,BootItemData,BOOTITEM_FILESIZE) == BOOTITEM_FILESIZE){
								DosFile.Close(fh);
								delete FileSystem;
								delete BootItemData;
								return 0;
							}
						}
					}
				}
			}
		}
	}
	delete FileSystem;
	delete BootItemData;
	return -1;
}

int CPartList::Retain(const char *DosFileName,unsigned short FileSize,const TPartition *Partition)
{
	CDisk Disk;
	CDosFile DosFile;
	CFAT16 *FileSystem = new CFAT16;
	TBootRecord BootRecord;
	int fh;
	char XoslFsFileName[13];

	if(FileSize > sizeof CDosFile::TransferBuffer)
		return -1;

	ConvertDOS2XoslFsName(DosFileName,XoslFsFileName);
	// Does this partition have sep XOSL installed
	if(Disk.Map(Partition->Drive, Partition->StartSector) != -1){
		if(Disk.Read(0, &BootRecord,1) != -1 ){
			if(MemCompare(BootRecord.BootFAT16.OEM_ID,"XOSLINST",8) == 0 ){
				if(FileSystem->Mount(Partition->Drive,Partition->StartSector) != -1 ){
					if (FileSystem->ReadFile(XoslFsFileName,CDosFile::TransferBuffer) == FileSize ){
						if( (fh=DosFile.Create(DosFileName)) != -1 ){
							if(DosFile.Write(fh,CDosFile::TransferBuffer,FileSize) == FileSize){
								DosFile.Close(fh);
								delete FileSystem;
								return 0;
							}
						}
					}
				}
			}
		}
	}
	delete FileSystem;
	return -1;
}
void CPartList::ConvertDOS2XoslFsName(const char *DosFileName, char *XoslFsFileName)
{
	int i;

	for(i = 0 ; i < 8 ; i++){
		if(*DosFileName && *DosFileName != '.'){
			*XoslFsFileName++ = *DosFileName++;
		}else{
			*XoslFsFileName++ = ' ';
		}
	}
	if(*DosFileName == '.')
		DosFileName++;
	for(i = 0 ; i < 3 ; i++){
		if(*DosFileName) {
			*XoslFsFileName++ = *DosFileName++;
		}else{
			*XoslFsFileName++ = ' ';
		}
	}
	*XoslFsFileName = '\0';
}
