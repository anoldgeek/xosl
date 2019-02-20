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
 * - To get rid of Warning! W389:  integral value may be truncated during 
 *   assignment or initialization use proper casting in:
 *   DosFile.Write(hClusterFile,CDosFile::TransferBuffer,FileSize)
 *   int ClusterCount = FileSize / CLUSTER_SIZE;
 *   TransferCount = (ImageSize >> 11) + 1;
 *   TransferCount = (ImageSize - 6) >> 11;
 * - Comment out local variable unsigned short Checksum    
 *   to get rid of Warning! W014: 
 *   no reference to symbol 'Checksum'
 * - Comment out local variable int Index
 *   to get rid of Warning! W014: 
 *   no reference to symbol 'Index'
 *
 */


#include <FsCreate.h>
#include <string.h>
#include <memory_x.h>

#include <disk.h>
#include <transfer.h>
#include <xoslver.h>
#include <dosio.h>

//char DiskFullMsg_ss[] = "failed\nDisk full %s %s.\n";
extern char DiskFullMsg_ss[];

CFsCreator::CFsCreator(CTextUI &TextUIToUse, CXoslFiles &XoslFilesToUse, CDosFile &DosFileToUse):
	TextUI(TextUIToUse),
	XoslFiles(XoslFilesToUse),
	DosFile(DosFileToUse)
{
}

CFsCreator::~CFsCreator()
{
}

int CFsCreator::InstallFs(unsigned short Drive, unsigned long long Sector, unsigned char MbrHDSector0)
{
	MemSet(Fat,0,sizeof(unsigned short[256]));
	MemSet(RootDir,0,sizeof(CDirectoryEntry[32]));

	if (LoadIplS(Drive) == -1)
		return -1;
	if (PackFiles() == -1)
		return -1;
	if (InitBootRecord(Drive,Sector,MbrHDSector0) == -1)
		return -1;

	if (BackupPartition(Drive,Sector) == -1)
		return -1;
	if (InstallXoslImg(Drive,Sector) == -1)
		return -1;
	
	return 0;
}

