/*******************************************************************************
	FILE:				analog.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions and data to control
						the analog dialog box and its features.

	UPDATE LOG:			
					9-29-97  Rewrote the Dialog Enable/Disable rountine.
							 Eliminated Processing of WM_RBUTTONDOWN, WM_MOVE
							 Removed MyWnd_RBUTTON, and MyWnd_Move
*******************************************************************************/
//====================INCLUDES===============

#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include <math.h>
 
//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============

#define MAX_STR_SIZE       20
#define NUM_TWEAKPARAMS    7
#define ID_DIFFERENCE      SPACE_STEP_TYPE - SPACE_STEP_NEW

//====================GLOBAL DATA===============

static int edit_id = 0; // Initialization for editing stage in this dialogbox
static int controlID_to_TweakID[NUM_TWEAKPARAMS];
 
//====================LOCAL FUNCTIONS ===============

static void  showparams(HWND);
static void enable_active_buttons( HWND hwnd, int castyle );
 
//====================EXTERNAL DATA===============

extern class CAlist *calife_list;
extern short focusflag;	
extern char  *szMyAppName;
extern HWND  hDlgAnalog, masterhwnd;
 
//====================EXTERNAL FUNCTIONS===============

extern void  textLabel (HWND, int, char*);
extern void  realLabel (HWND, int, Real);

/*------------------------------------------------------------------------------*/
//								Message Crackers
/*------------------------------------------------------------------------------*/
//********************************************************************************
// MyWnd_INITDIALOG
//
// Initializes the Analog Dialog Box

static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
	
//	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
    controlID_to_TweakID[0] = SPACE_STEP_TYPE;
    controlID_to_TweakID[1] = TIME_STEP_TYPE;
    controlID_to_TweakID[2] = MAX_INTENSITY_TYPE;
    controlID_to_TweakID[3] = MAX_VELOCITY_TYPE;
//  controlID_to_TweakID[4] depends on the type of ca
//  it will get set in showparams(HWND)
    controlID_to_TweakID[5] = CHUNK_TYPE;
	controlID_to_TweakID[6] = MAX_VALPERCENT_TYPE;
	edit_id = 0; //So I don't bail from showparams
	showparams( hDlg );
	return 0;
}


//********************************************************************************
// MyWnd_COMMAND
//
// Process the activity of the analog dialog.


static void MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	char tmpStr[MAX_STR_SIZE]; //2017 had MAX_STR_SIZE-1 which could be too short
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
				//Or you'll bail from showparams.
			showparams( hDlg );
			break;

		case CHK_DX_LOCK:
			if( focusflag )
			{
				if( calife_list->FocusCA()->Get_dx_lock() )
					calife_list->FocusCA()->Set_dx_lock( FALSE );
				else
					calife_list->FocusCA()->Set_dx_lock( TRUE );
				calife_list->ResetAllGenerationCount();
			} // if focusflag is on
			else
			{
				if( calife_list->FocusCA()->Get_dx_lock() )
					calife_list->Set_dx_lock( FALSE );
				else
					calife_list->Set_dx_lock( TRUE );
				calife_list->ResetAllGenerationCount();
			} // else focusflag is off
			edit_id = 0;
			showparams(hDlg);
			break;
		
			case HEAT_INC_TYPE:     // fall through
			case MAX_VELOCITY_TYPE:
			case MAX_INTENSITY_TYPE:
			case TIME_STEP_TYPE:
			case SPACE_STEP_TYPE:
			case CHUNK_TYPE:
				/*Here we enter the edit-box editing mode.  You can leave this
				mode either by clicking outside the edit box, which discards
				the changes, or by pressing ENTER which generates an IDOK and
				saves the changes.*/
				edit_id = id; //Enter the edit-box editing mode.
				break;

			case IDOK: //You get this when you press ENTER.
					// see tweakca.hpp for the order of *_TYPE
				GetWindowText (GetDlgItem(hDlg, edit_id), tmpStr, MAX_STR_SIZE);
				num = atof(tmpStr);
				if (focusflag)
					calife_list->FocusCA()->SetTweakParam(
						controlID_to_TweakID[ edit_id - SPACE_STEP_TYPE ],num);
				else
					calife_list->SetTweakParam(
						controlID_to_TweakID[ edit_id - SPACE_STEP_TYPE ],num);
				edit_id = 0;
				showparams(hDlg);
				break;
		
	} // end switch on id of the WM_COMMAND 
}


