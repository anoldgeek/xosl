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
 * - Which syntax to use to replace 
 *   #define SetProgramDSeg() (__emit__(0x1e,0xb8,(void _seg *)&MouseX,0x8e,0xd8))
 *   to make it compatible with Open Watcom. 
 * - Correct Error! E329: modifiers of 'MouseHandler' conflict with previous 
 *   declaration by specifying signed char dY and signed char dX in 
 *   parameter list of function
 *   void MouseHandler(short P0, signed char dY, signed char dX, short Status)
 * - Is far __cdecl calling convention needed for function 
 *   void far __cdecl MouseHandler(short P0, signed char dY, signed char dX, short Status)
 *     removing the far specifier results in identical binary files, hence far 
 *     seems to be redundant !
 *   What about the __cdecl specifier ????
 * - Comment out static short MouseX, MouseY, MouseClick to get rid of 
 *   Error! E042: symbol already defined
 * - Correct Error! E473: function argument(s) do not match those in prototype in
 *     PS2SetHandler(MouseHandler);
 *
 */

#include <mouse.h>
#include <ps2.h>
#include <com.h>
#include <int.h>
#include <mem.h>

//#include <comm.h>

#include <text.h>

//#define Cli() (__emit__(0xfa))
//#define Sti() (__emit__(0xfb))
//#define SetProgramDSeg() (__emit__(0x1e,0xb8,(void _seg *)&MouseX,0x8e,0xd8))
//#define SetHandlerDSeg() (__emit__(0x1f))
static short MouseX, MouseY, MouseClick;
void * pV = &MouseX;
#define VarAddr pV
//#define VarAddr (void *) &MouseX
//#pragma aux [MouseX] loadds;

#pragma aux Cli = 0xfa;
#pragma aux Sti = 0xfb;
//ML Need to correct this stuff!!!!
//FA                        cli
//1E                        push        ds
//B8 00 00                  mov         ax,seg void far * far pV 
//8E D8                     mov         ds,ax
//1F                        pop         ds 
//FB                        sti         

#pragma aux SetProgramDSeg = 0x1e \
                             0xb8 \
                             seg VarAddr \
                             0x8e 0xd8;   /*(void _seg *) &MouseX \*/
#pragma aux SetHandlerDSeg = 0x1f;


//void MouseHandler(short P0, char dY, char dX, short Status);  //Error! E329
//void __cdecl MouseHandler(short P0, signed char dY, signed char dX, short Status);  //Seems like far specifier is not needed
void far __cdecl MouseHandler(short P0, signed char dY, signed char dX, short Status);
void AdjustXY(void);

static int ComShift;
static short WinLeft, WinTop, WinRight, WinBottom;
//static short MouseX, MouseY, MouseClick;   //Error! E042
static void interrupt (*MouseOld)(void);
static const char *PortName[5] = {
	"COM1","COM2","COM3","COM4","PS/2"
};

void ClearCDNW(void);
/*
 * clears CD and NW in CR0.
 * needed for PS/2. Not used anymore
 */


CMouse::CMouse()
{
	WinLeft = WinTop = WinRight = WinBottom = MouseX = MouseY = 0;
	MouseClick = false;
	Port = -1;
	ComPort = -1;
}

CMouse::~CMouse()
{
	RemoveMouse();
}

int CMouse::Detect(int Port)
{
	if (Port == MOUSE_PS2)
		return PS2Initialize(3) ? 0 : 1;
	return ComDetect(COMPORT[Port]) == 1;
}

