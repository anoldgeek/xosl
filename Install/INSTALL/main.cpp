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
 *
 */


#include <stdio.h>
#include <applic.h>
#include <string.h>
#include <main.h>

int HDOffset; // Global

//int main()
int main(int argc, char* argv[])
{
	int HDOffset = 0;

	puts("XOSL INSTALL UTILITY");
	puts("Copyright (c) 1999/2000, Geurt Vos");
	puts("Copyright (c) 2010, Mario Looijkens");
	puts("Copyright (c) 2018, Norman Back");

	if(argc > 1){
		char *arg = argv[1];
		int digit = *arg - '0';
		if (digit >= 0 && digit <= 9 && arg[1] == 0 ){
			char msg[80];
			strcpy(msg,"\nUsing drive-offset ");
			strcat(msg,arg);
			HDOffset = digit;

			puts(msg);
		}
		else{
			puts("\nInvalid value for offset-drives. Valid values are 0 to 9");
			puts("Usage: 'install.exe [<offset-drives>]'. See Notes.txt");
			return ( -1 );
		}
	}

	CApplication *Application = new CApplication();
	Application->ApplicationLoop();
	delete Application;
	return 0;
}
