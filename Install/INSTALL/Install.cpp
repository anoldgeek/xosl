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
 * - Rename header file from "memory.h" to "memory_x.h" to make sure that
 *   the XOSL header file is used and not the Open Watcon header file.
 * - Modify string values for new XOSL version
 * - Comment out local variable const char *MbrFileName
 *   to get rid of Warning! W014: 
 *   no reference to symbol 'MbrFileName'
 *
 */



#include <Install.h>

#include <ptab.h>
#include <xosldata.h>
#include <memory_x.h>
#include <disk.h>
#include <fat16.h>
#include <string.h>
#include <text.h>
#include <key.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <xoslver.h>
#include <items.h>
#include <pre130a4>
#include <dosio.h>

char DiskFullMsg_ss[] = "failed\nDisk full %s %d.\n";

CInstaller::CInstaller(CTextUI &TextUIToUse, CPartList &PartListToUse):
	TextUI(TextUIToUse),
	PartList(PartListToUse),
	FatInstall(TextUIToUse,XoslFiles,DosFile),
	FsCreator(TextUIToUse,XoslFiles,DosFile)
{
}

CInstaller::~CInstaller()
{
}

int CInstaller::Install(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, const CDosDriveList::CDosDrive &DosDrive, bool PartMan, bool SmartBm, unsigned char MbrHDSector0)
{
	TIPL Ipl;

	if (!PartMan) {
		XoslFiles.IgnorePartManFiles();
	}
	if (CreateXoslData(GraphicsMode,MouseType) == -1)
		return -1;
	if (CreateBootItem(MbrHDSector0) == -1)
		return -1;
	if (BackupOriginalMbr(0,XoslFiles.GetOriginalMbrName(),MbrHDSector0) == -1)
		return -1; 

	if (SmartBm) {
		InstallSmartBootManager();
	}
	if (BackupOriginalMbr(-1,XoslFiles.GetSmartBmName(),MbrHDSector0) == -1) {
		return -1;
	}

	if (FatInstall.CreateIpl(DosDrive,Ipl) == -1)
		return -1;

	if (BackupCurrentMbr(&Ipl) == -1)
		return -1;

	if (FatInstall.InstallFiles(DosDrive) == -1)
		return -1;
	if (FatInstall.InstallIpl(&Ipl, MbrHDSector0) == -1)
		return -1;
	TextUI.OutputStr("\nInstall complete\n");
	return 0;
}

int CInstaller::Install(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, int PartIndex, bool PartMan, bool SmartBm, unsigned char MbrHDSector0)
{
	const TPartition *Partition;
	TIPL Ipl;
	CDosDriveList::CDosDrive DosDrive;

	if (!PartMan) {
		XoslFiles.IgnorePartManFiles();
	}
	Partition = PartList.GetPartition(PartIndex);
	DosDrive.Drive = Partition->Drive;
	DosDrive.FATType = FATTYPE_FAT16; // dedicated partition always FAT16
	DosDrive.StartSector = Partition->StartSector;

	if (Partition->SectorCount < 800) {
		TextUI.OutputStr("XOSL "XOSL_VERSION" requires a partition of\nat least 400kb\n\n");
		return -1;
	}
		
	if (CreateXoslData(GraphicsMode,MouseType) == -1)
		return -1;
	if (CreateBootItem(MbrHDSector0) == -1)
		return -1;
	if (BackupOriginalMbr(Partition->FSType,XoslFiles.GetOriginalMbrName(),MbrHDSector0) == -1)
		return -1;
	
	if (SmartBm) {
		InstallSmartBootManager();
	}
	if (BackupOriginalMbr(-1,XoslFiles.GetSmartBmName(), MbrHDSector0) == -1) {
		return -1;
	}

	if (FsCreator.InstallFs(Partition->Drive,Partition->StartSector,MbrHDSector0) == -1)
		return -1;

	if (FatInstall.CreateIpl(DosDrive,Ipl) == -1)
		return -1;

	if (BackupCurrentMbr(&Ipl,Partition->Drive,Partition->StartSector) == -1)
		return -1;
	if (Partition->Type != PART_GPT){
		// MBR Drive
		SetPartId(PartIndex,0x78);
		// Update the partition entries in case user returns to menu
		PartList.UpdateFSType(PartIndex, (unsigned short) 0x78, MbrHDSector0);
	}else{
		// GPT Drive
		SetPartId(PartIndex,0x7800);
		PartList.UpdateFSType(PartIndex, (unsigned short) 0x7800, MbrHDSector0);
	}

 	if (MbrHDSector0 != 0xff){
 		if (FatInstall.InstallIpl(&Ipl, MbrHDSector0) == -1)
 			return -1;
 		else
 			TextUI.OutputStr("\nInstall complete\n");
 	}else{
 		TextUI.OutputStr("\Install complete...\n   for chain loading only.\n");
 	}
	return 0;
}

