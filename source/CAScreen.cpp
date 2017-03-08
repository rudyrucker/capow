
/* --------------------------------------------------------------------
		This screensaver was written by Rudy Rucker.
		It is based on a screensaver downloaded as part of the freesave.exe
	"Complete Windows Package" from
	http://www.escape.ca/~bbuckels/freesave.htm 
		Buckels says this:
	    "If you use this code as the basis for your own screen saver, you
    should not have much trouble adapting it, but be careful to read
    the comments in scrnsave.h and perhaps the bouncer project and
    don't remove any of the code that Windows expects.
	    "A screensaver is only an EXE file that is produced using certain
    specific guidelines, and then renamed to a .SCR file. In Windows 3.1 
	these are kept in the Windows directory and in Windows 95 they are tossed
	into the WINDOWS\SYSTEM directory. They then become visible in the Control
	Panel and run exactly the same as the Microsoft-shipped screensavers."
	---Bill Buckels 1997
		Things Rudy has learned about screensavers:
	* You have to include <scrnsave.h> and link to SCRNSAVE.LIB.  There are some
		helpful comments in the scrnsave.h, which lives in your compiler's
		Include directory.
	* A screensaver does its painting in the WM_ERASEBKGND message.  It MUST NOT
		process WM_PAINT.
	* A screensaver procedure must be called ScreenSaverProc, a screensave
		initialization dialog procedure must be called ScreenSaverConfigureDialog
		and the dialog resource must be called DLG_SCRNSAVECONFIGURE.
	* As well as defining a ScreenSaverProc and a ScreenSaverConfigureDialog,
		you must also define a (trivial) RegisterDialogClasses function
		which just returns TRUE.
	* The running of the initialziation dialog and the screensaver are distinct.  
		The only way to save info from the dialog is into a profile string (easy)
		or into the Windows Registry using HKEY_USERS, RegSetValueEx, RegEnumValue,
		and RegQueryValue (don't know how to do yet, it looks gnarly.)  And you
		load the info out of the profile string at start of dialog or screensaver.
	* Use a WM_TIMER message to keep the screensaver running.  Don't worry, by the
		way, about too many WM_TIMER messages, as only one from a given timer is
		put in the queue at the same time.  You don't have access to the message
		loop for a screensaver in any case, so there is no  hope of using	
		a "Peekmessage else" or an "OnIdle" technique.
	* The scrnsave.h file defines an szAppName variable with the awkward type
		char [APPNAMEBUFFERLEN] instead of char *.  You must assign a value to
		this variable in your screensaver code.  You should not try and
		access this variable in other modules as it's not practical to try
		and reference a fixed-size buffer.  It's a poor team player and should
		not be included in any group activities.  Define it here, but don't
		use it anywhere.  Let it sulk alone.  Then define a char * szMyAppname
		which you can easily reference in other modules.  If you had a
		szAppName in your main .EXE module, change that to a szMyAppame for
		consistency.
	* scrensave.h also prototypes
		a function called RegisterDialogClasses.  You must provide a body for
		this function in your screensaver code.
	* For accessing your window, you can use the fact that scrensave.h defines
		an HINSTANCE hMainInstance and an HWND hMainWindow.  I think maybe the
		screensaver likes you to declare these in here as externs even if you
		don't use them.
	* The modal screen dialog closes with the default OK button press when you
		press enter, so you need to convert your editbox text to numbers here.
	---- Rudy Rucker, December, 1997, rucker@mathcs.sjsu.edu
 --------------------------------------------------------------------- */
//DEFINES==============
#define EXTERNAL_CA

//INCLUDES=====================================
//==============================================
#include "types.h" //Includes <windows.h>, defines some types used by CAPOW
#include <scrnsave.h>
#include "resource.h" //resources shared with CAPOW
	//Includes needed for CAPOW.
#include "ca.hpp"
#include "bitmap.hpp"
#include "capowGL.hpp"

//VARIABLES=======================================
//================================================
	//SCREENSAVER OVERHEAD
/* We need a name for our program.  We put it in a format used by SCRNSAVE.LIB
and required by any screensaver.  This format uses a static array size defined
in SCRNSAVE.H like this:
 #define APPNAMEBUFFERLEN 40
The SCRNSAVE.LIB also uses TCHAR instead of char, but since this isn't a Unicode application that
doesn't make any difference, as TCHAR has a default typedef of char.  The bad
thing about this variable is that if you try and reference it externally as
an extern TCHAR szAppName * this doesn't work.  So don't use it anywhere else.
Just put it here for SCRNSAVE.LIB.*/ 
TCHAR szAppName[APPNAMEBUFFERLEN] = "CAPOW"; //variable needed by SCRNAVE.LIB
/* THIS is the actual appname variable I'll use in the rest of my code.  Note
that this is also defined in CAPOW.CPP */
char *szMyAppName = "CAPOW"; 
/* These next two externals are defined in SCRNSAVE.LIB. I only use the 
hMainInstance, so comment out the hMainWindow.  */
extern HINSTANCE  hMainInstance;
//extern HWND  hMainWindow; //I keep my own HWND mainwind which I set in WM_CREATE.
	//=====MIXED SCREENSAVE AND EXECUTABLE FLAG====================
