/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

/*
 * Creates raw FAT16 file system data: 
 *
 * Specs:
 *	8k clusters
 *
 *
 * Boot record: 1 sector
 * FAT        : 1 sector (max. 256 clusters -> 2Mb)
 * Directory  : 2 sectors (max. 32 files)
 * Clusters   : n sectors (as many as needed, 4096 sectors)
 */


#ifndef FsCreatorH
#define FSCreatorH

#include <dosio.h>
#include <Files.h>
#include <IPL.h>
#include <textui.h>
#include <dosio.h>
#include <main.h>


#define XOSLIMG_FILE "XOSLIMG.FS"
#define PARTBACKUP_FILE "PARTIMG.BIN"
#define CLUSTER_SIZE 8192

class CFsCreator {
public:
	CFsCreator(CTextUI &TextUIToUse, CXoslFiles &XoslFilesToUse, CDosFile &DosFileToUse, TXoslWorkConfig *XoslWorkConfig);
	~CFsCreator();

	int InstallFs(unsigned short Drive, unsigned long long Sector, unsigned char MbrHDSector0, unsigned short FSType);

	unsigned short RestorePartition(unsigned short Drive,unsigned long long StartSector);

private:
	class CBootRecord {
	public:
		unsigned char Jump[3];			// jmp short + nop
		unsigned char OEM_ID[8];		// XOSLINST
		unsigned short SectorSize;		// 512
		unsigned char ClusterSize;		// 16 (8192 byte)
		unsigned short ReservedSectors;	// 1
		unsigned char FATCopies;		// 1
		unsigned short RootEntries;		// 32
		unsigned short SectorCount;		// ?
		unsigned char MediaDescriptor;	// 0xF8 (?) 
		unsigned short FATSize;			// 1
		unsigned short TrackSize;		// ? (sectors per head)
		unsigned short HeadCount;		// ?
		unsigned long HiddenSectors;	// ? (partition offset)
		unsigned long BigSectorCount;	// 0 (total sectors < 65536)
		unsigned short Drive;			// ?
		unsigned char Signature;		// 0x29 (?)
		unsigned long SerialNo;			// 0x4c534f58 (don't really care)
		unsigned char Label[11];		// XOSL110
		unsigned char FSID[8];			// FAT16
		unsigned long long HiddenSectors64;
		unsigned char Loader[440];		// IPL
		unsigned short MagicNumber;		// 0x534f (used by XOSLLOAD)
	};

	class CDirectoryEntry {
	public:
		unsigned char FileName[8];
		unsigned char Extension[3];
		unsigned char Attribute;
		unsigned char Reserved[10];
		unsigned short Time;
		unsigned short Date;
		unsigned short StartCluster;
		unsigned long FileSize;
	};

	class CPartBackupDetails{
	public:
		unsigned short Drive;
		unsigned long long StartSector;
		unsigned short FSType;
		char Reserved[500];
	} ;


	CTextUI &TextUI;
	CXoslFiles &XoslFiles;
	CDosFile &DosFile;

	CBootRecord BootRecord;
	unsigned short Fat[256];
	CDirectoryEntry RootDir[32];

	int FatIndex;
	int RootDirIndex;

	int LoadIplS(int Drive);
	int PackFile(int hClusterFile,const char *FileName, bool PartFile = false);
	int PackFiles(unsigned char MbrHDSector0);
	int InitBootRecord(unsigned short Drive, unsigned long long Sector, unsigned char MbrHDSector0);

	int BackupPartition(int Drive, unsigned long long Sector, unsigned short FSType);
	int InstallXoslImg(int Drive, unsigned long long Sector);


	void CFsCreator::AddRootDirEntry(const char *FileName, long FileSize,unsigned short FatDate,unsigned short FatTime);
	void AddFatEntries(long FileSize);

	TXoslWorkConfig *XoslWorkConfig;

	const char* AddFolderPath(const char *file);
};

#endif