int CInstaller::Uninstall(const CDosDriveList::CDosDrive &DosDrive, int OriginalMbr, unsigned char MbrHDSector0)
{
	char MbrBuffer[512];
	//const char *MbrFileName;

	if (!OriginalMbr || LoadDosMbr(DosDrive.DriveChar,XoslFiles.GetOriginalMbrName(),MbrBuffer) == -1)
		if (LoadDefaultMbr(MbrBuffer) == -1)
			return -1;
	
	if (FatInstall.InstallIpl(MbrBuffer, MbrHDSector0) == -1)
		return -1;

	FatInstall.RemoveXoslFiles(DosDrive.DriveChar);
	TextUI.OutputStr("\nUninstall complete\n");
	return 0;
}

int CInstaller::Uninstall(int PartIndex, int OriginalMbr, unsigned char MbrHDSector0)
{
	const TPartition *Partition;
	char OriginalMbrBuffer[512];
	char DefaultMbrBuffer[512];
	char *MbrBuffer;
	

	if (LoadRawMbr(PartIndex,XoslFiles.GetOriginalMbrName(),OriginalMbrBuffer) != -1)
		SetPartId(PartIndex,*(unsigned short *)&OriginalMbrBuffer[508]);

	if (!OriginalMbr) {
		if (LoadDefaultMbr(DefaultMbrBuffer) == -1)
			return -1;
		MbrBuffer = DefaultMbrBuffer;
	}
	else
		MbrBuffer = OriginalMbrBuffer;

		
	if (FatInstall.InstallIpl(MbrBuffer,MbrHDSector0) == -1)
		return -1;
		
	Partition = PartList.GetPartition(PartIndex);
	FsCreator.RestorePartition(Partition->Drive,Partition->StartSector);



	TextUI.OutputStr("\nUninstall complete\n");
	return 0;
}



int CInstaller::Restore(const CDosDriveList::CDosDrive &DosDrive, unsigned char MbrHDSector0)
{
	char Ipl[512];

	if (LoadDosMbr(DosDrive.DriveChar,XoslFiles.GetCurrentMbrName(),Ipl) == -1)
		return -1;

	if (FatInstall.InstallIpl(Ipl,MbrHDSector0) == -1)
		return -1;

	TextUI.OutputStr("\nRestore complete\n");
	return 0;
}

int CInstaller::Restore(int PartIndex, unsigned char MbrHDSector0)
{
	char CurrentMbr[512];


	if (LoadRawMbr(PartIndex,XoslFiles.GetCurrentMbrName(),CurrentMbr) == -1)
		return -1;

	if (FatInstall.InstallIpl(CurrentMbr,MbrHDSector0) == -1)
		return -1;

	TextUI.OutputStr("\nRestore complete\n");
	return 0;
}




