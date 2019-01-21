/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 * - const TMountPart MOUNT_PART = {0x80,0x06,0x000642cf};      
 *   For debugging the sector number should be the first sector of 
 *   the XOSL partition on the debugging machine.
 *   define Int19Reboot() (__emit__(0xcd,0x19)) is not used in the source 
 *   code, that's why no error caused by __emit__ is raised during compilation. 
 * - For debugging call void MemoryCheck() in CApplication() construction. 
 *   Used to check if conventional memory area (used by MMU) can be accessed 
 *   both for reading and writing. This is relevant only when debugging XOSL 
 *   as a "normal" DOS application.
 * - Comment-out unused variables to get rid of Warning! W014: no reference 
 *   to symbol.
 * - Flush keyboard buffer in CApplication::Execute() by calling 
 *   CKeyboard::Flush() to make sure keyboard buffer is empty when it is 
 *   should be.
 * - Delete StringTable in CApplication::DestroyCoreClasses(). Xosl115 did not
 *   delete StringTable and it is unclear why this is. It was the only object 
 *   that was not explicitly deleted. 
 * - Change implementation void CApplication::ScanForDebugKeys(int Key).
 * - Add implementation void MemoryCheck() for debugging.
 *
 */

#include <xoslapp.h>
#include <cstring.h>
#include <text.h>
#include <timer.h>
#include <mem.h>

#include <key.h>
#include <bypass.h>

#include <mouse.h>
#include <graph.h>
#include <key.h>
#include <screen.h>
#include <fs.h>
#include <fat16.h>
#include <fat32.h>
#include <quit.h>

#include <ptab.h>
#include <xosldata.h>
#include <items.h>

#include <loader.h>
#include <setup.h>
#include <prefer.h>
#include <about.h>
#include <dialogs.h>
#include <dcx.h>
#include <antivir.h>
#include <disk.h>
#include <cdrom.h>

#include <dump.h>

#include <strtable.h>

//#define PRELOAD2 1

void printf(const char *format, ...);
/*void getch()
{
	_asm xor ah,ah
	_asm int 0x16
}	*/

unsigned long CoreLeft();
//void *operator new (unsigned long Size);
void *operator new (size_t Size);

/**/

typedef struct {
	int Drive;
	int FSType;
	unsigned long StartSector;
} TMountPart;

#ifndef DOS_DEBUG

#define MOUNT_PART  ( *(const TMountPart *)0x00007c00 )

#else

//const TMountPart MOUNT_PART = {0x80,0x06,0x0031b600};

//const TMountPart MOUNT_PART = {0x80,0x06,0x00246c00};
// const TMountPart MOUNT_PART = {0x82,0x06,0xffff3000};
//const TMountPart MOUNT_PART = {0x82,0x06,0xffff3800}; // win311
const TMountPart MOUNT_PART = {0x83,0x06,0xffff3800}; // win311
//const TMountPart MOUNT_PART = {0x80,0x06,0x005ffa40};
static char *FileName = "dump_a.dmp";

#endif


/**/

#define LOGO_WIDTH 218
#define LOGO_HEIGHT 146

#define WALLPAPER_WIDTH 150
#define WALLPAPER_HEIGHT 150

#define Int19Reboot() (__emit__(0xcd,0x19))

#define BypassRequest() ( (peekb(0x0000,0x0417) & 0x02) == 0x02 )

/* Global vars. */
unsigned char *SplashLogo;
unsigned char *LogoBitmap;
unsigned char *WallpaperBitmap;
void *DefaultFont;
void *AdditionalFont;



/**/
const char *DefaultFontFileName = "DEFAULT XFF";
const char *AddFontFileName     = "EXTRA   XFF";
const char *LogoFileName	 = "XOSLLOGOXBF";
const char *SplashFileName      = "SPLASHLGXBF";
const char *WallpaperFileName   = "XOSLWALLXBF";

CApplication::CApplication()
{
	#ifdef DOS_DEBUG_NB
	MemoryCheck();   ///ML: Check if (MMU) memory is accessible.
	#endif

	TextCapture = new CTextCapture;
}

