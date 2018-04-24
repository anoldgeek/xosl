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

void puts(const char *str);

void AllocInit(void);

int main(void)
{
	int BootDrive;

	AllocInit();

	CApplication *Application;

	Application = new CApplication;

	BootDrive = Application->Execute();

	delete Application;

	return BootDrive;
}