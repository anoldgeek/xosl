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
 * - Add line at end of file to get rid of WCC Compiler 
 *   Warning! W138: No newline at end of file.
 *
 */

#ifndef __mouse__
#define __mouse__

#include <defs.h>

class CMouse {
public:
	enum TMouseType { enumMouseCom1 = 0, enumMouseCom2, enumMouseCom3, enumMouseCom4, enumMousePs2, enumMouseNone };


	CMouse();
	~CMouse();
	int MouseTypeSupported(TMouseType MouseType);
	const char *GetPortName(TMouseType MouseType);
private:
#define COMPORT_ADDRLIST 0x00400000

	unsigned short *ComPortAddr;

	static const char *PortNames[6];

	static int __cdecl Ps2Initialize(int PackSize);
	static int __cdecl MouseAvailable(int PortAddr);

};

#endif
