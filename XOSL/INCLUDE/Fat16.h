/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __fat16__
#define __fat16__

#include <fs.h>
#include <bootrec.h>

typedef struct {
	unsigned char FileName[8];
	unsigned char Extension[3];
	unsigned char Attribute;
	unsigned char Reserved[10];
	unsigned short Time;
	unsigned short Date;
	unsigned short StartCluster;
	unsigned long FileSize;
} TFAT16DirEntry;


class CFAT16: public CFileSystem {
	public:
		CFAT16();
		~CFAT16();
		int Mount(int Drive, unsigned long long StartSector);
		unsigned short ReadFile(const char *FileName, void *Buffer);
		int WriteFile(const char *FileName, void *Buffer);
	private:
		int Locate(const char *FileName, TFAT16DirEntry &Entry);
		void ReadFAT(unsigned short Cluster);
		void ReadDirectory(unsigned short Index, TFAT16DirEntry *Root);
		void WriteDirectory(unsigned short Index, TFAT16DirEntry *Root);

		void GetNextCluster(unsigned short &Cluster);
		void ReadCluster(unsigned short Cluster, void *Buffer);
		void WriteCluster(unsigned short Cluster, void *Buffer);
		void GetCurFatDateTime(unsigned short *pfatdate,unsigned short *pfattime);
		void UpdateFileDateTime(const char *FileName);


		TBootFAT16 BootSector;

		unsigned short *FAT;
		unsigned short FirstCluster;
		unsigned short LastCluster;

		unsigned short ClusterSize;
		unsigned long FATStart;
		unsigned long DirStart;
		unsigned long DataStart;
};

#endif