/* Here is a flag I use in CONFIGURE.CPP to decide whether that dialog's code
is for the dialog of the *.EXE or for the initializer of the *.SCR */
int buildtype = BUILD_SCR;
	//===TIMERS=================================================
/*update_timer is used to keep the program repeatedly updating itself
randomize_timer is used for the randomizing of the screensaver.  The randomize_timer
variables are declared in AUTORAND.CPP so that the *.EXE version of the program
can use them as well in an autorandomize mode which mimics the screensaver.
The timer ids UPDATE_TIMER_ID and RANDOMIZE_TIMER_ID are #defined in CA.HPP.
The ?_TIMER ids are passed to SetTimer by my setTimerCycle function,
and they are returned as the wParam with WM_TIMER messages so you know
which timer sent the signal. The ?_timer_handle integers are returned 
by SetTimer, and they are used only in the call to KillTimer to get rid
of a timer  */
int update_timer_handle = NULL;
int randomize_timer_handle = NULL;
int update_millisecs_per_cycle = 1; /*fire this often in milliseconds, lower than
	55 is the same as 55 due to granularity of system clock.  It doesn't matter
	if you send too many timer messages, the message queue only holds one from
	a given timer at once. Cahnge this to 1 for Win98, which has a faster
	system clock.  RR 2/16/98.*/

//===SCRENSAVER TWEAK Parameters=====================
//====================================
/*Tweak Parameters.  These are the parameers changed by my configure dialog.
They live in PROFILE.CPP so that the EXE version of the program can use them too.*/
extern int randomize_timer_cycle;
//default is in CA.HPP #define RANDOMIZE_TIMER_CYCLE_START 60000//60 seconds.
extern UINT fRandFlags; //Lives in profile.cpp, set to default there. Determines how to randomize in IDM_RANDOMIZE.
//  fRandFlags is made of a bunch of RF_??? bitflags defined in CA.HPP.  Default is RF_START.
//This is the name of the lable where I save szRandFlags in profile, defined in CONFIGURE.CPP
extern char szFlagsName[];
/* This is the standardname of the file I will look for to load my individual
rule if RF_FILE is on, this not a filename I change, it's just .\SCREENSAVER.CA,
it's defined in CONFIGURE.CPP.*/
extern char szScreenSaverFileShortName[]; //Defined in CONFIGURE.CPP
extern char szScreenSaverFileName[]; //Lives in CONFIGURE.CPP, we concatenate the CAPOW directory in front,
//and we get that from the Profile

//===CAPOW overhead variables======================================
int divider_width = 3; //Width of the gray line dividers, used in Bitmap.cpp
	//Note that I use 1 for this in the *.EXE.
CAlist *calife_list = NULL;
CapowGL *capowgl;
WindowBitmap *WBM;  // our memory bitmap
char CA_STYLE_NAME[256]; //Used in several places to get the current rule name.
//Dummy values for consistency with full CAPOW program.
#define ALL  0
#define FOCUS 1
short focusflag            = ALL; 
int  statusBarHeight = 0;  //Holds status bar height
int toolBarHeight = 0;
BOOL toolbarON = FALSE;
BOOL statusON = FALSE;
BOOL zoomviewflag = FALSE;
BOOL update_flag = 0;
char capowDirectory[256];
BOOL  not_seeded_yet_flag  = TRUE; //for loadsave.cpp
BOOL  load_save_cells_flag = FALSE;//for loadsave.cpp
HWND masterhwnd;
HINSTANCE hInst;
HWND hDlgOpenGL = NULL;
HWND hwndStatusBar = NULL;
HWND hDlgCycle = NULL;
HWND hDlgFourier = NULL;
HWND hUserDialog = NULL;
HWND hDlgConfigure = NULL; //

//========HELPER FUNCTIONS=======================
	//Generic functions I keep in AUTORAND.CPP for CAPOW.EXE and CAPOW.SCR
extern void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs);
extern void writeProfileInt(LPSTR szSection, LPSTR szKey, int save_int);
extern void readIniSettings(void);
extern void writeIniSettings(HWND);
	//Special for Capow
