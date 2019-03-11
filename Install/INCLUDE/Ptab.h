/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __ptab__
#define __ptab__

#include <defs.h>
#include <gptab.h>
#include <main.h>
//#include <bootrec.h>

#define PART_PRIMARY 1
#define PART_LOGICAL 2
#define PART_MBR     3
//#define PART_FLOPPY  4
//#define PART_SBM     5
// StartSector is set to PART_SBM (4) previous XOSL versions.
#define PART_SBM     4
#define PART_FLOPPY  5
#define PART_GPT     6
#define PART_GPT_PROT_MBR 7
#define PART_GPT_HEADER 8
#define PART_OMBR	9

typedef struct {
	unsigned char Activated;
	unsigned char StartHead;
	unsigned short StartSectCyl;
	unsigned char FSType;
	unsigned char EndHead;
	unsigned short EndSectCyl;
	unsigned long RelativeSector;
	unsigned long SectorCount;
} TPartMbrEntry;

/*
typedef struct {
	char IPL[446];
	TPartEntry Entries[4];
	unsigned short MagicNumber; // 0xaa55
//} TPartTable;
} TMBRTable;

typedef union {
	TMBRTable mbr;
	TGPTTable gpt;
	gpt_header_t gpth;
} TPartTable;
*/

typedef struct {
	union{
		struct {
			char IPL[446];
			TPartMbrEntry mbrEntries[4];
			unsigned short MagicNumber; // 0xaa55	
		};
		gpt_partentry_t gptEntries[4];
		gpt_header_t gpth;
	};
} TPartTable;

typedef struct S_MBRNode {
	unsigned long long AbsoluteSector;
	short Drive;
	short Type; // primary, logical, gtp, gpt protective mbr or gpt header
	union {
	TPartTable *Table;
	gpt_partentry_t *gptTable;
	gpt_header_t *gpthTable;
	};
	struct S_MBRNode *Next;
} TMBRNode;

typedef struct S_Partition {
	short Drive;
	unsigned long long StartSector;
	unsigned long long SectorCount;
	const char *FSName;
	short FSType;
	short Type; // primary, logical, mbr, floppy or gpt
	unsigned char MbrHDSector0;
} TPartition;

typedef struct S_PartNode {
	union {
	TPartMbrEntry *Entry;
	gpt_partentry_t *gptEntry;
	};
	TPartition *Partition;
	struct S_PartNode *Next;
} TPartNode;

class CPartList {
	public:
		CPartList();
		~CPartList();
		void ReadStructure();
		char* WriteStructure();
		const TPartition *GetPartition(int Index);
		int Locate(int Drive, unsigned long long StartSector);
		int GetCount();
		int CanHide(int Index);
		void Hide(int Index);
		void Reveal(int Index);
		int CanActivate(int Index);
		void SetAllowActiveHD(int Status);
		void SetActive(int Index);
		void SetFsType(int Index, unsigned short FsType, unsigned char MbrHDSector0);
		int UpgradeXoslBootItem(const TPartition *Partition, unsigned char MbrHDSector0, unsigned char &OldMbrHDSector0);
		int Retain(const char *DosFileName,unsigned short FileSize,const TPartition *Partition);
//		void UpdateFSType(int Index, unsigned short FSType, unsigned char MbrHDSector0);
		int GetGPTIndex(uuid_t GPTType);
		unsigned short GetGPTShortType(uuid_t GPTType);
		uuid_t* GetGPTType(unsigned short FSType);
		uint16_t GetGptMBRType(int gpt_index);
		char* GetGPTName(unsigned short FSType);
		void UpdatePartitionMbrHDSector0(int Index, unsigned char MbrHDSector0);
	public:
		typedef struct {
			int FSID;
			const char *FSName;
		} TFSNameEntry;
	private:
		void ClearActive(int Drive);

		TMBRNode *AddDrive(int Drive, unsigned long long StartSector, unsigned long ExtStart, int Type, TMBRNode *MBRList);
		void CreatePartList(int FloppyCount);
		TPartNode *CreatePartNode(const TMBRNode *MBRNode, int Index);
		TPartNode *CreateNonPartNode(int Drive);
		TPartNode *CreateGPTPartNode(const TMBRNode *MBRNode, int Index);
		const char *GetFSName(unsigned short FSID);
		void CreatePLUP();
		void GetPartMbrHDSector0(TPartition *Partition);
		void ConvertDOS2XoslFsName(const char *DosFileName, char *XoslFsFileName);
		uint32_t chksum_crc32 (uint32_t initial, const void *block, uint64_t length);
		void chksum_crc32gentab ();
//		int GetDriveIndex(TPartition *Partition, TBootRecord &BootRecord);
		TMBRNode MBRList;
		TPartNode PartList;
		int Count;
		TPartNode **PLUP;
		static TFSNameEntry FSNameList[];
		int AllowActiveHD;
};



#endif