//********************************************************************************
// MyWnd_CLOSE
//
// Destroys the analog dialog box when user closes dialog

static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
}

//********************************************************************************
// MyWnd_DESTROY
//
// Destroys the analog dialog box when user closes dialog

static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgAnalog = 0;
			InvalidateRect( masterhwnd, NULL, FALSE );
}

//********************************************************************************
// HandleUpDownControlAnalog
//
// Handle function for the dialogs Spin Controls... Function is called
// when a WM_NOTIFY is received indicating a spin control has been touched.

BOOL HandleUpDownControlAnalog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 	
	switch ( pnmud->hdr.idFrom )
	{
		case IDC_SPIN_ANALOG_SPACESTEP:
		case IDC_SPIN_ANALOG_VELOCITY:
		case IDC_SPIN_ANALOG_INTENSITY:
		case IDC_SPIN_ANALOG_MAXVELOCITY:
		case IDC_SPIN_ANALOG_HEATCOUNT:
		case IDC_SPIN_ANALOG_STATEGRAIN:
		case IDC_SPIN_ANALOG_3DHEIGHT:
			if ( pnmud->iDelta < 0 )
			{	
				if( focusflag )
				{
					calife_list->FocusCA()->BumpTweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ANALOG_SPACESTEP)/2],(+1));
					calife_list->FocusCA()->ResetGenerationCount();
				}
				else
				{
					calife_list->BumpCATweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ANALOG_SPACESTEP)/2],(+1));
					calife_list->ResetAllGenerationCount();
				}
				edit_id = 0;
				showparams(hDlg);
			}
			else
			{
				if( focusflag )
				{
					calife_list->FocusCA()->BumpTweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ANALOG_SPACESTEP+1)/2],(-1));
					calife_list->FocusCA()->ResetGenerationCount();
				}
				else
				{
					calife_list->BumpCATweakParam(controlID_to_TweakID[(pnmud->hdr.idFrom-IDC_SPIN_ANALOG_SPACESTEP+1)/2],(-1));
					calife_list->ResetAllGenerationCount();
				}
				edit_id = 0;
				showparams(hDlg);
			}
			break;

		}  // end switch
	// We must return true so that the position remain the same value
	return TRUE;
}

//********************************************************************************
// AnalogProc
//
// Message Switch for the analog dialog.

extern BOOL CALLBACK AnalogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,MyWnd_INITDIALOG);
		HANDLE_MSG(hDlg,WM_DESTROY,MyWnd_DESTROY);
		HANDLE_MSG(hDlg,WM_CLOSE,MyWnd_CLOSE);
		HANDLE_MSG(hDlg,WM_COMMAND,MyWnd_COMMAND);
		case WM_NOTIFY:
			return HandleUpDownControlAnalog(hDlg, message, wParam, lParam);				
		default:
			return FALSE; 
	} 

} 

//********************************************************************************
// showparams
//
// Function displays all parameters based on the CA_TYPE and enables/disables
// windows according to the CA_TYPE

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
	
	// stable DX check box
	if( calife_list->FocusCA()->Get_dx_lock() )
		CheckDlgButton( hDlg, CHK_DX_LOCK, 1 );
	else
		CheckDlgButton( hDlg, CHK_DX_LOCK, 0 );

	int castyle = calife_list->FocusCA()->Getcastyle();
	if (castyle == CA_AUTO_ULAM_WAVE ||	castyle == CA_CUBIC_ULAM_WAVE)
		controlID_to_TweakID[4] = NONLINEARITY2_TYPE;
	else if (castyle == CA_ULAM_WAVE ||  castyle == CA_WAVE_2D)
		controlID_to_TweakID[4] = NONLINEARITY1_TYPE;
	else
			controlID_to_TweakID[4] = HEAT_INC_TYPE;