int CInstaller::CreateXoslData(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType)
{
	CXoslData XoslData;
	CXoslData::CGraphData &GraphData = XoslData.GraphData;
	CXoslData::CColorData &ColorData = XoslData.ColorData;
	CXoslData::CMouseData &MouseData = XoslData.MouseData;
	CXoslData::CMiscPref &MiscPref = XoslData.MiscPref;
	int fh;

	TextUI.OutputStr("Creating data file...");

	MemSet(&XoslData,0,sizeof (CXoslData));

	GraphData.VideoMode = GraphicsMode;
	GraphData.FrameMove = 1;
	
	ColorData.ColorScheme = 15; // XOSL default
	ColorData.FadeColorInt = 100; // 100% -> White

	MouseData.MouseType = MouseType;
	MouseData.MouseSpeed = 5;
	MouseData.PS2Sampling = 5;

	MiscPref.KeyRestart = 0x4400;    // F10
	MiscPref.KeyColdReboot = 0x7100; // Alt-F10
	MiscPref.KeyShutdown = 0x6700;   // Ctrl-F10
	MiscPref.KeyCycle = 0x1100;      // Alt-W
	MiscPref.KeyPart = 0x1910;       // Ctrl-P
	MiscPref.ActiveAllow = 1;
	MiscPref.AutoSave = 1;

	XoslData.XoslVersion = 2;

	if ((fh = DosFile.Create(XoslFiles.GetXoslDataName())) == -1) {
		TextUI.OutputStr("failed\nUnable to create %s\n",XoslFiles.GetXoslDataName());
		return -1;
	}
	if (DosFile.Write(fh,&XoslData,sizeof (CXoslData)) != sizeof (CXoslData)) {
		TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
		DosFile.Close(fh);
		return -1;
	}
	DosFile.SetFileDateTime(fh);
	DosFile.Close(fh);
	TextUI.OutputStr("done\n");


	return 0;
}


int CInstaller::CreateBootItem(unsigned char MbrHDSector0)
{
	CBootItemFile *BootItemData = new CBootItemFile;
	int hFile;
	
	TextUI.OutputStr("Creating boot items file...");
	MemSet(BootItemData,0,BOOTITEM_FILESIZE);
	BootItemData->MbrHDSector0 = MbrHDSector0;
	BootItemData->BootItemVersion = CURRENT_BOOTITEM_VERSION;
	if ((hFile = DosFile.Create(XoslFiles.GetBootItemName())) == -1) {
		TextUI.OutputStr("failed\nUnable to create %s\n",XoslFiles.GetBootItemName());
		delete[] BootItemData;
		return -1;
	}
	if (DosFile.Write(hFile,BootItemData,BOOTITEM_FILESIZE) != BOOTITEM_FILESIZE) {
		TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
		DosFile.SetFileDateTime(hFile);
		DosFile.Close(hFile);
		delete[] BootItemData;
		return -1;
	}
	DosFile.SetFileDateTime(hFile);
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
	delete[] BootItemData;
	return 0;
}

int CInstaller::BackupOriginalMbr(int PartId, const char *DestFileName, unsigned char MbrHDSector0)
{
	CDisk Disk;
	unsigned short Mbr[256];
	int hFile;

	if (PartId != -1) {
		TextUI.OutputStr("Creating backup of MBR...");
	}
	else {
		TextUI.OutputStr("Creating SBM loader...");
	}
	Disk.Map(MbrHDSector0,0);
	Disk.Read(0,Mbr,1);

	if (PartId != -1) {
		Mbr[254] = PartId;
	}

	if ((hFile = DosFile.Create(DestFileName)) != -1) {
		if (DosFile.Write(hFile,Mbr,512) != 512) {
			TextUI.OutputStr("failed\nFloppy disk full.\n");
			DosFile.Close(hFile);
			return -1;
		}
		DosFile.SetFileDateTime(hFile);
		DosFile.Close(hFile);
	}

	TextUI.OutputStr("done\n");
	return 0;
}

