/*
 * Extended Operating System Loader (XOSL)
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


#ifdef __cplusplus
extern "C" {
#endif

void __cdecl PrintA20Status(unsigned short Status);

#ifdef __cplusplus
};
#endif 


//void PrintA20Status(unsigned short Status);

void printf(const char *,...);

//01h: A20 already disabled
//02h: A20 disabled using int15, ah=2400h
//03h: A20 disabled using port 92h
//04h: A20 disabled using KBC
//05h: KBC error
//10h: A20 already enabled
//20h: A20 enabled using int15, ah=2401h
//30h: A20 enabled using port 92h
//40h: A20 enabled using KBC
//50h: KBC error
void PrintA20Status(unsigned short Status)
{
	switch (Status) {
		case 0x01:
		  printf("A20 Disable: already disabled\n");
			break;
		case 0x02:
		  printf("A20 disabled using int15\n");
			break;
		case 0x03:
		  printf("A20 disabled using port 92h\n");
			break;
		case 0x04:
		  printf("A20 disabled using KBC\n");
			break;
		case 0x05:
		  printf("A20 Disable: KBC error\n");
			break;
		case 0x10:
		  printf("A20 Enable: already enabled\n");
			break;
		case 0x20:
		  printf("A20 enabled using int15\n");
			break;
		case 0x30:
		  printf("A20 enabled using port 92h\n");
			break;
		case 0x40:
		  printf("A20 enabled using KBC\n");
			break;
		case 0x50:
		  printf("A20 Enable: KBC error\n");
			break;
		default:
			break;
	}
}
