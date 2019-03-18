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
 * - Modify string values for new XOSL version
 * - Change return type of function main() from void to int to
 *   get rid of Warning! W914: col(1) 'main' must return 'int'
 *
 */


#include <stdio.h>
#include <string.h>
#include <main.h>
#include <text.h>
#include <xoslver.h>
#include <applic.h>

static const char *InstallWarning = 
"PLEASE READ THE FOLLOWING CAREFULLY\n"
"\n"
"You have selected to install XOSL on a dedicated\n"
"partition. Note that this is a destructive action. ALL\n"
"DATA ON THE SELECTED PARTITION WILL BE DESTROYED! So\n"
"make sure you double-check the partition you select,\n"
"that it is ABSOLUTELY the partition to install XOSL on.\n"
"\n"
"Also, XOSL does not create its own partition. Instead,\n"
"you have to create a dedicated partition for XOSL\n"
"yourself, BEFORE you run install.\n"
"\n"
"Press any key to continue...";

const char InstallNotes[] =
"Extended Operating System Loader "XOSL_VERSION"\n"
"\n"
"Make sure you have read NOTES.TXT!\n"
"\n"
"Before installling XOSL "XOSL_VERSION", it is\n"
"highly recommended you first read the\n"
"installation walkthrough, found in the\n"
"manual/install directroy\n"
"\n"
"Press any key to continue...";

CApplication::CApplication(TXoslWorkConfig *XoslWorkConfigToUse):
	TextScreen(new CTextScreen(0x1f00)),
	TextUI(*TextScreen),
	Data(TextUI,PartList),
	Installer(TextUI,PartList,XoslWorkConfig),
	InstallMenus(TextUI,Data,PartList,(CData::GetDosVersion() & 0xff) > 4 && (CData::GetDriveCount() - ('C' - 'A')) > 0)
{
	DoExit = 0;
	DoReboot = 0;
	TextUI.OutputStr("Extended Operating System Loader\ncomes with ABSOLUTELY NO WARRANTY\n\n");

	InstallMenus.InitMainMenu((CTextList::TListItemExecute)MainMenuExecute,this);
	
	TextUI.ShowPopup(18,5,44,14,InstallNotes);
	XoslWorkConfig = XoslWorkConfigToUse;
}

CApplication::~CApplication()
{
	delete TextScreen;
	if (DoReboot)
		Reboot();
}


void CApplication::ApplicationLoop()
{
	int Key;

	while (!DoExit) {
		while (!Keyboard.KbHit());
		Key = Keyboard.GetCh();
		if (Key == KEY_ESCAPE)
			DoExit = 1;
		TextUI.HandleKey(Key);
	}

}


int CApplication::StartInstallFat()
{
	CVesa::TGraphicsMode GraphicsMode;
	CMouse::TMouseType MouseType;

	CDosDriveList DosDriveList(PartList);
	CDosDriveList::CDosDrive DosDrive;
	unsigned char MbrHDSector0;
	int DosDriveIndex;
	bool PartMan;
	bool SmartBootManager;

	GraphicsMode = Data.GetGraphicsMode(TextUI.GetOptionIndex(0));
	MouseType = Data.GetMouseType(TextUI.GetOptionIndex(1));
	DosDriveIndex = TextUI.GetOptionIndex(2);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(3));

	PartMan = TextUI.GetOptionIndex(4) == 0;
	SmartBootManager = TextUI.GetOptionIndex(5) == 0;

	if (DosDriveList.LocateDrive(DosDriveIndex,DosDrive) == -1) {
		TextUI.OutputStr("Unable to locate drive %c:\n",'C' + DosDriveIndex);
		return -1;
	}

	if (Installer.Install(GraphicsMode,MouseType,DosDrive,PartMan,SmartBootManager,MbrHDSector0) == -1) {
		TextUI.OutputStr("Install error\n");
		return -1;
	}else{
		// Update MbrHDSector0 for this drive
		DosDriveList.UpdateDosDriveMbrHDSector0(DosDrive,MbrHDSector0);
		InstallMenus.UpdateDosDriveMbrHDSector0(DosDriveIndex,MbrHDSector0);
	}

	return 0;
}

