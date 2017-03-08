#include "ca.hpp"
#include "resource.h"
#include "userpara.hpp"

extern char *szMyAppName;
extern HWND hDlgWorld, hDlgLookup, hDlgOpenGL, masterhwnd;
extern CAlist *calife_list;
extern BOOL update_flag;

extern void ShowOpenGLParams(HWND hDlg);   //so that the 3Dview dlg will update upon a ca type change
extern void label(HWND, int, char *, short);
static void showparams(HWND);
extern short focusflag;
	
BOOL done_picking_new_type = FALSE; /*a kludge to try and keep
from processing type selections twice */

unsigned char world_drop;
	  /*	world_drop = 0 means the catype combo box is up, 1 means down
/* We need to match the type_ca numbers with the combo box order number*/
#define TYPES_COUNT	18 //Make sure this number is correct!
static int boxindex_to_type[TYPES_COUNT];
static int userboxindex;


static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	HWND hCntl;
	int boxindex = 0; // l.andrews 11/3/01 just so it has a value
    
//	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
	
	// Set CA type Combo Box
	hCntl = GetDlgItem( hDlg, IDC_CATYPE_WORLD);

	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, CA_STANDARD,
		(LONG)"Standard");
	boxindex_to_type[boxindex] = CA_STANDARD;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Reversible");
	boxindex_to_type[boxindex] = CA_REVERSIBLE;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Driven Heat");
	boxindex_to_type[boxindex] = CA_HEATWAVE;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Big Nabe Heat");
	boxindex_to_type[boxindex] = CA_HEATWAVE2;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Wave Equation");
	boxindex_to_type[boxindex] = ALT_CA_WAVE;
/*	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Viscous Wave");
	boxindex_to_type[boxindex] = CA_WAVE; */
  	userboxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
	 	(LONG)"User Rule (*.DLL)");
	boxindex_to_type[userboxindex] = CA_USER;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Big Nabe Wave");
	boxindex_to_type[boxindex] = CA_WAVE2;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Oscillators");
	boxindex_to_type[boxindex] = CA_OSCILLATOR;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Diverse Osc");
	boxindex_to_type[boxindex] = CA_DIVERSE_OSCILLATOR;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Wave Osc");
	boxindex_to_type[boxindex] = ALT_CA_OSCILLATOR_WAVE;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Diverse Wave Osc");
	boxindex_to_type[boxindex] = ALT_CA_DIVERSE_OSCILLATOR_WAVE;
/*	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Wave Osc");
	boxindex_to_type[boxindex] = CA_OSCILLATOR_WAVE;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Diverse Wave Osc");
	boxindex_to_type[boxindex] = CA_DIVERSE_OSCILLATOR_WAVE;
*/
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Quadratic Wave");
	boxindex_to_type[boxindex] = ALT_CA_ULAM_WAVE;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Cubic Wave");
	boxindex_to_type[boxindex] = CA_CUBIC_ULAM_WAVE;
/*	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Viscous FPU");
	boxindex_to_type[boxindex] = CA_ULAM_WAVE;*/
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Boiling Wave");
	boxindex_to_type[boxindex] = CA_AUTO_ULAM_WAVE;
/*	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Alt Wave 2");
	boxindex_to_type[boxindex] = ALT_CA_WAVE2;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Alt Oscillators");
	boxindex_to_type[boxindex] = ALT_CA_OSCILLATOR;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"Alt Diverse Osc");
	boxindex_to_type[boxindex] = ALT_CA_DIVERSE_OSCILLATOR;
*/
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"2D Wave");
	boxindex_to_type[boxindex] = CA_WAVE_2D;
	boxindex = (int)SendMessage( hCntl, CB_ADDSTRING, 0,
		(LONG)"2D Heat");
	boxindex_to_type[boxindex] = CA_HEAT_2D;
//JY ------- 10/12
	showparams(hDlg);
	return 0; //if successful
}



static void MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	//((fn)((hDlg), (int)(wParam), (HWND)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
	int comboint;
	
	switch (id)
	{
    	case RADIO_ALL:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEALLMENU, 0L);
			break;

		case RADIO_FOCUS:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEFOCUSMENU, 0L);
			break;

		case SC_UPDATE:
