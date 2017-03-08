/*******************************************************************************
	FILE:				color.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions and data to control
						the analog dialog box and its features.

	UPDATE LOG:			
					9-29-97  Commented Code
*******************************************************************************/
//====================INCLUDES===============

#include "ca.hpp"
#include <math.h>
#include "resource.h"
#include <COMMCTRL.H>
 
//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============
//====================GLOBAL DATA===============

static HWND hCtrlBlock, hCtrlEdit;

//====================LOCAL FUNCTIONS ===============

static void FixDialogMarks(HWND hDlg);
static void PaintBlock(HWND hCntrlBlock);
 
//====================EXTERNAL DATA===============

extern class CAlist *calife_list;
extern BOOL update_flag;
extern char szMyAppName;
extern HWND hDlgColor, masterhwnd;
extern short focusflag;

//====================EXTERNAL FUNCTIONS===============

extern void numlabel(HWND, int, int);




/*------------------------------------------------------------------------------*/
//								Message Crackers
/*------------------------------------------------------------------------------*/
//********************************************************************************
//  MyWnd_INITDIALOG
//
//  Initialize Color Dialog
//  Get a handle for color map, update window


static BOOL MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	hCtrlBlock = GetDlgItem(hDlg, IDD_PAINT);  // Make a handle for the color map
	hCtrlEdit = GetDlgItem(hDlg, IDC_NUMBAND);  // Make a handle edit box
	InvalidateRect(hDlg, NULL, FALSE);
//Need to do this so that the BeginPaint in PaintBlock will have
//an invalid region which includes the color lines.
	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
	UpdateWindow(hDlg);
	
//Calls WM_PAINT, which does PaintBlock and FixDialogMarks(hDlg);
	return TRUE;
}

//********************************************************************************
//  MyWnd_PAINT
//
//  Paints the color map
//  

static BOOL MyWnd_PAINT(HWND hDlg)
{
	PAINTSTRUCT ps;

	//BeginPaint(hDlg, &ps);
	PaintBlock(hCtrlBlock);
	FixDialogMarks(hDlg); 
	//EndPaint(hDlg, &ps);
	return TRUE;
}

//===============================================================
//  MyWnd_CLOSE
//
//  
//  

static BOOL MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	return TRUE;
}

//********************************************************************************
//  MyWnd_DESTROY
//
//   
  
static BOOL MyWnd_DESTROY(HWND hDlg)
{
	hDlgColor = 0;
	InvalidateRect(masterhwnd,NULL,FALSE);
	return TRUE;
}

//********************************************************************************
//  MyWnd_COMMAND
//
//  Handles all the controls
//  

static BOOL MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
	switch (id)
	{
    	case RADIO_ALL:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEALLMENU, 0L);
			break;

		case RADIO_FOCUS:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEFOCUSMENU, 0L);
			break;

		case SC_UPDATE:
			InvalidateRect(hDlg, NULL, FALSE);
//Need to do this so that the BeginPaint in PaintBlock will have
//an invalid region which includes the color lines.
			UpdateWindow(hDlg);
//Calls WM_PAINT, which does PaintBlock and FixDialogMarks(hDlg);
		break;

		case IDC_MONOCHROME:  // Monochrome Check box altered
			if (focusflag)
			{
				calife_list->FocusCA()->Set_monochromeflag(
					1-(calife_list->FocusCA()->Get_monochromeflag()));
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->Set_monochromeflag(
					1-(calife_list->FocusCA()->Get_monochromeflag()));
				calife_list->ResetAllGenerationCount();
			}
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case COL_RAND:
			if(focusflag)
			{
				calife_list->FocusCA()->SetColors();
				calife_list->FocusCA()->ResetGenerationCount();
			}
			else
			{
				calife_list->SetColors();
				calife_list->ResetAllGenerationCount();
			}
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case IDM_16:
			calife_list->Setnumcolor(16);
			InvalidateRect(hDlg, NULL, FALSE);
			calife_list->ResetAllGenerationCount();
			UpdateWindow(hDlg);
			break;

		case IDM_256:
			calife_list->Setnumcolor(256);
			//focusflag = 0;
			InvalidateRect(hDlg, NULL, FALSE);
			calife_list->ResetAllGenerationCount();
			UpdateWindow(hDlg);
			break;

		case IDM_1000:
			calife_list->Setnumcolor(1000); //Really uses as many as available
			InvalidateRect(hDlg, NULL, FALSE);
			calife_list->ResetAllGenerationCount();
			UpdateWindow(hDlg);
			break;

		case IDOK:
		case IDCANCEL:
			DestroyWindow(hDlg);
			break;
	}
   return TRUE;
}
//********************************************************************************
//  HandleUpDownControlColor
//
//  Handles the SPIN control 
//  