int CApplication::StartInstallSep()
{
	CVesa::TGraphicsMode GraphicsMode;
	CMouse::TMouseType MouseType;
	int PartIndex;
	unsigned char MbrHDSector0;
	bool PartMan;
	bool SmartBootManager;
	int PartNameIndex;
	
	GraphicsMode = Data.GetGraphicsMode(TextUI.GetOptionIndex(0));
	MouseType = Data.GetMouseType(TextUI.GetOptionIndex(1));
	PartMan = TextUI.GetOptionIndex(2) == 0;
	SmartBootManager = TextUI.GetOptionIndex(3) == 0;

	PartNameIndex = TextUI.GetOptionIndex(4);
	PartIndex = InstallMenus.ResolvePartIndex(PartNameIndex);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(8));

	if (Installer.Install(GraphicsMode,MouseType,PartIndex,PartMan,SmartBootManager,MbrHDSector0) == -1) {
		TextUI.OutputStr("Install error\n");
		return -1;
	}else{
		// Update PartNameList item after FSType change
		InstallMenus.UpdatePartNameItem(PartNameIndex,PartIndex,MbrHDSector0);
	}
	return 0;
}

int CApplication::StartUpgradeFat()
{
	CVesa::TGraphicsMode GraphicsMode;
	CMouse::TMouseType MouseType;

	CDosDriveList DosDriveList(PartList);
	CDosDriveList::CDosDrive DosDrive;
	int DosDriveIndex;
	unsigned char MbrHDSector0;
	bool PartMan;
	bool SmartBootManager;

	GraphicsMode = Data.GetGraphicsMode(TextUI.GetOptionIndex(0));
	MouseType = Data.GetMouseType(TextUI.GetOptionIndex(1));
	DosDriveIndex = TextUI.GetOptionIndex(2);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(3));

	PartMan = TextUI.GetOptionIndex(4) == 0;

	SmartBootManager = TextUI.GetOptionIndex(5) == 0;

	if (DosDriveList.LocateDrive(DosDriveIndex,DosDrive) == -1) {
		TextUI.OutputStr("Unable to locate drive %c:\n",'C' + DosDriveIndex);
		return -1;
	}

	if (Installer.Upgrade(GraphicsMode,MouseType,DosDrive,PartMan,SmartBootManager,MbrHDSector0) == -1) {
		TextUI.OutputStr("Upgrade error\n");
		return -1;
	}else{
		// Update MbrHDSector0 for this drive
		DosDriveList.UpdateDosDriveMbrHDSector0(DosDrive,MbrHDSector0);
		InstallMenus.UpdateDosDriveMbrHDSector0(DosDriveIndex,MbrHDSector0);
	}

	return 0;
}

int CApplication::StartUpgradeSep()
{
	CVesa::TGraphicsMode GraphicsMode;
	CMouse::TMouseType MouseType;
	int PartIndex;
	unsigned char MbrHDSector0;
	bool PartMan;
	bool SmartBootManager;
	int PartNameIndex;
	
	GraphicsMode = Data.GetGraphicsMode(TextUI.GetOptionIndex(0));
	MouseType = Data.GetMouseType(TextUI.GetOptionIndex(1));

	PartMan = TextUI.GetOptionIndex(2) == 0;

	SmartBootManager = TextUI.GetOptionIndex(3) == 0;

	PartNameIndex = TextUI.GetOptionIndex(4);

	PartIndex = InstallMenus.ResolvePartIndex(PartNameIndex);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(8));

	if (Installer.Upgrade(GraphicsMode,MouseType,PartIndex,PartMan,SmartBootManager,MbrHDSector0) == -1) {
		TextUI.OutputStr("Upgrade error\n");
		return -1;
	}else{
		// Update PartNameList item after HDSector0 change
		InstallMenus.UpdateMbrHDSector0(PartNameIndex,MbrHDSector0);
	}
	return 0;
}


int CApplication::StartRestoreFat()
{
	CDosDriveList DosDriveList(PartList);
	CDosDriveList::CDosDrive DosDrive;
	int DosDriveIndex;
	unsigned char MbrHDSector0;

	DosDriveIndex = TextUI.GetOptionIndex(0);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(1));

	if (DosDriveList.LocateDrive(DosDriveIndex,DosDrive) == -1) {
		TextUI.OutputStr("Unable to locate drive %c:\n",'C' + DosDriveIndex);
		return -1;
	}

	if (Installer.Restore(DosDrive,MbrHDSector0) == -1) {
		TextUI.OutputStr("Install error\n");
		return -1;
	}
	return 0;

}

int CApplication::StartRestoreSep()
{
	int PartIndex;
	unsigned char MbrHDSector0;
	
	PartIndex = InstallMenus.ResolvePartIndex(TextUI.GetOptionIndex(0));
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(4));

	if (Installer.Restore(PartIndex,MbrHDSector0) == -1) {
		TextUI.OutputStr("Install error\n");
		return -1;
	}

	return 0;
}


