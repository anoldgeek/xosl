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
 * - Declare virtual void Draw(long Left, long Top, long Width, long Height).
 *   Function is Pure virtual. Definition derived class required.
 * - Add line at end of file to get rid of WCC Compiler Warning! W138: 
 *   No newline at end of file.
 * 
 */

#ifndef AniCntrlH
#define AniCntrlH

#include <Control.h>

class CAnimatedControl: public CControl {
public:
	CAnimatedControl(int Left, int Top, int Width, int Height, int Visible, int OnTop, void *HandlerClass);
	virtual ~CAnimatedControl();

	virtual int MouseMove(int X, int Y);

	virtual void MouseOver();
	virtual void MouseOut();

	static bool Animate;

protected:
	bool MouseIsOver;
	virtual void Draw(long Left, long Top, long Width, long Height);   //ML Pure virtual, definition required !!!

};

#endif  //ML Add line to get rid of Warning! W138: No newline at end of file
