#include <stdio.h>
#include <stdlib.h>
#include "ca.hpp"
#include "resource.h" //resources shared with CAPOW

//This is the name of the file I will look for to load my individual rule.
char szScreenSaverFileShortName[] = "SCREENSAVER.CA";
char szScreenSaverFileName[256];

/* Here is a flag I use in CONFIGURE.CPP to decide whether that dialog's code
is for the dialog of the *.EXE or for the initializer of the *.SCR It can
be either BUILD_EXE or BUILD_SCR*/
extern int buildtype;


#define MAX_CHARS 26
extern char *szMyAppName;
extern HWND hDlgConfigure, masterhwnd;
extern CAlist *calife_list;
extern BOOL update_flag;

static void showparams(HWND);
extern short focusflag;
static char numberstring[MAX_CHARS];
static char szBuffer[32];

//From AUTORAND.CPP
extern UINT fRandFlags; 
//  fRandFlags is made of a bunch of RF_??? bitflags defined in CA.HPP.  Default is RF_START.
extern int randomize_timer_cycle; //
extern void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs);
extern void writeIniSettings(HWND hDlg);
extern void readIniSettings();
//From CAPOW.CPP or CASCREEN.CPP
extern int randomize_timer_handle;


int Config_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
//First of all, read fRandFlags and randomize_timer_cycle out of WIN.INI profile.
	readIniSettings(); 
	sprintf (szBuffer, "%d", (randomize_timer_cycle / 1000));
