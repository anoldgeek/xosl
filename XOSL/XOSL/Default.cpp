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
 * - Comment-out unused variables to get rid of Warning! W014: no reference 
 *   to symbol.
 * - Make sure Keyboard buffer is empty in CDefaultBoot::WaitTimeout()
 * - Use proper casting to get rid of Warning! W389: integral value may be 
 *   truncated during assignment or initialization.
 * 
 */

#include <Default.h>
#include <String.h>
#include <Timer.h>

#include <Graph.h>

CDefaultBoot::CDefaultBoot(CScreen &ScreenToUse, CLoader &LoaderToUse):
	Screen(ScreenToUse), Loader(LoaderToUse)
{
}

CDefaultBoot::~CDefaultBoot()
{
}

void CDefaultBoot::Show(const char *BootItemName, int TimeoutValue)
{
	//int Status;

	 this->BootItemName = BootItemName;
	Timeout = ((unsigned long)TimeoutValue << 16) / 3600;

	CreateBootString(0);
	CreateControls(TimeoutValue << 4);
	InstallControls();
	Screen.CenterWindow(Form,0,-100);
	Form->Show();
}

void CDefaultBoot::CreateControls(int ProgressBarMax)
{
	int Width;

	BootLabel = new CLabel(BootString,STYLE_REGULAR,true,17,8,12,true);

	Width = Graph->GetTextWidth(BootString,STYLE_REGULAR);
	Width += 16;
	if (Width < 241) {
		Width = 241;
	}
	Form = new CForm("XOSL auto boot",FORM_NORMAL,true,100,100,Width + 24,104,false);
	ProgressBar = new CProgressBar(0,ProgressBarMax,0,8,28,Width,13,true);
	KeyLabel = new CLabel("Press Enter to boot or Escape to abort...",STYLE_REGULAR,true,17,8,56,true);
}

void CDefaultBoot::InstallControls()
{
	Form->AddControl(BootLabel);
	Form->AddControl(ProgressBar);
	Form->AddControl(KeyLabel);
	Screen.AddWindow(Form);
}


void CDefaultBoot::CreateBootString(unsigned long TicksPassed)
{
	int Sec, Min;
	
	TicksPassed *= 3600;
	Sec = (int)((Timeout * 3600 - TicksPassed) >> 16);
	
	BootString = "Booting ";
	BootString += BootItemName;
	BootString += " in ";

	if (Sec >= 60) {
		Min = Sec / 60;
		Sec %= 60;
		BootString += Min;
		if (Sec >= 10) {
			BootString += ".";
		}
		else {
			BootString += ".0";
		}
		BootString += Sec;
		BootString += " minutes";
	}
	else {
		BootString += Sec;
		BootString += " seconds";
	}

}

int CDefaultBoot::WaitTimeout()
{
	unsigned long Time1, Time2, TimePassed, Progress;
	unsigned short Key = 0;

	CKeyboard::Flush();  //ML Make sure Keyboard buffer is empty
	Time1 = GetTimerTicks();
	for (;;) {
		Time2 = GetTimerTicks();
		TimePassed = Time2 - Time1;
		CreateBootString(TimePassed);
		BootLabel->SetCaption(BootString);
		Progress = (TimePassed * 3600) >> 12;
		ProgressBar->SetProgress((int)Progress);

		if (CKeyboard::KeyStrokeAvail()) {
			Key = CKeyboard::WaitKeyStroke();
		}


		if (Key && Key != KEY_ENTER && Key != KEY_K_ENTER) {
#ifndef DOS_DEBUG_ML
			Form->Hide();
#endif
			Loader.Show();
			Loader.SelectDefault();
			Graph->ShowCursor();
			return Key;
		}
	
		if (TimePassed >= Timeout || Key == KEY_ENTER || Key == KEY_K_ENTER) {
			Loader.SelectDefault();
			return -1;
		}
	
	}
}

