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
 * - Comment out local variable unsigned short Checksum    
 *   to get rid of Warning! W014: 
 *   no reference to symbol 'Checksum'
 * - Comment out local variable int Index
 *   to get rid of Warning! W014: 
 *   no reference to symbol 'Index'
 *
 */


#include <FatInst.h>
#include <disk.h>
#include <bootrec.h>
#include <string.h>
#include <install.h>
#include <memory_x.h>
#include <main.h>

CFatInstall::CFatInstall(CTextUI &TextUIToUse, CXoslFiles &XoslFilesToUse, CDosFile &DosFileToUse, TXoslWorkConfig *XoslWorkConfigToUse):
	TextUI(TextUIToUse),
	XoslFiles(XoslFilesToUse),
	DosFile(DosFileToUse)
{
	XoslWorkConfig = XoslWorkConfigToUse;
}

CFatInstall::~CFatInstall()
{

}

int CFatInstall::CreateIpl(const CDosDriveList::CDosDrive &DosDrive, TIPL &Ipl)
{
	CDiskAccess DiskAccess;
	int UseLba;

	TextUI.OutputStr("Initializing IPL...");

	UseLba = DiskAccess.LBAAccessAvail(DosDrive.Drive) == 0;
	if (DosDrive.FATType == FATTYPE_FAT16)
		return CreateIplFat16(DosDrive,UseLba,Ipl);
	return CreateIplFat32(DosDrive,UseLba,Ipl);
}

