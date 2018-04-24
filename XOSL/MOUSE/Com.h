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
 * - Use __cdecl calling convention
 *
 */

#ifndef __serial__
#define __serial__

#include <newdefs.h>

#define COMPORT ((unsigned short far *)0x00400000)
#define COMINT(Port) (((Port) & 1) ? 0x0b : 0x0c)

int __cdecl ComDetect(int port);

void __cdecl ComInit(int port, void far __cdecl (*MouseHandler)(short P0, signed char dY, signed char dX, short Status));  //ML
void __cdecl ComIRQMask(int enable);
void interrupt ComHandler(void);

extern int ComPort;
extern int MouseInt;


#endif
