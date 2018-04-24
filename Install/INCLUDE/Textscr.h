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

#ifndef __textscr__
#define __textscr__

class CTextScreen {
	public:
		__cdecl CTextScreen(int Attr);
		__cdecl ~CTextScreen();
		void __cdecl PutStr(int X, int Y, const char *Str, int Attr);
		void __cdecl PutChar(int X, int Y, int Ch, int Attr);
		void __cdecl FillX(int X, int Y, int Ch, int Attr, int Count);
		void __cdecl FillY(int X, int Y, int Ch, int Attr, int Count);
		void __cdecl PutImage(int X, int Y, int Width, int Height, unsigned short *Image);
		void __cdecl GetImage(int X, int Y, int Width, int Height, unsigned short *Image);
	private:
};

#endif
