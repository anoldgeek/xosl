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
 * - Symbol __emit__ not defined
 *   To get rid of Error! E029: symbol '__emit__' has not been declared, use
 *   #pragma aux instead
 * - Use proper casting to get rid of Error! E473: function argument(s) do not 
 *   match those in prototype.
 * - Add call to CKeyboard::Flush() in function CBypass::Execute to make sure
 *   keyboard buffer is empty before entering while loop
 * 
 */

#include <Bypass.h>
#include <disk.h>
#include <key.h>

#include <fat16.h>
#include <fat32.h>

#include <ctype.h>
#include <mem.h>

#include <mbrpass.h>
#include <encpwd.h>

//#define BootJump() (__emit__(0xea,0x00,0x7c,0x00,0x00))   //Error! E029
#pragma aux BootJump = 0xea 0x00 0x7c 0x00 0x00;            //EA JMP FAR seg:offs


void printf(const char *,...);


CBypass::CBypass()
{
	
}

CBypass::~CBypass()
{

}

void CBypass::Execute(const char *ErrorMsg)
{
	int Index;
	char VolumeLabel[16];
	const char *FSName;
	unsigned short Key;

	if (ErrorMsg) {
		printf("CRITICAL ERROR: %s\n\n",ErrorMsg);
	}
	printf("-- XOSL Bypass --\n\n");
	ReadMBR();
	CheckPassword();
	for (Index = 0; Index < 4; ++Index) {
		if (MBR.mbrEntries[Index].SectorCount) {
			FSName = CPartList::GetFSName(MBR.mbrEntries[Index].FSType);
			if (ReadVolumeLabel(0x80,MBR.mbrEntries[Index].RelativeSector,VolumeLabel) != -1) {
				printf(" %d - %s (%s)\n",Index + 1,FSName,VolumeLabel);
			}
			else {
				printf(" %d - %s\n",Index + 1,FSName);
			}
		}
		else {
			printf(" %d - <empty>\n",Index + 1);
		}
	}
	printf("\nF - Boot from floppy\n\nSelect partition to boot --> ");
	CKeyboard::Flush();   //ML Flush keyboard buffer before entering while loop
	while (((Key = tolower(CKeyboard::WaitKeyStroke() & 0x00ff)) < '1' || Key > '4') && Key != 'f');
	printf("%c\n\n",Key);

#ifndef DOS_DEBUG_ML
	RawBoot(Key);
#else
	_asm mov ah,0x4c
	_asm int 0x21
#endif

}

void CBypass::ReadMBR()
{
	CDisk Disk;

	Disk.Map(0x80,0);
	Disk.Read(0,&MBR,1);
}


int CBypass::ReadVolumeLabel(int Drive, unsigned long Sector, char *VolumeLabel)
{
	char BootRecord[512];
	TBootFAT16 *BootFAT16 = (TBootFAT16 *)BootRecord;
	TBootFAT32 *BootFAT32 = (TBootFAT32 *)BootRecord;
	CDisk Disk;

	// check return values -> in case the partition table is corrupted.
	if (Disk.Map(Drive,Sector) == -1)
		return -1;
	if (Disk.Read(0,BootRecord,1) == -1)
		return -1;

	if (memcmp(BootFAT16->FSID,"FAT1",4) == 0) {
		CPartList::CreateVolumeLabel((const char*)BootFAT16->Label,VolumeLabel);
		return 0;
	}
	else {
		if (memcmp(BootFAT32->FSID,"FAT3",4) == 0) {
			CPartList::CreateVolumeLabel((const char*)BootFAT32->Label,VolumeLabel);
			return 0;
		}
	}
	return -1;
}


void CBypass::CheckPassword()
{
	CMBRPassword *MBRPassword;
	char PasswordStr[64];
	int PasswordIndex;
	unsigned short Key;

	MBRPassword = (CMBRPassword *)&MBR;   
	if (!MBRPassword->Password) {
		return;      
	}

	for (;;) {
		printf("Enter XOSL password: ");

		for (PasswordIndex = 0; (Key = CKeyboard::WaitKeyStroke()) != KEY_ENTER && PasswordIndex < 63; ++PasswordIndex) {
			printf("*");
			PasswordStr[PasswordIndex] = (char)Key;
		}
		PasswordStr[PasswordIndex] = '\0';
		if (EncodePassword(PasswordStr) == MBRPassword->Password) {
			printf("\n\n");
			return;
		}
		printf("\nInvalid password!\n\n");
	}
}


void CBypass::RawBoot(int BootIndex)
// f  : boot from floppy
// 1-4: boot partition
{
	int Index;
	CDisk Disk;

	if (BootIndex != 'f') {
		BootIndex -= '1';
		for (Index = 0; Index < 4; ++Index)
			MBR.mbrEntries[Index].Activated = 0;
		MBR.mbrEntries[BootIndex].Activated = 0x80;
		Disk.Map(0x80,0);
		Disk.Write(0,&MBR,1);
		Disk.Read(MBR.mbrEntries[BootIndex].RelativeSector,IPL_ADDR,1);
	}
	else {
		Disk.Map(0x00,0);
		while (Disk.Read(0,IPL_ADDR,1) == -1) {
			printf("Unable to read from floppy drive\n");
			printf("Press any key to retry...\n");
			CKeyboard::WaitKeyStroke();
		}

	}
	BootJump();
}