//			CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
			showparams(hDlg);
			break;

		case IDC_RAND_COLOR:
			if (focusflag)
			{
				calife_list->FocusCA()->SetColors();
				calife_list->FocusCA()->ResetGenerationCount();
	 		}
			else
			{
				calife_list->SetColors();
				calife_list->ResetAllGenerationCount();
			}
			update_flag = 1;
			break;

		case IDC_RAND_LOOKUP:
			if (focusflag)
			{
				calife_list->FocusCA()->Lambdalookup();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Lambdalookup();
				calife_list->ResetAllGenerationCount();
			}
			update_flag = 1;
					/* Redraws lookup dialog menu */
			break;

		case IDC_ONESEED:
			if (focusflag)
			{
				calife_list->FocusCA()->Oneseed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Oneseed();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case IDC_ZEROSEED:
			if (focusflag)
			{
				calife_list->FocusCA()->Zeroseed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Zeroseed();
				calife_list->ResetAllGenerationCount();
			}
			break;
		case IDC_SEED_HALFMAX:
			if (focusflag)
			{
				calife_list->FocusCA()->Halfmaxseed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Halfmaxseed();
				calife_list->ResetAllGenerationCount();
			}
			break;
		case IDC_SEED_CAPOW:
			if (focusflag)
			{
				calife_list->FocusCA()->BitmapSeed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->BitmapSeed();
				calife_list->ResetAllGenerationCount();
			}
			break;
		case IDC_RANDOMTOUCH:
			if (focusflag)
			{
				calife_list->FocusCA()->RandomTouch_CA();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->RandomTouch_CA();
				calife_list->ResetAllGenerationCount();
			}
			break;
		case IDC_SMOOTH:
			if (focusflag)
			{
				calife_list->FocusCA()->One_smoothsteps();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->One_smoothsteps();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case IDC_SINESEED:
			if (focusflag)
			{
				calife_list->FocusCA()->Sineseed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Sineseed();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case IDC_RANDOMSEED:
			if (focusflag)
			{
				calife_list->FocusCA()->RandomSeed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->RandomSeed();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case IDC_FOURIERSEED:
			if (focusflag)
			{
				calife_list->FocusCA()->FourierSeed();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->FourierSeed();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case IDC_LOAD_USER_RULE:
			if (!calife_list->LoadUserRule(masterhwnd, focusflag))
					break;

			recreateUserDialog();
			if (focusflag)
				calife_list->SetCAType(calife_list->FocusCA(), CA_USER,TRUE);
			else // Second argument says adjust for the rule to be stable.
				calife_list->SetAllType(CA_USER, TRUE);
			if (hDlgOpenGL)
				InvalidateRect(hDlgOpenGL, NULL, TRUE);
			break;		
		case IDC_CATYPE_WORLD:
/* This code keeps you from getting a change type command when you
open up the list box */
			if (SendMessage( hwndCtl, CB_GETDROPPEDSTATE, 0, 0L)
				&& world_drop == 0)
			{
				world_drop = 1;
				break;
			}
			if (SendMessage( hwndCtl, CB_GETDROPPEDSTATE, 0, 0L)
				|| world_drop == 0)
			break;
/* Hopefully we only hit this, and hit it ONCE, when we select something
from the combo box */
		world_drop = 0; //You just closed the combo box.
			/* convert to ca.hpp's definition on standard .. wave */
		comboint = boxindex_to_type[(int)SendMessage( hwndCtl, CB_GETCURSEL, 0, 0L)];
/* For some reason this line was getting hit three times when I select a
	new type CA_USER*/
		if (comboint == CA_USER)
		{	// Care must be taken becuase user can't change focus or all
// ******************** // If LoadUserRule, it will handle it after user selected the
						// file name
			if (!calife_list->LoadUserRule(masterhwnd, focusflag))
					break;
// ********************
			recreateUserDialog();
			if (focusflag)
				calife_list->SetCAType(calife_list->FocusCA(), comboint,TRUE);
			else // Second argument says adjust for the rule to be stable.
				calife_list->SetAllType(comboint, TRUE);
		}
		else
		{
			if (focusflag)
			{	removeUserParam(calife_list->FocusCA(), FALSE);
				recreateUserDialog();
				calife_list->SetCAType(calife_list->FocusCA(), comboint, TRUE);
			}
			else
			{
				for(int i = 0; i < calife_list->Count(); i++)
					removeUserParam(calife_list->GetCA(i), FALSE);
				recreateUserDialog();
				calife_list->SetAllType(comboint, TRUE);
			}
		}

		update_flag = 1;
		/* Redraws lookup dialog menu */
		SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		break;

		case IDC_AUTOSMOOTH:
			if (focusflag)
				if (calife_list->FocusCA()->Get_smoothflag())
					calife_list->FocusCA()->Set_smoothflag(FALSE);
				else
					calife_list->FocusCA()->Set_smoothflag(TRUE);
			else
				if (calife_list->FocusCA()->Get_smoothflag())
					calife_list->Set_smoothflag(FALSE);
				else
					calife_list->Set_smoothflag(TRUE);
					// Autosmooth check box
			showparams(hDlg);
			break;

		case IDC_ZERO: //WF_ZERO
		case IDC_FIXED: //WF_FIXED
		case IDC_WRAP: //WF_WRAP
		case IDC_FREE: //WF_FREE
		case IDC_ABSORB: //WF_ABSORB
			if (focusflag)
			{
				calife_list->FocusCA()->Setwrapflag(WF_ZERO + id - IDC_ZERO);
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Setwrapflag(WF_ZERO + id - IDC_ZERO);
				calife_list->ResetAllGenerationCount();
			}
			showparams(hDlg);
			break;

		case IDC_GENERATOR:
			if (focusflag)
			{
				if (calife_list->FocusCA()->Getgeneratorflag())
					calife_list->FocusCA()->Setgeneratorflag(0);
				else
					calife_list->FocusCA()->Setgeneratorflag(1);
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				if (calife_list->FocusCA()->Getgeneratorflag())
					calife_list->Setgeneratorflag(0);
				else
					calife_list->Setgeneratorflag(1);
				calife_list->ResetAllGenerationCount();
			}	// Generator check box
			showparams(hDlg);
			break;

	}
			
}


static void MyWnd_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"WORLDX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"WORLDY",(LPSTR)buf);
			
}

static void MyWnd_RBUTTONDOWN(HWND hDlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	//((fn)((hDlg), FALSE, (int)LOWORD(lParam), (int)HIWORD(lParam), (UINT)(wParam)), 0L)
			DestroyWindow(hDlg);
			
}


static void MyWnd_CLOSE(HWND hDlg)
{
	 //((fn)(hDlg), 0L)
			DestroyWindow(hDlg);

}


static void MyWnd_DESTROY(HWND hDlg)
{
	 //((fn)(hDlg), 0L)
			CheckMenuItem( GetMenu(masterhwnd),IDM_WORLD,MF_BYCOMMAND|MF_UNCHECKED);
			hDlgWorld = 0;
			InvalidateRect(masterhwnd,NULL,FALSE);
}

BOOL CALLBACK WorldProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_RBUTTONDOWN:
			return (BOOL) !HANDLE_WM_RBUTTONDOWN(hDlg, wParam, lParam, MyWnd_RBUTTONDOWN);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, MyWnd_MOVE);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
}