/*I use these two message handlers from the EXE version of the configure dialog,
found in CONFIGURE.CPP */
extern  int Config_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam);
extern  void Config_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//=====================CAPOW code==========================
//-------------------Continually running ca update procedure

void Cellmain(HWND hwnd) //This is the continually running thing.
{
	HDC hdc;
	HPALETTE old_hpal;
	static long GenCount;
	static char GenCountChar[10];	
	MSG msg;

	hdc = GetDC(hwnd);
	if (calife_list->numcolor() == 256)
	{
		old_hpal = WBM->WBMSelectPalette(hdc, calife_list->hpal());
		WBM->WBMRealizePalette(hdc);
	}
	calife_list->Update_and_Show(hdc);
	calife_list->DrawDivider(hdc);
	if (calife_list->numcolor() == 256)
		WBM->WBMSelectPalette(hdc, old_hpal);
	ReleaseDC(hwnd, hdc);
}

//====================CAPOW SCREENSAVER Code====================================

LONG FAR PASCAL ScreenSaverProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Use these three variables in WM_ERASEBKGND
	HDC hdc;
	RECT rcPaint;
	HPALETTE old_hpal;

	switch (message)
	{
		case WM_CREATE:
			readIniSettings();
			Randomize();  // Seed the randomizer
			hInst = hMainInstance; //Used in capow dialog files as global main HINSTANCE
			masterhwnd = hwnd; // masterhwnd = hMainWindow; doesn't work.
			//masterhwnd is used in capow dialog files as global main HWND
			WBM         = new WindowBitmap(hwnd);
			capowgl = new CapowGL(hwnd);
			calife_list = new CAlist(hwnd, MAX_CAS); //Calls CA:Allocate for members
			calife_list->SetWindowBitmap(WBM); //Registers			
			calife_list->Locate(); //Uses masterhwnd.
			capowgl->Size(hwnd);
			setTimerCycle(hwnd, update_timer_handle, UPDATE_TIMER_ID,
				update_millisecs_per_cycle);
			setTimerCycle(hwnd, randomize_timer_handle, RANDOMIZE_TIMER_ID,
				randomize_timer_cycle);
			//Do a randomize at start.  This is kludgy, we maybe change
			//so the calist constructor does the randomize.
			SendMessage(hwnd, WM_TIMER, RANDOMIZE_TIMER_ID, 0);
			calife_list->initizlize_blt_lines(2); //Make it look a little faster.
			GetProfileString(szMyAppName, "Directory", ".//", capowDirectory,
				256);
			strcpy(szScreenSaverFileName, capowDirectory);
			strcat(szScreenSaverFileName, "\\Files To Open\\");
			strcat(szScreenSaverFileName, szScreenSaverFileShortName);
#ifndef EXTERNAL_CA
			fRandFlags &= (~RF_FILE);
#else
			if (fRandFlags & RF_FILE) 
			{ //Try and load the file.
				if (!(calife_list->Loadall_Individual(szScreenSaverFileName)))
				{
					fRandFlags &= ~RF_FILE; /* If you can't load, turn flag
						 off and tell Profile not to try again next time. */
					writeProfileInt(szMyAppName, szFlagsName, fRandFlags);
				}
			}
#endif //EXTERNAL_CA
			return 0;

		case WM_ERASEBKGND:
	/*If you process a WM_PAINT message, nothing shows up on full screen,
	but you do see your graphics in the little preview screen.  If you
	process WM_ERASEBKGND, then both the preview and the full screen modes work.
	Here we are free to think of the window as being the full screen window. */
			hdc = (HDC)wParam; //Cleaner than GetDC(hwnd)
/*	GetClipBox(hdc, &rcPaint); seems to miss a few pixels at the edges, so I
	go ahead and get the size of the WBM, which is set by its constructor to
	the pixel size of the screen. */
			rcPaint.left = rcPaint.top = 0;
			rcPaint.right = WBM->CX();
			rcPaint.bottom = WBM->CY();
			if (calife_list->numcolor() == 256)
			{
				old_hpal = WBM->WBMSelectPalette(hdc, calife_list->hpal());
				WBM->WBMRealizePalette(hdc);
			}
			calife_list->DrawDivider(hdc);
			calife_list->Show(hdc, rcPaint); //Just gets the rcPaint RECT from ps.
			if (calife_list->numcolor() == 256)
				WBM->WBMSelectPalette(hdc, old_hpal);
			return 0;

		case WM_SIZE:
		/* This gets called only once, at the startup */
			calife_list->Locate(); //Uses calife_list.hwnd
			SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L); 
				//Draws our dividers on top of the bitmap.
			calife_list->FourierSeed();
			capowgl->Size(hwnd); //For use by a possible 3D view
			return 0;
		case WM_TIMER:
		
			if (!calife_list)//Prevents doing update before WM_CREATE or after WM_DESTROY
				return 0;
			if (wParam == UPDATE_TIMER_ID)
				Cellmain(hwnd);
			else //RANDOMIZE_TIMER_ID
			{
				calife_list->Randomize(fRandFlags);
				SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
			}
			return 0;
		case WM_COMMAND:
			switch(wParam)
			{
				case IDM_CLEAR:	// Clears all CAs
					WBM->Clear(hwnd, RGB(0,0,0));
					hdc = GetDC(hwnd);
					calife_list->DrawDivider(hdc);
					ReleaseDC(hwnd,hdc);
					break;
			}
			return 0;
		case WM_DESTROY:
			if (update_timer_handle)
				KillTimer(hwnd, update_timer_handle);
			if (randomize_timer_handle)
				KillTimer(hwnd, randomize_timer_handle);
			delete WBM;// Free bitmap
			delete capowgl;//delete graph;
			delete calife_list;//Calls dll_list destructor.  Important to call FreeLibrary on DLLS.
			calife_list = NULL; //So any remaining WM_TIMER knows its over.
			PostQuitMessage (0);
			return 0;
	}
	return DefScreenSaverProc (hwnd, message, wParam, lParam) ;
}