int CInstaller::BackupCurrentMbr(void *Ipl)
{
	int hFile;

	TextUI.OutputStr("Creating backup of new MBR...");
	if ((hFile = DosFile.Create(XoslFiles.GetCurrentMbrName())) != -1) {
		if (DosFile.Write(hFile,Ipl,512) != 512) {
			TextUI.OutputStr("failed\nFloppy disk full.\n");
			DosFile.Close(hFile);
			return -1;
		}
		DosFile.SetFileDateTime(hFile);
		DosFile.Close(hFile);
		DosFile.SetFileDateTime(hFile);
	}

	TextUI.OutputStr("done\n");
	return 0;
}


int CInstaller::BackupCurrentMbr(void *Ipl, int Drive, unsigned long long StartSector)
{
	CFAT16 *FileSystem = new CFAT16;
	const char *DosFileName;
	char RawFileName[16];


	TextUI.OutputStr("Creating backup of new MBR...");
	
	DosFileName = XoslFiles.GetCurrentMbrName();
	FileSystem->DosFileToRawFile(RawFileName,DosFileName);

	FileSystem->Mount(Drive,StartSector);
	if (FileSystem->WriteFile(RawFileName,Ipl) == -1) {
		TextUI.OutputStr("failed\nFloppy disk full.\n");
		delete FileSystem;
		return -1;
	}

	TextUI.OutputStr("done\n");
	delete FileSystem;
	return 0;
}



