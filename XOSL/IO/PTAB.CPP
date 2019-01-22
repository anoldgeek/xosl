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
 * - Code change in function ReadVolumeLabels to get rid of 
 *   Error! E473: function argument(s) do not match those in prototype
 *
 */

#include <ptab.h>
#include <transfer.h>
#include <disk.h>
#include <mem.h>
#include <ctype.h>

#include <fat16.h>
#include <fat32.h>
//#include <ntfs.h>
#include <gptabdata.h>

#define FSTYPE_EXTENDED 0x05
#define FSTYPE_EXTENDEDLBA 0x0f
#define FSTYPE_HIDDENEXTENDED 0x1f
#define FSTYPE_LINUXEXT 0x85

extern void printf(const char *,...);

CPartList::CPartList()
{
	PartListChanged = false;
	VolumeLabelsRead = false;
	ReadStructure();
	AllowActiveHD = 0;
}

CPartList::~CPartList()
{
	// stuff should be deallocated here
}

void CPartList::ReadStructure()
{
	int DrvCount, Index;
	CDiskAccess DiskAccess;

	DrvCount = DiskAccess.DriveCount(0x80);
	for (Index = 0; Index < DrvCount; ++Index)
		AddDrive(Index | 0x80,0,0,PART_PRIMARY);
	CreatePartList(DiskAccess.DriveCount(0x00));
	// Create PartList Look-up Table
	CreatePLUP();
}
unsigned short CPartList::GetGPTShortType(uuid_t GPTType)
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