/* Don't do a SetDlgItemText in showparams because it generates WM_COMMAND
messages that can lead to endless regress */
	SetDlgItemText(hDlg, IDC_TIMER, (LPSTR)szBuffer);
	showparams(hDlg);
	return 0; //if successful
}

 void Config_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	//((fn)((hDlg), (int)(wParam), (HWND)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
	int comboint;
	int i, j;
	int time, old_randomize_timer_cycle;

	switch (id)
	{
		case IDC_TIMER:
/* Here we enter the edit-box editing mode.  You can leave this
mode either by clicking outside the edit box, which sends an id of IDC_TIMER
with a codeNotify of EN_KILLFOCUS.  In either case we save the changes. 
For future reference, here are the EN code names with their numerical values
(from the windows header file) along with the times I see them happening in the
debugger.  Note that I get two EN_ when the dialog opens and I get no EN_ when
it closes:
#define EN_SETFOCUS   0x0100 //At click on edit box
#define EN_KILLFOCUS  0x0200 //When you click another control
#define EN_CHANGE     0x0300 //Second at startup, and whenever you type something.
#define EN_UPDATE     0x0400 //First at dialog startup
*/
// if I get a EN_KILLFOCUS, I mimic an Enter, which is IDOK.
			if (codeNotify != EN_KILLFOCUS)
				break;
/*else if codeNotify is EN_KILLFOCUS just drop through to the
IDOK case.  This is not a general solution adn wouldn't work with multiple
edit controls.  In that case I would need to put the code frome case IDOK inside
a updateeditparms or something.  I can't just send an IDOK message becuase in
the screensaver version of the dialog, that gets processed differently */
		case IDOK: /*I should get this when I press ENTER.  For a long time,
	I was not!  Then I found the trick is that we had set the edit box
	Properies|Style|Align Text to Right instead of Left.  If it is set to
	Right, then you never get the Enter, as you will keep scrolling!
	Rudy, 12/3/97.  If I had more than one edit control it would be simplest
	to just update	them all or I could switch on edit_id.*/
			GetWindowText(GetDlgItem(hDlg, IDC_TIMER),
				numberstring, MAX_CHARS);
			time = atoi(numberstring);
			CLAMP(time, 5, 300);
			old_randomize_timer_cycle = randomize_timer_cycle;
			randomize_timer_cycle = time * 1000;
			sprintf (szBuffer, "%d", (randomize_timer_cycle / 1000));
			SetDlgItemText(hDlg, IDC_TIMER, (LPSTR)szBuffer);
			setTimerCycle(masterhwnd, randomize_timer_handle, RANDOMIZE_TIMER_ID,
				randomize_timer_cycle);
/* If you made the time shorter, hit it with a randomize right now, but if you
are asking for more time, leave it alone. BUT you only do this in the BUILD_EXE
case; you don't want to do it in the BUILD_SCR case, because in that case there
is no calife_list in the background; the BUILD_SCR case is running this dialog as
a standalone modal without any CAPOW happening in the background.*/
			if (buildtype == BUILD_EXE)
			{
				if (randomize_timer_cycle < old_randomize_timer_cycle)
					calife_list->Randomize(fRandFlags); 
			}
			showparams(hDlg); 
			break;
		case IDC_ALLOW_1D_RULES:
			fRandFlags &= (~(RF_2D | RF_BOTHD));
			fRandFlags |=  RF_1D;
			showparams(hDlg); 
			break;
		case IDC_ALLOW_2D_RULES:
			fRandFlags &= (~(RF_1D | RF_BOTHD));
			fRandFlags |=  RF_2D;
			showparams(hDlg); 
			break;
		case IDC_ALLOW_BOTH_RULES:
			fRandFlags &= (~(RF_1D | RF_2D));
			fRandFlags |=  RF_BOTHD;
			showparams(hDlg); 
			break;
		case IDC_DIGITAL:
			fRandFlags &= (~(RF_BOTHVAL | RF_ANALOGVAL));
			fRandFlags |=  RF_DIGITALVAL;
			showparams(hDlg); 
			break;
		case IDC_ANALOG:
			fRandFlags &= (~(RF_BOTHVAL | RF_DIGITALVAL));
			fRandFlags |=  RF_ANALOGVAL;
			showparams(hDlg); 
			break;
		case IDC_DIGITALANALOG:
			fRandFlags &= (~(RF_ANALOGVAL | RF_DIGITALVAL));
			fRandFlags |=  RF_BOTHVAL;
			showparams(hDlg); 
			break;
		case IDC_SPLIT:
			fRandFlags &= (~(RF_BOTHVW | RF_SCROLLVW | RF_ALLVW));
			fRandFlags |=  RF_SPLITVW;
			showparams(hDlg); 
			break;
		case IDC_SCROLL:
			fRandFlags &= (~(RF_SPLITVW | RF_BOTHVW | RF_ALLVW));
			fRandFlags |=  RF_SCROLLVW;
			showparams(hDlg); 
			break;
		case IDC_SPLITSCROLL:
			fRandFlags &= (~(RF_SPLITVW | RF_SCROLLVW | RF_ALLVW));
			fRandFlags |=  RF_BOTHVW;
			showparams(hDlg); 
			break;
		case IDC_ALLVIEW:
			fRandFlags &= (~(RF_SPLITVW | RF_SCROLLVW | RF_BOTHVW));
			fRandFlags |=  RF_ALLVW;
			showparams(hDlg); 
			break;
		case IDC_PROFILE_FILE:
			fRandFlags ^= RF_FILE;
			if ((fRandFlags & RF_FILE) && (buildtype == BUILD_EXE)) //You just turned it on.
			{ //Try and load the file.
				if (!(calife_list->Loadall_Individual(szScreenSaverFileName)))
					fRandFlags &= (~RF_FILE); //If you can't, turn flag off.
			}
			showparams(hDlg); 
			break;
		case IDC_ONE_CA:
			fRandFlags |= RF_COUNT1;
			fRandFlags &= (~(RF_COUNT4 | RF_COUNT9));
			showparams(hDlg);
			break;
		case IDC_FOUR_CA:
			fRandFlags |= RF_COUNT4;
			fRandFlags &= (~(RF_COUNT1 | RF_COUNT9));
			showparams(hDlg);
			break;
		case IDC_NINE_CA:
			fRandFlags |= RF_COUNT9;
			fRandFlags &= (~(RF_COUNT1 | RF_COUNT4));
			showparams(hDlg);
			break;
		case IDC_VARYCOUNT:
			fRandFlags |= (RF_COUNT1 | RF_COUNT4 | RF_COUNT9);
			showparams(hDlg);
			break;
		case IDC_ALLOW_3D_VIEW:
			fRandFlags |= RF_3D;
			fRandFlags &= (~(RF_3DFLAT | RF_3DBOTH));
			showparams(hDlg);
			break;
		case IDC_FLAT_3D_VIEW:
			fRandFlags |= RF_3DFLAT;
			fRandFlags &= (~(RF_3D | RF_3DBOTH));
			showparams(hDlg);
			break;
		case IDC_BOTH_3D_VIEW:
			fRandFlags |= RF_3DBOTH;
			fRandFlags &= (~(RF_3DFLAT | RF_3D));
			showparams(hDlg);
			break;
		case IDC_ALLOW_RANDOM_COLOR:
			fRandFlags ^= RF_COLOR;
			showparams(hDlg);
			break;
		case IDC_DING:
			fRandFlags ^= RF_DING;
			showparams(hDlg);
			break;
		case IDC_RANDOMIZE:
			if (fRandFlags & RF_FILE) 
			{ //Try and load the file.
				if (!(calife_list->Loadall_Individual(szScreenSaverFileName)))
					fRandFlags &= ~RF_FILE; //If you can't, turn flag off.
			}
			calife_list->Randomize(fRandFlags);
			break;
		case IDC_AUTORANDOMIZE_ON:
			if (randomize_timer_handle) //already on
				break;
			if (fRandFlags & RF_FILE) 
			{ //Try and load the file.
				if (!(calife_list->Loadall_Individual(szScreenSaverFileName)))
					fRandFlags &= ~RF_FILE; //If you can't, turn flag off.
			}
			calife_list->Randomize(fRandFlags); //Hit it once even before the timer.
			setTimerCycle(masterhwnd, randomize_timer_handle, RANDOMIZE_TIMER_ID,
				randomize_timer_cycle);
			showparams(hDlg);
			break;
		case IDC_AUTORANDOMIZE_OFF:
			if (!randomize_timer_handle) //already off
				break;
			KillTimer(masterhwnd, randomize_timer_handle);
			randomize_timer_handle = NULL;
			showparams(hDlg);
			break;
		case IDC_RESET:
			randomize_timer_cycle =  RANDOMIZE_TIMER_CYCLE_START;
			fRandFlags =  RF_START;
			showparams(hDlg);
			break;
		case IDC_PROFILE_SAVE:
			writeIniSettings(hDlg);
			break;
		default:
			showparams(hDlg);
			break;
	}
}


