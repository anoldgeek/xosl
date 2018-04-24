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
 *   truncated during assignment or initialization.
 * 
 */

#include <lastconf.h>
#include <mem.h>

static const char *LastConfFile = "LASTCONFXDF";

CLastConfig::CLastConfig(CFileSystem *FileSystemToUse, CPartList *PartListToUse)
{
	FileSystem = FileSystemToUse;
	PartList = PartListToUse;
	FileData = new TLastConfig;
	FileSystem->ReadFile(LastConfFile,FileData);
}

CLastConfig::~CLastConfig()
{
}

const short *CLastConfig::GetRelocTable()
{
	int Index, Count;
	TPartKey *LastPartList;

	Count = FileData->PartitionCount;
	LastPartList = FileData->LastPartList;
	memset(RelocTable,-1,sizeof (short[92]));
	for (Index = 0; Index < Count; ++Index, ++LastPartList)
		RelocTable[Index] = PartList->Locate(LastPartList->Drive,LastPartList->StartSector);
	return RelocTable;
}

void CLastConfig::SetNewConfig()
{
	int Index, Count;
	const TPartition *Partition;
	TPartKey *KeyList;

	FileData->PartitionCount = Count = PartList->GetCount();
	KeyList = FileData->LastPartList;
	for (Index = 0; Index < Count; ++Index, ++KeyList) {
		Partition = PartList->GetPartition(Index);
		KeyList->Drive = (unsigned char) Partition->Drive;  //ML: TPartition->Drive is short, TPartKey->Drive is unsigned char
		KeyList->StartSector = Partition->StartSector;
	}
	FileSystem->WriteFile(LastConfFile,FileData);
}
