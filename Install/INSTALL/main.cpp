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
	printf("Usage: install [-p <file-path>] [-o <HD-offset>] [-b [0|1] [-h]\n");
    printf("  -o <drive-offset>\n"
           "     Enables installing from a USB device that has booted as C:\n"
           "     Use '-o 1' to direct xosl install to ignore the first drive.\n"
           "  -p <file-path>\n"
           "     Chooce the folder to save/restore created files\n"
	       "     when installing or upgrading. e.g.\n"
	       "     -p C:\\temp\\  or -p E:\\backups\\<this-pc>\\ \n"
	       "     Use -p C:\\temp\\ when installing to a specifically created dedicated\n"
	       "     partition from a 1.44MB floppy. So that no additional space is required\n"
	       "     on the floppy. The folder must already exist.\n"
           "  -b <enable-disable-part-backup-data>  when installing to a\n"
           "     dedicated partition. e.g.\n"
           "     -b 1 Backup the partition details and data.\n"
	       "     -b 0 Only backup the partition details.\n");
	printf("e.g.\t install -p E:\\PBACKUPS\\ -o 1\n\t install -b 0 \n");
	printf("\t install -p E:\\PBACKUPS\\FREDS-PC\\ -o 1 -b 0\n");
	printf("Also see file Notes.txt\n");
}

//int main()
int main(int argc, char* argv[])
{
	int optc;
	char *arg;
	int digit = 0;
	int pathlen;
	char *tmp;
	char *workfolder;

	TXoslWorkConfig XoslWorkConfig;

	XoslWorkConfig.BackupPartData = 1;
	XoslWorkConfig.WorkFolder = "";

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
				workfolder = new char[strlen(optarg) + 1];
				strcpy(workfolder,optarg);
				XoslWorkConfig.WorkFolder = workfolder;
				break;
			case 'b':
				if (*optarg != '0' && *optarg != '1'){
					Usage(argv);
					return( -1 );
				}
				XoslWorkConfig.BackupPartData = (*optarg == '1');
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
	CApplication *Application = new CApplication(&XoslWorkConfig);
	Application->ApplicationLoop();
	delete Application;
	return 0;
}
