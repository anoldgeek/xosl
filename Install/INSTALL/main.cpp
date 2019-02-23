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

void Usage(char *argv[])
{
	printf("Usage: %s [-p <partbackup-path>|NONE] [-o <HD-offset>] [-b [0|1] [-h]\n", argv[0]);
	printf("e.g.\n\t '%s -p E:\PBACKUPS\ -o 1\n\t %s -b 0 \n", argv[0], argv[0]);
	printf("\t '%s -p E:\PBACKUPS\BACKUP.IMG -o 1 -b 0\n", argv[0], argv[0]);
	printf("\nSee file Notes.txt\n");
}

//int main()
int main(int argc, char* argv[])
{
	int optc;
	char *arg;
	int digit = 0;

	TPartBackControl PartBackupControl;

	PartBackupControl.PartBackupPath = "";
	PartBackupControl.BackupPartData = 1;

	HDOffset = 0;

	puts("XOSL INSTALL UTILITY");
	puts("Copyright (c) 1999/2000, Geurt Vos");
	puts("Copyright (c) 2010, Mario Looijkens");
	puts("Copyright (c) 2018, Norman Back");

	while ((optc = getopt(argc, argv, ":hp:o:b:" )) != -1) {
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
				PartBackupControl.PartBackupPath = optarg;
				break;
			case 'b':
				if (*optarg != '0' && *optarg != '1'){
					Usage(argv);
					return( -1 );
				}
				PartBackupControl.BackupPartData = (*optarg == '1');
				break;
			case ':':
				printf( "-%c without param\n", optopt );
				Usage(argv);
				return( -1 );
			case '?':
			case 'h':
				Usage(argv);
				return ( 0 );
		}
	}
	CApplication *Application = new CApplication(&PartBackupControl);
	Application->ApplicationLoop();
	delete Application;
	return 0;
}
