#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"

#include <math.h> // for atof in the 16 bit version

extern char *szMyAppName;
extern HWND hDlgElectric, masterhwnd;
extern class CAlist *calife_list;
extern void textLabel (HWND, int, char*);
extern void realLabel (HWND, int, Real);

static void showparams(HWND);
static void set_active_buttons(int);
static void enable_active_buttons(HWND);

static int edit_id = 0; //static because other dialog modules use same name
extern short focusflag;

#define MAX_PLUSES_MINUSES 10		// number of incrementors and decrementors
#define BUTT_START         101	// starting number of the push buttons of inc/dec
#define MAX_STR_SIZE 20
#define NUM_TWEAKPARAMS 5
#define ID_DIFFERENCE (SPRING_TYPE - SPRING_NEW)

static int active_buttons[MAX_PLUSES_MINUSES] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	// all are active

static int controlID_to_TweakID[NUM_TWEAKPARAMS];

/*----------------------------------------------------------------------------------------*/
// 							Messager Cracker

static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	HINSTANCE hInstance;
	
	hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
	
//	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
//  controlID_to_TweakID[0] depends on the type of ca
    controlID_to_TweakID[1] = FRICTION_TYPE;
    controlID_to_TweakID[2] = DRIVER_AMP_TYPE;
    controlID_to_TweakID[3] = DRIVER_FREQ_TYPE;
    controlID_to_TweakID[4] = MASS_TYPE;
	edit_id = 0;
	showparams( hDlg );
	return 0;
}

static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgElectric = 0;
			InvalidateRect( masterhwnd, NULL, FALSE );
}


static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);

}



static void MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
   char tmpStr[MAX_STR_SIZE-1];
   Real num;

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

        case SPRING_TYPE:
        case FRICTION_TYPE:
        case DRIVER_AMP_TYPE:
        case DRIVER_FREQ_TYPE:
        case MASS_TYPE:
			edit_id = id;
            break;

		case IDOK:
			// see tweakca.hpp for the order of *_TYPE
            GetWindowText (GetDlgItem(hDlg, edit_id), tmpStr, MAX_STR_SIZE);
			num = atof (tmpStr);
			if (focusflag)
				calife_list->FocusCA()->SetTweakParam(
					controlID_to_TweakID[ edit_id - SPRING_TYPE ],num);
			else
	            calife_list->SetTweakParam(
					controlID_to_TweakID[ edit_id - SPRING_TYPE ],num);
			edit_id = 0;
			showparams(hDlg);
			break;

	} // switch on id of control
}
BOOL HandleUpDownControlElectric(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 
	switch ( pnmud->hdr.idFrom )
	{
		case IDC_SPIN_ELECTRIC_CAP:
		case IDC_SPIN_ELECTRIC_FRICTION:
		case IDC_SPIN_ELECTRIC_AMP:
		case IDC_SPIN_ELECTRIC_FREQ:
		case IDC_SPIN_ELECTRIC_MASS:
			if ( pnmud->iDelta < 0 )
			{
				if( focusflag )
				{
					calife_list->FocusCA()->BumpTweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ELECTRIC_CAP)/2],(+1));
					calife_list->FocusCA()->ResetGenerationCount();					
				}
				else
				{
					calife_list->BumpCATweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ELECTRIC_CAP)/2],(+1));
					calife_list->ResetAllGenerationCount();
				}
				edit_id = 0;
				showparams(hDlg);
			}
			else
			{
				if( focusflag )
				{
					calife_list->FocusCA()->BumpTweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ELECTRIC_CAP+1)/2],(-1));
					calife_list->FocusCA()->ResetGenerationCount();					
				}
				else
				{
					calife_list->BumpCATweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ELECTRIC_CAP+1)/2],(-1));
					calife_list->ResetAllGenerationCount();
				}
				edit_id = 0;
				showparams(hDlg);
			}
			break;

	
	} // Switch
	// We must return true so that the position remain the same value
	return TRUE;
}

static void MyWnd_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"ELECTIRCX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"ELECTIRCY",(LPSTR)buf);
			
}
/*----------------------------------------------------------------------------------------*/