CApplication::~CApplication()
{
	if (!ClearScreen)
		delete TextCapture;
}

int CApplication::Execute()
/*
 * XOSL main application function
 */
{
	int Key;
	int X, Y;
	int Status;
	TMiscPref *MiscPref;
	//CBootItem *BootItem;

	if (BypassRequest())
		ExecuteBypass();


	CreateCoreClasses();
	InitializeCore();

	// reveal all partitions if requested
	MiscPref = XOSLData->GetMiscPref();
	PartList->RevealAll();

	CreateApplication();

	DoSplashFadeOut();

	InitAppGraphics();

	/* Display main window */

	if (!AutoBootAvail()) {
		Graph->ShowCursor();
		Loader->Show();
		Screen->InitScreen();
		DoAppFadeIn();
		CKeyboard::Flush(); //ML: Flush keyboard buffer
		Key = 0;
	}
	else {
		if (!MiscPref->NoAutoBootDialog) {
			DefaultBoot->Show(BootItems->Get(BootItems->GetDefault())->ItemName,BootItems->GetTimeout());
		}
		else {
			Loader->Show(true);
			Loader->SelectDefault();
		}
		
		Screen->InitScreen();
		DoAppFadeIn();

		if (!MiscPref->NoAutoBootDialog) {
			Key = DefaultBoot->WaitTimeout();
		}
		else {
			Key = Loader->WaitTimeout();
		}
		if (Key != -1) {
			CKeyboard::Flush();
			CKeyboard::StoreKeyStroke(Key);
		}
	}

	/* Main loop */
	do {
		while (Key != -1) {
			while (!CKeyboard::KeyStrokeAvail() && !Loader->CanBoot()) {
				Mouse->GetXY(X,Y);
				Graph->SetCursorXY(X,Y);
				Screen->MouseStatus(X,Y,Mouse->MouseDown());
			}
			if (Loader->CanBoot()){
				Key = -1;
				CKeyboard::Flush();  //ML
			}
			else {
				Key = CKeyboard::WaitKeyStroke();

				Screen->KeyPress(Key);
				if (!Loader->GetIgnoreNextKey()) {
					if (Key == MiscPref->KeyCycle)
						Screen->FocusNext();
						//Loader->CycleWindows();
					else
#ifndef DOS_DEBUG_ML
						if (Key == MiscPref->KeyShutdown) {
							Loader->HandleAutoSave();
							Shutdown();
						}
						else if (Key == MiscPref->KeyRestart) {
							Loader->HandleAutoSave();
							Restart();
						}
						else if (Key == MiscPref->KeyColdReboot) { 
							Loader->HandleAutoSave();
							Reboot();
						}
						else if (Key == MiscPref->KeyPart) {
							Loader->HandleAutoSave();
							if (XOSLData->GetPassword()) {
								Dialogs->SetPasswordHandler(this,(TPwdProc)PartPwdProc);
								Dialogs->ShowPasswordDialog(NULL,"Ranish Partition Manager");
							}
							else {
								StartPart();
							}
						}

#else
						ScanForDebugKeys(Key);
//						unsigned short NewKey = CKeyboard::GetNewKeyCode(Key);
//						printf("\r[%x - %c] [%x - %c]      ",Key,Key & 0xff,NewKey,NewKey & 0xff);

#endif
				}
			}
		}

#ifdef DOS_DEBUG_ML
		if (MiscPref->SetAutoBoot) {
			BootItems->SetDefault(Loader->GetBootItemIndex());
			BootItems->Save();
		}
		/* terminate program instead of booting */
		delete Mouse;
		Graph->SetMode(modeText,false);
		puts(BootItems->Get(Loader->GetBootItemIndex())->ItemName);
		_asm mov ah,0x4c
		_asm int 0x21
#endif

		// save settings _before_ drive swapping is done!
		if (MiscPref->SetAutoBoot) {
			BootItems->SetDefault(Loader->GetBootItemIndex());
			BootItems->Save();
		}


		PartList->SetAllowActiveHD(MiscPref->ActiveAllow);
		Status = BootItems->InitBoot(Loader->GetBootItemIndex());
		if (Status == -1) {
			DisplayBootError();
			Key = 0;
		}
	} while (Status == -1);

	ClearScreen = XOSLData->GetGraphics()->ClearScreen;

	if (!XOSLData->GetGraphics()->DisableFadeOut)
		Graph->Palette->FadeToBlack();
	Graph->SetMode(modeText,false);

	DestroyApplication();
	FinalizeCore();
	DestroyCoreClasses();

	return Status;
}