int CApplication::StartUninstallFat()
{
	CDosDriveList DosDriveList(PartList);
	CDosDriveList::CDosDrive DosDrive;
	int DosDriveIndex;
	int OriginalMbr;
	unsigned char MbrHDSector0;

	DosDriveIndex = TextUI.GetOptionIndex(0);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(1));
	OriginalMbr = !TextUI.GetOptionIndex(2);

	if (DosDriveList.LocateDrive(DosDriveIndex,DosDrive) == -1) {
		TextUI.OutputStr("Unable to locate drive %c:\n",'C' + DosDriveIndex);
		return -1;
	}
	if(Installer.Uninstall(DosDrive,OriginalMbr,MbrHDSector0) == -1){
		return -1;
	}else{
		// Update MbrHDSector0 for this drive
		DosDriveList.UpdateDosDriveMbrHDSector0(DosDrive,0x80);
		InstallMenus.UpdateDosDriveMbrHDSector0(DosDriveIndex,0x80);
	}
	return 0;
}

int CApplication::StartUninstallSep()
{
	int PartIndex;
	int OriginalMbr;
	int PartNameIndex;
	unsigned char MbrHDSector0;
	
	OriginalMbr = !TextUI.GetOptionIndex(0);
	PartNameIndex = TextUI.GetOptionIndex(2);
	PartIndex = InstallMenus.ResolvePartIndex(PartNameIndex);
	MbrHDSector0 = InstallMenus.ResolveHDIndex(TextUI.GetOptionIndex(6));
	if (Installer.Uninstall(PartIndex,OriginalMbr,MbrHDSector0) == -1){
		TextUI.OutputStr("Uninstall error\n");
		return -1;
	}
	else{
		// Update PartNameList item after FSType change
		InstallMenus.UpdatePartNameItem(PartNameIndex,PartIndex,MbrHDSector0);
		return 0;
	}
}

