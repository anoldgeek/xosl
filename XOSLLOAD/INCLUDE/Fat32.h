/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __fat32__
#define __fat32__

#include <fs.h>
#include <bootrec.h>

typedef struct {
	unsigned char FileName[8];
	unsigned char Extension[3];
	unsigned char Attribute;
	unsigned short NT;				// This is not right
	unsigned short CreateTime;
	unsigned short CreateDate;
	unsigned short Accessed;
	unsigned short StartClusterH;
	unsigned short Time;
	unsigned short Date;
	unsigned short StartClusterL;
	unsigned long FileSize;
} TFAT32DirEntry;



class CFAT32: public CFileSystem {
	public:
		CFAT32();
		~CFAT32();
		int Mount(int Drive, unsigned long long StartSector);
		unsigned short ReadFile(const char *FileName, void *Buffer);
	private:
		int Locate(const char *FileName, TFAT32DirEntry &Entry);
		void ReadFAT(long Cluster);

		void GetNextCluster(long &Cluster);
		void ReadCluster(long Cluster, void *Buffer);


		TBootFAT32 BootSector;

		long *FAT;
		long FirstCluster;
		long LastCluster;

		unsigned short ClusterSize;
		unsigned long FATStart;
		unsigned long DataStart;
};

#endif