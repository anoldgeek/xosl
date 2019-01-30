/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __items__
#define __items__

#include <newdefs.h>
#include <lastconf.h>
#include <ptab.h>
#include <fs.h>
#include <fatfix.h>

#ifdef PRE130BISIZE
#define BI_PARTS 56
#define BI_BOOTITEMS 24
#define BI_BOOTITEMS_STR "24"
#define BI_RESERVED 11
#define BI_FILE_RES 17
#else
/*
#define BI_PARTS 56
#define BI_BOOTITEMS 20
#define BI_BOOTITEMS_STR "20"
#define BI_RESERVED 9
#define BI_FILE_RES 17
#define PD_RES 8
*/
#define BI_PARTS 230
#define BI_BOOTITEMS 24
#define BI_BOOTITEMS_STR "24"
#define BI_RESERVED 9
#define BI_FILE_RES 449
#define PD_RES 8

#define CURRENT_BOOTITEM_VERSION 2

#endif

class CBootItem {
public:
	char ItemName[48];
	unsigned char PartIndex;
	unsigned short Hotkey;
	char Activate;
	char FixDriveNum;
	char Disabled;
	unsigned long Password;
	unsigned short Keys[16];
	unsigned char HideList[BI_PARTS];
	char SwapDrives;
	char Reserved[BI_RESERVED]; // reserved for future use
}; // sizeof(CBootItem) == 330

class CPartDesc {
public:
	unsigned char Drive;
	unsigned long long StartSector;
	char Reserved[PD_RES];  // reserved for future use
}; // sizeof(CPartDesc) == 17


class CBootItemFile {
public:
	CBootItem BootItems[BI_BOOTITEMS];
	CPartDesc PartList[BI_PARTS];
	unsigned char BootItemCount;
	unsigned char DefaultItem;
	unsigned char Reserved0; // was timeout
	unsigned char PartCount;
	unsigned short Timeout;
	unsigned char MbrHDSector0; // Which HD sector 0 (mbr) was choosen for this xosl install
	unsigned short	BootItemVersion;
	char Reserved[BI_FILE_RES]; // reserved for future use
}; //  sizeof(CBootItemFile) == 4096 * 3 = 12288



class CBootItems {
	public:
		CBootItems(CFileSystem &FileSystemToUse, CPartList &PartListToUse);
		~CBootItems();
		int GetCount();
		CBootItem *Get(int Index);
		int Add(CBootItem &NewItem);
		void Remove(int Index);
		void Clear();
		void Swap(int Index1, int Index2);

		
		void Save();
		void Reset();

		void SetDefault(int Index);
		int GetDefault();
		int GetTimeout();
		void SetTimeout(int Timeout);

		int InitBoot(int BootIndex);

		bool IsDefault(int Index);
		bool CanFixFAT(int Index);
		bool CanSwap(int Index);

	private:
		CDriveFix DriveFix;
		CBootItemFile ItemFile;
		
		CFileSystem &FileSystem;
		CPartList &PartList;
            
		void MoveUp(int Index);
		void StoreKeys(const unsigned short *Keys);

		void Initialize();
	    bool PartListIsChanged();
		void AdjustPartList();

		void BuildRelocTable(int *RelocTable);
		void UpdatePartList(int *RelocTable);

		void UpdatePartIndex(CBootItem &BootItem, int *RelocTable);
		void UpdateHideList(unsigned char *HideList, int *RelocTable);

};



#endif