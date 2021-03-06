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
 * - Modify string values for new XOSL version
 * - To get rid of Warning! W389:  integral value may be truncated during 
 *   assignment or initialization use proper casting in:
 *   itoa(*(unsigned long *)argl++,buf,DEC);
 *   itoa defined in stdlib.h in $(%watcom)/h
 *   _WCRTLINK extern char *itoa( int __value, char *__buf, int __radix );
 *   itoa(*(int *)argl++,buf,DEC);
 * - Change MenuItems(3,5,35,11,TextScreenToUse,HelpText) to 
 *   MenuItems(3,5,35,12,TextScreenToUse,HelpText) for proper array size 
 *   definition of CListItem objects (ListItems) to be instantiated in the 
 *   CTextList constructor (textlist.cpp). 
 *   Minimum Array size should be 12, not 11.
 *
 */

#include <textui.h>
#include <textscr.h>

#include <stdlib.h>
#include <string.h>
#include <key.h>
#include <xoslver.h>

#define DEC 10
#define HEX 16

const char *Version = "Extended Operating System Loader "XOSL_VERSION"";
const char *Prog = "XOSL INSTALL UTILITY";
const char *Copyright = "Copyright (c) 1999/2000, Geurt Vos";
const char *StrInfoQuit = "Esc : Quit";
const char *StrInfoSelect = "\x18 \x19 : Select Item";
const char *StrInfoExec = "Enter     : Execute";
const char *StrInfoModify = "PU/PD/-/+ : Modify";
const char *StrOutput = "Output:";


CTextUI::CTextUI(CTextScreen &TextScreenToUse):
	TextScreen(TextScreenToUse),
	Output(41,5,37,12,0x1f00,TextScreenToUse),
	HelpText(1,22,78,1,0x1e00,TextScreenToUse),
	MenuItems(3,5,35,12,TextScreenToUse,HelpText)
{
	DrawText();
	DrawBorders();
	Popup = NULL;
}

CTextUI::~CTextUI()
{
}

void CTextUI::DrawText()
{
	int Offset;

	Offset = 80 - strlen(Version) >> 1;
	TextScreen.PutStr(Offset,0,Version,0x1f00);
	Offset = 80 - strlen(Prog) >> 1;
	TextScreen.PutStr(Offset,1,Prog,0x1f00);
	Offset = 80 - strlen(Copyright) >> 1;

	// ;void CTextScreen::PutStr(int X, int Y, const char *Str, int Attr);
	TextScreen.PutStr(Offset,2,Copyright,0x1f00);
	TextScreen.PutStr(2,18,StrInfoQuit,0x1f00);
	TextScreen.PutStr(2,19,StrInfoSelect,0x1f00);
	TextScreen.PutStr(41,18,StrInfoExec,0x1f00);
	TextScreen.PutStr(41,19,StrInfoModify,0x1f00);
	TextScreen.PutStr(41,4,StrOutput,0x1e00);
}

void CTextUI::DrawBorders()
{
	// ;void CTextScreen::FillX(int X, int Y, int Ch, int Attr, int Count);
	TextScreen.FillX(1,3,205,0x1f00,78);
	TextScreen.FillX(1,24,205,0x1f00,78);
	TextScreen.FillX(1,17,196,0x1f00,78);
	TextScreen.FillX(1,20,196,0x1f00,78);

	// ;void FillY(int X, int Y, int Ch, int Attr, int Count);
	TextScreen.FillY(0,4,186,0x1f00,20);
	TextScreen.FillY(79,4,186,0x1f00,20);
	TextScreen.FillY(39,4,179,0x1f00,13);

	// ;void CTextScreen::PutChar(int X, int Y, int Ch, int Attr);
	TextScreen.PutChar(0,3,201,0x1f00);
	TextScreen.PutChar(79,3,187,0x1f00);
	TextScreen.PutChar(79,24,188,0x1f00);
	TextScreen.PutChar(0,24,200,0x1f00);
	TextScreen.PutChar(39,3,209,0x1f00);
	TextScreen.PutChar(39,17,193,0x1f00);
	TextScreen.PutChar(0,17,199,0x1f00);
	TextScreen.PutChar(79,17,182,0x1f00);
	TextScreen.PutChar(0,20,199,0x1f00);
	TextScreen.PutChar(79,20,182,0x1f00);

}

void CTextUI::AddMenuItem(int Index, const char *Caption,
	const char *HelpText, int Enabled)
{
	MenuItems.AddItem(Index,Caption,HelpText,Enabled);
}

void CTextUI::AddMenuItem(int Index, const char *Caption, const char *HelpText,
	int Enabled, int OptionCount, const char **OptionList, int IsCombo, int LinkedListItemIndex, unsigned char *MbrHDSector0List)
{
	MenuItems.AddItem(Index,Caption,HelpText,Enabled,OptionCount,OptionList,IsCombo,LinkedListItemIndex,MbrHDSector0List);
}

void CTextUI::RefreshMenu()
{
	MenuItems.Refresh();
	TextScreen.PutStr(3,3,MenuTitle,0x1f00);
}

void CTextUI::ClearMenu()
{
	MenuItems.Clear();
}

void CTextUI::SetItemIndex(int ItemIndex)
{
	MenuItems.SetItemIndex(ItemIndex);
}

void CTextUI::HandleKey(int Key)
{
	if (Popup) {
		Popup->Hide();
		delete Popup;
		Popup = NULL;
		return;
	}
	MenuItems.HandleKeyAction(Key);
}


void CTextUI::OutputStr(const char *format, ...)
{
	unsigned short *argl;
	char buf[32];

	for (argl = &((unsigned short *)&format)[2]; *format; ++format)
		switch (*format) {
			case '%':
				switch (*++format) {
					case 'c':
						Output.PutChar(*argl++);
						break;
					case 'd':
						itoa(*argl++,buf,DEC);
						Output.PutStr(buf);
						break;
					case 'x':
						itoa(*argl++,buf,HEX);
						Output.PutStr(buf);
						break;
					case 's':
						Output.PutStr((const char *)*(long *)argl);
						argl += 2;
						break;
					case 'l':
						if (format[1] == 'd') {
							++format;
							   ltoa(*(long *)argl++,buf,DEC);
							   argl +=2;
							   Output.PutStr(buf);
							   break;
						}
						if (format[1] == 'u') {
							   ++format;
							   ultoa(*(unsigned long *)argl,buf,DEC);
							   argl +=2;
							Output.PutStr(buf);
							break;
						}
						if (format[1] == 'x') {
							++format;
						   ltoa(*(long *)argl,buf,HEX);
						   argl +=2;
							Output.PutStr(buf);
							break;
						}
					default:
						Output.PutChar('%');
						--format;
						break;
				}
				break;
			default:
				Output.PutChar(*format);
				break;
		}
}

int CTextUI::GetOptionIndex(int Index)
{
	return MenuItems.GetOptionIndex(Index);
}

void CTextUI::ConnectEventHandler(CTextList::TListItemExecute Handler, void *_this)
{
	MenuItems.ConnectEventHandler(Handler,_this);
}

void CTextUI::SetMenuTitle(const char *Title)
{
	MenuTitle = Title;
	MenuTitleLength = strlen(MenuTitle);
}

void CTextUI::ShowPopup(int Left, int Top, int Width, int Height, const char *Text)
{
	Popup = new CTextPopup(Left,Top,Width,Height,Text,TextScreen);
	
	Popup->Show();
}

void CTextUI::SetOptionIndex(int OptionIndex, int Index)
{
	MenuItems.SetOptionIndex(OptionIndex,Index);
}
