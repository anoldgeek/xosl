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
 * - Rename header file from "Memory.h" to "Memory_x.h" to make sure that
 *   the XOSL header file is used and not the Open Watcon header file.
 * - Modify string values for new XOSL version
 * - To get rid of Warning! W389: integral value may be truncated during 
 *   assignment or initialization use proper casting in:
 *   itoa(Partition->SectorCount >> 11,&PartStr[29],10);
 *
 */


#include "menus.h"
#include <string.h>
#include <stdlib.h>
#include <memory_x.h>
#include <xoslver.h>
#include <gptab.h>

static const char *YesNoList[2] = {"Yes", "No"};


CInstallMenus::CInstallMenus(CTextUI &TextUIToUse, CData &DataToUse, CPartList &PartListToUse, int FatAvailToUse):
	TextUI(TextUIToUse),
	Data(DataToUse),
	PartList(PartListToUse)
{
	FatAvail = FatAvailToUse;
	DosDriveList = NULL;
	PartNameList = NULL;
}

CInstallMenus::~CInstallMenus()
{
	if (DosDriveList) {
		delete DosDriveList[0];
		delete DosDriveList;
	}
	// ResetPartNameList();
}

void CInstallMenus::InitMainMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Main \xcd\xcd\xcd\xcd\xcd");
	TextUI.AddMenuItem(0,"Install XOSL","Install Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(2,"Uninstall XOSL","Uninstall Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(4,"Restore XOSL","Make your system start XOSL again (typically after installing an OS)",1);

	TextUI.AddMenuItem(6,"Exit install","Quit without installing Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(8,"Upgrade XOSL","Upgrade Extended Operating System Loader to "XOSL_VERSION"",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitInstallMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Install \xcd\xcd");
	TextUI.AddMenuItem(0,"Install on a DOS drive","Install XOSL "XOSL_VERSION" on a DOS drive (FAT16/FAT32)",FatAvail);
	TextUI.AddMenuItem(2,"Install on a dedicated partition","Install XOSL "XOSL_VERSION" on an existing dedicated partition",1);
	TextUI.AddMenuItem(6,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(FatAvail ? 0 : 2);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUpgradeMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Upgrade \xcd\xcd");
	TextUI.AddMenuItem(0,"Upgrade a DOS drive","Upgrade XOSL to "XOSL_VERSION" on a DOS drive (FAT16/FAT32)",FatAvail);
	TextUI.AddMenuItem(2,"Upgrade a dedicated partition","Upgrade XOSL to "XOSL_VERSION" on an existing dedicated partition",1);
	TextUI.AddMenuItem(6,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(FatAvail ? 0 : 2);
	TextUI.RefreshMenu();
}


void CInstallMenus::InitFixMbrMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Restore \xcd\xcd");
	TextUI.AddMenuItem(0,"Restore on a DOS drive","Restore XOSL "XOSL_VERSION", installed on a DOS drive",FatAvail);
	TextUI.AddMenuItem(2,"Restore on a dedicated partition","Restore XOSL "XOSL_VERSION", installed on a dedicated partition",1);
	TextUI.AddMenuItem(6,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(FatAvail ? 0 : 2);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUninstallMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Uninstall ");
	TextUI.AddMenuItem(0,"Uninstall off a DOS drive","Uninstall XOSL "XOSL_VERSION" off a DOS drive",FatAvail);
	TextUI.AddMenuItem(2,"Uninstall off a dedicated partition","Uninstall XOSL "XOSL_VERSION" off a dedicated partition",1);
	TextUI.AddMenuItem(6,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(FatAvail ? 0 : 2);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitInstFatMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	const char **ModeNameList;
	int ModeCount;
	const char **MouseTypeNames;
	int MouseTypeCount;

	Data.GetGraphicsModeNames(ModeNameList,ModeCount);
	Data.GetMouseTypeNames(MouseTypeNames,MouseTypeCount);
	if (!DosDriveList){
		CreateHDList();
		CreateDosDriveList();
	}
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Video mode","Initial video mode",1,ModeCount,ModeNameList,1);
	TextUI.AddMenuItem(1,"Mouse type","Initial mouse type",1,MouseTypeCount,MouseTypeNames,1);
	TextUI.AddMenuItem(2,"Install on drive","Drive to install XOSL on",1,DosDriveCount,(const char **)DosDriveList,1);
	TextUI.AddMenuItem(3,"Drv for Sector0 MBR","Drive to install Sector 0 XOSL "XOSL_VERSION" MBR on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(4,"Ranish Partition Manager","Install Ranish Partition Manager 2.44 beta together with XOSL "XOSL_VERSION"",1,2,YesNoList,1);
	TextUI.AddMenuItem(5,"Smart Boot Manager","Install Smart Boot Manager 3.7.1 for CD-ROM booting support.",1,2,YesNoList,1);

	TextUI.AddMenuItem(7,"Start installation","Install Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(9,"Return to install menu","Return to install menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitInstSepMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	const char **ModeNameList;
	int ModeCount;
	const char **MouseTypeNames;
	int MouseTypeCount;

	Data.GetGraphicsModeNames(ModeNameList,ModeCount);
	Data.GetMouseTypeNames(MouseTypeNames,MouseTypeCount);
	if (!PartNameList){
		CreateHDList();
		CreatePartList();
	}

	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Video mode","Initial video mode",1,ModeCount,ModeNameList,1);
	TextUI.AddMenuItem(1,"Mouse type","Initial mouse type",1,MouseTypeCount,MouseTypeNames,1);
	TextUI.AddMenuItem(2,"Ranish Partition Manager","Install Ranish Partition Manager 2.44 beta",1,2,YesNoList,1);
	TextUI.AddMenuItem(3,"Smart Boot Manager","Install Smart Boot Manager 3.7.1 for CD-ROM booting support.",1,2,YesNoList,1);
	TextUI.AddMenuItem(4,"Drv Type System (for XOSL)   Size ","Partition to install XOSL "XOSL_VERSION" on",1,PartNameCount,(const char **)PartNameList,0,8,MbrHDSector0List);
	TextUI.AddMenuItem(8,"Drv for Sector0 MBR","Drive to install Sector 0 XOSL "XOSL_VERSION" MBR on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(10,"Start installation","Install Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(11,"Return to install menu","Return to install menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUpgradeFatMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	const char **ModeNameList;
	int ModeCount;
	const char **MouseTypeNames;
	int MouseTypeCount;

	Data.GetGraphicsModeNames(ModeNameList,ModeCount);
	Data.GetMouseTypeNames(MouseTypeNames,MouseTypeCount);
	if (!DosDriveList){
		CreateHDList();
		CreateDosDriveList();
	}
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Video mode","Initial video mode",1,ModeCount,ModeNameList,1);
	TextUI.AddMenuItem(1,"Mouse type","Initial mouse type",1,MouseTypeCount,MouseTypeNames,1);
	TextUI.AddMenuItem(2,"Upgrade drive","Drive to upgrade XOSL on",1,DosDriveCount,(const char **)DosDriveList,1);
	TextUI.AddMenuItem(3,"Drv for Sector0 MBR","Drive to upgrade Sector 0 XOSL to "XOSL_VERSION" MBR",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(4,"Ranish Partition Manager","Install Ranish Partition Manager 2.44 beta together with XOSL "XOSL_VERSION"",1,2,YesNoList,1);
	TextUI.AddMenuItem(5,"Smart Boot Manager","Install Smart Boot Manager 3.7.1 for CD-ROM booting support.",1,2,YesNoList,1);

	TextUI.AddMenuItem(7,"Start upgrade","Upgrade Extended Operating System Loader to "XOSL_VERSION"",1);
	TextUI.AddMenuItem(9,"Return to upgrade menu","Return to upgrade menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUpgradeSepMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	const char **ModeNameList;
	int ModeCount;
	const char **MouseTypeNames;
	int MouseTypeCount;

	Data.GetGraphicsModeNames(ModeNameList,ModeCount);
	Data.GetMouseTypeNames(MouseTypeNames,MouseTypeCount);
	if (!PartNameList){
		CreateHDList();
		CreatePartList();
	}

	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Video mode","Initial video mode",1,ModeCount,ModeNameList,1);
	TextUI.AddMenuItem(1,"Mouse type","Initial mouse type",1,MouseTypeCount,MouseTypeNames,1);
	TextUI.AddMenuItem(2,"Ranish Partition Manager","Install Ranish Partition Manager 2.44 beta",1,2,YesNoList,1);
	TextUI.AddMenuItem(3,"Smart Boot Manager","Install Smart Boot Manager 3.7.1 for CD-ROM booting support.",1,2,YesNoList,1);
	TextUI.AddMenuItem(4,"Drv Type System (XOSL UG)    Size ","Partition to upgrade XOSL to "XOSL_VERSION,1,PartNameCount,(const char **)PartNameList,0,8,MbrHDSector0List);
	TextUI.AddMenuItem(8,"Drv for Sector0 MBR","Drive to upgrade Sector 0 XOSL to "XOSL_VERSION" MBR",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(10,"Start upgrade","Upgrade Extended Operating System Loader to "XOSL_VERSION"",1);
	TextUI.AddMenuItem(11,"Return to upgrade menu","Return to upgrade menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();
}


void CInstallMenus::InitErrorMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.SetMenuTitle(" Error \xcd\xcd\xcd\xcd");
	TextUI.AddMenuItem(0,"An error has occured!","",0);
	TextUI.AddMenuItem(2,"Exit install","Quit without installing Extended Operating System Loader "XOSL_VERSION"",1);
	TextUI.AddMenuItem(4,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(2);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitDoneMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Extended Operating System Loader","",0);
	TextUI.AddMenuItem(1,"has been successfully installed","",0);

	TextUI.AddMenuItem(3,"Reboot system","Quit install and reboot to run XOSL for the first time",1);
	TextUI.AddMenuItem(5,"Exit install","Quit install without rebooting",1);
	TextUI.AddMenuItem(7,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(3);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUnDoneMenu(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);
	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Extended Operating System Loader","",0);
	TextUI.AddMenuItem(1,"has been successfully uninstalled","",0);

	TextUI.AddMenuItem(3,"Reboot system","Quit install and reboot",1);
	TextUI.AddMenuItem(5,"Exit install","Quit install",1);
	TextUI.AddMenuItem(7,"Return to main menu","Return to main menu",1);
	TextUI.SetItemIndex(3);
	TextUI.RefreshMenu();
}

void CInstallMenus::InitUninstallFat(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	static const char *MbrIplList[2] = { "Original","Default" };

	if (!DosDriveList){
		CreateHDList();
		CreateDosDriveList();
	}
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);

	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Uninstall off drive","Drive XOSL is currently installed on",1,DosDriveCount,(const char **)DosDriveList,1);
	TextUI.AddMenuItem(1,"Drv for Sector0 MBR","Drive Sector 0 XOSL "XOSL_VERSION" MBR is currently installed on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(2,"Restore MBR","Restore the Master Boot Record with the original or default loader",1,2,MbrIplList,1);
	TextUI.AddMenuItem(5,"Start uninstall","Uninstall Extended Operating System Loader",1);
	TextUI.AddMenuItem(7,"Return to uninstall menu","Return to uninstall menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();

}

void CInstallMenus::InitUninstallSep(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	static const char *MbrIplList[2] = { "Original","Default" };

	if (!PartNameList){
		CreateHDList();
		CreatePartList();
	}

	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);

	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Restore MBR","Restore the Master Boot Record with the original or default loader",1,2,MbrIplList,1);
	TextUI.AddMenuItem(2,"Drv Type System              Size ","Partition XOSL is currently installed on",1,PartNameCount,(const char **)PartNameList,0,6,MbrHDSector0List);
	TextUI.AddMenuItem(6,"Drv for Sector0 MBR","Drive Sector 0 XOSL "XOSL_VERSION" MBR is currently installed on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(8,"Start uninstall","Uninstall Extended Operating System Loader",1);
	TextUI.AddMenuItem(10,"Return to uninstall menu","Return to uninstall menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();

}

void CInstallMenus::InitFixMbrFat(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	if (!DosDriveList){
		CreateHDList();
		CreateDosDriveList();
	}
	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);

	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"XOSL on drive","DOS drive XOSL is currently installed on",1,DosDriveCount,(const char **)DosDriveList,1);
	TextUI.AddMenuItem(1,"Drv for Sector0 MBR","Drive Sector 0 XOSL "XOSL_VERSION" MBR is currently installed on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(4,"Start restore","Restore XOSL "XOSL_VERSION"",1);
	TextUI.AddMenuItem(6,"Return to restore menu","Return to restore menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();

}

void CInstallMenus::InitFixMbrSep(CTextList::TListItemExecute MenuHandler, void *HandlerClass)
{
	if (!PartNameList){
		CreateHDList();
		CreatePartList();
	}

	TextUI.ConnectEventHandler(MenuHandler,HandlerClass);

	TextUI.ClearMenu();
	TextUI.AddMenuItem(0,"Drv Type System              Size ","Partition XOSL is currently installed on",1,PartNameCount,(const char **)PartNameList,0,4,MbrHDSector0List);
	TextUI.AddMenuItem(4,"Drv for Sector0 MBR","Drive Sector 0 XOSL "XOSL_VERSION" MBR is currently installed on",1,HDNameCount,(const char **)HDNameList,1);
	TextUI.AddMenuItem(6,"Start restore","Restore XOSL "XOSL_VERSION"",1);
	TextUI.AddMenuItem(8,"Return to upgrade menu","Return to upgrade menu",1);
	TextUI.SetItemIndex(0);
	TextUI.RefreshMenu();

}


void CInstallMenus::CreateDosDriveList()
{
	int Index;
	int *Buffer;

	DosDriveCount = Data.GetLastDrive() - ('C' - 'A') - HDOffset;
	if (DosDriveCount > 0) {
		DosDriveList = new char *[DosDriveCount];
		Buffer = new int[DosDriveCount];
		for (Index = 0; Index < DosDriveCount; ++Index) {
			Buffer[Index] = Index + 'C';
			DosDriveList[Index] = (char *)&Buffer[Index];
		}
	} else {
		DosDriveList = (char **) 0;
		FatAvail = 0;
	}
}

void CInstallMenus::CreatePartList()
// TODO: Implement the sprintf() function
{
	char *PartStr;
	const TPartition *Partition;
	int Index, Count;

	Count = PartList.GetCount();
	PartNameList = new char *[Count];
	PartResolveList = new int[Count];
	MbrHDSector0List = new unsigned char [Count];
	for (PartNameCount = Index = 0; Index < Count; ++Index) {
		Partition = PartList.GetPartition(Index);
		switch (Partition->Type) {
			case PART_PRIMARY:
			case PART_LOGICAL:
				switch (Partition->FSType) {
					case 0x05: // Extended
					case 0x0f: // Extended LBA
					case 0x85: // Linux Extended
						PartResolveList[Index] = -1;
						break;
					default:
						PartResolveList[Index] = PartNameCount;

						PartStr = new char [48];
						PartStr = UpdatePartNameList(PartStr, Partition);
						PartNameList[PartNameCount] = PartStr;

						if( Partition->FSType == 0x78 && Partition->MbrHDSector0 > 0x80) { // XOSL FS
							MbrHDSector0List[PartNameCount] = GetHDIndex(Partition->MbrHDSector0);
						}else{
							MbrHDSector0List[PartNameCount] = 0; // HD0
						}

						PartNameCount++;
						break;
				}
				break;

			case PART_GPT:
				if (Partition->FSType == 0L){
					// Empty partition entry
					PartResolveList[Index] = -1;
				}
				else{
					PartResolveList[Index] = PartNameCount;
					PartStr = new char [48];
					PartStr = UpdatePartNameList(PartStr, Partition);
					PartNameList[PartNameCount] = PartStr;
					if ( Partition->FSType == 0x7800 && Partition->MbrHDSector0 > 0x80) { // XOSL FS
						MbrHDSector0List[PartNameCount] = GetHDIndex(Partition->MbrHDSector0);
					}else{
						MbrHDSector0List[PartNameCount] = 0; // HD0
					}
					PartNameCount++;
				}
				break;
			default:
				break;
		}
	}
}
char * CInstallMenus::UpdatePartNameList(char * PartStr, const TPartition *Partition)
{
	unsigned long long PartitionSize;
	char SPartitionSize[16];

	MemSet(PartStr,' ',32);
	MemCopy(PartStr,"HD",2);
	PartStr[2] = '0' + (Partition->Drive & ~0x80);
	MemCopy(&PartStr[4],Partition->Type == PART_PRIMARY ? "pri" : Partition->Type == PART_GPT ? "gpt" : "log",3);
	MemCopy(&PartStr[9],Partition->FSName,strlen(Partition->FSName));

	// Shorten partition size display by using M of G suffix
	PartitionSize = (Partition->SectorCount >> 11); // Get size in Mb
	if (PartitionSize < 1024L){
		strcat(ultoa((unsigned long)PartitionSize,&SPartitionSize[0],10),"M");
	}else{ 
		strcat(ultoa((unsigned long)(PartitionSize >> 10),&SPartitionSize[0],10),"G");
	}
	//MemCopy(&PartStr[29], SPartitionSize, strlen(SPartitionSize));
	strcpy(&PartStr[29], SPartitionSize);
	return PartStr;
}
void  CInstallMenus::UpdatePartNameItem(int PartNameIndex, int PartIndex, unsigned char MbrHDSector0)
{
	const TPartition *Partition;

	Partition = PartList.GetPartition(PartIndex);

	PartNameList[PartNameIndex] = UpdatePartNameList(PartNameList[PartNameIndex], Partition);
	MbrHDSector0List[PartNameIndex] = GetHDIndex(MbrHDSector0);
}

void CInstallMenus::CreateHDList()
// TODO: Implement the sprintf() function
{
	char *HDStr;
	const TPartition *Partition;
	int Index, Count;
	int HDIndex = -1;
	int HD;

	
	HDNameCount = 0;

	Count = PartList.GetCount();
	

	for (Index = 0; Index < Count; ++Index) {

		Partition = PartList.GetPartition(Index);
		HD = Partition->Drive;
		if(HD < 0x80){
			// Floppy
			continue;
		}
		HD = (HD & ~0x80);
		if( HD != HDIndex ) {
			HDIndex = HD;
			HDNameCount++;
		}
	}
	HDNameList = new char *[HDNameCount+1];
	HDResolveList = new unsigned char[HDNameCount+1];

	for(Index=0; Index < HDNameCount ; ++Index){
			HDStr = new char [10];
			MemSet(HDStr,0,10);
			MemCopy(HDStr,"HD",2);
			HDStr[2] = '0' + Index;
			HDNameList[Index] = HDStr;
            HDResolveList[Index] = Index + 0x80;
	}
	HDStr = new char [10];
 	MemSet(HDStr,0,10);
 	MemCopy(HDStr,"NONE",4);
 	HDNameList[HDNameCount] = HDStr;
 	HDResolveList[HDNameCount] = 0xff;
	HDNameCount++;
}
int CInstallMenus::ResolvePartIndex(int ListIndex)
{
	int Count, Index;

	Count = PartList.GetCount();
	for (Index = 0; Index < Count; ++Index)
		if (PartResolveList[Index] == ListIndex)
			return Index;
	return -1;
}
int CInstallMenus::ResolveHDIndex(int ListIndex)
{
	return HDResolveList[ListIndex];
}

int CInstallMenus::GetHDIndex(unsigned char MbrHDSector0)
{
	for(int index = 0; index < HDNameCount; index++){
		if (HDResolveList[index] == MbrHDSector0){
			return index;
		}
	}
	return 0;
}

void CInstallMenus::ResetPartNameList(void)
{
	int Count;

	Count = PartList.GetCount();

	if (!PartNameList) {
		for (int index = 0;index < Count ; ++index ){
			delete PartNameList[index];
		}
		delete PartNameList;
	}

	if (HDNameCount != 0){
		for (int index;index < HDNameCount ; ++index ){
			delete HDNameList[index];
		}
		delete HDNameList;
	}

	if (PartResolveList != 0)
		delete PartResolveList;

	if (MbrHDSector0List != 0)
		delete MbrHDSector0List;

	if (HDResolveList != 0)
		delete HDResolveList;
}