// gray out buttons for the pluses and minuses and their labels
	enable_active_buttons( hDlg, castyle );

 /* have a controlID_to_TweakID array like in world you have boxindex_to_type
 and then do this with a i loop from low button to high button value , use
 the array to get the tweakID, and you change the meaning of the heatbutton ID
 each time you change the type of the CA */

      // see resource.h for order
	for (int i = SPACE_STEP_NEW; i <= CHUNK_NEW; i++)
    {
		textLabel(hDlg, i, calife_list->FocusCA()->GetTweakParam(
			controlID_to_TweakID[i-SPACE_STEP_NEW])->Label());
		realLabel (hDlg, i + ID_DIFFERENCE, calife_list->FocusCA()->GetTweakParam(
			controlID_to_TweakID[i-SPACE_STEP_NEW])->Val());
    }
	edit_id = 0;  //This undoes the unwanted setting of edit_id by SetWindowText.

} // showparams */



//********************************************************************************
// enable_active_buttons
//
// Based on the CA_TYPE this function will enable/disalbe various controls
// in the dialog.


void enable_active_buttons( HWND hDlg, int castyle )
{
	
	switch( castyle )
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_SPACESTEP ), FALSE );
			EnableWindow( GetDlgItem( hDlg, SPACE_STEP_TYPE ), FALSE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_VELOCITY ), FALSE );
			EnableWindow( GetDlgItem( hDlg, TIME_STEP_TYPE ), FALSE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_INTENSITY ), FALSE );
			EnableWindow( GetDlgItem( hDlg, MAX_INTENSITY_TYPE), FALSE);
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_MAXVELOCITY ), FALSE );
			EnableWindow( GetDlgItem( hDlg, MAX_VELOCITY_TYPE), FALSE);
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_HEATCOUNT ), FALSE );
			EnableWindow( GetDlgItem( hDlg, HEAT_INC_TYPE), FALSE);
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_STATEGRAIN ), FALSE );
			EnableWindow( GetDlgItem( hDlg, CHUNK_TYPE ), FALSE );
			break;

		
		case CA_WAVE:
		case CA_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
		case CA_WAVE_2D: //Some WAVE_2D use nonlinearity. previously using spring plus and minus.  Now using heat count plus and minus
		case CA_HEATWAVE:
		case CA_HEAT_2D:
		case CA_CUSTOM:
		case CA_CUSTOM_2D:
		case CA_NETWORK:
		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_SPACESTEP ), TRUE );
			EnableWindow( GetDlgItem( hDlg, SPACE_STEP_TYPE ), TRUE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_VELOCITY ), TRUE );
			EnableWindow( GetDlgItem( hDlg, TIME_STEP_TYPE), TRUE);
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_INTENSITY ), TRUE );
			EnableWindow( GetDlgItem( hDlg, MAX_INTENSITY_TYPE ), TRUE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_MAXVELOCITY ), TRUE );
			EnableWindow( GetDlgItem( hDlg, MAX_VELOCITY_TYPE), TRUE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_HEATCOUNT ), TRUE );
			EnableWindow( GetDlgItem( hDlg, HEAT_INC_TYPE ), TRUE );
			EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_STATEGRAIN ), TRUE );
			EnableWindow( GetDlgItem( hDlg, CHUNK_TYPE ), TRUE );
			
			if ( castyle == CA_OSCILLATOR ||  castyle == CA_DIVERSE_OSCILLATOR )
			{
				EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_HEATCOUNT ), FALSE );
				EnableWindow( GetDlgItem( hDlg, HEAT_INC_TYPE ), FALSE );
			}
				
			break;
	}
	
	if( calife_list->FocusCA()->Get_dx_lock() ) // if locked, can't hand change it
	{
		EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_SPACESTEP ), FALSE );
		EnableWindow( GetDlgItem( hDlg, SPACE_STEP_TYPE ), FALSE );
	} // if dx locked
	else
	{	
		EnableWindow( GetDlgItem( hDlg, IDC_SPIN_ANALOG_SPACESTEP ), TRUE );
		EnableWindow( GetDlgItem( hDlg, SPACE_STEP_TYPE ), TRUE );
	} // else dx is not locked
} // enable_active_buttons */
