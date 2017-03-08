//GENDLG.CPP manages the generator dialog box
//  Mike L. 10/97
//  Ted C.	11/97


#include "ca.hpp"
#include "resource.h"
#include "CapowGL.hpp"
#include <winuser.h>

extern HWND hDlgGenerators, masterhwnd;
extern char *szMyAppName;
extern void realLabel (HWND, int, Real);

#pragma hdrstop
//====================EXTERNAL DATA===============

extern class CAlist *calife_list;
extern BOOL zoomviewflag;
extern CapowGL *capowgl;

//====================DEFINE CONSTANTS ===============

#define MAX_STR_SIZE       20

//====================GLOBAL DATA===============

static int edit_id;			// Initialization for editing stage in this dialogbox
static int focusGen;		// zero based index of generator in the generator list box
static int lb_index = -1;
static char szIndex[10] = " ";
static BOOL phase_synced = FALSE;
 
//====================LOCAL FUNCTIONS ===============

BOOL HandleUpDownControlGenerators(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void showparams(HWND hDlg);

//------------------------ Message Processing -----------------------//

static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	edit_id = 0;
	focusGen = -1;	
	if(calife_list->FocusCA()->generatorlist.Count() >0)
		focusGen = calife_list->FocusCA()->generatorlist.Count() - 1;

	InvalidateRect(hDlg, NULL, FALSE);
	UpdateWindow(hDlg);
	return TRUE;
}


static void  MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	char tmpStr[MAX_STR_SIZE-1];
	Real num;
	int i, nIndex, newcount;
	char buffer[64];


	switch (id)
	{
		case IDC_GENERATORS_CLEAR:
			if (zoomviewflag)
			{
				calife_list->FocusCA()->generatorlist.Clear();
				focusGen = -1;
				edit_id = 0;
				showparams(hDlg);
			}	
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case IDC_GENERATORS_REMOVE_INDEX_I:
			if (focusGen >= 0)
			{	
				newcount = calife_list->FocusCA()->generatorlist.Count() - 1;

				if(calife_list->FocusCA()->Getdimension()==1) //1D CA
					calife_list->FocusCA()->generatorlist.Delete(focusGen);
				else if(calife_list->FocusCA()->Getdimension()==2)//2D CA
					calife_list->FocusCA()->generatorlist.Deletexy(focusGen);
				else	
					break; //you don't belong here
				
				if (focusGen == newcount || newcount == 0) //if deleted generator was last in list
					focusGen--;							   //or if only one in list, decrement focusGen
													//Otherwise, the focus stays on the current generator
				edit_id = 0;
				showparams(hDlg);					
			}
			break;

		case IDC_GENERATORS_PICK_CUR:
				SendMessage(masterhwnd, WM_COMMAND, CUR_PICK, 0L);
			break;

		case IDC_GENERATORS_GEN_CUR:
			if (zoomviewflag)
				SendMessage(masterhwnd, WM_COMMAND, CUR_GENERATOR, 0L);
			else 
			{
				SendMessage(masterhwnd, WM_LBUTTONDOWN, CUR_PICK, 0L); // zooms view 
				SendMessage(masterhwnd, WM_COMMAND, CUR_GENERATOR, 0L);		
				SendMessage(hDlg, WM_COMMAND, IDC_GENERATORS_GEN_CUR, 0L);		
			}                                                         
			break;
 

		case IDC_GENERATORS_LIST1:
			if(codeNotify==LBN_SELCHANGE)
			{
				focusGen = SendMessage(hwndCtl, LB_GETCURSEL,0,0L);	
				if(focusGen != LB_ERR)
				{
					edit_id = 0; // need this to counter recursive calls to OMEGA and AMP edit boxes
					showparams(hDlg);
				}
			}
			break;

		case IDC_GENERATORS_OMEGA:
				/*Here we enter the edit-box editing mode.  You can leave this
				mode either by clicking outside the edit box, which discards
				the changes, or by pressing ENTER which generates an IDOK and
				saves the changes.*/
				edit_id = id; //Enter the edit-box editing mode.
				break;

		case IDC_GENERATORS_AMP:
				/*Here we enter the edit-box editing mode.  You can leave this
				mode either by clicking outside the edit box, which discards
				the changes, or by pressing ENTER which generates an IDOK and
				saves the changes.*/
				edit_id = id; //Enter the edit-box editing mode.
				break;

      case IDC_GENERATORS_PHASE:
               if(calife_list->FocusCA()->generatorlist.Count() >0)
                  calife_list->FocusCA()->generatorlist.Reset();
            break;

		case SETCURSEL:  //called externally when a generator is added
			focusGen = calife_list->FocusCA()->generatorlist.Count() - 1;
			edit_id = 0;
			showparams(hDlg);
			break;

		case WM_VSCROLL:
		
			switch(LOWORD(codeNotify))
			{
				case SB_LINEUP:
				

					edit_id = 0;
					//showparams(hDlg);
					break;

				case SB_LINEDOWN:
					
					edit_id = 0;
					//showparams(hDlg);
			}
			break;
		
		case IDOK:
			switch (edit_id)
			{
				case IDC_GENERATORS_LIST1:
					edit_id = 0;
					showparams(hDlg);
					break;

				case IDC_GENERATORS_OMEGA:
					GetWindowText (GetDlgItem(hDlg, edit_id), tmpStr, MAX_STR_SIZE);
					num = atof(tmpStr);
					calife_list->FocusCA()->generatorlist.SetOmega(focusGen, num);
					edit_id = 0;
					showparams(hDlg);
					break;

				case IDC_GENERATORS_AMP:
					GetWindowText (GetDlgItem(hDlg, edit_id), tmpStr, MAX_STR_SIZE);
					num = atof(tmpStr);
					calife_list->FocusCA()->generatorlist.SetAmplitude(focusGen, num);
					edit_id = 0;
					showparams(hDlg);
					break;
			}
			break;
		case IDCANCEL:
		case IDIGNORE:
			EndDialog(hDlg, 0);
	}
}