void CApplication::CriticalError(const char *ErrorMsg)
{
	//int Key;
	CBypass XoslBypass;

	Graph->SetMode(modeText,false);
	delete Mouse;
	XoslBypass.Execute(ErrorMsg);
}

void CApplication::ExecuteBypass(/*int Password*/)
{
	CBypass XoslBypass;

	XoslBypass.Execute();
}

void CApplication::CreateCoreClasses()
{
	StringTable = new CXoslStringTable;
	Mouse = new CMouse;
	Graph = new CGraph(Mouse);
	Screen = new CScreen;
	switch (MOUNT_PART.FSType) {
		case 0x06: /* FAT16 */
			FileSystem = new CFAT16;
			break;
		case 0x0b: /* FAT32 */
			FileSystem = new CFAT32;
			break;
		default:
			CriticalError("Unknown filesystem.");
			break;
	}
}

void CApplication::DestroyCoreClasses()
{
	delete FileSystem;
	delete Screen;
	delete Graph;
	delete Mouse;
}

void CApplication::InitializeCore()
{
	TGraphData *GraphData;

	/* Initialize splash screen */
	if(FileSystem->Mount(MOUNT_PART.Drive,MOUNT_PART.StartSector) == -1 ) {
		printf("Failed to mount drive %x Startsector %lu\r\n",MOUNT_PART.Drive,MOUNT_PART.StartSector);
		CriticalError("Mount failed\r\n");
	}
	LoadXOSLSplashLogo();
	Screen->SetSplashLogo(LOGO_WIDTH,LOGO_HEIGHT,SplashLogo);
	
/*
	CCdRom *CdRom = new CCdRom(*FileSystem);
	if (CdRom->Install(0,0) == -1) {
		CriticalError("EDD30.BIN not found.");
	}
	for (int Drive = 0x80; Drive < 0x84; ++Drive) {
		printf("Drive %d: %d\n",Drive,CdRom->IsDriveCdRom(Drive));
	}

	for ( Drive = 0x80; Drive < 0x84; ++Drive) {
		printf("Drive %d: %d\n",Drive,CdRom->IsDriveCdRom(Drive));
	}

	CdRom->Uninstall();
	delete CdRom;
	getch();
*/	
	
	
	XOSLData = new CXOSLData(FileSystem);
	GraphData = XOSLData->GetGraphics();

	PartList = new CPartList;
	BootItems = new CBootItems(*FileSystem,*PartList);

	//if (Screen->ShowSplashScreen(GraphData->VideoMode,GraphData->LinearFB) == -1)
	bool bTmpLinearFB=GraphData->LinearFB;   //ML: Code Change
	if (Screen->ShowSplashScreen(GraphData->VideoMode,bTmpLinearFB) == -1)
		CriticalError("Graphics mode not supported.");
	/* Load additional data */
	LoadDefaultFont();
	Graph->SetFont(DefaultFont);

	// Credits for Ranish Partition Manager
	Screen->ShowSubTitle();

	if (XOSLData->GetMiscPref()->AntiVirus) {
		PerformAntiVirus();
	}

	LoadAdditionalFont();
	LoadXOSLLogo();
	LoadWallpaper();
	Screen->SetWallpaper(WALLPAPER_WIDTH,WALLPAPER_HEIGHT,WallpaperBitmap);
	/* Additional initialization */
	InitializeMouse();
}

void CApplication::FinalizeCore()
{
	delete XOSLData;
	delete BootItems;
	delete PartList;
}

