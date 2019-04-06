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
 * - Use __cdecl calling convention
 *
 */

#ifndef __io__
#define __io__

#include <defs.h>
#include <main.h>

class CDosFile {
public:
	enum TFileAccess {
		accessReadOnly = 0, accessWriteOnly = 1, accessReadWrite = 2
	};
	enum TFileAttr {
		attrReadOnly = 0x0001, attrHidden = 0x0002, attrSystem = 0x0004, attrArchive = 0x0020
	};
	enum TWhence {
		seekStart = 0, seekCurrent = 1, seekEnd = 2
	};

	CDosFile();
	~CDosFile();

	static int __cdecl Create(const char *FileName);
	int Create(const char *FileName, const char *Path);
	static int __cdecl Open(const char *FileName, TFileAccess Access);
	int Open(const char *FileName, TFileAccess Access, const char * Path);
	static void __cdecl Close(int Handle);
	static unsigned short __cdecl Read(int Handle, void *Buffer, unsigned short Length);
	static unsigned short __cdecl Write(int Handle, const void *Buffer, unsigned short Length);

	static int __cdecl SetAttrib(const char *FileName, int Attributes);
	int Delete(const char *FileName);
	int Delete(const char *FileName, const char * Path);

	int Copy(const char *Src, const char *Dest);
	int Append(int hOutFile, const char *FileName);
	int Append(int hOutFile, const char *FileName, const char * Path);
	long FileSize(const char *FileName);
	long FileSize(const char *FileName, const char * Path);
	void GetNameExt(const char *FileName, char *Name, char *Ext);

	static char TransferBuffer[32768];
	static char PathBuffer[MAX_PATH_LEN];
	void SetFileDateTime(int hFile);
	void CDosFile::GetCurFatDateTime(unsigned short *pfatdate,unsigned short *pfattime);
	static long __cdecl LSeek(int Handle, long Offset, TWhence Whence);
	int CDosFile::FileDateTime(const char *FileName, unsigned short *pFatDate, unsigned short *pFatTime);
	int CDosFile::FileDateTime(const char *FileName, unsigned short *pFatDate, unsigned short *pFatTime, const char *Path);
	int Copy(const char *Src, const char *Dest, const char *DestPath);
	int SetAttrib(const char *FileName, int Attributes, const char *Path);
	void SetXoslWorkConfig(TXoslWorkConfig *XoslWorkConfigToUse);
private:
	static int __cdecl Unlink(const char *FileName);
	char* AddFolderPath(const char *file, const char * Path);

};


#endif
