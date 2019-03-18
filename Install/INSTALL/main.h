/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#ifndef __main__
#define __main__

#define MAX_PATH_LEN 128

extern int HDOffset;

typedef struct{
	int BackupPartData;
	char *WorkFolder;
} TXoslWorkConfig;

#endif
