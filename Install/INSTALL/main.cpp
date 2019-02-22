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
#include <unistd.h>
#include <applic.h>
#include <string.h>
#include <main.h>

int HDOffset; // Global

//int main()
int main(int argc, char* argv[])
{
	int optc;
	char *arg;
	int digit = 0;
	char *PartBackupPath = "";

	HDOffset = 0;

	puts("XOSL INSTALL UTILITY");
	puts("Copyright (c) 1999/2000, Geurt Vos");
	puts("Copyright (c) 2010, Mario Looijkens");
	puts("Copyright (c) 2018, Norman Back");
/*
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
*/

	optc = getopt(argc, argv, ":hp:o:");
	switch (optc){
		case 'o':
			arg = optarg;
			digit = *arg - '0';
			if (digit >= 0 && digit <= 9 && arg[1] == 0 ){
				HDOffset = digit;
				printf("\nUsing drive-offset %d\n", HDOffset);
			}
			break;
		case 'p':
			PartBackupPath = optarg;
			break;
		case ':':
			printf( "-%c without param\n", optopt );
		case '?':
		case 'h':
			printf("Usage: %s [-p <partbackup-path>|NONE] [-o <HD-offset>] [-h]\n", argv[0]);
			printf("e,g,\n\t '%s -p E:\PBACKUPS -o 1\n\t %s -p NONE \n", argv[0], argv[0]);
			printf("\nSee file Notes.txt\n");
			return ( -1 );
	}


	CApplication *Application = new CApplication(PartBackupPath);
	Application->ApplicationLoop();
	delete Application;
	return 0;
}