static BOOL MyWnd_PAINT(HWND hDlg)
{
	//PaintBlock(hCtrlBlock);
	showparams(hDlg);
	return TRUE;
}

static BOOL MyWnd_DESTROY(HWND hDlg)
{
	hDlgGenerators = 0;
	InvalidateRect( masterhwnd, NULL, TRUE );

	return TRUE;
}

static BOOL MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	return TRUE;
}




static void MyWnd_MOVE(HWND hDlg, int x, int y)
{
	RECT rect;
	char buf[32];
	//((fn)((hDlg), (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)

			GetWindowRect(hDlg, &rect);
			wsprintf((LPSTR)buf,"%i",rect.left);
			WriteProfileString((LPSTR)szMyAppName,(LPSTR)"GENERATORSX",(LPSTR)buf);
			wsprintf((LPSTR)buf,"%i",rect.top);
			WriteProfileString((LPSTR)szMyAppName,(LPSTR)"GENERATORSY",(LPSTR)buf);

	showparams(hDlg);
}
/*
static BOOL MyWnd_MOVE(HWND hDlg, int x, int y)
{
	InvalidateRect(hDlg,NULL,FALSE);
	UpdateWindow(hDlg);
	return TRUE;
}
*/



extern BOOL CALLBACK GeneratorsProc (HWND hDlg, UINT message,
												UINT wParam, LONG lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,MyWnd_INITDIALOG);
		HANDLE_MSG(hDlg,WM_PAINT,MyWnd_PAINT);
		HANDLE_MSG(hDlg,WM_MOVE,MyWnd_MOVE); //Need this or the long button text doesn't come back.
		HANDLE_MSG(hDlg,WM_COMMAND,MyWnd_COMMAND);
		HANDLE_MSG(hDlg,WM_CLOSE,MyWnd_CLOSE);
		HANDLE_MSG(hDlg,WM_DESTROY,MyWnd_DESTROY);
	case WM_NOTIFY:
			return HandleUpDownControlGenerators(hDlg, message, wParam, lParam);
		default:
			return FALSE;
	}
}


