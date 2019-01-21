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
#include <mem.h>
#include <memory_x.h>
#include <dosio.h>
#include <files.h>
#include <xosldata.h>
#include <gptabdata.h>

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
int CPartList::GetGPTIndex(uuid_t GPTType)
{
	int i;
	for(i = 0; i++;){
		if (gpt_fstypes[i].MBRType == 0xffff){
			return 0;
		}
		if(memcmp(&GPTType,&gpt_fstypes[i].GUIDype,UUID_SIZE) == 0 ){
			return i;
		}

	}
	return 0;
}

int CPartList::GetGPTShortType(uuid_t GPTType)
{
	int i;
	for(i = 0; ;i++){
		if (gpt_fstypes[i].MBRType == 0xffff){
			return 0;
		}
		if(memcmp(&GPTType,&gpt_fstypes[i].GUIDype,UUID_SIZE) == 0 ){
			return gpt_fstypes[i].MBRType;
		}
	}
	return 0;
}

TMBRNode *CPartList::AddDrive(int Drive, unsigned long long StartSector, unsigned long ExtStart, int Type, TMBRNode *MBRList)
{
	CDisk Disk;
	TPartTable *PartTable;
	const TPartEntry *Entries;
	int Index;
	unsigned long long NewStartSector;
	unsigned int i;
	unsigned int gpt_sector;

	if (Disk.Map(Drive,StartSector) == -1)
		return MBRList;
	PartTable = new TPartTable;
	if (Disk.Read(0,PartTable,1) == -1 || PartTable->mbr.MagicNumber != 0xaa55) {
		delete PartTable;
		return MBRList;
	}
	// gpt code from grub:2 grub-core/partmap/gpt.c
	// Check to see if the MBR is a protective MBR for GPT disk and not a normal MBR.
	for (i=0;i < 4 ; i++ ){
		if(PartTable->mbr.Entries[0].FSType == 0xee){
			break;
		}
	}
	if (i!=4){
		// Check for GPT Protective MBR
		char *gpt_magic = "EFI PART";
		int j;
		gpt_header_t *gpth;
//		gpt_partentry_t *gpart_entry;
//		TGPTTable *GPTTable;
		int last_offset = 0;

		// Add the Protective MBR
		MBRList = MBRList->Next = new TMBRNode;
		MBRList->AbsoluteSector = StartSector;
		MBRList->Drive = Drive;
		MBRList->Type = PART_GPT_PROT_MBR;
		MBRList->Table = PartTable;

		PartTable = new TPartTable;
//		gpth = new gpt_header_t;
//		unsigned __int64 guid = 0x0ULL;

		/* Read the GPT header.  */
		// Code assume 512 byte logical sectors on disc
		if (Disk.Read(1,PartTable,1) == -1 ) {
			delete PartTable;
			return MBRList;
		}

		if (memcmp (PartTable->gpth.magic, gpt_magic, sizeof (gpt_magic)) != 0){
			delete PartTable;
			return MBRList;
		}
		// Add the gpt header
		MBRList = MBRList->Next = new TMBRNode;
		MBRList->AbsoluteSector = 1;
		MBRList->Drive = Drive;
		MBRList->Type = PART_GPT_HEADER;
		MBRList->Table = PartTable;

	
		gpth = &PartTable->gpth;

		// GPT disk. This code only handles little endian
		// Code assume 512 byte logical sectors on disc
		for (j = 0, gpt_sector = 2; j < gpth->maxpart ; gpt_sector++ , j+=4 ){
			PartTable = new TPartTable;
			if (Disk.Read(gpt_sector,PartTable,1) == -1 ) {
				delete PartTable;
				return MBRList;
			}
			MBRList = MBRList->Next = new TMBRNode;
			MBRList->AbsoluteSector = gpt_sector;
			MBRList->Drive = Drive;
			MBRList->Type = PART_GPT;
			MBRList->Table = PartTable;
/*
			for (j = 0;j < 4 && i < gpth->maxpart ; j++, i++ ){
				if (GetGPTShortType(GPTTable->Entries[j].type) != 0){
					// linux filesystem
					gpart_entry = new gpt_partentry_t;
					memcpy(gpart_entry,&GPTTable->Entries[j],sizeof(gpt_partentry_t) );

					MBRList = MBRList->Next = new TMBRNode;
					MBRList->AbsoluteSector = gpart_entry->start;
					MBRList->Drive = Drive;
					MBRList->Type = PART_GPT;
					MBRList->Table = (TPartTable*) gpart_entry;
				}
			}
*/
		}
		return MBRList;
	}
	// MBR disk
	MBRList = MBRList->Next = new TMBRNode;
	MBRList->AbsoluteSector = StartSector;
	MBRList->Drive = Drive;
	MBRList->Type = Type;
	MBRList->Table = PartTable;
	Entries = MBRList->Table->mbr.Entries;
	for (Index = 0; Index < 4; ++Index)
		switch (Entries[Index].FSType) {
			case FSTYPE_EXTENDED:
			case FSTYPE_EXTENDEDLBA:
			case FSTYPE_LINUXEXT:
			case FSTYPE_HIDDENEXTENDED:
				if (Type != PART_LOGICAL) {
					ExtStart = (unsigned long)Entries[Index].RelativeSector;
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
		if (MBRNode->Type == PART_GPT){
			// GPT Disk
			for (Index = 0 ; Index < 4 ; ++Index){
				PartList = PartList->Next = CreateGPTPartNode(MBRNode,Index);
				++Count;
			}
		}else{
			// MBR Disk
			for (Index = 0; Index < 4; ++Index) {
				Entries = &MBRNode->Table->mbr.Entries[Index];
				if (Entries->RelativeSector && ((Entries->FSType != FSTYPE_EXTENDED &&
					Entries->FSType != FSTYPE_EXTENDEDLBA && Entries->FSType != FSTYPE_LINUXEXT) ||
					MBRNode->Type != PART_LOGICAL)) {
					// ( RelativeSector && (( ! EXTENDED && ! EXTENDEDLBA && ! LINUXEXT ) || ! PART_LOGICAL )  )
					PartList = PartList->Next = CreatePartNode(MBRNode,Index);
					++Count;
				}
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
	PartEntry = PartNode->Entry = &MBRNode->Table->mbr.Entries[Index];

	Partition->Drive = MBRNode->Drive;
	Partition->StartSector = MBRNode->AbsoluteSector + PartEntry->RelativeSector;
	Partition->SectorCount = PartEntry->SectorCount;
	Partition->FSName = GetFSName(PartEntry->FSType);
	Partition->FSType = PartEntry->FSType;
	Partition->Type = MBRNode->Type;
	GetPartMbrHDSector0(Partition);
	return PartNode;
}

TPartNode *CPartList::CreateGPTPartNode(const TMBRNode *MBRNode, int Index)
{
	TPartNode *PartNode;
	TPartition *Partition;
	const gpt_partentry_t *PartEntry;
	int GptShortType;

	PartNode = new TPartNode;
	Partition = PartNode->Partition = new TPartition;
	PartEntry = PartNode->gptEntry = &MBRNode->Table->gpt.Entries[Index];

	Partition->Drive = MBRNode->Drive;
	Partition->StartSector = PartEntry->start;
	Partition->SectorCount = PartEntry->end - PartEntry->start;
	GptShortType = GetGPTShortType(PartEntry->type);
	Partition->FSName = GetFSName(GptShortType);
	Partition->FSType = GptShortType;
	Partition->Type = MBRNode->Type;
	Partition->MbrHDSector0 = 0xff;
	return PartNode;
}

const char *CPartList::GetFSName(unsigned short FSID)
{
	int Index;

	if (FSID > 0xff ){
		// GPT FSID
		for (Index = 0; gpt_fstypes[Index].MBRType != FSID && gpt_fstypes[Index].MBRType != 0xffff; ++Index);
		return gpt_fstypes[Index].Name;

	}else{
		// MBR FSID
		for (Index = 0; FSNameList[Index].FSID != FSID && FSNameList[Index].FSID != 0xff; ++Index);
		return FSNameList[Index].FSName;
	}
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

char* CPartList::WriteStructure()
{
	CDisk Disk;
	TMBRNode *MBRList;
	TMBRNode *GTPList;
	gpt_header_t *gpth;
	uint64_t prt_bu_offset;
	int Index;
	uint32_t part_crc;
	gpt_header_t *gtpbuheader;

	for (MBRList = this->MBRList.Next; MBRList; MBRList = MBRList->Next) {
		if(MBRList->Type == PART_GPT_PROT_MBR){
			// gtp protective mbr 
			// Don't need to overwrite this
			continue;
		}
		if(MBRList->Type == PART_GPT_HEADER){
			// gpt header, we will need this
			gpth = MBRList->gpthTable;
			// Make sure we can read the gpt backup header before updating partitions
			gtpbuheader = new gpt_header_t;
			Disk.Map(MBRList->Drive,gpth->backup);
			if(Disk.Read(0,gtpbuheader,1) == -1){
				return "failed\nUnable to read gpt backup header. ";
			}
			// Generate the crc table
			chksum_crc32gentab();
			// Initialize the crc
			part_crc = 0L;

			for(Index = 0, GTPList = MBRList ;Index < gpth->maxpart ; Index+=4){
				GTPList = GTPList->Next;
				// Update primary partition entries
				if (GTPList->Type != PART_GPT){
					// Internal error
					return "failed\nExcected PART_GPT.";
				}
				// update the crc
				part_crc = chksum_crc32(part_crc, GTPList->gptTable, sizeof(TGPTTable));
				// write gpt table sector to disc
				Disk.Map(GTPList->Drive,GTPList->AbsoluteSector);
				Disk.Write(0,GTPList->gptTable,1);
			}
			// Update the primary header
			gpth->partentry_crc32 = part_crc;
			gpth->crc32 = 0L;
			gpth->crc32 = chksum_crc32(0,gpth,gpth->headersize);
			Disk.Map(GTPList->Drive,1);
			Disk.Write(0,gpth,1);

			// Now update backup partition entries
			prt_bu_offset = gtpbuheader->partitions - 2;
			for(Index = 0, GTPList = MBRList;Index < gpth->maxpart ; Index+=4){
				GTPList = GTPList->Next;
				// Update primary partition entries
				if (GTPList->Type != PART_GPT){
					// Internal error
				}
				// A gpt table sector to update
				Disk.Map(GTPList->Drive,GTPList->AbsoluteSector+prt_bu_offset);
				Disk.Write(0,GTPList->gptTable,1);
			}
			// Update the backup header
			gtpbuheader->partentry_crc32 = part_crc;
			gtpbuheader->crc32 = 0L;
			gtpbuheader->crc32 = chksum_crc32(0,gtpbuheader,gpth->headersize);
			Disk.Map(GTPList->Drive,gpth->backup);
			Disk.Write(0,gtpbuheader,1);

			// All GTP entries done update MBRList;
			MBRList = GTPList;
		}
		else{
			// ordinary mbr sector
			Disk.Map(MBRList->Drive,MBRList->AbsoluteSector);
			Disk.Write(0,MBRList->Table,1);
		}
	}
	return NULL;
}

const TPartition *CPartList::GetPartition(int Index)
{
	return PLUP[Index]->Partition;
}

void CPartList::UpdateFSType(int Index, unsigned short FSType, unsigned char MbrHDSector0)
{
	PLUP[Index]->Partition->FSType = FSType;
	PLUP[Index]->Partition->FSName = GetFSName(FSType);
	PLUP[Index]->Partition->MbrHDSector0 = MbrHDSector0;
	if (FSType < 0x100){
		// mbr type
		PLUP[Index]->Entry->FSType = FSType;
	}
	else{
		// gpt type
		char *Name, *gptname;
		int i,j;

		memcpy(&PLUP[Index]->gptEntry->type,GetGPTType(FSType), sizeof(uuid_t));
		// wide charcter fudge
		Name = GetGPTName(FSType);
		gptname = PLUP[Index]->gptEntry->name;
		for(i = 0, j = 0; Name[i] != 0 ;){
			gptname[j++] = Name[i++];
			gptname[j++] = 0;
		}
		gptname[j++] = 0;
		gptname[j++] = 0;
	}
}

uuid_t* CPartList::GetGPTType(int FSType)
{
	for (int Index = 0; ;++Index ){
		if (gpt_fstypes[Index].MBRType == FSType) {
			return &gpt_fstypes[Index].GUIDype;
		}
		if (gpt_fstypes[Index].MBRType == 0xffff){
			return 0;
		}
	}
}
char* CPartList::GetGPTName(int FSType)
{
	for (int Index = 0; ;++Index ){
		if (gpt_fstypes[Index].MBRType == FSType) {
			return gpt_fstypes[Index].Name;
		}
		if (gpt_fstypes[Index].MBRType == 0xffff){
			return 0;
		}
	}
}

int CPartList::Locate(int Drive, unsigned long long StartSector)
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
	if(PLUP[Index]->Partition->FSType < 0x100 ){
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
	return 0;
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
	return Type == PART_PRIMARY || Type == PART_LOGICAL;
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
		for (Index = 0; Index < Count; ++Index){
			if(CanActivate(Index)){
				PLUP[Index]->Entry->Activated = 0x00;
			}
		}
	else
		// clear active flag only for all partitions on same drive
		for (Index = 0; Index < Count; ++Index){
			if (PLUP[Index]->Partition->Drive == Drive){
				if(CanActivate(Index)){
					PLUP[Index]->Entry->Activated = 0x00;
				}
			}
		}
}

void CPartList::SetActive(int Index)
{

	if(CanActivate(Index)){
		ClearActive(PLUP[Index]->Partition->Drive);
		PLUP[Index]->Entry->Activated = 0x80;
	}
}

void CPartList::SetFsType(int Index, int FSType)
{
	if (FSType < 0x100){
		// MBR FSType
		PLUP[Index]->Entry->FSType = FSType;
	}
	else{
		// gpt type
		char *Name, *gptname;
		int i,j;

		memcpy(&PLUP[Index]->gptEntry->type,GetGPTType(FSType), sizeof(uuid_t));
		// wide charcter fudge
		Name = GetGPTName(FSType);
		gptname = PLUP[Index]->gptEntry->name;
		for(i = 0, j = 0; Name[i] != 0 ;){
			gptname[j++] = Name[i++];
			gptname[j++] = 0;
		}
		gptname[j++] = 0;
		gptname[j++] = 0;
	}
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
	{0xee,"Protective MBR"},
	{0xff,"Unknown"},
};

//int memcmp(const void far *s1, const void far *s2, size_t count);
int far memcmp(const void *s1, const void *s2, unsigned short count);
#define BOOTITEM_FILESIZE sizeof(CBootItemFile)

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
uint16_t CPartList::GetGptMBRType(int gptindex){
	return gpt_fstypes[gptindex].MBRType;
}

/* crc_tab[] -- this crcTable is being build by chksum_crc32GenTab().
 *              so make sure, you call it before using the other
 *              functions!
 */
uint32_t crc_tab[256];

/* chksum_crc() -- to a given block, this one calculates the
 *                              crc32-checksum until the length is
 *                              reached. the crc32-checksum will be
 *                              the result.
 */
uint32_t CPartList::chksum_crc32 (uint32_t initial, const void *pblock, uint64_t length)
{
   register unsigned long crc;
   uint64_t i;
   const char *block;

   block = (char *)pblock;
   crc = initial ^ 0xFFFFFFFF;
   for (i = 0; i < length; i++)
   {
      crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
   }
   return (crc ^ 0xFFFFFFFF);
}

/* chksum_crc32gentab() --      to a global crc_tab[256], this one will
 *                              calculate the crcTable for crc32-checksums.
 *                              it is generated to the polynom [..]
 */

void CPartList::chksum_crc32gentab ()
{
   unsigned long crc, poly;
   int i, j;

   poly = 0xEDB88320L;
   for (i = 0; i < 256; i++)
   {
      crc = i;
      for (j = 8; j > 0; j--)
      {
         if (crc & 1)
         {
            crc = (crc >> 1) ^ poly;
         }
         else
         {
            crc >>= 1;
         }
      }
      crc_tab[i] = crc;
   }
}