void CApplication::CreateApplication()
{
	// ignore screen updates
	Screen->SetIgnoreSU(true);

	Loader = new CLoader(*Screen,*BootItems,*XOSLData,*PartList,*Mouse);
	DefaultBoot = new CDefaultBoot(*Screen,*Loader);
#ifdef PRELOAD1
	About = new CAbout(Loader,Screen);
	Setup = new CSetup(*Loader,*Screen,*PartList,*BootItems,*XOSLData,*Keyboard);
	Preference = new CPreference(*Loader,*Setup,*About,*Screen,*XOSLData,*Keyboard,*Mouse);
#endif

	Dialogs = new CDialogs(*Screen);

#ifdef PRELOAD2
	Loader->Initialize(Preference,Setup,About,Screen,BootItems,XOSLData);
	Preference->InitializeData();
	Setup->InitializeData();
#endif	
	Screen->SetIgnoreSU(false);
}

void CApplication::DestroyApplication()
/*
 * delete takes time, so if not really necessary, then don't.
 */
{
/*	delete About;
	delete Preference;
	delete Setup;
	delete Loader;*/
}

void CApplication::LoadDefaultFont()
{
	DefaultFont = new char[8192];
	if (!FileSystem->ReadFile(DefaultFontFileName,DefaultFont))
		CriticalError("Font file not found.");
}

void CApplication::LoadXOSLSplashLogo()
{
	SplashLogo = new unsigned char [(unsigned int)(LOGO_WIDTH * LOGO_HEIGHT)];
	if (!FileSystem->ReadFile(SplashFileName,SplashLogo))
		CriticalError("Logo bitmap not found.");
}

void CApplication::LoadXOSLLogo()
{
	LogoBitmap = new unsigned char [(unsigned int)(LOGO_WIDTH * LOGO_HEIGHT)];
	if (!FileSystem->ReadFile(LogoFileName,LogoBitmap))
		CriticalError("Logo bitmap not found.");
}

void CApplication::LoadAdditionalFont()
{
	AdditionalFont = new char [8192];
	if (!FileSystem->ReadFile(AddFontFileName,AdditionalFont))
		CriticalError("Font file not found.");
}

void CApplication::LoadWallpaper()
{
	WallpaperBitmap = new unsigned char [(unsigned int)(WALLPAPER_WIDTH * WALLPAPER_HEIGHT)];
	if (!FileSystem->ReadFile(WallpaperFileName,WallpaperBitmap))
		CriticalError("Wallpaper bitmap not found.");
}

void CApplication::InitializeMouse()
{
	TMouseData *MouseData;
	int MousePort;

	/* Initialize mouse */
	MouseData = XOSLData->GetMouse();
	if (MouseData->MouseType != -1) {
		if (Mouse->SetMouse(MouseData->MouseType) == -1) {
			for (MousePort = 0; MousePort < 5 && !Mouse->Detect(MousePort); ++MousePort);
			if (MousePort == 5) {
				// Mouse not found, so set to 'no mouse'
				MouseData->MouseType = -1;
			}
			else {
				Mouse->SetMouse(MousePort);
				MouseData->MouseType = MousePort;
			}
		}
	}
}

