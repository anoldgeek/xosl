/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Norman Back
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */


#ifndef _PRE130A4_
#define _PRE130A4_

// old, pre 130a4 values
#define O_BI_PARTS 56
#define O_BI_BOOTITEMS 24
#define O_BI_BOOTITEMS_STR "24"
#define O_BI_RESERVED 11
#define O_BI_FILE_RES 17

#if FALSE // 130a4 values
#define BI_PARTS 56
#define BI_BOOTITEMS 20
#define BI_BOOTITEMS_STR "20"
#define BI_RESERVED 9
#define BI_FILE_RES 17
#define PD_RES 8
#endif



//namespace pre130a4{

class pre130a4CBootItem {
	public:
		char ItemName[48];
		unsigned char PartIndex;
		unsigned short Hotkey;
		char Activate;
		char FixDriveNum;
		char Disabled;
		unsigned long Password;
		unsigned short Keys[16];
		unsigned char HideList[56];
		char SwapDrives;
		char Reserved[11]; // reserved for future use
	}; // sizeof(CBootItem) == 158

	class pre130a4CPartDesc {
	public:
		unsigned char Drive;
		unsigned long StartSector;
	}; // sizeof(CPartDesc) == 5


	class pre130a4CBootItemFile {
	public:
		pre130a4CBootItem BootItems[24];
		pre130a4CPartDesc PartList[56];
		unsigned char BootItemCount;
		unsigned char DefaultItem;
		unsigned char Reserved0; // was timeout
		unsigned char PartCount;
		unsigned short Timeout;
		unsigned char MbrHDSector0; // Which HD sector 0 (mbr) was choosen for this xosl install
		char Reserved[17]; // reserved for future use
	}; //  sizeof(CBootItemFile) == 4096
//}

class CUpgrade {
	public:
	CUpgrade();
	~CUpgrade();
	CBootItemFile* upgradeBootItems(pre130a4CBootItemFile *oldBootItems, unsigned char MbrHDSector0);
};
#endif