int CFsCreator::LoadIplS(int Drive)
{
	CDiskAccess DiskAccess;
	const char *IplFileName;
	int hFile;

	MemSet(&BootRecord,0,512);
	if (DiskAccess.LBAAccessAvail(Drive + HDOffset) != -1)
		IplFileName = XoslFiles.GetIplFileName(CXoslFiles::enumIplSLba);
	else
		IplFileName = XoslFiles.GetIplFileName(CXoslFiles::enumIplS);

	if ((hFile = DosFile.Open(IplFileName,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("Unable to open %s\n",IplFileName);
		return -1;
	}
	DosFile.Read(hFile,&BootRecord,512);
	DosFile.Close(hFile);
	return 0;
}


int CFsCreator::InitBootRecord(unsigned short Drive, unsigned long long Sector, unsigned char MbrHDSector0)
{
	CDiskAccess DiskAccess;
	int HeadCount, SectorCount;


	TextUI.OutputStr("Initializing boot record...");
	if (DiskAccess.GetDriveInfo(Drive + HDOffset,HeadCount,SectorCount) == -1) {
		TextUI.OutputStr("failed\nUnable to retreive drive info\n");
		return -1;
	}

/*	BootRecord.Jump[0] = 0xeb;
	BootRecord.Jump[1] = 0x3c;
	BootRecord.Jump[2] = 0x90;*/

	MemCopy(BootRecord.OEM_ID,"XOSLINST",8);
	BootRecord.SectorSize = 512;
	BootRecord.ClusterSize = 16;
	BootRecord.ReservedSectors = 1;
	BootRecord.FATCopies = 1;
	BootRecord.RootEntries = 32;
	BootRecord.SectorCount = (FatIndex - 2) * 16 + 4;
	BootRecord.MediaDescriptor = 0xf8;
	BootRecord.FATSize = 1;

	BootRecord.TrackSize = SectorCount;
	BootRecord.HeadCount = HeadCount;

/*  HiddenSectors64 added to end of the MBR BootRecord BIOS Parameter Block
	to facilitate booting from HDs greater than 2TB for LBA drives, 
	taking the first 8 bytes of the original BootRecord.Loader.
	Old HiddenSectors also set for use by CHS drives and display
	of XOSL FS as FAT16 in linux.
*/ 
	BootRecord.HiddenSectors = (unsigned long) Sector;
	BootRecord.HiddenSectors64 = Sector;

	BootRecord.BigSectorCount = 0;
/*
	if(MbrHDSector0 != 0xff){
		// User may have selected a diffent drive to install the BootRecord too.
		// Assume the user has booted from other media that is now the first
		// drive during the install process.
		BootRecord.Drive = Drive - (MbrHDSector0 & 0x7f);
	}
	else{
		BootRecord.Drive = Drive;
	}
*/
	// is corrected by XOSL App before launch.
	BootRecord.Drive = Drive;

	BootRecord.Signature = 0x29;
	BootRecord.SerialNo = 0x4c534f58;
	MemCopy(BootRecord.Label,XOSL_LABEL,11);
	MemCopy(BootRecord.FSID,"FAT16   ",8);
	BootRecord.MagicNumber = 0x534f;
	TextUI.OutputStr("done\n");
	return 0;
}
int CFsCreator::PackFile(int hClusterFile,const char *FileName)
{
	long FileSize;
	int AppendStat;
	unsigned short FatDate;
	unsigned short FatTime;

	FileSize = DosFile.FileSize(FileName);
	if (FileSize == -1) {
		TextUI.OutputStr("failed\nin DosFile.FileSize for %s.\n", FileName);
		DosFile.Close(hClusterFile);
		return -1;
	}
	if(DosFile.FileDateTime(FileName,&FatDate, &FatTime) == -1 ){
		TextUI.OutputStr("failed\nin DosFile.FileDateTime for %s.\n", FileName);
		DosFile.Close(hClusterFile);
		return -1;
	}
	AppendStat = DosFile.Append(hClusterFile,FileName);
	if (AppendStat == -1) {
		TextUI.OutputStr("failed\nin DosFile.Append for %s.\n", FileName);
		DosFile.Close(hClusterFile);
		return -1;
	}

	// AddRootDirEntry() needs current FatIndex, 
	// so can't swap next two function calls
	AddRootDirEntry(FileName,FileSize,FatDate,FatTime);
	AddFatEntries(FileSize);

	// fill last part of the cluster with random data
	FileSize = CLUSTER_SIZE - (FileSize % CLUSTER_SIZE);
	if (FileSize != CLUSTER_SIZE)
		if (DosFile.Write(hClusterFile,CDosFile::TransferBuffer,(unsigned short)FileSize) != FileSize) {
//		if (DosFile.LSeek(hClusterFile,FileSize, DosFile.seekCurrent) != FileSize) {
			TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
			DosFile.Close(hClusterFile);
			return -1;
		}
	return 0;
}


int CFsCreator::PackFiles()
{
	int hClusterFile;
	int Index, Count;
	const char *FileName;
//	long FileSize;
//	int AppendStat;
	int NextImgFile;
	int ImgFileCount;
	char SrcFile2[13];

	TextUI.OutputStr("Packing XOSL files...\n");
	FatIndex = 2; // first two are reserved!
	RootDirIndex = 0;

	TextUI.OutputStr("Creating %s... ",XOSLIMG_FILE);
	if ((hClusterFile = DosFile.Create(XOSLIMG_FILE)) == -1) {
		TextUI.OutputStr("failed\n");
		return - 1;
	}
	TextUI.OutputStr("done\n");


	// Start with "XOSLLOAD.XCF"
	FileName = XoslFiles.GetXoslLoadName();
	TextUI.OutputStr("Packing %s... ",FileName);
	if(CFsCreator::PackFile(hClusterFile,FileName) == -1){
		return -1;
	}
	TextUI.OutputStr("done\n");
/* */
	// Now the "XOSLIMGx.XXF files
	strcpy(SrcFile2,XoslFiles.GetXoslImgXName());
	// Get the first one it conatains the image count
	FileName = SrcFile2;
	TextUI.OutputStr("Packing %s... ",FileName);
	if(CFsCreator::PackFile(hClusterFile,FileName) == -1){
		return -1;
	}
	TextUI.OutputStr("done\n");

	// Now pack the remaining img files
	ImgFileCount=(*(short *)CDosFile::TransferBuffer) + 1;  // XOSLIMG0 is still in transfer buffer. zero based img count!
	for(NextImgFile=1;NextImgFile < ImgFileCount;++NextImgFile){
		SrcFile2[7] = NextImgFile +'0';
		FileName = SrcFile2;
		TextUI.OutputStr("Packing %s... ",FileName);
		if(CFsCreator::PackFile(hClusterFile,FileName) == -1){
			return -1;
		}
		TextUI.OutputStr("done\n");
	}

	// Now pack the rest
	Count = XoslFiles.GetCount();
	for (Index = 0; Index < Count; ++Index) {
		FileName = XoslFiles.GetFileName(Index);
		TextUI.OutputStr("Packing %s... ",FileName);
		if(CFsCreator::PackFile(hClusterFile,FileName) == -1){
			return -1;
		}
		TextUI.OutputStr("done\n");
	}
	DosFile.SetFileDateTime(hClusterFile);
	DosFile.Close(hClusterFile);
	return 0;
}


void CFsCreator::AddRootDirEntry(const char *FileName, long FileSize,unsigned short FatDate,unsigned short FatTime)
{
	CDirectoryEntry &Entry = RootDir[RootDirIndex++];
	char Name[9];
	char Ext[4];
	int Len;

	DosFile.GetNameExt(FileName,Name,Ext);

	Len = strlen(Name);
	MemSet(&Name[Len],' ',8 - Len);

	Len = strlen(Ext);
	MemSet(&Ext[Len],' ',3 - Len);

	MemCopy(Entry.FileName,Name,8);
	MemCopy(Entry.Extension,Ext,3);
	Entry.StartCluster = FatIndex;
	Entry.FileSize = FileSize;

	Entry.Date = FatDate;
	Entry.Time = FatTime;
}

void CFsCreator::AddFatEntries(long FileSize)
{
	int ClusterCount = (int)(FileSize / CLUSTER_SIZE);
	int Index;

	if ((FileSize % CLUSTER_SIZE) != 0)
		++ClusterCount;

	for (Index = 0; Index < ClusterCount - 1; ++Index, ++FatIndex)
		Fat[FatIndex] = FatIndex + 1;
	Fat[FatIndex++] = 0xffff;
}

int CFsCreator::BackupPartition(int Drive, unsigned long long Sector)
{
	unsigned long ImageSize;
	int TransferCount;
	int Index;
	CDisk Disk;
	int hFile;

	TextUI.OutputStr("Creating backup...");
	if ((ImageSize = DosFile.FileSize(XOSLIMG_FILE)) == -1) {
		TextUI.OutputStr("failed\nUnable to determine image size\n");
		return -1;
	}

	TransferCount = (int)((ImageSize >> 11) + 1);

	if (Disk.Map(Drive,Sector) == -1) {
		TextUI.OutputStr("failed\nUnable to map partition\n");
		return -1;
	}

	if ((hFile = DosFile.Create(PARTBACKUP_FILE)) == -1) {
		TextUI.OutputStr("failed\nUnable to create "PARTBACKUP_FILE"\n");
		return -1;
	}

	if (DosFile.Write(hFile,&Drive,sizeof (unsigned short)) != sizeof (unsigned short)) {
		TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
		DosFile.Close(hFile);
		return -1;
	}

	if (DosFile.Write(hFile,&Sector,sizeof (unsigned long)) != sizeof (unsigned long)) {
		TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
		DosFile.Close(hFile);
		return -1;
	}

	for (Index = 0; Index < TransferCount; ++Index) {
		if (Disk.Read(Index,CDosFile::TransferBuffer,4) == -1) {
			TextUI.OutputStr("failed\nUnable to read partition data\n");
			DosFile.Close(hFile);
			return -1;
		}
		if (DosFile.Write(hFile,CDosFile::TransferBuffer,2048) != 2048) {
			TextUI.OutputStr(DiskFullMsg_ss, __FILE__, __LINE__);
			DosFile.Close(hFile);
			return -1;
		}
	}
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
	return 0;
}


void CFsCreator::RestorePartition(unsigned short Drive, unsigned long long StartSector)
{
	long ImageSize;
	int TransferCount;
	int Index;
	CDisk Disk;
	int hFile;
	unsigned short BackupDrive;
	unsigned long long BackupStartSector;

	TextUI.OutputStr("Restoring partition data...");
	if (DosFile.SetAttrib(PARTBACKUP_FILE,0) == -1) {
		TextUI.OutputStr("ignored\nBackup not found\n");
		return;
	}

	if ((ImageSize = DosFile.FileSize(PARTBACKUP_FILE)) == -1) {
		TextUI.OutputStr("ignored\nUnable to determine backup size\n");
		return;
	}


	if (Disk.Map(Drive,StartSector) == -1) {
		TextUI.OutputStr("ignored\nUnable to map partition\n");
		return;
	}

	if ((hFile = DosFile.Open(PARTBACKUP_FILE,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("ignored\nUnable to open "PARTBACKUP_FILE"\n");
		return;
	}

	Disk.Lock();
	TransferCount = (int)((ImageSize - 6) >> 11); // always a multiple of 2048!

	DosFile.Read(hFile,&BackupDrive,sizeof (unsigned short));
	DosFile.Read(hFile,&BackupStartSector,sizeof (unsigned long long));
	if (BackupDrive != Drive || BackupStartSector != StartSector) {
		TextUI.OutputStr("ignored\nInvalid backup image\n");
		return;
	}
	
	for (Index = 0; Index < TransferCount; ++Index) {
		DosFile.Read(hFile,CDosFile::TransferBuffer,2048);
		Disk.Write(Index,CDosFile::TransferBuffer,4);
	}
	Disk.Unlock();
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
}



int CFsCreator::InstallXoslImg(int Drive, unsigned long long Sector)
{
	int hFile;
	CDisk Disk;
	int WriteIndex;


	TextUI.OutputStr("Writing XOSL image...");

	if (Disk.Map(Drive,Sector) == -1) {
		TextUI.OutputStr("failed\nUable to map partition\n");
		return -1;
	}
	
	if (Disk.Write(0,&BootRecord,1) == -1) {
		TextUI.OutputStr("failed\nUnable to write Boot Record\n");
		return -1;
	}
	if (Disk.Write(1,Fat,1) == -1) {
		TextUI.OutputStr("failed\nUnable to write Fat\n");
		return -1;
	}
	if (Disk.Write(2,RootDir,2) == -1) {
		TextUI.OutputStr("failed\nUnable to write Root Dir\n");
		return -1;
	}

	if ((hFile = DosFile.Open(XOSLIMG_FILE,CDosFile::accessReadOnly)) == -1) {
		TextUI.OutputStr("failed\nUnable to open "XOSLIMG_FILE"\n");
		return -1;
	}

	for (WriteIndex = 4; DosFile.Read(hFile,CDosFile::TransferBuffer,2048); WriteIndex += 4)
		if (Disk.Write(WriteIndex,CDosFile::TransferBuffer,4) == -1) {
			TextUI.OutputStr("failed\nUnable to write image data\n");
			DosFile.Close(hFile);
			return -1;
		}
	DosFile.Close(hFile);
	TextUI.OutputStr("done\n");
	return 0;
}

