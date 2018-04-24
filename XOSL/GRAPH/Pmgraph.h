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
 * - Comment-out function DrawText. Not used.
 */

#ifndef __pmgraph__
#define __pmgraph__

#include <newdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl UltraFlush(void);
void __cdecl FlushArea(long Left, long Top, long Width, long Height);
void __cdecl UltraFlushVBE2(void);
void __cdecl FlushAreaVBE2(long Left, long Top, long Width, long Height);
void __cdecl StoreBuffer(long Left, long Top, long Width, long Height);
void __cdecl RestoreBuffer(long Left, long Top, long Width, long Height);
void __cdecl PutPixel(long Left, long Top, long Color);
void __cdecl HLine(long Left, long Top, long Width, long Color);
void __cdecl VLine(long Left, long Top, long Height, long Color);
void __cdecl Bar(long Left, long Top, long Width, long Height, long Color);
//void DrawText(long Left, long Top, long PhysStr, long Color);    //ML Not used anywhere
void __cdecl PutImage(long Left, long Top, long Width, long Height, long IAdd, long PhysImage);
void __cdecl GetImage(long Left, long Top, long Width, long Height, long IAdd, long PhysImage);

void __cdecl SetClippingRegion(long PhysLeft, long PhysTop, long PhysWidth, long PhysHeight);
void __cdecl GetClippingRegion(long PhysLeft, long PhysTop, long PhysWidth, long PhysHeight);
void __cdecl SetViewportOrigin(long PhysLeft, long PhysTop);
void __cdecl GetViewportOrigin(long PhysLeft, long PhysTop);


//extern void MapFontData();   //ML Comment out function in GRAPH\PMGRAPH.ASM
long __cdecl PMTextWidth(long PhysStr);
void __cdecl CreateText(long Width, long PhysStr, long Color);
//extern void CreateText(long Left, long Top, long PhysStr, long Color);

extern long PhysFontHeader;
extern long PhysFontData;
extern int DataSize;

#ifdef __cplusplus
};
#endif

#endif