BOOL HandleUpDownControlGenerators(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	float value;
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 	//focusGen = calife_list->FocusCA()->generatorlist.Count() - 1;
	switch ( pnmud->hdr.idFrom )
	{

		case IDC_GENERATORS_SPIN_OMEGA:  //frequency 
			if ( pnmud->iDelta > 0 )
			{	
				value = calife_list->FocusCA()->generatorlist.GetOmega(focusGen);
//				calife_list->FocusCA()->generatorlist.SetOmega(focusGen, (value - 0.1));
				calife_list->FocusCA()->generatorlist.SetSmoothOmega(focusGen, (value - 0.1));

				showparams(hDlg);
			}
			else
			{
				value = calife_list->FocusCA()->generatorlist.GetOmega(focusGen);
				calife_list->FocusCA()->generatorlist.SetSmoothOmega(focusGen, (value + 0.1));

				showparams(hDlg);
			}
			break;
			
		case IDC_GENERATORS_SPIN_AMP:
			if ( pnmud->iDelta > 0 )
			{	
				
				calife_list->FocusCA()->generatorlist.SetAmplitude(focusGen, calife_list->FocusCA()->generatorlist.GetAmplitude(focusGen)-.1);	
				showparams(hDlg);
			}
			else
			{
				calife_list->FocusCA()->generatorlist.SetAmplitude(focusGen, calife_list->FocusCA()->generatorlist.GetAmplitude(focusGen)+.1);
				showparams(hDlg);
			}
			break;

		}  // end switch
	// We must return true so that the position remain the same value
	return TRUE;

}


void showparams(HWND hDlg)
{
	int i;
	char buffer[64], buffer1[64], buffer2[64];
	float value;

	if (edit_id)
		return; // This bails on regressive showparams calls from WM_COMMAND.

	// if not editing any edit box, update!!!		



		//fill the list box
	SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_RESETCONTENT,0,0L);
	for (i=0; i < calife_list->FocusCA()->generatorlist.Count(); i++)
	{
		if(calife_list->FocusCA()->Getdimension()==1)
			_ltoa(calife_list->FocusCA()->generatorlist.Location(i)  ,buffer, 10);
		else if(calife_list->FocusCA()->Getdimension()==2)//2D
		{
			_ltoa(calife_list->FocusCA()->generatorlist.Location_x(i)  ,buffer1, 10);
			_ltoa(calife_list->FocusCA()->generatorlist.Location_y(i)  ,buffer2, 10);
			wsprintf((LPSTR) buffer, "%2s %4s %2s",(LPSTR)buffer1, (LPSTR)"    ", (LPSTR)buffer2);
		}
		SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_ADDSTRING, 0, (LONG)(LPSTR)buffer);
		SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_SETITEMDATA, i, (LPARAM) i); 
		SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_SETCURSEL, focusGen, 0L); //highlights list box sellection
	}
	
	
	if(focusGen >= 0)// fill the edit boxes
	{
		value = calife_list->FocusCA()->generatorlist.GetOmega(focusGen);		
		realLabel (hDlg, IDC_GENERATORS_OMEGA, value);
		realLabel (hDlg, IDC_GENERATORS_AMP, calife_list->FocusCA()->generatorlist.GetAmplitude(focusGen));
		SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_SETITEMDATA, focusGen, (LPARAM) focusGen); 
		SendMessage(GetDlgItem(hDlg, IDC_GENERATORS_LIST1), LB_GETTEXT,(WPARAM)(focusGen),(LPARAM)(LPCTSTR)szIndex);	
		SetDlgItemText(hDlg, IDC_GENERATORS_INDEX, (LPSTR)szIndex);
	}
	else
	{
		realLabel (hDlg, IDC_GENERATORS_OMEGA, 0.0);
		realLabel (hDlg, IDC_GENERATORS_AMP, 0.0);
		SetDlgItemText(hDlg, IDC_GENERATORS_INDEX, " ");
	}

	edit_id = 0;  //This undoes the unwanted setting of edit_id by SetWindowText.
}



 