int CMouse::SetMouse(int Port)
{
	int Vect;

	RemoveMouse();
	if (Port > MOUSE_PS2 || Port < 0)
		return -1;
	if (Port == MOUSE_PS2) {
		if (PS2Initialize(3))
			return -1;
		//PS2SetHandler(MouseHandler);  //Error! E473
		PS2SetHandler((TPS2Handler)MouseHandler);
		this->Port = MOUSE_PS2;
		PS2Enable();
		ComPort = -1;
		return 0;
	}
	else {
		this->Port = Port;
		ComPort = COMPORT[Port];
		ComShift = 3;
		Vect = COMINT(Port);
		MouseInt = Vect;
		if (ComDetect(ComPort) != 1)
			return -1;
		ComIRQMask(false);
		ComInit(ComPort,MouseHandler);
//		CommMouseInit(ComPort);
		MouseOld = GetVect(Vect);
		SetVect(Vect,ComHandler);
		ComIRQMask(true);
		return 0;
	}
}

int CMouse::GetMouse()
{
	return Port;
}

void CMouse::RemoveMouse()
{
	int Vect;

	MouseInt = 0;
	if (Port < 0 || Port > MOUSE_PS2)
		return;
	if (Port == MOUSE_PS2) {
		PS2Disable();
		PS2SetHandler(0);
	}
	else {
		ComIRQMask(false);
		Vect = COMINT(Port);
		SetVect(Vect,MouseOld);
	}
	Port = -1;
}

void CMouse::SetWindow(int Left, int Top, int Width, int Height)
{
	WinLeft = Left;
	WinTop = Top;
	WinRight = Left + Width - 1;
	WinBottom = Top + Height - 1;
	AdjustXY();
}

void CMouse::SetXY(int X, int Y)
{
	MouseX = X;
	MouseY = Y;
	AdjustXY();
}

void CMouse::GetXY(int &X, int &Y)
{
	X = MouseX;
	Y = MouseY;
}

int CMouse::MouseDown()
{
	return MouseClick;
}

const char *CMouse::MousePortName(int Port)
{
	if (Port == -1)
		return PortName[this->Port];
	return PortName[Port];
}

void CMouse::SetSpeed(int NewSpeed)
{
	if (Port == MOUSE_PS2) {
		NewSpeed >>= 1;
		if (NewSpeed == 4) {
			MouseSpeed.PS2.Scale = 2;
			MouseSpeed.PS2.Resolution = 3;
		}
		else {
			MouseSpeed.PS2.Scale = 1;
			MouseSpeed.PS2.Resolution = NewSpeed;
		}
		NewPS2Speed();
	}
	else {
		MouseSpeed.Com.Speed = NewSpeed;
		ComShift = 8 - NewSpeed;
	}
}

void CMouse::SetSampling(int NewRate)
{
	if (Port == MOUSE_PS2) {
		MouseSpeed.PS2.Sampling = NewRate;
		NewPS2Speed();
	}
}

void CMouse::GetSpeed(TMouseSpeed &Speed)
{
	memcpy(&Speed,&MouseSpeed,sizeof (TMouseSpeed));
}

void AdjustXY(void)
{
	if (MouseX < WinLeft)
		MouseX = WinLeft;
	else
		if (MouseX > WinRight)
			MouseX = WinRight;
	if (MouseY < WinTop)
		MouseY = WinTop;
	else
		if (MouseY > WinBottom)
			MouseY = WinBottom;
}

//void far MouseHandler(short P0, char dY, char dX, short Status)    //Error! E329
void far MouseHandler(short P0, signed char dY, signed char dX, short Status)
{

	if (P0)
		return;
	Cli();
	SetProgramDSeg();

	if (ComPort != -1) {
		dY = (dY << 3) >> ComShift;
		dX = (dX << 3) >> ComShift;
	}
	//else
	//	ClearCDNW();  //ML

	MouseX += dX;
	MouseY -= dY;
	MouseClick = Status & 0x07;
	AdjustXY();

	SetHandlerDSeg();
	Sti();
}

void CMouse::NewPS2Speed()
{
	PS2Disable();
	PS2Scaling(MouseSpeed.PS2.Scale);
	PS2Sampling(MouseSpeed.PS2.Sampling);
	PS2Resolution(MouseSpeed.PS2.Resolution);
	PS2Enable();
}

int CMouse::Available()
{
	return Port != -1;
}