static void showparams(HWND hDlg)
{
	int  boxindex = 0;  // l.andrews  11/3/01  just so it has a value
	for (int i=0; i< TYPES_COUNT; i++)
	{
		if (boxindex_to_type[i] == calife_list->FocusCA()->Gettype())
		{
			boxindex = i;
			break;
		}
	}

// set parameters and their buttons to inactive/active state
//Most we set simply on the basis of the type
//But we set ACC_??? on the basis of a flag in t//focus/all

	CheckRadioButton(hDlg,RADIO_ALL,RADIO_FOCUS,RADIO_ALL+focusflag);
 //	setworldtates (currcatype);

// CA type combo box
	if (!SendMessage( GetDlgItem( hDlg, IDC_CATYPE_WORLD), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_CATYPE_WORLD),
			CB_SETCURSEL, (WORD)boxindex, 0);

// Autosmooth check box
	if (calife_list->FocusCA()->Get_smoothflag())
		CheckDlgButton(hDlg,IDC_AUTOSMOOTH,1);
	else
		CheckDlgButton(hDlg,IDC_AUTOSMOOTH,0);

// Wrap radio buttons
		CheckRadioButton(hDlg, IDC_ZERO, IDC_ABSORB,
			IDC_ZERO + calife_list->FocusCA()->Getwrapflag() - WF_ZERO);

// Generator check box
	if (calife_list->FocusCA()->Getgeneratorflag())
		CheckDlgButton(hDlg,IDC_GENERATOR,1);
	else
		CheckDlgButton(hDlg,IDC_GENERATOR,0);
}





