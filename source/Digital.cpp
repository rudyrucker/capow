#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include <math.h> // for atof in 16 bit.  32 bit version doesn't need it.

extern char *szMyAppName;
extern HWND hDlgDigital, masterhwnd;
extern class CAlist *calife_list;
extern BOOL update_flag;
extern void textLabel (HWND, int, char*);
extern void realLabel (HWND, int, Real);

static void showparams(HWND);
static void set_active_buttons(int);
static void enable_active_buttons(HWND);

static int edit_id = 0;
extern short focusflag;	
	// 0 is all, 1 is one only

#define MAX_PLUSES_MINUSES 4		// number of incrementors and decrementors
#define BUTT_START         101	// starting number of the push buttons of inc/dec
#define MAX_STR_SIZE 20

static int active_buttons[MAX_PLUSES_MINUSES] = { 1, 1, 1, 1 };
	// all are active

unsigned char new_drop;	// 0 means the combo box is up, 1 means down

#define MAX_TYPES 12
int NEW_RADIUS[ MAX_TYPES ] = { 1, 1, 1,  1, 2, 2, 3, 3, 4, 5, 6,   1 };
int NEW_STATES[ MAX_TYPES ] = { 2, 4, 8, 16, 2, 4, 2, 4, 2, 2, 2, 256 };

/*----------------------------------------------------------------------------------------*/
// 							Messager Cracker

static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	HINSTANCE hInstance;
	HBITMAP Bitmap;

	hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);

	HWND hCntl;
//			CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
			hCntl = GetDlgItem( hDlg, RADIUSSTATES_NEW );
			SendMessage( hCntl, CB_ADDSTRING, CA1_2,  (LONG)"Radius-1 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA1_4,  (LONG)"Radius-1 States-4" );
			SendMessage( hCntl, CB_ADDSTRING, CA1_8,  (LONG)"Radius-1 States-8" );
			SendMessage( hCntl, CB_ADDSTRING, CA1_16, (LONG)"Radius-1 States-16" );
			SendMessage( hCntl, CB_ADDSTRING, CA2_2,  (LONG)"Radius-2 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA2_4,  (LONG)"Radius-2 States-4" );
			SendMessage( hCntl, CB_ADDSTRING, CA3_2,  (LONG)"Radius-3 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA3_4,  (LONG)"Radius-3 States-4" );
			SendMessage( hCntl, CB_ADDSTRING, CA4_2,  (LONG)"Radius-4 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA5_2,  (LONG)"Radius-5 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA6_2,  (LONG)"Radius-6 States-2" );
			SendMessage( hCntl, CB_ADDSTRING, CA1_256,(LONG)"" );
			edit_id = 0;
			showparams( hDlg );
			return 0;

}


static void MyWnd_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"DIGITALX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"DIGITALY",(LPSTR)buf);
			
}

static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgDigital = 0;
			InvalidateRect( masterhwnd, NULL, FALSE );
			
}



static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	
}






static void MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
	Real num;
	char tmpStr[20];
	switch( id )
	{
    	case RADIO_ALL:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEALLMENU, 0L);
			break;

		case RADIO_FOCUS:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEFOCUSMENU, 0L);
			break;

		case SC_UPDATE: //Used to send in an external update command if params change.
			edit_id = 0; //You have to kill any active edit-box session.
			showparams( hDlg );
			break;



		case RADIUSSTATES_NEW:
			if( SendMessage((HWND)hwndCtl, CB_GETDROPPEDSTATE, 0, 0L ) &&
				new_drop == 0)
			{
				new_drop = 1;
				break;
			} // if

			if(!SendMessage((HWND)hwndCtl, CB_GETDROPPEDSTATE, 0, 0L ) && 
				new_drop == 1)
			{
				int combo = CA1_2 + (unsigned char)SendMessage( (HWND)hwndCtl,
					CB_GETCURSEL, 0, 0L );
				if( focusflag )
				{
					calife_list->FocusCA()->Changeradiusandstates(
						NEW_STATES[ combo ] );
					calife_list->FocusCA()->ResetGenerationCount();
				}
				else
				{
					calife_list->Changeradiusandstates(
						NEW_RADIUS[ combo ], NEW_STATES[ combo ] );
					calife_list->ResetAllGenerationCount();
				}
				update_flag = 1;
				showparams(hDlg);
				new_drop = 0;
			} // if
			break;

		case BUTT_SYM:
			if( focusflag )
			{
				calife_list->FocusCA()->Symmetrize();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Symmetrize();
				calife_list->ResetAllGenerationCount();
			}
			break;

		case BUTT_REVERSE:
			if( focusflag )
			{
				calife_list->FocusCA()->Reverse();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Reverse();
				calife_list->ResetAllGenerationCount();
			}
			break;

		
		case VARIANCE_TYPE:
        case LAMBDA_TYPE:
             edit_id = id;
             break;

        case IDOK:
			// lambda is NOT a tweakParam; therefore, "if" statement is necessary
            GetWindowText (GetDlgItem(hDlg, edit_id), tmpStr, MAX_STR_SIZE);
			num = atof (tmpStr);
			if (focusflag)
			{
				if (edit_id == VARIANCE_TYPE)
					calife_list->FocusCA()->SetTweakParam(VARIANCE_TYPE,num);
                else
                    calife_list->FocusCA()->Setlambda(num);
			}
			else
			{
                 if (edit_id == VARIANCE_TYPE)
	                 calife_list->SetTweakParam(VARIANCE_TYPE,num);
                 else
                     calife_list->Setlambda(num);
			}
			edit_id = 0;
			showparams(hDlg);
			break;
			} // switch wParam

}
/*----------------------------------------------------------------------------------------*/