/*-------------------------------- */
/* RegisterDialogClasses -- required Entry point for registering window   */
/*  classes required by configuration dialog box.                      */
/* ------------------------------------------------------------------- */
extern "C" BOOL WINAPI RegisterDialogClasses (HANDLE hInst)
{ //We don't use this for anything more than this required minimum.
	return TRUE;
}

/* ------------------------------------------------------------------- */
/* ScreenSaverConfigureDialog -- Dialog box function for configuration */
/*  dialog.                                                            */
/* ------------------------------------------------------------------- */

BOOL FAR PASCAL ScreenSaverConfigureDialog(HWND hDlg, UINT msg,
                                           WPARAM wParam, LPARAM lParam)
{
	static BOOL bHelpActive = FALSE;
	switch (msg)
	{
		case WM_INITDIALOG:
/* We read our two variables, fRandFlags and randomzier_timer_cycle,  out of
the WIN.INI.  Then we need to use these values to set the radio buttons and
the edit box of our dialog.  We could use the same code which appears in our
CONFIGURE.CPP version of the dialog, which does this same thing.  Except we may
want to block RF_FILE, so we do a jiggly-do for that */
			//First of all, read fRandFlags and randomize_timer_cycle out of WIN.INI profile.
			readIniSettings(); 
#ifndef EXTERNAL_CA
			fRandFlags &= (~RF_FILE);
			writeProfileInt(szMyAppName, szFlagsName, fRandFlags);
#endif //EXTERNAL_CA
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, Config_INITDIALOG);
		case WM_COMMAND:
			switch(wParam)
			{
/* My modeless EXE Configure dialog code uses the standard IDOK identifier to
notice when I press Enter in an edit box.  But my SCR dialog is a modal dialog
with a default button, which happens to be the OK button, and when I press
Enter it is like I pressed the default button.  So it makes sense to give this
button the IDOK ID as well.  But now I won't hit the IDOK edit box processing code
that is in the modeless EXE dialog, so I put that code into the writeIniSettings. */
				case IDOK:
					writeIniSettings(hDlg);
					if (bHelpActive)
						WinHelp(hDlg, "\\Capow98\\Capow.hlp", HELP_QUIT, 0);
					EndDialog(hDlg, TRUE);
					return TRUE;
				case ID_CANCEL:
					SendMessage(hDlg, WM_CLOSE, 0, 0);
					return TRUE;
				case ID_LOCALHELP:
					bHelpActive = WinHelp(hDlg, "\\Capow98\\Capow.hlp", HELP_FINDER, 0);
					return TRUE;
/* Our trick here is to use an autorandomize dialog box from our *.EXE to
process most of the messages.  Our shared dialog code is in CONFIG.CPP.
We don't have exactly the same handling because the dialog box 
in our *.EXE is modelss and a screensaver dialog is modal, also the EXE dialog
doesn't do the loading and saving of profile strings (though maybe it should).
We have a buildtype variable that is set to BUILD_SCR or BUILD_EXE which is
used in one minor spot in CONFIGURE.CPP to switch between screensaver dialog
handling and regualar EXE dialog handling. */
				default:
					return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, Config_COMMAND);
			}
			return FALSE;
		case WM_CLOSE:
			if (bHelpActive)
				WinHelp(hDlg, "\\Capow98\\Capow.hlp", HELP_QUIT, 0);
			EndDialog(hDlg, TRUE);
			return TRUE;
		default:
			return FALSE;
	}
}



