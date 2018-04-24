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
 * - Add line at end of file to get rid of Warning! W138: No newline at end of file
 * 
 */

#ifndef __vpgraph__
#define __vpraph__

#include <newdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl VPUltraFlush(void);
void __cdecl VPFlushArea(long Left, long Top, long Width, long Height);
void __cdecl VPStoreBuffer(long Left, long Top, long Width, long Height);
void __cdecl VPRestoreBuffer(long Left, long Top, long Width, long Height);
void __cdecl VPPutPixel(long Left, long Top, long Color);
void __cdecl VPHLine(long Left, long Top, long Width, long Color);
void __cdecl VPVLine(long Left, long Top, long Height, long Color);
void __cdecl VPRectangle(long Left, long Top, long Width, long Height, long Color);
void __cdecl VPBar(long Left, long Top, long Width, long Height, long Color);
void __cdecl VPPutImage(long Left, long Top, long Width, long Height, long PhysImage);
void __cdecl VPGetImage(long Left, long Top, long Width, long Height, long PhysImage);
void __cdecl VPLine(long X1, long Y1, long X2, long Y2, long Color);
void __cdecl VPTextOut(long Left, long Top, long PhysStr, long Color);

#ifdef __cplusplus
};
#endif

#endif