void CApplication::MainMenuExecute(CApplication *Application, TEnumMainMenu Item)
{
	switch (Item) {
		case enumInstall:
			if (Application->Data.CollectData(1,1,1) == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitInstallMenu((CTextList::TListItemExecute)Application->InstallMenuExecute,Application);
			break;
		case enumRestore:
			Application->Data.CollectData(0,0,1);
			Application->InstallMenus.InitFixMbrMenu((CTextList::TListItemExecute)Application->RestoreMenuExecute,Application);
			break;
		case enumUninstall:
			Application->Data.CollectData(0,0,1);
			Application->InstallMenus.InitUninstallMenu((CTextList::TListItemExecute)Application->UninstallMenuExecute,Application);
			break;
		case enumExit:
			Application->DoExit = 1;
			break;
		case enumUpgrade:
			if (Application->Data.CollectData(1,1,1) == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitUpgradeMenu((CTextList::TListItemExecute)Application->UpgradeMenuExecute,Application);
			break;
		default:
			break;
	}
}


void CApplication::InstallMenuExecute(CApplication *Application, TEnumInstallMenu Item)
{
	switch (Item) {
		case enumInstFat:
			Application->InstallMenus.InitInstFatMenu((CTextList::TListItemExecute)Application->InstFatMenuExecute,Application);
			break;
		case enumInstSep:
			Application->InstallMenus.InitInstSepMenu((CTextList::TListItemExecute)Application->InstSepMenuExecute,Application);
			Application->TextUI.OutputStr("WARNING:\nAll data on the partition you install XOSL on will be destroyed!\n\n");
			Application->TextUI.ShowPopup(10,4,60,17,InstallWarning);
			break;
		case enumInstAbort:
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::UpgradeMenuExecute(CApplication *Application, TEnumUpgradeMenu Item)
{
	switch (Item) {
		case enumUpgradeFat:
			Application->InstallMenus.InitUpgradeFatMenu((CTextList::TListItemExecute)Application->UpgradeFatMenuExecute,Application);
			break;
		case enumUpgradeSep:
			Application->InstallMenus.InitUpgradeSepMenu((CTextList::TListItemExecute)Application->UpgradeSepMenuExecute,Application);
//			Application->TextUI.OutputStr("WARNING:\nAll data on the partition you install XOSL on will be destroyed!\n\n");
//			Application->TextUI.ShowPopup(10,4,60,17,InstallWarning);
			break;
		case enumUpgradeAbort:
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		default:
			break;
	}
}



void CApplication::RestoreMenuExecute(CApplication *Application, TEnumInstallMenu Item)
{
	switch (Item) {
		case enumInstFat:
			Application->InstallMenus.InitFixMbrFat((CTextList::TListItemExecute)Application->RestoreFatExecute,Application);
			break;
		case enumInstSep:
			Application->InstallMenus.InitFixMbrSep((CTextList::TListItemExecute)Application->RestoreSepExecute,Application);
			break;
		case enumInstAbort:
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		default:
			break;
	}
}


void CApplication::UninstallMenuExecute(CApplication *Application, TEnumInstallMenu Item)
{
	switch (Item) {
		case enumInstFat:
			Application->InstallMenus.InitUninstallFat((CTextList::TListItemExecute)Application->UninstallFatExecute,Application);
			break;
		case enumInstSep:
			Application->InstallMenus.InitUninstallSep((CTextList::TListItemExecute)Application->UninstallSepExecute,Application);
			break;
		case enumInstAbort:
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		default:
			break;
	}
}




void CApplication::InstFatMenuExecute(CApplication *Application, TEnumInstFatMenu Item)
{
	switch (Item) {
		case enumInstFatStart:
			if (Application->StartInstallFat() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumInstFatAbort:
			Application->InstallMenus.InitInstallMenu((CTextList::TListItemExecute)Application->InstallMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::UpgradeFatMenuExecute(CApplication *Application, TEnumUpgradeFatMenu Item)
{
	switch (Item) {
		case enumUpgradeFatStart:
			if (Application->StartUpgradeFat() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumUpgradeFatAbort:
			Application->InstallMenus.InitInstallMenu((CTextList::TListItemExecute)Application->UpgradeMenuExecute,Application);
			break;
		default:
			break;
	}
}


void CApplication::InstSepMenuExecute(CApplication *Application, TEnumInstSepMenu Item)
{
	switch (Item) {
		case enumInstSepStart:
			if (Application->StartInstallSep() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumInstSepAbort:
			Application->InstallMenus.InitInstallMenu((CTextList::TListItemExecute)Application->InstallMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::UpgradeSepMenuExecute(CApplication *Application, TEnumUpgradeSepMenu Item)
{
	switch (Item) {
		case enumUpgradeSepStart:
			if (Application->StartUpgradeSep() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumUpgradeSepAbort:
			Application->InstallMenus.InitUpgradeMenu((CTextList::TListItemExecute)Application->UpgradeMenuExecute,Application);
			break;
		default:
			break;
	}
}


void CApplication::ErrorMenuExecute(CApplication *Application, TEnumErrorMenu Item)
{
	switch (Item) {
		case enumErrorMain:
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		case enumErrorQuit:
			Application->DoExit = 1;
			break;
		default:
			break;
	}
}

void CApplication::DoneMenuExecute(CApplication *Application, TEnumDoneMenu Item)
{
	switch (Item) {
		case enumDoneReboot:
			Application->DoReboot = 1;
		case enumDoneExit:
			Application->DoExit = 1;
			break;
		case enumDoneMain:
			// Application->InstallMenus.ResetPartNameList(); // Ensure changed partitions are reread. But causes corruption!!
			Application->InstallMenus.InitMainMenu((CTextList::TListItemExecute)Application->MainMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::UninstallFatExecute(CApplication *Application, TEnumFatUninstallMenu Item)
{
	switch (Item) {
		case enumFatUnStart:
			if (Application->StartUninstallFat() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitUnDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);			
			break;
		case enumFatUnAbort:
			Application->InstallMenus.InitUninstallMenu((CTextList::TListItemExecute)Application->UninstallMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::UninstallSepExecute(CApplication *Application, TEnumSepUninstallMenu Item)
{
	switch (Item) {
		case enumSepUnStart:
			if (Application->StartUninstallSep() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitUnDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);			
			break;
		case enumSepUnAbort:
			Application->InstallMenus.InitUninstallMenu((CTextList::TListItemExecute)Application->UninstallMenuExecute,Application);
			break;
		default:
			break;
	}
}


void CApplication::RestoreFatExecute(CApplication *Application, TEnumFatUninstallMenu Item)
{
	switch (Item) {
		case enumFatFixStart:
			if (Application->StartRestoreFat() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumFatFixAbort:
			Application->InstallMenus.InitFixMbrMenu((CTextList::TListItemExecute)Application->RestoreMenuExecute,Application);
			break;
		default:
			break;
	}
}

void CApplication::RestoreSepExecute(CApplication *Application, TEnumSepUninstallMenu Item)
{
	switch (Item) {
		case enumSepFixStart:
			if (Application->StartRestoreSep() == -1)
				Application->InstallMenus.InitErrorMenu((CTextList::TListItemExecute)Application->ErrorMenuExecute,Application);
			else
				Application->InstallMenus.InitDoneMenu((CTextList::TListItemExecute)Application->DoneMenuExecute,Application);
			break;
		case enumSepFixAbort:
			Application->InstallMenus.InitFixMbrMenu((CTextList::TListItemExecute)Application->RestoreMenuExecute,Application);
			break;
		default:
			break;
	}
}
