/*******************************************************************************
	FILE:				about.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	Provides an about box for CAPOW!

	UPDATE LOG:			
				
*******************************************************************************/
//====================INCLUDES===============

#include "ca.hpp"
#include "resource.h"

//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============
//====================GLOBAL DATA===============

static HWND hCtrlBlock;
static void PaintBlock(HWND hCntrlBlock);

//====================LOCAL FUNCTIONS ===============
//====================EXTERNAL DATA===============

extern class CAlist *calife_list;

//====================EXTERNAL FUNCTIONS===============
//------------------------ Message Processing -----------------------//
// Message Cracker put in by Chi Pan Lao at 9/10/96

static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	hCtrlBlock = GetDlgItem(hDlg, IDD_PROGICON);
	InvalidateRect(hDlg, NULL, FALSE);
	UpdateWindow(hDlg);
	return TRUE;
}

static void  MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
		case IDOK:
		case IDCANCEL:
		case IDIGNORE:
			EndDialog(hDlg, 0);
	}
}

static BOOL MyWnd_PAINT(HWND hDlg)
{
	PaintBlock(hCtrlBlock);
	return TRUE;
}

static BOOL MyWnd_DESTROY(HWND hDlg)
{
	return TRUE;
}

static BOOL MyWnd_CLOSE(HWND hDlg)
{
	EndDialog(hDlg,TRUE);
	return TRUE;
}

static BOOL MyWnd_MOVE(HWND hDlg, int x, int y)
{
	InvalidateRect(hDlg,NULL,FALSE);
	UpdateWindow(hDlg);
	return TRUE;
}

extern BOOL CALLBACK AboutProc (HWND hDlg, UINT message,
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
		default:
			return FALSE;
	}
}

static void PaintBlock(HWND hwnd)
{
	HDC hdc, hdcMem;
	HBITMAP	hBitmap, oldhBitmap;
	BITMAP	bm;
	POINT		ptSize;
	HPALETTE old_hpal;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	hBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hCtrlBlock, GWL_HINSTANCE),
			"AboutBMP");
	if (calife_list->numcolor() == 256)
	{
		old_hpal = SelectPalette(hdc, calife_list->hpal(), 0);
			RealizePalette(hdc);
	}
	hdcMem = CreateCompatibleDC(hdc);
	oldhBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
	GetObject(hBitmap, sizeof (BITMAP), (LPSTR) &bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdc, &ptSize, 1);
	BitBlt(hdc, 0, 0, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
	if (calife_list->numcolor() == 256)
	SelectPalette(hdc, old_hpal, 0);
	SelectObject(hdcMem, oldhBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(hCtrlBlock, hdc);
	DeleteObject(hBitmap);
	EndPaint(hwnd, &ps);
}