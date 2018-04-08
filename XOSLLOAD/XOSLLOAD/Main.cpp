/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#include <defs.h>
#include <disk.h>
#include <ipl.h>

#include <fat16.h>
#include <fat32.h>

#include <text.h>

#include <mem.h>
#include <exestruc.h>
#include <execute.h>
#include <string.h>

#include <Bypass.h>

/**/



#define Ipl ( (TFat16Ipl *)0x00007c00 )
#define BootRecord ( (TBootFAT16 *)0x00007c00 )


typedef struct {
	int Drive;
	int FSType;
	unsigned long StartSector;
} TMountPart;

// address where XOSL expects to find which partition it is located on.
#define XoslMountPart ( *(TMountPart *)0x00007c00 )

// location to load the XOSL image files
#define IMAGE_DESTADDR 0x20000000

#define IMAGE_NAME "XOSLIMG0XXF"

#define START_SEG (IMAGE_DESTADDR >> 16)


#define WaitKeypress() (__emit__(0x32,0xe4,0xcd,0x16))

#define BypassRequest() ( (peekb(0x0000,0x0417) & 0x02) == 0x02 )


/**/

static void CleanMemory();

static CFileSystem *MountFileSystem();
static void CreatePartition();
static void *LoadImage(CFileSystem *FileSystem);
static void HandleRelocate(const void *ImageData);
static void CriticalError(const char *Msg);

static char HeaderData[32768];

#define FREESEGSTART 0x5000
#define FREESEGEND 0x8000

void printf(const char *format, ...);
void AllocInit(unsigned short MemSegStart, unsigned short MemSegEnd);

_extern void CPPMain()
{

	CFileSystem *FileSystem;
	void *ImageData;
	TExeHeader *ExeHeader = (TExeHeader *)&HeaderData[2];
	unsigned short EndSeg;

	printf("AllocInit( %x, %x )\r\n",FREESEGSTART, FREESEGEND);
	AllocInit(FREESEGSTART, FREESEGEND);

	PutS("\r\nExtended Operating System Loader 1.1.9\r\n\n");
	if (BypassRequest())
		CriticalError(NULL);
//    PutS("0\r\n");
	CleanMemory();
	FileSystem = MountFileSystem();
//	PutS("1\r\n");
	ImageData = LoadImage(FileSystem);
//	PutS("2\r\n");
	HandleRelocate(ImageData);
	PutS("3\r\n");
	delete FileSystem;
	
	EndSeg=START_SEG + ExeHeader->PageCount + ExeHeader->MinMem;

	printf("START_SEG %x, EndSeg %x\r\n");
/* */
printf("Execute(START_SEG,ExeHeader->ReloSS,ExeHeader->ExeSP,ExeHeader->ReloCS,ExeHeader->ExeIP,EndSeg,0x8000)\r\n");
	printf("Execute(START_SEG %x,ExeHeader->ReloSS %x,ExeHeader->ExeSP %x,ExeHeader->ReloCS %x,ExeHeader->ExeIP %x,EndSeg %x,0x8000 %x)\r\n",
		START_SEG,ExeHeader->ReloSS,ExeHeader->ExeSP,ExeHeader->ReloCS,ExeHeader->ExeIP,EndSeg,0x8000);
/**/
	Execute(START_SEG,ExeHeader->ReloSS,ExeHeader->ExeSP,
			  ExeHeader->ReloCS,ExeHeader->ExeIP,EndSeg,0x8000);  // TODO Find last free Seg and replace 0x8000
}

void CleanMemory()
{
	/* just for sure */
//	memset((void *)0x20000000,0,32768);
//	memset((void *)0x20008000,0,32768);
//	memset((void *)0x30000000,0,32768);
//!	memset((void *)0x30008000,0,32768);
//	memset((void *)0x40000000,0,32768);
//	memset((void *)0x40008000,0,32768);
}

CFileSystem *MountFileSystem()
{
	CFileSystem *FileSystem;
	int ret;

	CreatePartition();
	switch (XoslMountPart.FSType) {
		case 0x06:
			FileSystem = new CFAT16;
			break;
		case 0x0b:
			FileSystem = new CFAT32;
			break;
		default:
			CriticalError("Unknown file system.");
			break;
	}
	//printf("XoslMountPart.Drive %x XoslMountPart.StartSector %lu\r\n",(int)XoslMountPart.Drive,(unsigned long)XoslMountPart.StartSector);
	ret=FileSystem->Mount(XoslMountPart.Drive,XoslMountPart.StartSector);
	//printf("Exited FileSystem->Mount %d\r\n", ret ); 
	return FileSystem;
}

void *LoadImage(CFileSystem *FileSystem)
{
	void *Dest;
	int ImgIndex;
	char *ImageName = IMAGE_NAME;
	char  *ImageLogErrorMsg = "Unable to load XOSL image ";
	char ErrorMsg[80];
	int Index;
	int ImageCount;

	strcpy(ErrorMsg,ImageLogErrorMsg); // Prepare error msg
	if (!FileSystem->ReadFile(ImageName,HeaderData)){
		strcat(ErrorMsg,ImageName);
		CriticalError(ErrorMsg);
	}
	++ImageName[7];
	ImageCount = *(short *)HeaderData;
	Dest = (void *)IMAGE_DESTADDR;
	for (Index = 0; Index < ImageCount; ++Index) {
		if (!FileSystem->ReadFile(ImageName,Dest)){
			strcat(ErrorMsg,ImageName);
			CriticalError(ErrorMsg);
		}
		++ImageName[7];
		(unsigned long)Dest += 0x08000000;
	}
	return (void *)IMAGE_DESTADDR;
}

void HandleRelocate(const void *ImageData)
{
	const TExeHeader *ExeHeader;
	unsigned long *RelocTable;
	int Index;
	unsigned short *Entry;
	char *pHeaderData;

	pHeaderData = &HeaderData[2];
	ExeHeader = (TExeHeader *)pHeaderData;
	RelocTable = (unsigned long *)&pHeaderData[ExeHeader->TableOff];
	for (Index = 0; Index < ExeHeader->ReloCount; ++Index, ++RelocTable) {
		Entry = (unsigned short *)((unsigned long)ImageData + *RelocTable);
		*Entry += START_SEG;
	}
}

void CriticalError(const char *Msg)
{
	CBypass Bypass;

	Bypass.Execute(Msg);

}

void CreatePartition()
{
	if (memcmp(BootRecord->OEM_ID,"XOSLINST",8) == 0) {
		XoslMountPart.Drive = BootRecord->Drive;
		XoslMountPart.FSType = 0x06;
		XoslMountPart.StartSector = BootRecord->StartSector;
	}
	else {
		XoslMountPart.Drive = Ipl->IplData.DriveNumber;
		XoslMountPart.FSType = Ipl->IplData.FSType;
		XoslMountPart.StartSector = Ipl->IplData.ABSSectorStart;
	}
}

char *strcat( char *to, const char *from )
{
	char *temp ;

	temp = to ;
	while( *to++ ) ;
	to-- ;
	while( *to++ = *from++ ) ;
	return temp ;
}


char *strcpy( char *to, const char *from )
{
	char *temp ;

	temp = to ;
	while( *to++ = *from++ ) ;
	return temp ;
}