static void Config_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"CONFIGUREX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"CONFIGUREY",(LPSTR)buf);
			
}


static void Config_CLOSE(HWND hDlg)
{
	 //((fn)(hDlg), 0L)
			DestroyWindow(hDlg);

}


static void Config_DESTROY(HWND hDlg)
{
	 //((fn)(hDlg), 0L)
			CheckMenuItem( GetMenu(masterhwnd),IDM_CONFIGURE,MF_BYCOMMAND|MF_UNCHECKED);
			hDlgConfigure = 0;
			InvalidateRect(masterhwnd,NULL,FALSE);
}

BOOL CALLBACK ConfigureProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, Config_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, Config_COMMAND);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, Config_MOVE);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, Config_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, Config_DESTROY);
		default:
			return FALSE;
	}
}

static void showparams(HWND hDlg)
{

/* First of all we do a bit of selection valication.  If you turned off ALL of  
some possibilities, we make you take default*/
	if (!(fRandFlags & (RF_COUNT1 | RF_COUNT4 | RF_COUNT9)))
		fRandFlags |= RF_COUNT9;
	if (!(fRandFlags & (RF_1D | RF_2D | RF_BOTHD)))
		fRandFlags |= RF_BOTHD;
	if (!(fRandFlags & (RF_DIGITALVAL | RF_ANALOGVAL | RF_BOTHVAL)))
		fRandFlags |= RF_BOTHVAL;
	if (!(fRandFlags & (RF_SPLITVW | RF_SCROLLVW | RF_BOTHVW)))
		fRandFlags |= RF_BOTHVW;

/* Need to set the radio buttons one by one because we don't keep track of
the active button because the info is stored as a bit field.*/

	SendMessage(GetDlgItem(hDlg, IDC_ONE_CA), BM_SETCHECK,
		((fRandFlags & RF_COUNT1)&&(!(fRandFlags & RF_COUNT4)))?
		BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FOUR_CA), BM_SETCHECK,
		((fRandFlags & RF_COUNT4)&&(!(fRandFlags & RF_COUNT1)))?
		BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_NINE_CA), BM_SETCHECK,
		((fRandFlags & RF_COUNT9)&&(!(fRandFlags & RF_COUNT4)))?
		BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_VARYCOUNT), BM_SETCHECK,
		((fRandFlags & RF_COUNT1)&&(fRandFlags & RF_COUNT4))?
		BST_CHECKED:BST_UNCHECKED, 0);


	SendMessage(GetDlgItem(hDlg, IDC_ALLOW_1D_RULES), BM_SETCHECK,
		(fRandFlags & RF_1D)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_ALLOW_2D_RULES), BM_SETCHECK,
		(fRandFlags & RF_2D)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_ALLOW_BOTH_RULES), BM_SETCHECK,
		(fRandFlags & RF_BOTHD)?BST_CHECKED:BST_UNCHECKED, 0);

	SendMessage(GetDlgItem(hDlg, IDC_DIGITAL), BM_SETCHECK,
		(fRandFlags & RF_DIGITALVAL)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_ANALOG), BM_SETCHECK,
		(fRandFlags & RF_ANALOGVAL)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_DIGITALANALOG), BM_SETCHECK,
		(fRandFlags & RF_BOTHVAL)?BST_CHECKED:BST_UNCHECKED, 0);

	SendMessage(GetDlgItem(hDlg, IDC_SPLIT), BM_SETCHECK,
		(fRandFlags & RF_SPLITVW)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_SCROLL), BM_SETCHECK,
		(fRandFlags & RF_SCROLLVW)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_SPLITSCROLL), BM_SETCHECK,
		(fRandFlags & RF_BOTHVW)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_ALLVIEW), BM_SETCHECK,
		(fRandFlags & RF_ALLVW)?BST_CHECKED:BST_UNCHECKED, 0);

	SendMessage(GetDlgItem(hDlg, IDC_ALLOW_3D_VIEW), BM_SETCHECK,
		(fRandFlags & RF_3D)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FLAT_3D_VIEW), BM_SETCHECK,
		(fRandFlags & RF_3DFLAT)?BST_CHECKED:BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg, IDC_BOTH_3D_VIEW), BM_SETCHECK,
		(fRandFlags & RF_3DBOTH)?BST_CHECKED:BST_UNCHECKED, 0);

	CheckRadioButton(hDlg, IDC_AUTORANDOMIZE_OFF, IDC_AUTORANDOMIZE_ON, IDC_AUTORANDOMIZE_OFF +
		(randomize_timer_handle?1:0));	
	

	if((fRandFlags & RF_FILE)||((fRandFlags & RF_COUNT1) && (fRandFlags & ( RF_2D | RF_BOTHD))))
	{
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_3D_VIEW), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_FLAT_3D_VIEW), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_BOTH_3D_VIEW), TRUE );
	}
	else
	{
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_3D_VIEW), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_FLAT_3D_VIEW), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_BOTH_3D_VIEW), FALSE );
	}

	if(fRandFlags & (RF_1D | RF_BOTHD))
	{
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITAL), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ANALOG), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITALANALOG), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_SPLIT), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_SCROLL), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_SPLITSCROLL), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLVIEW), TRUE );
	}
	else
	{
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITAL), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ANALOG), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITALANALOG), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_SPLIT), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_SCROLL), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_SPLITSCROLL), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLVIEW), FALSE );
	}

	if(fRandFlags & RF_COLOR)
		CheckDlgButton(hDlg, IDC_ALLOW_RANDOM_COLOR, MF_CHECKED);
	else
		CheckDlgButton(hDlg, IDC_ALLOW_RANDOM_COLOR, MF_UNCHECKED);

	if(fRandFlags & RF_DING)
		CheckDlgButton(hDlg, IDC_DING, MF_CHECKED);
	else
		CheckDlgButton(hDlg, IDC_DING, MF_UNCHECKED);

	if(fRandFlags & RF_FILE)
		CheckDlgButton(hDlg, IDC_PROFILE_FILE, MF_CHECKED);
	else
		CheckDlgButton(hDlg, IDC_PROFILE_FILE, MF_UNCHECKED);

	if(!(fRandFlags & RF_FILE))
	{ //Normal non-file randomize situation
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITAL), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ANALOG), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITALANALOG), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_1D_RULES), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_2D_RULES), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_BOTH_RULES), TRUE );
	}
	else //Using file randomize, i.e. CA rules are fixed by a file.
	{
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITAL), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ANALOG), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_DIGITALANALOG), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_1D_RULES), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_2D_RULES), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_ALLOW_BOTH_RULES), FALSE );
	}

/* In the Screensaver case I am not IN the execution mode, so I won't have
the randomize_timer_handle, but I want to be able to edit the time anyway. */
	if(randomize_timer_handle || buildtype == BUILD_SCR)
		EnableWindow( GetDlgItem( hDlg, IDC_TIMER), TRUE );
	else
		EnableWindow( GetDlgItem( hDlg, IDC_TIMER ), FALSE );

}