int CInstaller::LoadDosMbr(int DriveChar, const char *DosFileName, void *MbrBuffer)
{
	int hFile;
	char FileName[24];

	FileName[0] = DriveChar;
	FileName[1] = ':';
	FileName[2] = '\\';
	strcpy(&FileName[3],DosFileName);

	TextUI.OutputStr("Loading backup of MBR...");
	if ((hFile = DosFile.Open(FileName,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("failed\nUnable to open %s\n",FileName);
		return -1;
	}
	DosFile.Read(hFile,MbrBuffer,512);
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
	return 0;
}


int CInstaller::LoadRawMbr(int PartIndex, const char *DosFileName, void *MbrBuffer)
{
	const TPartition *Partition;
	CFAT16 *FileSystem = new CFAT16; // dedicated partition always FAT16

	char RawFileName[16];

	Partition = PartList.GetPartition(PartIndex);

	if (FileSystem->Mount(Partition->Drive,Partition->StartSector) == -1) {
		TextUI.OutputStr("Unable to mount the partition\n");
		delete FileSystem;
		return -1;
	}

	FileSystem->DosFileToRawFile(RawFileName,DosFileName);

	TextUI.OutputStr("Loading backup of MBR...");
	if (FileSystem->ReadFile(RawFileName,MbrBuffer) != 512) {
		TextUI.OutputStr("failed\nUnable to load %s\n",DosFileName);
		delete FileSystem;
		return -1;
	}
	TextUI.OutputStr("done\n");
	delete FileSystem;
	return 0;
}

int CInstaller::LoadDefaultMbr(void *MbrBuffer)
{
	int hFile;

	TextUI.OutputStr("Loading default MBR...");
	if ((hFile = DosFile.Open(XoslFiles.GetDefaultMbrName(),CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("failed\nUnable to open %s\n",XoslFiles.GetDefaultMbrName());
		return -1;
	}
	DosFile.Read(hFile,MbrBuffer,512);
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
	return 0;
}


void CInstaller::SetPartId(int PartIndex, int PartId)
{
	char *errstr;

	TextUI.OutputStr("Updating partition tables...");
	PartList.SetFsType(PartIndex,PartId);
	if((errstr=PartList.WriteStructure()) != NULL ){
		TextUI.OutputStr(errstr);
	}
	else{
		TextUI.OutputStr("done\n");
	}
}


const char *SmartBmStatement =
"XOSL INSTALL UTILITY will now install Smart Boot Manager 3.7.1.\n"
"\n"
"* XOSL uses Smart Boot Manager (SBM) to provide CD-ROM booting support.\n"
"* To install SBM, sbminst.exe will be used. Because of this, XOSL \n"
"  install cannot determine whether installation of SBM was successful.\n"
"  sbminst.exe will ask for confirmation if you wish to install SBM.\n"
"* After install, sbmbckup.bin will hold a backup of the data over-\n"
"  written and can be used to fully uninstall SBM. XOSL, however, will\n"
"  not used this file to uninstall SBM - it does not required it. It is\n"
"  still provided so you are free to use it to fully uninstall SBM (and\n"
"  XOSL, if still installed).\n"
"* To start SBM, select 'Smart Boot Manager' from the partition list in\n"
"  XOSL. Use the boot keys feature to control SBM from XOSL (see manual\n"
"  for details).\n"
"* If installing SBM was not successful, booting 'Smart Boot Manager'\n"
"  from XOSL will have the same effect as booting the original MBR.\n"
"\n"
"Smart Boot Manager, Copyright (C) 2000 Suzhe";



void CInstaller::InstallSmartBootManager()
{
	TextUI.ShowPopup(3,2,74,20,SmartBmStatement);
	CKeyboard Keyboard;
	TextUI.HandleKey(Keyboard.GetCh());

	LockScreen();
	ClearScreen();

	puts("Loading SBMINST.EXE...");
	int Status = spawnl(P_WAIT,"SBMINST.EXE","SBMINST.EXE","-b sbmbckup.bin","-d 128",0);

	if (Status != -1) {
		puts("\nPress any key to continue XOSL install...");
		Keyboard.GetCh();
	}
	else {
		perror("");
		Keyboard.GetCh();
	}

	UnlockScreen();

	if (Status == -1) {
		TextUI.OutputStr("Unable to run sbminst.exe...ignored\n");
	}



}

int CInstaller::CopyFileForUpgrade(const char *FileName, char DriveChar)
{
	CDosFile DosFile;
	char CopySrc[20];

	char RootPath[] = "C:\\";

	strcpy(CopySrc,RootPath);
	strcat(CopySrc,FileName);
	*CopySrc = DriveChar;
	if(DosFile.Copy(CopySrc,FileName) == -1){
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade %s \n\n",FileName);
		return -1;
	}
	return 0;
}
int CInstaller::UpgradeXoslBootItem(unsigned char MbrHDSector0)
{
	CDosFile DosFile;
	CXoslFiles XoslFiles;
	CBootItemFile *BootItemData;
	int fh;
	const char *BootItemsFileName;
	int filesize;
	
	BootItemsFileName = XoslFiles.GetBootItemName();
	filesize = DosFile.FileSize(BootItemsFileName);
	if (filesize == BOOTITEM_FILESIZE || filesize == O_BOOTITEM_FILESIZE){
		if ( (fh = DosFile.Open(BootItemsFileName,CDosFile::accessReadWrite)) != -1){
			BootItemData = new CBootItemFile;
			DosFile.Read(fh,BootItemData,filesize);
			if( filesize == O_BOOTITEM_FILESIZE || BootItemData->BootItemVersion != CURRENT_BOOTITEM_VERSION){
				// BootItemData is from old version, upgrade it 
				pre130a4CBootItemFile *oldBootItemData;
				CUpgrade *upgradeBootItems = new CUpgrade();
				oldBootItemData = (pre130a4CBootItemFile*)BootItemData;
				BootItemData = upgradeBootItems->upgradeBootItems(oldBootItemData,MbrHDSector0);
			}
			DosFile.LSeek(fh,0,CDosFile::seekStart);
			if(DosFile.Write(fh,BootItemData,BOOTITEM_FILESIZE) == BOOTITEM_FILESIZE){
				DosFile.Close(fh);
				delete BootItemData;
				return 0;
			}
		}
	}
	delete BootItemData;
	return -1;
}

int CInstaller::Upgrade(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, const CDosDriveList::CDosDrive &DosDrive, bool PartMan, bool SmartBm, unsigned char MbrHDSector0)
{
	TIPL Ipl;

	if (!PartMan) {
		XoslFiles.IgnorePartManFiles();
	}
	if(CopyFileForUpgrade(XoslFiles.GetXoslDataName(),DosDrive.DriveChar) == -1)
		return -1;

	if(CopyFileForUpgrade(XoslFiles.GetBootItemName(),DosDrive.DriveChar) == -1)
		return -1;
	// Upgrade bootitems 
	UpgradeXoslBootItem(MbrHDSector0);

	if(CopyFileForUpgrade(XoslFiles.GetOriginalMbrName(),DosDrive.DriveChar) == -1)
		return -1; 

	if (SmartBm)
		InstallSmartBootManager();

	if(CopyFileForUpgrade(XoslFiles.GetSmartBmName(),DosDrive.DriveChar) == -1)
		return -1;

	if (FatInstall.CreateIpl(DosDrive,Ipl) == -1)
		return -1;

	if(CopyFileForUpgrade(XoslFiles.GetCurrentMbrName(),DosDrive.DriveChar) == -1)
		return -1;

	if (FatInstall.InstallFiles(DosDrive) == -1)
		return -1;
	if (FatInstall.InstallIpl(&Ipl, MbrHDSector0) == -1)
		return -1;
	TextUI.OutputStr("\Upgrade complete\n");
	return 0;
}

int CInstaller::Upgrade(CVesa::TGraphicsMode GraphicsMode, CMouse::TMouseType MouseType, int PartIndex, bool PartMan, bool SmartBm, unsigned char MbrHDSector0)
{
	const TPartition *Partition;
	TIPL Ipl;
	CDosDriveList::CDosDrive DosDrive;

	if (!PartMan) {
		XoslFiles.IgnorePartManFiles();
	}
	Partition = PartList.GetPartition(PartIndex);
	DosDrive.Drive = Partition->Drive;
	DosDrive.FATType = FATTYPE_FAT16; // dedicated partition always FAT16
	DosDrive.StartSector = Partition->StartSector;

	if (Partition->FSType != 0x78){ // XOSL FS
		TextUI.OutputStr("XOSL "XOSL_VERSION". Please select a XOSL FS partition for upgrade\n\n");
		return -1;
	}

	if (Partition->SectorCount < 800) {
		TextUI.OutputStr("XOSL "XOSL_VERSION" requires a partition of\nat least 800kb\n\n");
		return -1;
	}
		
	// Save the existing data file and restore 
	if (PartList.Retain(XoslFiles.GetXoslDataName(),512,Partition) == -1){
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade %s \n\n",XoslFiles.GetXoslDataName());
		return -1;
	}

	// Save the existing bootitem file, upgrade and restore
	if (PartList.UpgradeXoslBootItem(Partition,MbrHDSector0) == -1){
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade %s \n\n",XoslFiles.GetBootItemName());
		return -1;
	}

	if (PartList.Retain(XoslFiles.GetOriginalMbrName(),512,Partition) == -1){
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade %s \n\n",XoslFiles.GetOriginalMbrName());
		return -1;
	}
	
	if (SmartBm) {
		InstallSmartBootManager();
	}

	if (PartList.Retain(XoslFiles.GetSmartBmName(),512,Partition) == -1) {
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade %s \n\n",XoslFiles.GetSmartBmName());
		return -1;
	}

	if (FsCreator.InstallFs(Partition->Drive,Partition->StartSector,MbrHDSector0) == -1){
		TextUI.OutputStr("XOSL "XOSL_VERSION" failed to upgrade in %s \n\n","FsCreator.InstallFs");
		return -1;
	}
	if (FatInstall.CreateIpl(DosDrive,Ipl) == -1)
		return -1;

	if (BackupCurrentMbr(&Ipl,Partition->Drive,Partition->StartSector) == -1)
		return -1;

	SetPartId(PartIndex,0x78);

 	if (MbrHDSector0 != 0xff){
 		if (FatInstall.InstallIpl(&Ipl, MbrHDSector0) == -1)
 			return -1;
 		else
 			TextUI.OutputStr("\nUpgrade complete\n");
 	}else{
 		TextUI.OutputStr("\Upgrade complete...\n   for chain loading only.\n");
 	}
	return 0;
}