BOOL HandleUpDownControlColor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 
	switch ( pnmud->hdr.idFrom )
	{
		case IDC_SPINBANDCOUNT:
			if ( pnmud->iDelta > 0 )
			{
				if (focusflag)
				{
					calife_list->FocusCA()->ResetGenerationCount();
					calife_list->FocusCA()->Change_band_count(-1);
				}
				else
				{
					calife_list->Change_band_count(-1);
					calife_list->ResetAllGenerationCount();
				}
				InvalidateRect(hDlg, NULL, FALSE);
				UpdateWindow(hDlg);
			}
			else
			{
				if (focusflag)
				{

					calife_list->FocusCA()->ResetGenerationCount();
					calife_list->FocusCA()->Change_band_count(+1);
				}
				else
				{
					calife_list->Change_band_count(+1);
					calife_list->ResetAllGenerationCount();
				}
				InvalidateRect(hDlg, NULL, FALSE);
				UpdateWindow(hDlg);
			}
			break;
	} // Switch
	// We must return true so that the position remain the same value
	return TRUE;
}

//********************************************************************************
//  ColorProc
//
//  Main Message interpretor for Color Dialog
//  


extern BOOL CALLBACK ColorProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	switch (message)
	{

		HANDLE_MSG(hDlg,WM_PAINT,MyWnd_PAINT);
		HANDLE_MSG(hDlg,WM_INITDIALOG,MyWnd_INITDIALOG);
		HANDLE_MSG(hDlg,WM_DESTROY,MyWnd_DESTROY);
		HANDLE_MSG(hDlg,WM_CLOSE,MyWnd_CLOSE);
		HANDLE_MSG(hDlg,WM_COMMAND,MyWnd_COMMAND);
		case WM_NOTIFY:
			return HandleUpDownControlColor(hDlg, message, wParam, lParam);

		default:
			return FALSE;
	}
}
//********************************************************************************
//  PaintBlock
//
//  Paints the Color map in the Dialog
//  

void PaintBlock (HWND hwnd)
{
	HDC hdc ;
	HPEN hpen;
	RECT rect ;
	short i;
	COLORREF  *colortable;
	HPALETTE old_hpal;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
//	hdc = GetDC(hwnd);  This doesn't work.
	if (calife_list->numcolor() == 256)
	{
		old_hpal = SelectPalette(hdc, calife_list->hpal(), 0);
		RealizePalette(hdc);
	}
	GetClientRect (hwnd, &rect);
	colortable = calife_list->FocusCA()->GetColortable();
	for (i=0;i<=rect.bottom-1;i++)
	{
	//Windows draws the top line of a rectangle but not the bottom.
		int screen_color =   //ranges from 0 to MAX_COLOR-1
			(((float)(MAX_COLOR-1) *i) /(rect.bottom - 1));
		hpen = CreatePen(PS_SOLID, 1, colortable[screen_color]);
		SelectObject(hdc, hpen);
		MoveToEx(hdc, 0, rect.bottom - i, NULL);
		LineTo(hdc, rect.right, rect.bottom - i);
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DeleteObject(hpen);
	}
	if (calife_list->numcolor() == 256)
		SelectPalette(hdc, old_hpal, 0);
//	ReleaseDC(hwnd, hdc); You need a BeginPaint/EndPaint pair in here.
	EndPaint(hwnd, &ps);
}

//********************************************************************************
//  FixDialogMarks 
//
//  Fixes the dialogs buttons and controls, etc. when they have chanced
//  

static void FixDialogMarks(HWND hDlg)
{
	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
	CheckRadioButton(hDlg,IDM_ALL,IDM_FOCUS, IDM_ALL+focusflag);
	CheckRadioButton(hDlg,IDM_16,IDM_1000,
		(calife_list->numcolor()==16)?IDM_16:
		((calife_list->numcolor()==256)?IDM_256:IDM_1000));
	
		numlabel(hDlg ,IDC_NUMBAND,calife_list->FocusCA()->Getband()-1);
	if (calife_list->FocusCA()->Get_monochromeflag())
	{
		CheckDlgButton(hDlg,IDC_MONOCHROME,1);
		EnableWindow (GetDlgItem (hDlg, COL_RAND), FALSE);
	}
	else
	{
		CheckDlgButton(hDlg,IDC_MONOCHROME,0);
		EnableWindow (GetDlgItem (hDlg, COL_RAND), TRUE);
	}
	if (calife_list->FocusCA()->Get_monochromeflag())
		CheckDlgButton(hDlg,IDC_MONOCHROME,1);
	else
		CheckDlgButton(hDlg,IDC_MONOCHROME,0);
	if (calife_list->numcolor() == 256)
		EnableWindow (GetDlgItem (hDlg, IDM_FOCUS), FALSE);
	else
		EnableWindow (GetDlgItem (hDlg, IDM_FOCUS), TRUE);
	if (calife_list->numcolor() == 16)
		EnableWindow (GetDlgItem (hDlg, IDC_MONOCHROME), FALSE);
	else
		EnableWindow (GetDlgItem (hDlg, IDC_MONOCHROME), TRUE);

}
