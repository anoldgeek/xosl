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
 */



#include <dosio.h>
#include <string.h>
#include <memory_x.h>

char CDosFile::TransferBuffer[32768];


CDosFile::CDosFile()
{
}

CDosFile::~CDosFile()
{
}

int CDosFile::Delete(const char *FileName)
{
	if (SetAttrib(FileName,0) == -1)
		return -1;
	return Unlink(FileName);
}

int CDosFile::Copy(const char *Src, const char *Dest)
{
	int hInFile;
	int hOutFile;
	unsigned short Size;

	if ((hInFile = Open(Src,accessReadOnly)) == -1)
		return -1;
	if ((hOutFile = Create(Dest)) == -1) {
		Close(hInFile);
		return -1;
	}

	while ((Size = Read(hInFile,TransferBuffer,32768)) != 0)
		Write(hOutFile,TransferBuffer,Size);

	_asm{
		// Copy the dat/time stamps
		mov al,0
		mov bx,hInFile
		mov ah,057h
		int 21h
		mov al,1
		mov bx,hOutFile
		mov ah,057h
		int 021h
	}
	Close(hInFile);
	Close(hOutFile);
	return 0;
}


int CDosFile::Append(int hOutFile, const char *FileName)
{
	int hInFile;
	unsigned short Size;

	if ((hInFile = Open(FileName,accessReadOnly)) == -1)
		return -1;

	while ((Size = Read(hInFile,TransferBuffer,32768)) != 0)
		Write(hOutFile,TransferBuffer,Size);
	Close(hInFile);
	return 0;
}

long CDosFile::FileSize(const char *FileName)
{
	int fh;
	long FileSize;

	if ((fh = Open(FileName,accessReadOnly)) == -1)
		return -1;
	FileSize = LSeek(fh,0,seekEnd);
	Close(fh);
	return FileSize;
}

int CDosFile::FileDateTime(const char *FileName, unsigned short *pFatDate, unsigned short *pFatTime)
{
	int fh;
	unsigned short FatDate;
	unsigned short FatTime;

	if ((fh = Open(FileName,accessReadOnly)) == -1)
		return -1;
	_asm{
		mov bx,fh
		mov al,0		// ;Get
		mov ah,057h		// File Date and time
		int 21h
		mov FatTime,cx
		mov FatDate,dx
	}
	Close(fh);
	*pFatDate = FatDate;
	*pFatTime = FatTime;
	return 0;
}


void CDosFile::GetNameExt(const char *FileName, char *Name, char *Ext)
{
	char *pExt;
	int NameLen, ExtLen;

	pExt = strchr(FileName,'.');
	if (!pExt) {
		NameLen = strlen(FileName);
		ExtLen = 0;
	}
	else {
		NameLen = (unsigned short)pExt - (unsigned short)FileName;
		ExtLen = strlen(FileName) - NameLen - 1;
	}

	MemCopy(Name,FileName,NameLen);
	MemCopy(Ext,&FileName[NameLen + 1],ExtLen);
	Name[NameLen] = '\0';
	Ext[ExtLen] = '\0';
}
void CDosFile::SetFileDateTime(int hFile)
{	
	unsigned short fatdate;
	unsigned short fattime;

	GetCurFatDateTime(&fatdate, &fattime);

	_asm{
		mov bx,hFile
		mov cx,fattime
		mov dx,fatdate
		mov al,1		// ;Set
		mov ah,057h		// File Date and time
		int 21h
	}
}
void CDosFile::GetCurFatDateTime(unsigned short *pfatdate,unsigned short *pfattime)
{
	unsigned short bcdhrmin;
	unsigned char bcdsec;

	unsigned short bcdcentyr;
	unsigned short bcdmonday;

	unsigned short fatdate;
	unsigned short fattime;

	int temp;

	_asm{
		mov	ah,2 //; get rtc time 
		int 1ah
		mov	bcdhrmin,cx
		mov	bcdsec,dh

		mov	ah,4 //; get rtc date
		int	1ah
		mov	bcdcentyr,cx
		mov	bcdmonday,dx
	}
	fattime = ((bcdsec & 0xf) + ((( bcdsec >> 4)  & 0xf ) * 10) >> 1); // convert bcd sec ( 2 sec resolotion)

	fattime = fattime + (( (bcdhrmin & 0xf) + (( bcdhrmin >> 4)  & 0xf ) * 10) << 5); // convert bcd min
	fattime = fattime + (( ((bcdhrmin >> 8 ) & 0xf) + (( bcdhrmin >> 12)  & 0xf ) * 10) << 11); // convert bcd hr


	fatdate = (bcdmonday & 0xf)  + (( bcdmonday >> 4) & 0xf) * 10; // convert bcd day
	fatdate = fatdate + (( ( (bcdmonday >> 8) & 0xf)  + (( bcdmonday >> 12) & 0xf) * 10) << 5);  // convert bcd month

	temp = (bcdcentyr & 0xf)  + (( bcdcentyr >> 4) & 0xf) * 10; // convert bcd year
	temp = temp +  ( ((bcdcentyr >> 8 ) & 0xf) * 100  + (( bcdcentyr >> 12 ) & 0xf) * 1000 ); // convert bcd centry
	temp = temp - 1980; // Convert to DOS epoch date
	fatdate = fatdate + (temp << 9);
	
	*pfatdate = fatdate;
	*pfattime = fattime;
}