void CApplication::InitAppGraphics()
{
	int Width, Height;
	TGraphData *GraphData;
	TMouseData *MouseData;
	TColorData *ColorData;

	Graph->GetModeMetrics(Width,Height);
	/* Initialize mouse window, etc.
		Doesn't matter if no mouse installed */
	Mouse->SetWindow(0,0,Width,Height);
	Width >>= 1;
	Height >>= 1;
	Mouse->SetXY(Width,Height);
	Graph->SetCursorXY(Width,Height);

	GraphData = XOSLData->GetGraphics();
	MouseData = XOSLData->GetMouse();
	ColorData = XOSLData->GetColor();

	// keyboard layout
	CKeyboard::SelectLayout((CKeyboard::TKeyLayout)MouseData->KbdLayout);
	CKeyboard::SetEnhancedSupport(MouseData->EnhancedSupport);

	// FrameMove
	CForm::FrameMove = GraphData->FrameMove;
	CAnimatedControl::Animate = !GraphData->NoAnimation;

	// Cursor
	InvertCursor(GraphData->CursorInverted);
	// Font
	if (GraphData->Font9) {
		Graph->SetFont(AdditionalFont);
	}

	// Wallpaper
	Screen->SetUseWallpaper(GraphData->UseWallpaper,false);

	// Color
	Graph->Palette->SetScheme(ColorData->ColorScheme);
	Graph->Palette->SetHue(ColorData->Hue);
	Graph->Palette->SetSaturation(ColorData->Saturation);
	if (ColorData->Luminance < 0) {
		Graph->Palette->SetColorLayer(0,0,0,-ColorData->Luminance);
	}
	else {
		Graph->Palette->SetColorLayer(63,63,63,ColorData->Luminance);
	}
	Graph->Palette->Install();

	// Mouse speed & sampling
	Mouse->SetSpeed(MouseData->MouseSpeed);
	Mouse->SetSampling(MouseData->PS2Sampling);

}

void CApplication::DisplayBootError()
{
	CString ErrMsg;

	ErrMsg = "Unable to boot '";
	ErrMsg += BootItems->Get(Loader->GetBootItemIndex())->ItemName;
	ErrMsg += "'.";
	Loader->ShowBootError(ErrMsg);
}

void CApplication::Shutdown()
{
	CQuit Quit(Mouse);

	Loader->Hide();
	Dialogs->SetAlertHandler(this,(TAlertProc)ShutDownReboot);
	Dialogs->ShowAlertDialog(NULL,"Shutdown","It is now safe to turn off your computer","Reboot");
	if (XOSLData->GetMiscPref()->SoftPowerOff)
		Quit.Shutdown();
}

void CApplication::ShutDownReboot(CApplication *Application)
{
	Application->Reboot();
}

void CApplication::Restart()
{
	CQuit Quit(Mouse);

	Quit.Restart();
}

void CApplication::Reboot()
{
	CQuit Quit(Mouse);

	Quit.Reboot();
}

void CApplication::DoSplashFadeOut()
{
	Graph->Palette->LockFading();
	if (!XOSLData->GetGraphics()->DisableFadeIn) {
		Graph->Palette->SetFadeOutColor(XOSLData->GetColor()->FadeColorInt);
		Graph->Palette->FadeOut();
	}
	else {
		Graph->Palette->SetFadeOutColor(0);
		Graph->Palette->UltraFadeOut();
	}
}

void CApplication::DoAppFadeIn()
{
	if (!XOSLData->GetGraphics()->DisableFadeIn)
		Graph->Palette->FadeIn();
	else
		Graph->Palette->UltraFadeIn();
}


bool CApplication::AutoBootAvail()
{
	return BootItems->GetDefault() != -1 && !BootItems->Get(BootItems->GetDefault())->Disabled;
}


#ifdef DOS_DEBUG_ML
void CApplication::ScanForDebugKeys(int Key)
/*
 * Keys for debug information
 * F9 - Terminate XOSL
 * F8 - Refresh screen
 * F7 - Set cursor position to 0,0
 * F6 - Screen shot
 * F5 - Dump RGB palette
 * F4 - Print CoreLeft()
 */
{
		TGraphMode graphMode;
		bool bUseLFB;
		bool bSwitchToDbgScreen;
		
		if ((Key >= KEY_F1) && (Key <= KEY_F9))
		{
			bSwitchToDbgScreen=true;
			Graph->GetMode(graphMode, bUseLFB);
			Graph->SetMode(modeText,false);
		}
		else
			bSwitchToDbgScreen=false;

	if (Key == KEY_F9) {
		printf("delete Mouse\n");
		delete Mouse;
		printf("Graph->SetMode(modeText,false);\n");
		Graph->SetMode(modeText,false);
		printf(".exit\n");
		_asm mov ah,0x4c
		_asm int 0x21
	}
	if (Key == KEY_F8)
		Screen->Refresh();
	if (Key == KEY_F7)
		gotoxy(0,0);
	if (Key == KEY_F6) {
		DumpScreen(FileName);
		++FileName[5];
	}
	if (Key == KEY_F5)
		DumpPalette("pal.dmp");
	if (Key == KEY_F4) {
		printf("\nCoreLeft(): %lu\n",CoreLeft());
		//gotoxy(0,0);
	}
	if (Key == KEY_F1) {
		printf("\nF4 - Print CoreLeft()\nF5 - Dump RGB palette\nF6 - Screen shot\n");
		printf("F7 - Set cursor position to (0,0)\nF8 - Refresh screen\nF9 - Terminate XOSL\n");
	}
	if (bSwitchToDbgScreen)
	{
		printf("\nPress key to leave DBG Screen");
		CKeyboard::Flush();			 //ML Flush keyboard buffer
		CKeyboard::WaitKeyStroke(); //   and wait for keypressed

		Graph->SetMode(graphMode,bUseLFB);
		Screen->Refresh();
	}
}
#endif

