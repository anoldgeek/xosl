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
#include <alloc.h>

void puts(const char *str);

//void AllocInit(void);

int xoslmain(void *BrecAddr)
{
	int BootDrive;
	unsigned long mmu_addr;

	// Calculate the first seg above the top of stack
	// for the MMU
	_asm{
		mov	dx,ss
		add	dx,1000h
		xor ax,ax
		mov	word ptr mmu_addr,ax
		mov word ptr mmu_addr+02,dx
	}
	AllocInit(mmu_addr);
//	AllocInit(0x60000000);

	CApplication *Application;

	Application = new CApplication;

	BootDrive = Application->Execute(BrecAddr);

	delete Application;

	return BootDrive;
}