BOOL HandleUpDownControlDigital(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//
// Up down control work as follow:
//		alway remain at 50
//		the value or variable change only.
//
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 
	switch ( pnmud->hdr.idFrom )
	{
		case IDC_SPINLAMBDA:
			if( focusflag )
			{
				calife_list->FocusCA()->Changelambda( pnmud->iDelta * 0.1 );
				calife_list->FocusCA()->ResetGenerationCount();					
			}
			else
			{
				calife_list->Changelambda( pnmud->iDelta * 0.1);
				calife_list->ResetAllGenerationCount();
			}
            edit_id = 0;
			showparams(hDlg);
			break;
		
		case IDC_SPINDIVERSE:
			if( focusflag )
			{
				calife_list->FocusCA()->BumpTweakParam(VARIANCE_TYPE,-pnmud->iDelta );
				calife_list->FocusCA()->ResetGenerationCount();					
			}
			else
			{
				calife_list->BumpCATweakParam(VARIANCE_TYPE,-pnmud->iDelta);
				calife_list->ResetAllGenerationCount();
			}
            edit_id = 0;
			showparams(hDlg);
			break;

		
	} // Switch
	// We must return true so that the position remain the same value
	return TRUE;
}

extern BOOL CALLBACK DigitalProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, MyWnd_MOVE);
		case WM_NOTIFY:
			return HandleUpDownControlDigital(hDlg, message, wParam, lParam);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
} // DigitalProc */

/*----------------------------------------------------------------------------------------*/

void showparams( HWND hDlg )
{
	/* Because we are using an edit text box, care needs to be taken
		to prevent an  endless regress of showparams calling showparams which
		calls showparams, etc.  The SetWindowText call used inside textLabel
		and realLabel to show the current value of an edit params  generates
		a call to WM_COMMAND with a *_TYPE identifier of the edit box in wParam,
		and this sets edit_id to *_TYPE and sends a call to showparams() at the
		end of the WM_COMMAND.  We have	to do two things about this (1) bail from
		showparams if edit_id is non-zero, and (2) restore edit_id to zero at the
		end of showparams.*/
	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
	if (edit_id)
			return; // This bails on regressive showparams calls from WM_COMMAND.

	if( calife_list->FocusCA()->Getcastyle() == CA_REVERSIBLE ||
		 calife_list->FocusCA()->Getcastyle() == CA_WAVE || 
		 calife_list->FocusCA()->Getcastyle() == CA_ULAM_WAVE || 
		 calife_list->FocusCA()->Getcastyle() == CA_CUBIC_ULAM_WAVE || 
		 calife_list->FocusCA()->Getcastyle() == CA_WAVE_2D )
		EnableWindow( GetDlgItem( hDlg, BUTT_REVERSE ), TRUE );
	else
		EnableWindow( GetDlgItem( hDlg, BUTT_REVERSE ), FALSE );

	if( calife_list->FocusCA()->Getcastyle() != CA_STANDARD &&
		 calife_list->FocusCA()->Getcastyle() != CA_REVERSIBLE )
		EnableWindow( GetDlgItem( hDlg, BUTT_SYM ), FALSE );
	else
		EnableWindow( GetDlgItem( hDlg, BUTT_SYM ), TRUE );

	int curr_radius = calife_list->FocusCA()->Getradius();
	int curr_states = calife_list->FocusCA()->Getstates();
	int curr_radius_states;
	for( int i = 0; i < MAX_TYPES; i++ )
		if( curr_radius == NEW_RADIUS[i] && curr_states == NEW_STATES[i] )
		{	curr_radius_states = i;
			break;
		} // end of if

	if( !SendMessage( GetDlgItem( hDlg, RADIUSSTATES_NEW ), CB_GETDROPPEDSTATE, 0, 0L ))
		SendMessage( GetDlgItem( hDlg, RADIUSSTATES_NEW ), CB_SETCURSEL,
						 (int)curr_radius_states, 0 );

	
	set_active_buttons(calife_list->FocusCA()->Getcastyle());
	enable_active_buttons( hDlg );
	textLabel (hDlg, TARGET_LAMBDA_NEW, "lambda");
	realLabel (hDlg, LAMBDA_TYPE, calife_list->FocusCA()->Getlambda());
	textLabel (hDlg, VARIANCE_NEW, calife_list->FocusCA()->GetTweakParam(VARIANCE_TYPE)->Label());
	realLabel (hDlg, VARIANCE_TYPE, calife_list->FocusCA()->GetTweakParam(VARIANCE_TYPE)->Val());
	edit_id = 0;

} // showparams */

