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
 * - Code change in functions GetKeyName, GetShortKeyName, 
 *   GetNewKeyCode, GetKeyCode and IsSpecialKey to get rid of
 *   compiler Error! E419: cannot call non-const function for a constant object
 *
 */

#include <newdefs.h>
#include <key.h>
#include <search.hpp>
#include <string.h>

// US keyboard layout is the default
const CKeyDesc *CKeyboard::SelectedLayout = CKeyboard::USKeyList;
//CKeyDesc *CKeyboard::Current = &CKeyboard::SelectedLayout[0];

bool CKeyboard::EnhancedSupport = false;

CKeyboard::CKeyboard()
{
}

CKeyboard::~CKeyboard()
{
}

/*
static void CKeyboard::StoreKeyStroke( short unsigned KeyCode ){
	int a = 1;
	return;
}
*/
unsigned short CKeyboard::WaitKeyStroke()
{
  if (EnhancedSupport) {
    return WaitKeyStrokeEnh();
  }
  return WaitKeyStrokeDef();
}

bool CKeyboard::KeyStrokeAvail()
{
  if (EnhancedSupport) {
    return KeyStrokeAvailEnh();
  }
  return KeyStrokeAvailDef();
}


void CKeyboard::Flush()
{
  for (; KeyStrokeAvail(); WaitKeyStroke());
}

void CKeyboard::SetEnhancedSupport(bool Status)
{
  EnhancedSupport = Status;
}

void CKeyboard::SelectLayout(TKeyLayout KeyLayout)
{
  switch (KeyLayout) {
    case enumLayoutUS:
      SelectedLayout = USKeyList; // qwerty
      break;
    case enumLayoutFrench:
      SelectedLayout = FrenchKeyList; // azerty
      break;
    case enumLayoutGR:
      SelectedLayout = GRKeyList; // qwertz
      break;
    default:
      break;
  }
}

void CKeyboard::GetKeyName(unsigned short KeyCode, char *KeyName)
{
  int Index;

  //if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,CKeyCodeCompare())) == -1) {  //Error! E419
  CKeyCodeCompare keyCodeCompare;
  if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,keyCodeCompare)) == -1) {
    KeyName[0] = (char)KeyCode;
    KeyName[1] = '\0';
  }
  else {
    strcpy(KeyName,SelectedLayout[Index].KeyName);
  }
}

void CKeyboard::GetShortKeyName(unsigned short KeyCode, char *KeyName)
{
  int Index;

  //if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,CKeyCodeCompare())) == -1) {  //Error! E419
  CKeyCodeCompare keyCodeCompare;
  if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,keyCodeCompare)) == -1) {
    KeyName[0] = (char)KeyCode;
    KeyName[1] = '\0';
  }
  else {
    strcpy(KeyName,SelectedLayout[Index].ShortKeyName);
  }
}

unsigned short CKeyboard::GetNewKeyCode(unsigned short KeyCode)
{
  int Index;

  //if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,CKeyCodeCompare())) == -1) {  //Error! E419
  CKeyCodeCompare keyCodeCompare;
  if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,keyCodeCompare)) == -1) {
    return KeyCode;
  }
  else {
    return SelectedLayout[Index].NewKeyCode;
  }
}

unsigned short CKeyboard::GetKeyCode(unsigned short NewKeyCode)
{
  int Index;

  //if ((Index = LinearSearch(SelectedLayout,KEY_COUNT,NewKeyCode,CNewKeyCodeCompare())) == -1) {   //Error! E419
  CNewKeyCodeCompare newKeyCodeCompare;
  if ((Index = LinearSearch(SelectedLayout,KEY_COUNT,NewKeyCode,newKeyCodeCompare)) == -1) {
    return NewKeyCode;
  }
  else {
    return SelectedLayout[Index].KeyCode;
  }
}
 
bool CKeyboard::IsSpecialKey(unsigned short KeyCode)
{
  int Index;

  //if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,CKeyCodeCompare())) == -1) {  //Error! E419
  CKeyCodeCompare keyCodeCompare;
  if ((Index = BinarySearch(SelectedLayout,KEY_COUNT,KeyCode,keyCodeCompare)) == -1) {
    return false;
  }
  else {
    return SelectedLayout[Index].SpecialKey;
  }

}