void CApplication::StartPart()
// have to clean-up this mess...
{
	void *Ptr1 = (void *)0x10000000;
	void *Ptr2 = (void *)0x18000000;


	if (FileSystem->ReadFile("XRPART00XXF",Ptr1) != 32768) {
		Dialogs->ShowMessageDialog(NULL,"Ranish Partition Manager","Either Ranish Partition Manager is not installed, or its files are missing");
		return;
	}

	Graph->SetMode(modeText,false);
	puts("\nStarting Ranish Partition Manager...");

	if (  FileSystem->ReadFile("XRPART01XXF",Ptr2) != 32768) {
		CriticalError("\r\nError loading XRPART01XXF\r\n");
	}

	DestroyApplication();
	FinalizeCore();
	DestroyCoreClasses();

	_asm {
		xor	ax,ax
		mov es,ax
		push 0x1000
		push 0x0200
		retf
	}

}

void CApplication::PartPwdProc(CApplication &Application, const char *Password)
{
	unsigned long Code;

	Code = EncodePassword(Password);
	if (Application.XOSLData->GetPassword() == Code) {
		Application.StartPart();
	}
	else {
		Application.Dialogs->ShowMessageDialog(NULL,"Ranish Partition Manager","Invalid Password");
	}
}

void CApplication::PerformAntiVirus()
{
	CAntiVirus AntiVirus(*FileSystem);

	AntiVirus.Sweep();
}

#ifdef DOS_DEBUG
void CApplication::MemoryCheck()
{
	//ML: Function to test MMU read/write access 
	long lPageAddr;
	unsigned char ucOrgMemData[16];
	unsigned char ucTestMemData[16];
	int iIndex;

	//void memcpy (void far *dest, void far *str, U16B size);
	//void memset(void far *dest, U8B value, U16B count);
	//int memcmp(const void far *s1, const void far *s2, U16B count); 

	for (iIndex=0;iIndex<16;iIndex++)
	{
		 ucTestMemData[iIndex]=0x80+iIndex;
	}

	#define MMU_STARTADDRESS 0x6500 //Value should be in line with value set in starter.asm 

	for (lPageAddr=MMU_STARTADDRESS;lPageAddr<0x9fff;lPageAddr++)
	{
		memcpy(ucOrgMemData,(void *)(lPageAddr << 16),16);
		memcpy((void *)(lPageAddr << 16),ucTestMemData,16);
		if (memcmp((void *)(lPageAddr << 16),ucTestMemData,16)){
			//if memcmp return value is not 0 then data in memory is not equal to data written
			lPageAddr=0x10000;
			printf("CApplication::MemCheck failed: Abort debugging or press key to continue\n");  //ML->any output ???
			CKeyboard::Flush();	  //Flush keyboard buffer
			CKeyboard::WaitKeyStroke(); 
		} else
		{
			//Need to restore original data for memory that already has been allocated by the MMU
			if (lPageAddr<MMU_STARTADDRESS+0x10)
				memcpy((void *)(lPageAddr << 16),ucOrgMemData,16);
		}
	}
}
#endif