/*----------------------------------------------------------------------------------------*/

void set_active_buttons( int castyle )
{
	for( int i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 0;

	switch( castyle)
	{	case CA_STANDARD:
		case CA_REVERSIBLE:
			//active_buttons[ TARGET_LAMBDA_INC - BUTT_START ] = 1;
			active_buttons[ IDC_SPINLAMBDA - BUTT_START ] = 1;
			break;

		case CA_HEATWAVE:
		case CA_WAVE:
		case CA_WAVE_2D:
		case CA_ULAM_WAVE:
		case CA_OSCILLATOR:
			break;

		case CA_AUTO_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
		case CA_DIVERSE_OSCILLATOR:
		case ALT_CA_DIVERSE_OSCILLATOR:
			active_buttons[ IDC_SPINDIVERSE - BUTT_START ] = 1;
			//active_buttons[ VARIANCE_DEC - BUTT_START ] = 1;
			break;

		case CA_NETWORK:
		case CA_CUSTOM:
		case CA_CUSTOM_2D:
			active_buttons[ IDC_SPINLAMBDA - BUTT_START ] = 1;
		//	active_buttons[ TARGET_LAMBDA_DEC - BUTT_START ] = 1;
			active_buttons[ IDC_SPINDIVERSE - BUTT_START ] = 1;
//			active_buttons[ VARIANCE_DEC - BUTT_START ] = 1;
			break;
	} // switch castyle
} // set_active_buttons */

/*----------------------------------------------------------------------------------------*/

void enable_active_buttons( HWND hwnd )
{	for( int i = BUTT_START; i < BUTT_START+MAX_PLUSES_MINUSES; i++ )
		if( !active_buttons[ i-BUTT_START ] )
		{	EnableWindow( GetDlgItem( hwnd, i ), FALSE );
			switch( i )
			{	case IDC_SPINLAMBDA://TARGET_LAMBDA_INC:
					EnableWindow( GetDlgItem( hwnd, IDC_SPINLAMBDA ), FALSE );
					EnableWindow( GetDlgItem( hwnd, LAMBDA_TYPE ), FALSE );
					break;

				case IDC_SPINDIVERSE: // VARIANCE_INC:
					EnableWindow( GetDlgItem( hwnd, IDC_SPINDIVERSE ), FALSE );
					EnableWindow( GetDlgItem( hwnd, VARIANCE_TYPE ), FALSE );
					break;
			} // switch
		} // if not active
		else
		{	EnableWindow( GetDlgItem( hwnd, i ), TRUE );
			switch( i )
			{	case IDC_SPINLAMBDA:
					EnableWindow( GetDlgItem( hwnd, IDC_SPINLAMBDA ), TRUE );
					EnableWindow( GetDlgItem( hwnd, LAMBDA_TYPE ), TRUE );
					break;

				case IDC_SPINDIVERSE:
					EnableWindow( GetDlgItem( hwnd, IDC_SPINDIVERSE ), TRUE );
					EnableWindow( GetDlgItem( hwnd, VARIANCE_TYPE ), TRUE );
					break;
			} // switch
		} // else
} // enable_active_buttons */