extern BOOL CALLBACK ElectricProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
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
			return HandleUpDownControlElectric(hDlg, message, wParam, lParam);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
} // ElectricProc */

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
	// if not editing any edit box, update!!!
	int castyle = calife_list->FocusCA()->Getcastyle();
    if (castyle == CA_AUTO_ULAM_WAVE || castyle == CA_CUBIC_ULAM_WAVE)
		controlID_to_TweakID[0] = NONLINEARITY2_TYPE;
	else if (castyle == CA_ULAM_WAVE)
        controlID_to_TweakID[0] = NONLINEARITY1_TYPE;
	else
        controlID_to_TweakID[0] = SPRING_TYPE;

	set_active_buttons(castyle);
	enable_active_buttons( hDlg );

    for (int i = SPRING_NEW; i <= MASS_NEW; i++)
    {
		textLabel (hDlg, i, calife_list->FocusCA()->GetTweakParam(
			controlID_to_TweakID[i-SPRING_NEW])->Label());
        realLabel (hDlg, i+ID_DIFFERENCE, calife_list->FocusCA()->GetTweakParam(
			controlID_to_TweakID[i-SPRING_NEW])->Val());
	}
	edit_id = 0;
} // showparams */

/*----------------------------------------------------------------------------------------*/

void set_active_buttons( int castyle )
{  int i;
   for(i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 1;

	switch( castyle )
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
			for( i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 0;
			break;

		case CA_HEATWAVE:
		case CA_HEAT_2D:
			for( i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 0;
			break;

		case CA_WAVE:
			for( i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 0;
			break;

		case CA_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
			for( i = 0; i < MAX_PLUSES_MINUSES; i++ ) active_buttons[i] = 0;
			break;

		case CA_WAVE_2D: //We use CA_WAVE_2D as a general style that MAY be electric
		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
			break;

		case CA_NETWORK:
		case CA_CUSTOM:
		case CA_CUSTOM_2D:
 			break; //Leave all controls on.
	} // switch catype
} // set_active_buttons */

/*----------------------------------------------------------------------------------------*/

void enable_active_buttons( HWND hwnd )
{	for( int i = BUTT_START; i < BUTT_START+MAX_PLUSES_MINUSES; i++ )
	if( !active_buttons[ i-BUTT_START ] )
	{	EnableWindow( GetDlgItem( hwnd, i ), FALSE );
		switch( i )
		{	case IDC_SPIN_ELECTRIC_CAP:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_CAP ), FALSE );
            EnableWindow( GetDlgItem( hwnd, SPRING_TYPE), FALSE );
				break;

			case IDC_SPIN_ELECTRIC_FRICTION:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_FRICTION ), FALSE );
            EnableWindow( GetDlgItem( hwnd, FRICTION_TYPE), FALSE );
				break;

			case IDC_SPIN_ELECTRIC_AMP:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_AMP ), FALSE );
				EnableWindow( GetDlgItem( hwnd, DRIVER_AMP_TYPE ), FALSE );
				break;

			case IDC_SPIN_ELECTRIC_FREQ:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_FREQ ), FALSE );
				EnableWindow( GetDlgItem( hwnd, DRIVER_FREQ_TYPE ), FALSE );
				break;

			case IDC_SPIN_ELECTRIC_MASS:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_MASS ), FALSE );
				EnableWindow( GetDlgItem( hwnd, MASS_TYPE ), FALSE );
				break;
		} // switch i
	} // if not active
	else
	{	EnableWindow( GetDlgItem( hwnd, i ), TRUE );
		switch( i )
		{	case IDC_SPIN_ELECTRIC_CAP:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_CAP ), TRUE );
				EnableWindow( GetDlgItem( hwnd, SPRING_TYPE ), TRUE );
				break;

			case IDC_SPIN_ELECTRIC_FRICTION:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_FRICTION ), TRUE );
				EnableWindow( GetDlgItem( hwnd, FRICTION_TYPE ), TRUE );
				break;

			case IDC_SPIN_ELECTRIC_AMP:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_AMP ), TRUE );
				EnableWindow( GetDlgItem( hwnd, DRIVER_AMP_TYPE ), TRUE );
				break;

			case IDC_SPIN_ELECTRIC_FREQ:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_FREQ ), TRUE );
				EnableWindow( GetDlgItem( hwnd, DRIVER_FREQ_TYPE ), TRUE );
				break;

			case IDC_SPIN_ELECTRIC_MASS:
				EnableWindow( GetDlgItem( hwnd, IDC_SPIN_ELECTRIC_MASS ), TRUE );
				EnableWindow( GetDlgItem( hwnd, MASS_TYPE ), TRUE );
				break;
		} // switch i
	} // else active
} // enable_active_buttons */


