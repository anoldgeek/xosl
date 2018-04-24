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
 * - CAnimatedControl::Draw is a pure virtual function.
 *   Implementation is added because for pure virtual functions implementation
 *   in derived class it required.
 *
 */

#include <AniCntrl.h>


bool CAnimatedControl::Animate = true;


CAnimatedControl::CAnimatedControl(int Left, int Top, int Width, int Height, int Visible, int OnTop, void *HandlerClass):
	CControl(Left,Top,Width,Height,Visible,OnTop,true,HandlerClass)
{
	MouseIsOver = false;
}

CAnimatedControl::~CAnimatedControl()
{
}

int CAnimatedControl::MouseMove(int X, int Y)
{
	if (!Enabled) {
		return -1;
	}
	CControl::MouseMove(X,Y);
	
	if (MouseX >= Left && MouseX <= Right && MouseY >= Top && MouseY <= Bottom) {
		if (Animate && !MouseIsOver) {
			MouseIsOver = true;
			MouseOver();
		}
//		return 0;
		return -1;
	}
	else {
		if (MouseIsOver) {
			MouseIsOver = false;
			MouseOut();
		}
	}
	return -1;
}

void CAnimatedControl::MouseOver()
{
	Refresh();
}

void CAnimatedControl::MouseOut()
{
	Refresh();
}

void CAnimatedControl::Draw(long, long, long, long)   //ML pure virtual function ... implementation required !!!
{
}
