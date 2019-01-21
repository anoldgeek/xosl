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

#include <newdefs.h>
#include <list.hpp>
#include <gptab.h>

#define PART_PRIMARY 0
#define PART_LOGICAL 1
#define PART_MBR     2
#define PART_FLOPPY  3
#define PART_SBM     4
#define PART_GPT     5
#define PART_GPT_PROT_MBR 6
#define PART_GPT_HEADER 7

#define PART_STR "primary","logical","mbr","floppy","loader","gpt","gptpmbr","gpthder"

typedef struct {
	unsigned char Activated;
	unsigned char StartHead;
	unsigned short StartSectCyl;
	unsigned char FSType;
	unsigned char EndHead;
	unsigned short EndSectCyl;
	unsigned long RelativeSector;
	unsigned long SectorCount;
} TPartEntry;

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
			TPartEntry Entries[4];
			unsigned short MagicNumber; // 0xaa55	
		};
		gpt_partentry_t gptEntries[4];
		gpt_header_t gpth;
	};
} TPartTable;

class CMBRNode {
public:
	unsigned long long AbsoluteSector;
	short Drive;
    short Type; // primary, logical, gtp, gpt protective mbr or gpt header
    TPartTable *Table;
};

typedef struct S_Partition {
	short Drive;
	unsigned long long StartSector;
	unsigned long long SectorCount;
	const char *FSName;
	short FSType;
	short Type; // primary, logical, mbr or floppy
	char *VolumeLabel;
} TPartition;

class CPartNode {
public:
   union {
       TPartEntry *Entry;
       gpt_partentry_t *gptEntry;
    };
 	TPartition *Partition;
};

class CPartList {
	public:
		CPartList();
		~CPartList();
		char* WriteStructure();
		const TPartition *GetPartition(int Index);
		int Locate(int Drive, unsigned long long StartSector);
		int GetCount();
		int CanHide(int Index);
		void Hide(int Index);
		void Reveal(int Index);
		void RevealAll();
		int CanActivate(int Index);
		void SetAllowActiveHD(int Status);
		void SetActive(int Index);

		void ReadVolumeLabels();

		void InsertMbrPTab(void *DestAddr);

		static const char *GetFSName(int FSID);
		static void CreateVolumeLabel(const char *RawLabel, char *VolumeLabel);

		void GetDiskPartitions(int Drive, list<CPartNode>::iterator &First, list<CPartNode>::iterator &Last);
		int GetPartIndex(int Drive, int PartIndex);
		void GetDiskPartIndex(int Index, int &Drive, int &PartIndex);
		int GetGPTShortType(uuid_t GPTType);


	public:
		typedef struct {
			int FSID;
			const char *FSName;
		} TFSNameEntry;
	private:
		list<CMBRNode> MBRList;
		list<CPartNode> PartList;

		void ReadStructure();

		void AddDrive(int Drive, unsigned long long StartSector, unsigned long long ExtStart, int Type);
		void CreatePartList(int FloppyCount);
		void CreatePartNode(list<CMBRNode>::iterator MBRNode, int Index);
		void CreateGPTPartNode(list<CMBRNode>::iterator MBRNode, int Index);
		void CreateNonPartNode(int Drive);
		const char *GetFSName(unsigned short FSID);
		uint32_t chksum_crc32 (uint32_t initial, const void *block, uint64_t length);
		void chksum_crc32gentab ();
		
		void CreatePLUP();

		CPartNode **PLUP;
		static const TFSNameEntry FSNameList[];
		int AllowActiveHD;

		bool PartListChanged;
		bool VolumeLabelsRead;

};



#endif