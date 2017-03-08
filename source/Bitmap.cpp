/*******************************************************************************
	FILE:				bitmap.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions to implement our
						bitmap class.

	UPDATE LOG:			
					
*******************************************************************************/
//====================INCLUDES===============

#include "bitmap.hpp"  //includes types.h
#include "ca.hpp"

//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============
//====================GLOBAL DATA===============
//====================LOCAL FUNCTIONS ===============
//====================EXTERNAL DATA===============

extern int  toolBarHeight;
extern int  statusBarHeight;

//====================EXTERNAL FUNCTIONS===============

//=================================================================

//Constructor and destructor =============================

WindowBitmap::WindowBitmap(HWND hwnd) //constructor
{
	HDC hdc;

#ifdef FIXED_640_480
	WBM_CX = 640;
	WBM_CY = 480;
#else
	WBM_CX = GetSystemMetrics(SM_CXSCREEN);
	WBM_CY = GetSystemMetrics(SM_CYSCREEN);
#endif
#ifdef FORCENARROW
	WBM_CX = FORCEXSIZE;
	WBM_CY = FORCEYSIZE;
#endif //FORCENARROW

	hdc = GetDC(hwnd);
	hdcWBM = CreateCompatibleDC(hdc);
	hBitmapWBM = CreateCompatibleBitmap(hdc, WBM_CX, WBM_CY);
	if (!hBitmapWBM)
	{
		MessageBox( hwnd,
			(LPSTR)"Failure in Bitmap Allocation!",
			(LPSTR)"Memory Problems!",
			MB_OK | MB_ICONEXCLAMATION );
		SendMessage( hwnd, WM_DESTROY, 0, 0L );
		return;
	}
	hBitmapWBMOld = (HBITMAP)SelectObject(hdcWBM, hBitmapWBM);
	Clear(hwnd, GetBkColor(hdc)); // paint with blankcolor
	ReleaseDC(hwnd, hdc);
}


WindowBitmap::~WindowBitmap() //destructor
{
	SelectObject(hdcWBM, hBitmapWBMOld);
	DeleteDC(hdcWBM);
     if (hBitmapWBM)
		DeleteObject(hBitmapWBM);
}

//------These act only on hdcWBM-==============================


void WindowBitmap::ClearSection(int left, int top, int right,
	int bottom)
{
	HBRUSH oldWBMbrush, blankbrush;

	blankbrush = CreateSolidBrush(RGB(0,0,0));
	oldWBMbrush = (HBRUSH)SelectObject(hdcWBM, blankbrush);
	PatBlt(hdcWBM, left, top, right-left+1, bottom-top+1, PATCOPY);
	SelectObject(hdcWBM, oldWBMbrush);
	DeleteObject(blankbrush);
}

void WindowBitmap::WBMBumpup(int left, int top, int right, int bottom,
	int length)
{
	BitBlt(hdcWBM, left, top, right-left+1, bottom - length - top + 1,
		hdcWBM, left, top + length, SRCCOPY);
}


void WindowBitmap::WBMBitBlt(HDC hdc, int left, int top, int right,	int bottom)
{
	BitBlt(hdc, left, top, right-left+1, bottom-top+1, hdcWBM, left, top,
		SRCCOPY);
}

void WindowBitmap::WBMWireBlt(HDC hdc, int left, int top, int right,
	int num)
{
#define WIRE_THICKNESS 16
	StretchBlt (hdc, left, top, right-left+1, num*WIRE_THICKNESS,
	 hdcWBM, left, top, right-left+1, num, SRCCOPY);
}

//------These act on hdcWBM and mirror the action in hdc===================

#pragma argsused
HPALETTE WindowBitmap::WBMSelectPalette(HDC hdc, HPALETTE hPal)
{
	HPALETTE oldhPal;
//      HPALETTE oldWBMhPal;

//	oldWBMhPal =
	SelectPalette(hdcWBM, hPal, 0);
	RealizePalette(hdcWBM);
	oldhPal = SelectPalette(hdc, hPal, 0); //The old palette, select it
		//back before you delete the hdc.
	RealizePalette(hdc);
//	assert (oldhPal == oldWBMhPal);
	return oldhPal;
}

void WindowBitmap::WBMRealizePalette(HDC hdc)
{
	RealizePalette(hdc);
	RealizePalette(hdcWBM);
}

void WindowBitmap::Clear(HWND hwnd, COLORREF col)
{
	HBRUSH oldbrush, oldWBMbrush, blankbrush;
	RECT rect;
	HDC hdc;

	blankbrush = CreateSolidBrush(col);
	oldWBMbrush = (HBRUSH)SelectObject(hdcWBM, blankbrush);
	PatBlt(hdcWBM, 0, 0, WBM_CX, WBM_CY, PATCOPY);
	GetClientRect(hwnd, &rect);
	rect.top += toolBarHeight - 1;
	hdc = GetDC(hwnd);
	oldbrush = (HBRUSH)SelectObject(hdc, blankbrush);
	PatBlt(hdc, rect.left, rect.top,	rect.right, rect.bottom - toolBarHeight, PATCOPY);
	SelectObject(hdc, oldbrush);
	SelectObject(hdcWBM, oldWBMbrush);
	DeleteObject(blankbrush);
	ReleaseDC(hwnd,hdc);
}

//Used to draw rectangles in hdc and hdcWBM ==============================

void WindowBitmap::Getpen(HDC hdc, int style, int width, COLORREF col)
{
	currentpen = CreatePen(style, width, col);
	oldpen = (HPEN)SelectObject(hdc, currentpen);
	oldpenWBM =  (HPEN)SelectObject(hdcWBM, currentpen);
}

void WindowBitmap::Discardpen(HDC hdc)
{
	SelectObject(hdcWBM,oldpenWBM);     // l.andrews 11/3/01 reversed the order of the
	SelectObject(hdc,oldpen);           // two SelectObject calls and removed invalid object 
                                       // complaints on 2nd and 11th calls to Discardpen
                                       // I do not understand (must need to undo in reverse order)
	if (currentpen)
		DeleteObject(currentpen);
	//Be careful not to delete it until it's selected
	//out of BOTH the hdc.
}

void WindowBitmap::WBMMoveTo(HDC hdc, int x, int y)
{
	MoveToEx(hdc, x, y, NULL);
	MoveToEx(hdcWBM, x, y, NULL);
}

void WindowBitmap::WBMLineTo(HDC hdc, int x, int y)
{
	LineTo(hdc, x, y);
	LineTo(hdcWBM, x, y);
}

void WindowBitmap::WBMOnlyMoveTo(int x, int y)
{
	MoveToEx(hdcWBM, x, y, NULL);
}

void WindowBitmap::WBMOnlyLineTo(int x, int y)
{
	LineTo(hdcWBM, x, y);
}
extern int divider_width; //Defined in CAPOW.CPP and CASCREEN.CPP
void WindowBitmap::PutRectangle(HDC hdc, int x0, int y0, int x1, int y1,
	COLORREF color)
{
	Getpen(hdc, PS_SOLID, divider_width, color);
	//divider_width = 1 for normal, 3 for screensaver
	WBMMoveTo(hdc, x0, y0);
	WBMLineTo(hdc, x1, y0);
	WBMLineTo(hdc, x1, y1);
	WBMLineTo(hdc, x0, y1);
	WBMLineTo(hdc, x0, y0);
	Discardpen(hdc);
}