void CPartList::AddDrive(int Drive, unsigned long long StartSector, unsigned long long ExtStart, int Type)
{
	CMBRNode NewNode;
	CDisk Disk;
	TPartTable *PartTable;
	const TPartMbrEntry *mbrEntries;
	int Index;
	unsigned long long NewStartSector;
	unsigned int i;
	unsigned int gpt_sector;

	if (Disk.Map(Drive,StartSector) == -1)
		return;
	PartTable = new TPartTable;
	if (Disk.Read(0,PartTable,1) == -1 || PartTable->MagicNumber != 0xaa55) {
		delete PartTable;
		return;
	}

	// gpt code from grub:2 grub-core/partmap/gpt.c
	// Check to see if the MBR is a protective MBR for GPT disk and not a normal MBR.
	for (i=0;i < 4 ; i++ ){
		if(PartTable->mbrEntries[0].FSType == 0xee){
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
		NewNode.AbsoluteSector = StartSector;
		NewNode.Drive = Drive;
		NewNode.Type = PART_GPT_PROT_MBR;
		NewNode.Table = PartTable;
		MBRList.insert(MBRList.end(),NewNode);

		PartTable = new TPartTable;

		/* Read the GPT header.  */
		// Code assume 512 byte logical sectors on disc
		if (Disk.Read(1,PartTable,1) == -1 ) {
			delete PartTable;
			return;
		}

		if (memcmp (PartTable->gpth.magic, gpt_magic, sizeof (gpt_magic)) != 0){
			delete PartTable;
			return;
		}
		// Add the gpt header
		NewNode.AbsoluteSector = 1;
		NewNode.Drive = Drive;
		NewNode.Type = PART_GPT_HEADER;
		NewNode.Table = PartTable;
		MBRList.insert(MBRList.end(),NewNode);

	
		gpth = &PartTable->gpth;

		// GPT disk. This code only handles little endian
		// Code assume 512 byte logical sectors on disc
		for (j = 0, gpt_sector = 2; j < gpth->maxpart ; gpt_sector++ , j+=4 ){
			PartTable = new TPartTable;
			if (Disk.Read(gpt_sector,PartTable,1) == -1 ) {
				delete PartTable;
				return;
			}
			NewNode.AbsoluteSector = gpt_sector;
			NewNode.Drive = Drive;
			NewNode.Type = PART_GPT;
			NewNode.Table = PartTable;
			MBRList.insert(MBRList.end(),NewNode);
		}
		return;
	}

	// MBR partition
	NewNode.AbsoluteSector = StartSector;
	NewNode.Drive = Drive;
	NewNode.Type = Type;
	NewNode.Table = PartTable;
	MBRList.insert(MBRList.end(),NewNode);
                               
	mbrEntries = PartTable->mbrEntries;
	for (Index = 0; Index < 4; ++Index)
	{
		switch (mbrEntries[Index].FSType) {
			case FSTYPE_EXTENDED:
			case FSTYPE_EXTENDEDLBA:
			case FSTYPE_LINUXEXT:
			case FSTYPE_HIDDENEXTENDED:
				if (Type != PART_LOGICAL) {
					ExtStart = mbrEntries[Index].RelativeSector;
					NewStartSector = ExtStart;
				}
				else
					NewStartSector = ExtStart + mbrEntries[Index].RelativeSector;
				AddDrive(Drive,NewStartSector,ExtStart,PART_LOGICAL);
				break;
			default:
				break;
		}
	}
}


void CPartList::CreatePartList(int FloppyCount)
{
	list<CMBRNode>::iterator MBRNode;
	int Index, Drive;
	const TPartMbrEntry *mbrEntries;

	if (MBRList.size()) {
		MBRNode = MBRList.begin();
		Drive = (*MBRNode).Drive;
		CreateNonPartNode(Drive);
		CreateNonPartNode(-1);

		for (; MBRNode != MBRList.end(); ++MBRNode) {
			if (Drive != (*MBRNode).Drive) {
				Drive = (*MBRNode).Drive;
				CreateNonPartNode(Drive);
			}
			if ((*MBRNode).Type == PART_GPT_PROT_MBR || (*MBRNode).Type == PART_GPT_HEADER){
				// Don't display protctive mbr or header
				continue;
			}
			if ((*MBRNode).Type == PART_GPT){
				// GPT Disk
				for (Index = 0 ; Index < 4 ; ++Index){
					CreateGPTPartNode(MBRNode,Index);
				}
			}else{
				for (Index = 0; Index < 4; ++Index) {
					mbrEntries = &(*MBRNode).Table->mbrEntries[Index];
					if (mbrEntries->RelativeSector && ((mbrEntries->FSType != FSTYPE_EXTENDED && mbrEntries->FSType != FSTYPE_EXTENDEDLBA &&
						mbrEntries->FSType != FSTYPE_HIDDENEXTENDED && mbrEntries->FSType != FSTYPE_LINUXEXT) ||
						(*MBRNode).Type != PART_LOGICAL)) {
						CreatePartNode(MBRNode,Index);
					}
				}
			}
		}
	}
	for (Index = 0; Index < FloppyCount; ++Index)
		CreateNonPartNode(Index);
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


void CPartList::CreateNonPartNode(int Drive)
{
	CPartNode PartNode;
	TPartition *Partition;

	Partition = PartNode.Partition = new TPartition;
	PartNode.mbrEntry = NULL;

	Partition->Drive = Drive;
	Partition->StartSector = 0;
	Partition->SectorCount = 0;
	Partition->VolumeLabel = "";
	if (Drive >= 0x80) {
		if (Drive == 0x80)
			Partition->FSName = "Original MBR";
		else
			Partition->FSName = "Master Boot Record";
		Partition->FSType = -1;
		Partition->Type = PART_MBR;
	}
	else if (Drive == -1) {
		Partition->FSName = "Smart Boot Manager";
		Partition->FSType = -1;
		Partition->Type = PART_SBM;
		Partition->Drive = 0x80;
		Partition->StartSector = PART_SBM;
	}
	else {
		Partition->FSName = "Boot floppy";
		Partition->FSType = -1;
		Partition->Type = PART_FLOPPY;
	}
	PartList.insert(PartList.end(),PartNode);
}

void CPartList::CreatePartNode(list<CMBRNode>::iterator MBRNode, int Index)
{
	CPartNode PartNode;
	TPartition *Partition;
	const TPartMbrEntry *mbrPartEntry;

	Partition = PartNode.Partition = new TPartition;
	mbrPartEntry = PartNode.mbrEntry = &(*MBRNode).Table->mbrEntries[Index];

	Partition->Drive = (*MBRNode).Drive;
	Partition->StartSector = (*MBRNode).AbsoluteSector + mbrPartEntry->RelativeSector;
	Partition->SectorCount = mbrPartEntry->SectorCount;
	Partition->FSName = GetFSName(mbrPartEntry->FSType);
	Partition->FSType = mbrPartEntry->FSType;
	Partition->Type = (*MBRNode).Type;
	Partition->VolumeLabel = "";
	PartList.insert(PartList.end(),PartNode);
}
void CPartList::CreateGPTPartNode(list<CMBRNode>::iterator MBRNode, int Index)
{
	CPartNode PartNode;
	TPartition *Partition;
	const gpt_partentry_t *PartEntry;
	unsigned short GptShortType;

	PartEntry = PartNode.gptEntry = &(*MBRNode).Table->gptEntries[Index];
	GptShortType = GetGPTShortType(PartEntry->type);
	if (GptShortType != 0 ){
		// Don't add unused partitions.
		Partition = PartNode.Partition = new TPartition;
		Partition->Drive = (*MBRNode).Drive;
		Partition->StartSector = PartEntry->start;
		Partition->SectorCount = PartEntry->end - PartEntry->start;
		Partition->FSName = GetFSName(GptShortType);
		Partition->FSType = GptShortType;
		Partition->Type = (*MBRNode).Type;
		PartList.insert(PartList.end(),PartNode);
	}
}

void CPartList::CreatePLUP()
{
	int Index;
	list<CPartNode>::iterator PartListEntry;

	PartListEntry = PartList.begin();
	PLUP = new CPartNode *[PartList.size()];
	for (Index = 0; Index < PartList.size(); ++Index, ++PartListEntry)
		PLUP[Index] = &*PartListEntry; // --> assignment operator has to be implemented for list<>
}

char* CPartList::WriteStructure()
{
	CDisk Disk;
	list<CMBRNode>::iterator MBRListEntry;
	list<CMBRNode>::iterator GPTListEntry;
	gpt_header_t *gpth;
	uint64_t prt_bu_offset;
	int Index;
	uint32_t part_crc;
	gpt_header_t *gtpbuheader;


	if (PartListChanged) {
		// TODO: Detect that active partition is already active
		for (MBRListEntry = MBRList.begin(); MBRListEntry != MBRList.end(); ++MBRListEntry) {
			if( (*MBRListEntry).Type == PART_GPT_PROT_MBR){
				// gtp protective mbr 
				// Don't need to overwrite this
				continue;
			}
			if ((*MBRListEntry).Type == PART_GPT_HEADER ) {
				// gpt header, we will need this
				gpth = &(*MBRListEntry).Table->gpth;
				// Make sure we can read the gpt backup header before updating partitions
				gtpbuheader = new gpt_header_t;
				Disk.Map( (*MBRListEntry).Drive,gpth->backup);
				if(Disk.Read(0,gtpbuheader,1) == -1){
					return "failed\nUnable to read gpt backup header. ";
				}
				// Generate the crc table
				chksum_crc32gentab();
				// Initialize the crc
				part_crc = 0L;

				//for(Index = 0, GTPList = MBRList ;Index < gpth->maxpart ; Index+=4){
				for (Index = 0, GPTListEntry = MBRListEntry; MBRListEntry != MBRList.end() && Index < gpth->maxpart; Index+=4) {
					++MBRListEntry;
					// Update primary partition entries
					if ( (*MBRListEntry).Type != PART_GPT){
						// Internal error
						return "failed\nExpected PART_GPT while writing primary partitions.";
					}
					// update the crc
					part_crc = chksum_crc32(part_crc, (*MBRListEntry).Table->gptEntries, sizeof(TGPTTable));
					// write gpt table sector to disc
					Disk.Map( (*MBRListEntry).Drive, (*MBRListEntry).AbsoluteSector);
					Disk.Write(0,(*MBRListEntry).Table->gptEntries,1);
				}
				// Update the primary header
				gpth->partentry_crc32 = part_crc;
				gpth->crc32 = 0L;
				gpth->crc32 = chksum_crc32(0,gpth,gpth->headersize);
				Disk.Map( (*MBRListEntry).Drive,1);
				Disk.Write(0,gpth,1);

				// Now update backup partition entries
				prt_bu_offset = gtpbuheader->partitions - 2;
	//			for(Index = 0, GTPList = MBRList;Index < gpth->maxpart ; Index+=4){
				for (Index = 0, MBRListEntry = GPTListEntry ; MBRListEntry != MBRList.end() && Index < gpth->maxpart; Index+=4) {
					++MBRListEntry;
					// Update backup partition entries
					if ( (*MBRListEntry).Type != PART_GPT){
						// Internal error
						return "failed\nExpected PART_GPT while writing backup partitions.";
					}
					// A gpt table sector to update
					Disk.Map( (*MBRListEntry).Drive, (*MBRListEntry).AbsoluteSector+prt_bu_offset);
					Disk.Write(0, (*MBRListEntry).Table->gptEntries,1);
				}
				// Update the backup header
				gtpbuheader->partentry_crc32 = part_crc;
				gtpbuheader->crc32 = 0L;
				gtpbuheader->crc32 = chksum_crc32(0,gtpbuheader,gpth->headersize);
				Disk.Map( (*MBRListEntry).Drive,gpth->backup);
				Disk.Write(0,gtpbuheader,1);
			} // All GTP entries done
			else{
				// ordinary mbr sector
				Disk.Map((*MBRListEntry).Drive,(*MBRListEntry).AbsoluteSector);
				Disk.Write(0,(*MBRListEntry).Table,1);
			}
		}
	}
	return 0;
}

const TPartition *CPartList::GetPartition(int Index)
{
	if (Index >= PartList.size()) {
		return NULL;
	}
	return PLUP[Index]->Partition;
}

int CPartList::Locate(int Drive, unsigned long long StartSector)
{
	int Index;
	const TPartition *Partition;

	for (Index = 0; Index < PartList.size(); ++Index) {
		Partition = PLUP[Index]->Partition;
		if (Partition->Drive == Drive && Partition->StartSector == StartSector)
			return Index;
	}
	return -1;
}

int CPartList::GetCount()
{
	return PartList.size();
}

int CPartList::CanHide(int Index)
{
	if(PLUP[Index]->Partition->FSType < 0x100 ){
		switch (PLUP[Index]->mbrEntry->FSType & 0xef) {
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
	unsigned char &FSType = PLUP[Index]->mbrEntry->FSType;

	if (CanHide(Index) && (FSType | 0x10) != FSType) {
		FSType |= 0x10;
		PartListChanged = true;
	}
}

void CPartList::Reveal(int Index)
{
	unsigned char &FSType = PLUP[Index]->mbrEntry->FSType;

	if (CanHide(Index) && (FSType & 0xef) != FSType) {
		FSType &= 0xef;
		PartListChanged = true;
	}
}

void CPartList::RevealAll()
{
	int Index;

	for (Index = 0; Index < PartList.size(); ++Index)
		switch (PLUP[Index]->mbrEntry->FSType) {
			case 0x11:
			case 0x14:
			case 0x16:
			case 0x17:
			case 0x1b:
			case 0x1c:
			case 0x1e:
			case 0x1f:
				PLUP[Index]->mbrEntry->FSType &= 0x0ef;
				PLUP[Index]->Partition->FSName = GetFSName(PLUP[Index]->mbrEntry->FSType);
				PartListChanged = true;
				break;
			default:
				break;
		}
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

void CPartList::SetActive(int PartIndex)
{
	int Index;
	int Drive;

	if(CanActivate(Index)){
		if (!AllowActiveHD) {
			// clear active flag for all partitions
			for (Index = 0; Index < PartList.size(); ++Index) {
				if (Index != PartIndex && PLUP[Index]->mbrEntry->Activated == 0x80) {
					PLUP[Index]->mbrEntry->Activated = 0x00;
					PartListChanged = true;            
				}
			}
		}
		else {
			// clear active flag for all partitions on same drive only
			Drive = PLUP[PartIndex]->Partition->Drive;
			for (Index = 0; Index < PartList.size(); ++Index) {
				if (Index != PartIndex && PLUP[Index]->mbrEntry->Activated == 0x80 && PLUP[Index]->Partition->Drive == Drive) {
					PLUP[Index]->mbrEntry->Activated = 0x00;
					PartListChanged = true;
				}
			}
			if (PLUP[PartIndex]->mbrEntry->Activated != 0x80) {
				PLUP[PartIndex]->mbrEntry->Activated = 0x80;
				PartListChanged = true;
			}	
		}
	}
}

void CPartList::InsertMbrPTab(void *DestAddr)
{
	char *Dest = (char *)DestAddr;

	if (MBRList.size())
		memcpy(&Dest[446],(*MBRList.begin()).Table->mbrEntries,sizeof(TPartMbrEntry[4]));
}

void CPartList::ReadVolumeLabels()
// TODO: volume labels of NTFS partitions?
{
	TPartition *Partition;
	CDisk Disk;
	char BootRecord[512];
	TBootFAT16 *BootFAT16;
	TBootFAT32 *BootFAT32;
	int Index;

	if (VolumeLabelsRead) {
		return;
	}
	BootFAT16 = (TBootFAT16 *)BootRecord;
	BootFAT32 = (TBootFAT32 *)BootRecord;
	for (Index = 0; Index < PartList.size(); ++Index) {
		Partition = PLUP[Index]->Partition;

		if (Partition->Drive < 0x80) {
			VolumeLabelsRead = true;
			return;
		}
		Disk.Map(Partition->Drive,Partition->StartSector);
		Disk.Read(0,BootRecord,1);

		if (memcmp(BootFAT16->FSID,"FAT1",4) == 0) {
			Partition->VolumeLabel = new char [12];
			CreateVolumeLabel((const char*)BootFAT16->Label,Partition->VolumeLabel);
		}
		else {
			if (memcmp(BootFAT32->FSID,"FAT3",4) == 0) {
				Partition->VolumeLabel = new char [12];
				CreateVolumeLabel((const char*)BootFAT32->Label,Partition->VolumeLabel);
			}
		}
	}
	VolumeLabelsRead = true;
}

void CPartList::CreateVolumeLabel(const char *RawLabel, char *VolumeLabel)
{
	int Index;

	memcpy(VolumeLabel,RawLabel,11);
	for (Index = 10; Index >= 0 && VolumeLabel[Index] == ' '; --Index);
	VolumeLabel[Index + 1] = '\0';
	if (VolumeLabel[0])
		for (Index = 1; VolumeLabel[Index]; ++Index)
			VolumeLabel[Index] = tolower(VolumeLabel[Index]);
}


void CPartList::GetDiskPartitions(int Drive, list<CPartNode>::iterator &First, list<CPartNode>::iterator &Last)
{
	if (Drive >= 0x80) {
		for (First = PartList.begin(); First != PartList.end() && (*First).Partition->Drive != Drive; ++First);
		for (Last = First; Last != PartList.end() && (*Last).Partition->Drive == Drive; ++Last);
	}
	else {
		// Get all floppies, and not just the one specified.
		Last = PartList.end();
		First = PartList.end();
		// (*--First).Partition->Drive is too complex for BC++3.1
		for (--First; (*First).Partition->Drive == 0x00 && First != PartList.begin() ; --First);
		++First;
	}
}

int CPartList::GetPartIndex(int Drive, int PartIndex)
{
	int Index;
	list<CPartNode>::iterator Entry;

	if (Drive < 0x80) {
		Drive = 0x00;
	}
	for (Index = 0, Entry = PartList.begin(); Entry != PartList.end() && (*Entry).Partition->Drive != Drive; ++Index, ++Entry);
	Index += PartIndex;
	if (Index > PartList.size()) {
		return -1;
	}
	return Index;
}

void CPartList::GetDiskPartIndex(int Index, int &Drive, int &PartIndex)
{
	list<CPartNode>::iterator Entry;

	if (Index > PartList.size()) {
		return;
	}

	Drive = 0x80;
	PartIndex = -1;
	Entry = PartList.begin();
	do {
		if ((*Entry).Partition->Drive != Drive && Drive != 0x00) {
			Drive = (*Entry).Partition->Drive;
			PartIndex = 0;
		}
		else {
			++PartIndex;
		}
		++Entry;
	} while (Index--);
/*	for (Entry = PartList.begin(); Index; ++Entry, --Index) {
		if ((*Entry).Partition->Drive != Drive && Drive != 0x00) {
			Drive = (*Entry).Partition->Drive;
			PartIndex = 0;
		}
		else {
			++PartIndex;
		}
	}*/
}

const CPartList::TFSNameEntry CPartList::FSNameList[] = {
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
	{0x38,"TheOS"},
	{0x63,"Unix SysV/386"},
	{0x78,"XOSL FS"},
	{0x82,"Linux Swap"},
	{0x83,"Linux Native"},
	{0x85,"Linux Extended"},
	{0x8e,"Linux LVM"},
	{0xa5,"FreeBSD, BSD/386"},
	{0xeb,"BeOS"},
	{0xee,"Protective MBR"},
	{0xff,"Unknown"}
};
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
