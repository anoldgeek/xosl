/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */


#ifndef InstallH
#define InstallH

#include <defs.h>
#include <textui.h>

#include <vesa.h>
#include <mouse.h>
#include <dosdrv.h>
#include <files.h>

#include <dosio.h>

#include <FatInst.h>
#include <FsCreate.h>
#include <main.h>


#define BOOTITEM_FILESIZE sizeof(CBootItemFile)
#define LASTCONF_FILESIZE 512


class CInstaller {
public:
	CInstaller(CTextUI &TextUIToUse, CPartList &PartListToUse);
	~CInstaller();
	int Install(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, const CDosDriveList::CDosDrive &DosDrive, bool PartMan, bool SmartBm, unsigned char MbrHDSector0);
	int Install(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, int PartIndex, bool PartMan, bool SmartBm, unsigned char MbrHDSector0);

	int Uninstall(const CDosDriveList::CDosDrive &DosDrive, int OriginalMbr, unsigned char MbrHDSector0);
	int Uninstall(int PartIndex, int OriginalMbr, unsigned char MbrHDSector0);

	int Restore(const CDosDriveList::CDosDrive &DosDrive, unsigned char MbrHDSector0);
	int Restore(int PartIndex, unsigned char MbrHDSector0);

	int Upgrade(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, const CDosDriveList::CDosDrive &DosDrive, bool PartMan, bool SmartBm, unsigned char MbrHDSector0);
	int Upgrade(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, int PartIndex, bool PartMan, bool SmartBm, unsigned char MbrHDSector0);
	
private:
	CTextUI &TextUI;
	CPartList &PartList;
	CXoslFiles XoslFiles;
	CDosFile DosFile;

	CFatInstall FatInstall;
	CFsCreator FsCreator;

	int CreateXoslData(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType);
	int CreateBootItem(unsigned char MbrHDSector0);

	int BackupOriginalMbr(int PartId, const char *DestFileName, unsigned char MbrHDSector0);
	int BackupCurrentMbr(void *Ipl);
	int BackupCurrentMbr(void *Ipl, int Drive, unsigned long long StartSector);

	void RemoveXoslFiles(char DosDriveChar);

	int LoadDosMbr(int DriveChar, const char *DosFileName, void *MbrBuffer);
	int LoadRawMbr(int PartIndex, const char *DosFileName, void *MbrBuffer);

	int LoadDefaultMbr(void *MbrBuffer);

	void SetPartId(int PartIndex, int PartId);

	void InstallSmartBootManager();

	int CopyFileForUpgrade(const char *FileName, char DriveChar);

	int UpgradeXoslBootItem(unsigned char MbrHDSector0);

};

#endif
