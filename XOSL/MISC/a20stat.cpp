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
#define AVOID_LD_CS_BX
#ifdef AVOID_LD_CS_BX

	char *change,*via;

	if (Status < 0x10){
		change = "disabled";
	}else{
		change = "enabled";
	}
	via = "";
	if (Status == 0x01 || Status == 0x10 )
		via = "already set";
	if (Status == 0x02 || Status == 0x20 )
		via = "using int15";
	if (Status == 0x03 || Status == 0x30 )
		via = "using port 92h";
	if (Status == 0x04 || Status == 0x40 )
		via = "using KBC";
	if (Status == 0x05 || Status == 0x50 )
		via = "KBC error";
	if (via == "" ){
		via = "invalid method";
		change = "failure";
	}
	printf("A20 %s: %s\n",change,via);

#else
// Using the switch statement seems to cause issues here
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
#endif
}