int CFatInstall::CreateIplFat16(const CDosDriveList::CDosDrive &DosDrive, int UseLba, TIPL &Ipl)
{
	TIPLFAT16Conv &Fat16Ipl = Ipl.IPLFAT16Conv;
	TBootFAT16 Fat16;
	const char *IplFile;
	int fh;
	CDisk Disk;

	Disk.Map(DosDrive.Drive,DosDrive.StartSector);
	Disk.Read(0,&Fat16,1);


	if (!UseLba){
		IplFile = XoslFiles.GetIplFileName(CXoslFiles::enumIpl16Conv);
		if (DosDrive.StartSector >> 32 != 0)
			// /2TB Can't use CHS on drives > 2TB
			return -1;
	}
	else
		IplFile = XoslFiles.GetIplFileName(CXoslFiles::enumIpl16Lba);

	if ((fh = DosFile.Open(IplFile,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("failed\nUnable to open %s",IplFile);
		return -1;
	}
	DosFile.Read(fh,&Fat16Ipl,sizeof (TIPLFAT16Conv) - 10);
	DosFile.Close(fh);

	if (!UseLba) {
		Fat16Ipl.IPLData.DiskSectors = Fat16.TrackSize;
		Fat16Ipl.IPLData.DiskHeads = Fat16.HeadCount;
	}
	Fat16Ipl.IPLData.RootEntries = Fat16.RootEntries;
	Fat16Ipl.IPLData.ClusterSectSize = Fat16.ClusterSize;
	Fat16Ipl.IPLData.ClusterByteSize = (unsigned short)Fat16.ClusterSize << 9;
	Fat16Ipl.IPLData.FATStart = Fat16.ReservedSectors;
	Fat16Ipl.IPLData.DirStart = (unsigned long)Fat16.ReservedSectors + (unsigned long)Fat16.FATCopies * (unsigned long)Fat16.FATSize;
	Fat16Ipl.IPLData.DataStart = (unsigned long)Fat16Ipl.IPLData.DirStart + (unsigned long)Fat16.RootEntries / 16;

	Fat16Ipl.IPLData.FSType = 0x06;
	// This is correct when launched by the BIOS
	// and is adjusted when loaded by XOSL App before launch.
	Fat16Ipl.IPLData.DriveNumber = DosDrive.Drive;
	Fat16Ipl.IPLData.ABSSectorStart = DosDrive.StartSector;
	TextUI.OutputStr("done\n");
	return 0;
}

int CFatInstall::CreateIplFat32(const CDosDriveList::CDosDrive &DosDrive, int UseLba, TIPL &Ipl)
{
	TIPLFAT32Conv &Fat32Ipl = Ipl.IPLFAT32Conv;
	TBootFAT32 Fat32;
	const char *IplFile;
	int fh;
	unsigned long FATSize;
	CDisk Disk;

	Disk.Map(DosDrive.Drive,DosDrive.StartSector);
	Disk.Read(0,&Fat32,1);

	if (!UseLba){
		IplFile = XoslFiles.GetIplFileName(CXoslFiles::enumIpl32Conv); 
		if (DosDrive.StartSector >> 32 != 0)
			// /2TB Can't use CHS on drives > 2TB
			return -1;
	}
	else
		IplFile = XoslFiles.GetIplFileName(CXoslFiles::enumIpl32Lba);

	if ((fh = DosFile.Open(IplFile,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("failed\nUnable to op %s",IplFile);
		return -1;
	}
	DosFile.Read(fh,&Fat32Ipl,sizeof (TIPLFAT32Conv) - 10);
	DosFile.Close(fh);

	if (!UseLba) {
		Fat32Ipl.IPLData.DiskSectors = Fat32.TrackSize;
		Fat32Ipl.IPLData.DiskHeads = Fat32.HeadCount;
	}
	Fat32Ipl.IPLData.ClusterSectSize = Fat32.ClusterSize;
	Fat32Ipl.IPLData.ClusterByteSize = (unsigned short)Fat32.ClusterSize << 9;
	Fat32Ipl.IPLData.FATStart = Fat32.ReservedSectors;
	FATSize = Fat32.BigFATSize;
	Fat32Ipl.IPLData.DataStart = (unsigned long)Fat32.ReservedSectors + (unsigned long)Fat32.FATCopies * FATSize;
	Fat32Ipl.IPLData.RootCluster = Fat32.RootCluster;

	Fat32Ipl.IPLData.FSType = 0x0b;
	// This is correct when launched by the BIOS
	// and is adjusted when loaded by XOSL App before launch.
	Fat32Ipl.IPLData.DriveNumber = DosDrive.Drive;
	Fat32Ipl.IPLData.ABSSectorStart = DosDrive.StartSector;
	TextUI.OutputStr("done\n");
	return 0;
}

int CFatInstall::InstallFile(const char *SrcFile, const char *DestFile)
{
	TextUI.OutputStr("Copying %s...",SrcFile);
	if (DosFile.SetAttrib(DestFile,0) != -1)
		if (DosFile.Delete(DestFile) == -1) {
			TextUI.OutputStr("failed\nUnable to delete existing %s\n",SrcFile);
			return -1;
		}
	if (DosFile.Copy(SrcFile,DestFile) == -1) {
		TextUI.OutputStr("failed\nUnabled to copy %s\n",SrcFile);
		return -1;
	}
	DosFile.SetAttrib(DestFile,CDosFile::attrHidden);
	TextUI.OutputStr("done\n");
	return 0;
}

int CFatInstall::InstallFiles(const CDosDriveList::CDosDrive &DosDrive, unsigned char MbrHDSector0)
{
	char DestFile[32];
	const char *SrcFile;
	int Index, Count;
	char SrcFile2[13];
	int NextImgFile;

	*DestFile = DosDrive.DriveChar + HDOffset;
	*(unsigned short *)&DestFile[1] = 0x5c3a; // ':\'

	// Install XOSLLOAD.XCF first
	SrcFile = XoslFiles.GetXoslLoadName();
	strcpy(&DestFile[3],SrcFile);
	if(CFatInstall::InstallFile(SrcFile,DestFile) == -1){
		return -1;
	}
	// Now the "XOSLIMGx.XXF files
	strcpy(&SrcFile2[0],XoslFiles.GetXoslImgXName());
	SrcFile = SrcFile2;
	NextImgFile=0;
	for(;;++NextImgFile){
		int fh;

		SrcFile2[7] = NextImgFile +'0';
		strcpy(&DestFile[3],SrcFile);
		if ((fh = DosFile.Open(SrcFile,CDosFile::accessReadOnly)) != -1){
			DosFile.Close(fh);
			if(CFatInstall::InstallFile(SrcFile,DestFile) == -1){
				return -1;
			}
		}else{
			// All img files in source folder copied OK
			break;
		}
	}
	// Now ensure any other img files are deleted
	for(;;++NextImgFile){
		SrcFile2[7] = NextImgFile +'0';
		strcpy(&DestFile[3],SrcFile);
		if (DosFile.SetAttrib(DestFile,0) != -1){
			if (DosFile.Delete(DestFile) == -1) {
				TextUI.OutputStr("failed\nUnable to delete existing %s\n",SrcFile);
				return -1;
			}
		}else{
			// Next file not found
			break;
		}
	}
	// Now the remaining files
	Count = XoslFiles.GetIssuedFileCount();
	for (Index = 0; Index < Count; ++Index) {
		strcpy(&DestFile[3],XoslFiles.GetIssuedFileName(Index));
		SrcFile = XoslFiles.GetIssuedFileName(Index);
		if(CFatInstall::InstallFile(SrcFile,DestFile) == -1){
			return -1;
		}
	}
	if ( MbrHDSector0 != 0xff ){
		strcpy(&DestFile[3],XoslFiles.GetOriginalMbrName());
		SrcFile = XoslFiles.GetOriginalMbrName();
		if(CFatInstall::InstallFile(AddFolderPath(SrcFile),DestFile) == -1){
			return -1;
		}
	}
	Count = XoslFiles.GetPartFileCount();
	for (Index = 0; Index < Count; ++Index) {
		strcpy(&DestFile[3],XoslFiles.GetPartFileName(Index));
		SrcFile = XoslFiles.GetPartFileName(Index);
		if(CFatInstall::InstallFile(AddFolderPath(SrcFile),DestFile) == -1){
			return -1;
		}
	}
	return 0;
}



int CFatInstall::InstallIpl(void *Ipl,unsigned char MbrHDSector0)
{
	CDisk Disk;
	unsigned char Mbr[512];

	TextUI.OutputStr("Installing IPL...");
	if (Disk.Map(MbrHDSector0,0) == -1) {
		TextUI.OutputStr("failed\nUnable to access MBR. ");
		return -1;
	}
	Disk.Lock();
	if (Disk.Read(0,Mbr,1) == -1) {
		Disk.Unlock();
		TextUI.OutputStr("failed\nUnable to read MBR. ");
		return -1;
	}
	MemCopy(Mbr,Ipl,436);
	if (Disk.Write(0,Mbr,1) == -1) {
		Disk.Unlock();
		TextUI.OutputStr("failed\nUnable to write to MBR. ");
		return -1;
	}

	Disk.Unlock();
	TextUI.OutputStr("done\n");
	return 0;

}


void CFatInstall::RemoveXoslFiles(char DosDriveChar)
{
	char FileStr[32];
	const char *XoslFileName;
	int Index, Count;

	FileStr[0] = DosDriveChar + HDOffset;
	FileStr[1] = ':';
	FileStr[2] = '\\';
	Count = XoslFiles.GetIssuedFileCount();
	for (Index = 0; Index < Count; ++Index) {
		XoslFileName = XoslFiles.GetIssuedFileName(Index);
		TextUI.OutputStr("Removing %s...",XoslFileName);
		strcpy(&FileStr[3],XoslFileName);
		if (DosFile.Delete(FileStr) == -1)
			TextUI.OutputStr("failed\n");
		else
			TextUI.OutputStr("done\n");
	}
	XoslFileName = XoslFiles.GetOriginalMbrName();
	TextUI.OutputStr("Removing %s...",XoslFileName);
	strcpy(&FileStr[3],XoslFileName);
	if (DosFile.Delete(FileStr) == -1)
		TextUI.OutputStr("not found\n");
	else
		TextUI.OutputStr("done\n");

	Count = XoslFiles.GetPartFileCount();
	for (Index = 0; Index < Count; ++Index) {
		XoslFileName = XoslFiles.GetPartFileName(Index);
		TextUI.OutputStr("Removing %s...",XoslFileName);
		strcpy(&FileStr[3],XoslFileName);
		if (DosFile.Delete(FileStr) == -1)
			TextUI.OutputStr("failed\n");
		else
			TextUI.OutputStr("done\n");
	}



	XoslFileName = "XOSLLOAD.XCF";
	strcpy(&FileStr[3],XoslFileName);
	TextUI.OutputStr("Removing %s...",XoslFileName);
	if (DosFile.Delete(FileStr) == -1)
		TextUI.OutputStr("failed\n");
	else
		TextUI.OutputStr("done\n");
	XoslFileName = "XOSLIMGx.XXF";
	strcpy(&FileStr[3],XoslFileName);
	TextUI.OutputStr("Removing %s files...",XoslFileName);
	for( Index = 0 ; ; Index++){
		FileStr[10] = '0' + Index;
		if (DosFile.Delete(FileStr) == -1){
			break;
		}
	}
	TextUI.OutputStr("done\n");
}

const char* CFatInstall::AddFolderPath(const char *file)
{
	int pathlen;
	char *buffer = CDosFile::PathBuffer;
	char *tmp;

	strcpy(buffer, XoslWorkConfig->WorkFolder);

	pathlen = strlen(buffer);
	if (pathlen > 0){
		tmp = &buffer[pathlen - 1];
		if (*tmp++ != '\\'){
			*tmp++ = '\\';
			*tmp = 0;
		}
	}
	strcat(buffer,file);

	return buffer;
}
