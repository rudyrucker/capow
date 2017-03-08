/*******************************************************************************
	FILE:				cell.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions and data to control
						the cell dialog box and its features.
 
	UPDATE LOG:		9-29-97   Code Commented	
					
*******************************************************************************/
//====================INCLUDES===============
#include "ca.hpp"
#include "resource.h"

//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============
//====================GLOBAL DATA===============
//====================LOCAL FUNCTIONS ===============

void showcellparams(HWND);

//====================EXTERNAL DATA===============

extern char *szMyAppName;
extern class CAlist *calife_list;
extern BOOL   update_flag;
extern HWND   hDlgCell, masterhwnd;
extern short  focusflag;	

//====================EXTERNAL FUNCTIONS===============
extern void realLabel (HWND, int, Real);

/*------------------------------------------------------------------------------*/
//								Message Crackers
/*------------------------------------------------------------------------------*/

//********************************************************************************
// MyWnd_INITDIALOG
//
// Initializes the CELL Dialog Box

#pragma argsused
static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	showcellparams(hDlg);
	return 0;
}

//********************************************************************************
// MyWnd_COMMAND
//
// Process the activity of the CELL dialog.


#pragma argsused
static void MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
			switch( id )
			{  
				case SC_UPDATE:
					showcellparams(hDlg);
					break;

				case BUTT_MUTATE:
					if( focusflag )
					{	
						calife_list->FocusCA()->Mutate( calife_list->Getmutation() );
						calife_list->FocusCA()->ResetGenerationCount();					
					}
					else
					{
						calife_list->Mutate();
						calife_list->ResetAllGenerationCount();
					}
					update_flag = 1;
					break;

				case BUTT_COPYMUTATE:
					calife_list->ResetAllGenerationCount();					
					calife_list->Copymutate();
					break;

				case BUTT_BREED:
					calife_list->ResetAllGenerationCount();
					calife_list->Breed();
					break;
			} // switch wParam
			

}
//********************************************************************************
// MyWnd_CLOSE
//
// Destroys the CELL dialog box when user closes dialog

#pragma argsused
static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	
}

//********************************************************************************
// MyWnd_DESTROY
//
// Destroys the CELL dialog box when user closes dialog

#pragma argsused
static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgCell = 0;
			InvalidateRect( masterhwnd, NULL, TRUE );
}

BOOL HandleUpDownControlCell(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	
		case IDC_SPIN_BREED_MUTATE:
			if ( pnmud->iDelta < 0 )
			{		
				calife_list->Setmutation(+1);
				SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
			else
			{
				calife_list->Setmutation(-1);
				SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
			break;

	} // Switch
	// We must return true so that the position remain the same value
	return TRUE;
}


//********************************************************************************
// CellProc
//
// Message Switch for the CELL dialog.

#pragma argsused // To disable "Parameter 'lParam' is never used" warning.

extern BOOL CALLBACK CellProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
/*All the message-crackers other than HANDLE_WM_INITDIALOG return by
default a void, which comes through as FALSE, but we want to return
a TRUE for messages which really are handled, so we do a (BOOL) ! cast
on those.  We make HANDLE_WM_INITDIALOG  return a 0 for success, so cast
it the same way.*/
	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_NOTIFY:
			return HandleUpDownControlCell(hDlg, message, wParam, lParam);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
} // CellProc */

void showcellparams(HWND hDlg)
{
	realLabel (hDlg,IDC_MUTATION,100.0 * calife_list->Getmutation());
}
