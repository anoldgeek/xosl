/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#include <xoslapp.h>
#include <mem.h>
#include <text.h>

//void AllocInit(unsigned long MemStart);
void AllocInit(unsigned short FreeSegStart, unsigned short FreeSegEnd);
void MemoryCheck(unsigned short StartSeg, unsigned short EndSeg);
void printf(const char *format, ...);

//int main(void)
int main(unsigned short FreeSegStart,unsigned short FreeSegEnd )
{
	int BootDrive;

	printf("AllocInit( %x, %x )\r\n",FreeSegStart, FreeSegEnd);
	AllocInit(FreeSegStart, FreeSegEnd);

	#ifdef DOS_DEBUG
//	#define MMU_START_ADDRESS 0x65000000L
//	AllocInit(MMU_START_ADDRESS);
	MemoryCheck(FreeSegStart,FreeSegEnd);   ///ML: Check if (MMU) memory is accessible.
//	#else
//	puts("20\r\n");
//	AllocInit(FreeStartAddress,0x000a0000);
	#endif
	
	puts("21\r\n");
	CApplication *Application;

	puts("22\r\n");
	Application = new CApplication;
	puts("23\r\n");
	BootDrive = Application->Execute();
	delete Application;
	return BootDrive;
}
#ifdef DOS_DEBUG
void MemoryCheck(unsigned short StartSeg, unsigned short EndSeg)
{

	//ML: Function to test MMU read/write access 
	long lPageAddr;
	unsigned char ucOrgMemData[16];
	unsigned char ucTestMemData[16];
	int iIndex;

	//void memcpy (void far *dest, void far *str, U16B size);
	//void memset(void far *dest, U8B value, U16B count);
	//int memcmp(const void far *s1, const void far *s2, U16B count); 

	for (iIndex=0;iIndex<16;iIndex++)
	{
		 ucTestMemData[iIndex]=0x80+iIndex;
	}

//	#define MMU_STARTADDRESS 0x6500 //Value should be in line with value set in starter.asm 

//	for (lPageAddr=MMU_STARTADDRESS;lPageAddr<0x9fff;lPageAddr++)
	for (lPageAddr=(StartSeg);lPageAddr<EndSeg;lPageAddr++)
	{
		memcpy(ucOrgMemData,(void *)(lPageAddr << 16),16);
		memcpy((void *)(lPageAddr << 16),ucTestMemData,16);
		if (memcmp((void *)(lPageAddr << 16),ucTestMemData,16)){
			//if memcmp return value is not 0 then data in memory is not equal to data written
			lPageAddr=0x10000;
			printf("MemCheck failed: Abort debugging or press key to continue\n");  //ML->any output ???
			CKeyboard::Flush();	  //Flush keyboard buffer
			CKeyboard::WaitKeyStroke(); 
		} else {
			//Need to restore original data for memory that already has been allocated by the MMU
			if (lPageAddr<StartSeg+0x10)
				memcpy((void *)(lPageAddr << 16),ucOrgMemData,16);
		}
	}
}
#endif