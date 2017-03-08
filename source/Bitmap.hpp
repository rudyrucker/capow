/*******************************************************************************
	FILE:				bitmap.hpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains class definitions for
						Bitmap class.

	UPDATE LOG:			
					
*******************************************************************************/


#ifndef BITMAP_HPP
#define BITMAP_HPP
#define BITMAP_HPP
#include "types.h"  

/* The original WindowBitmap class is used to maintain a bitmap copy of your
window for restoring it.  Everything you write to the screen must also
be written to your WindowBitmap.  WindowBitmap uses a constructor that
takes an HWND as param.  To declare it globally, put at the top a global:
WindowBitmap *WBM;
then in WM_CREATE: put
	WBM = new WindowBitmap(hwnd);
and in WM_PAINT: put simply
	WBM->Repaint(hwnd);
	return 0;
and in WM_DESTROPY put
	delete WBM;

 In Feb 28, 1996, we changed this to a DIB bitmap format so as to speed
 up the WBMOnlyPutPixel operation.  At present we get black screen
 with DIBCOLORS 256 and startup crash with DIBCOLORS 1.
May 4, 96, we eliminated the unsuccessful DIB code.
*/



class WindowBitmap
{
private:
	HBITMAP hBitmapWBM, hBitmapWBMOld;
	HDC hdcWBM;
	int WBM_CX, WBM_CY;
	HPEN currentpen, oldpen, oldpenWBM;
public:
//Constructor, Destructor, Accessor============
	WindowBitmap(HWND hwnd); //constructor
	~WindowBitmap(); //destructor
	HDC GetHDC() {return hdcWBM;} //accessor, handy for direct BitBlt.
	int CX(){return WBM_CX;}
	int CY(){return WBM_CY;}
//Act on hdcWBM only========================================
	void ClearSection(int left, int top, int right, int bottom);
	void WBMOnlyPutPixel(int x, int y, COLORREF color)
		{SetPixel(hdcWBM, x, y, color);}
	void WBMBumpup(int left, int top, int right, int bottom,
		int length);
//Transfer from hdcWBM to hdc ==========================
	void Repaint(HWND hwnd);
	void Repaint(HWND hwnd, HPALETTE hpal);
	void WBMBitBlt(HDC hdc, int left, int top, int right,	int bottom);
	void WBMWireBlt(HDC hdc, int left, int top, int right, int num);
//Act on hdcWBM and mirror in hdc =======================
	void Clear(HWND hwnd, COLORREF col);
	HPALETTE WBMSelectPalette(HDC hdc, HPALETTE hPal);
	void WBMRealizePalette(HDC hdc);
//Functions used in to draw a rectanlge in hdc and hdcWBM =============
	void Getpen(HDC hdc, int style, int width, COLORREF col);
	void Discardpen(HDC hdc);
	void WBMMoveTo(HDC hdc, int x, int y);
	void WBMLineTo(HDC hdc, int x, int y);
	void WBMOnlyMoveTo(int x, int y);
	void WBMOnlyLineTo(int x, int y);
	void PutRectangle(HDC hdc, int x0, int y0, int x1, int y1,
		COLORREF color);
};
#endif //BITMAP_HPP