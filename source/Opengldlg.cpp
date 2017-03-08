#include "ca.hpp"
#include "resource.h"
#include "capowgl.hpp"

extern HWND hDlgOpenGL, masterhwnd;
extern char *szMyAppName;
extern CapowGL *capowgl;
#pragma hdrstop
extern class CAlist *calife_list;
extern WindowBitmap *WBM;

extern BOOL zoomviewflag;
extern void realLabel (HWND, int, Real);


unsigned char drop;
// int  oldtype;  //RR 2007 Don't use it.

void ShowOpenGLParams(HWND hDlg);

//static HWND hCtrlBlock;
//static void PaintBlock(HWND hCntrlBlock);

//------------------------ Message Processing -----------------------//
// Message Cracker put in by Chi Pan Lao at 9/10/96

static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	

	HWND hCntl;
//	oldtype = 0; //RR 2007 don't use it

	hCntl = GetDlgItem(hDlg, IDC_OPENGL_GRAPHTYPES);
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"2D Color");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Sheet");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Torus");

	hCntl= GetDlgItem(hDlg, IDC_OPENGL_SURFACE);
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Dots");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Color Dots");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Lines");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Color Lines");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Facets");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Smooth");


	hCntl = GetDlgItem(hDlg, IDC_OPENGL_MATERIAL);
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Multicolor");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Unlit Multicolor");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Emerald");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Jade");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Obsidian");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Pearl");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Ruby");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Turquoise");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Brass");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Bronze");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Chrome");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Copper");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Gold");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Silver");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Black Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Cyan Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Green Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Red Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"White Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Yellow Plastic");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Black Rubber");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Cyan Rubber");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Green Rubber");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Red Rubber");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"White Rubber");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Yellow Rubber");

	hCntl = GetDlgItem(hDlg, IDC_OPENGL_RESOLUTION);
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"High");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Medium");
	SendMessage( hCntl, CB_ADDSTRING, 0, (LONG)"Low");

	ShowOpenGLParams( hDlg);
	//hCtrlBlock = GetDlgItem(hDlg, IDD_PROGICON);
//	InvalidateRect(hDlg, NULL, FALSE);
	//UpdateWindow(hDlg);
	return TRUE;
}

static void  MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	int comboint;
	HDC testhdc;
//	if (!zoomviewflag)
//		return;
	switch (id)
	{
	case SC_UPDATE:
//		ShowOpenGLParams(hDlg);
		InvalidateRect(hDlgOpenGL, NULL, TRUE);
		break;

	case IDC_OPENGL_ZOOM_IN:
		capowgl->ZoomIn();
		break;
	case IDC_OPENGL_ZOOM_OUT:
		capowgl->ZoomOut();
		break;

	case IDC_OPENGL_GRAPHTYPES:
		comboint = SendMessage((HWND)hwndCtl, CB_GETCURSEL, 0, 0L); 
		capowgl->Type(comboint);
		UpdateWindow(hDlg);
		break;

	case IDC_OPENGL_SURFACE:
		comboint = SendMessage((HWND)hwndCtl, CB_GETCURSEL, 0 , 0L);
		capowgl->SurfaceType(comboint);
		UpdateWindow(hDlg);
		break;
	case IDC_OPENGL_MATERIAL:
		comboint = SendMessage((HWND)hwndCtl, CB_GETCURSEL, 0, 0L); 
		capowgl->Material(comboint);
		UpdateWindow(hDlg);
		break;

	case IDC_OPENGL_RESOLUTION:
		comboint = SendMessage((HWND)hwndCtl, CB_GETCURSEL, 0, 0L);
		capowgl->Resolution(comboint);
		UpdateWindow(hDlg);
		break;

	case IDC_OPENGL_PAN_UP:
		capowgl->Pan(0);
		break;
	case IDC_OPENGL_PAN_DOWN:
		capowgl->Pan(1);
		break;
	case IDC_OPENGL_PAN_LEFT:
		capowgl->Pan(2);
		break;
	case IDC_OPENGL_PAN_RIGHT:
		capowgl->Pan(3);
		break;
	case IDC_OPENGL_MOUSE_CA:
	case IDC_OPENGL_ORIENT:
		capowgl->MouseMode(0);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_LIGHT:
		capowgl->MouseMode(1);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_PAN:
	case IDC_OPENGL_PAN:
		capowgl->MouseMode(2);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_POINT:
		capowgl->MouseMode(3);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_ZOOM:
	case IDC_OPENGL_ZOOM:
		capowgl->MouseMode(4);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_HEIGHT:
	case IDC_OPENGL_HEIGHT:
		capowgl->MouseMode(5);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MOUSE_FLY:
	case IDC_OPENGL_FLY:
		capowgl->MouseMode(6);
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_RESET:
		capowgl->Reset();
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_FLAT:
		break;
	case IDC_OPENGL_FLIP_TYPE:  //to let clicking on the bitmap be a switch
		/* Was like this.  the three types are FLATCOLOR, SHEET, TORUS defined as 0, 1, 2.
		I changed this code so Flip works on FLATCOLOR. */
		/*===============
		if (oldtype == 0)  //flat
		{
			oldtype  = capowgl->Type();
			capowgl->Type(0);
		}
		else
		{
			capowgl->Type(oldtype);
			oldtype = 0;
		}
		==================*/
			//New RR 2007 code below.
		if (capowgl->Type() == FLATCOLOR)  //flat
			capowgl->Type(SHEET);
		else // capowgl->Type() is SHEET or TORUS
			capowgl->Type(FLATCOLOR);
		// End New RR 2007 code.
		InvalidateRect(hDlg,NULL,FALSE);
		break;
	case IDC_OPENGL_SHOW_GENERATORS:
		capowgl->ShowGenerators(!capowgl->ShowGenerators());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_ZERO_PLANE:
		capowgl->ZeroPlane(!capowgl->ZeroPlane());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_MAX_PLANE:
		capowgl->MaxPlane(!capowgl->MaxPlane());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_SPIN:
		capowgl->Spin(!capowgl->Spin());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_FLY_SHOW:
		capowgl->ShowFlyPos(!capowgl->ShowFlyPos());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_GLASSES:
		capowgl->ThreeDGlasses(!capowgl->ThreeDGlasses());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_ANTIALIASED:
		capowgl->AntiAliased(!capowgl->AntiAliased());
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_PAUSE:
		SendMessage(masterhwnd, WM_COMMAND, IDM_PAUSE, 0);
		/*this has the effect of calling
			calife_list->ToggleSleep();
			capowgl->FocusIsActive(calife_list->GetSleep());
		*/
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_GLSLEEP: //Rudy added this 12/2/97
		calife_list->ToggleGlSleep();
/*
		if (calife_list->GetGlSleep()) //gl_sleep on
			calife_list->SetGlSleep(0); //turn it off
		else //gl_sleep off
			calife_list->SetGlSleep(1); //TRY to turn it on (you can't if sleep is off)
*/
		ShowOpenGLParams(hDlg);
		break;
	case IDC_OPENGL_TO_VRML:
//		capowgl->CaptureToVRML();  //this one doesn't work
		capowgl->CaptureVRML();
		break;
//		case IDOK:
//		case IDCANCEL:
//		case IDIGNORE:
		EndDialog(hDlg, 0);
	}
}


static BOOL MyWnd_PAINT(HWND hDlg)
{
	HWND hCtrlBlock;
	HDC hdc;
	RECT sourcerect, targetrect;
	//PaintBlock(hCtrlBlock);
	if (WBM)
	{
		//if zoomed on 2-D focus
		if (zoomviewflag&& calife_list->Focus()->Getdimension() ==2)
		{
			hCtrlBlock = GetDlgItem(hDlgOpenGL, IDC_OPENGL_FLAT);
			hdc = GetDC(hCtrlBlock);
			GetWindowRect(hCtrlBlock, &targetrect);
			//get the rectangle of the focus CA
			calife_list->GetFocusRect(&sourcerect);
			//The stretchblt should resize the rect to fit the box in the dialog
			//but it doesn't work.
			StretchBlt(hdc, 0,0, targetrect.left-targetrect.right, targetrect.bottom-targetrect.top,
				WBM->GetHDC(),sourcerect.left, sourcerect.top, sourcerect.left-sourcerect.right, sourcerect.bottom-sourcerect.top, SRCCOPY);
		//	BitBlt(hdc, 0,0, sourcerect.left-sourcerect.right, sourcerect.bottom-sourcerect.top,
		//		WBM->GetHDC(), sourcerect.left, sourcerect.top, SRCCOPY);
			ReleaseDC(hCtrlBlock, hdc);
		}
	}
	ShowOpenGLParams(hDlg);
	return TRUE;
}

static BOOL MyWnd_DESTROY(HWND hDlg)
{
	hDlgOpenGL = 0;
	InvalidateRect( masterhwnd, NULL, FALSE);

	return TRUE;
}

static BOOL MyWnd_LBUTTONDOWN(HWND hDlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RECT rect, rect2;

//((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
	SetCapture(hDlg);
	GetWindowRect(GetDlgItem(hDlg, IDC_OPENGL_FLAT), &rect);
	GetWindowRect(hDlgOpenGL, &rect2);
	capowgl->SetCellXY(x-(rect.left-rect2.left),y-(rect.top-rect2.top));
	if(calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW)
		capowgl->LeftButtonDown(fDoubleClick, x, y, keyFlags);  //mike 4/97	
	return 0;
}

static void MyWnd_MOUSEMOVE(HWND hwnd, int x, int y, UINT flags) 
{
  //  ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
		//if zoomed on a 2D CA, and capowgl is not in fly mode, then interpret the mousemove
		if(zoomviewflag && calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW && capowgl->MouseMode()!= 6)
			capowgl->MouseMove(x, y, flags);

}

static void MyWnd_LBUTTONUP(HWND hwnd, int x, int y, UINT flags)
{
//    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
		if(zoomviewflag && calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW)
			capowgl->LeftButtonUp(x, y, flags);

	ReleaseCapture();

}

static BOOL MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	return TRUE;
}

static void MyWnd_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"CYCLEX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"CYCLEY",(LPSTR)buf);

	UpdateWindow(hDlg);  //Send a WM_PAINT message to dialog

}

static void MyWnd_PARENTNOTIFY(HWND hDlg, UINT fwEvent, HWND lValue, UINT idChild)
{
//WM_PARENTNOTIFY 
//fwEvent = LOWORD(wParam);  // event flags 
//idChild = HIWORD(wParam);  // identifier of child window 
//lValue = lParam;           // child handle, or cursor coordinates 
 
}

extern BOOL CALLBACK OpenGLProc (HWND hDlg, UINT message,
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
		HANDLE_MSG(hDlg, WM_LBUTTONDOWN, MyWnd_LBUTTONDOWN);
		HANDLE_MSG(hDlg, WM_MOUSEMOVE, MyWnd_MOUSEMOVE);
		HANDLE_MSG(hDlg, WM_LBUTTONUP, MyWnd_LBUTTONUP);
		HANDLE_MSG(hDlg, WM_PARENTNOTIFY, MyWnd_PARENTNOTIFY);
	default:
			return FALSE;
	}
}


void ShowOpenGLParams(HWND hDlg)
{
	BOOL GL_active;
	int i;
	if (!SendMessage( GetDlgItem( hDlg, IDC_OPENGL_GRAPHTYPES), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_OPENGL_GRAPHTYPES), 
			CB_SETCURSEL, (WORD)capowgl->Type(), 0);

	if (!SendMessage( GetDlgItem( hDlg, IDC_OPENGL_SURFACE), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_OPENGL_SURFACE), 
			CB_SETCURSEL, (WORD)capowgl->SurfaceType(), 0);

	if (!SendMessage( GetDlgItem( hDlg, IDC_OPENGL_MATERIAL), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_OPENGL_MATERIAL), 
			CB_SETCURSEL, (WORD)capowgl->Material(), 0);

	if (!SendMessage( GetDlgItem( hDlg, IDC_OPENGL_RESOLUTION), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_OPENGL_RESOLUTION), 
			CB_SETCURSEL, (WORD)capowgl->Resolution(), 0);

//	CheckRadioButton( hDlg, IDC_OPENGL_MOUSE_CA, IDC_OPENGL_MOUSE_FLY,
//	  IDC_OPENGL_MOUSE_CA + capowgl->MouseMode() );
	SendMessage(GetDlgItem(hDlg,IDC_OPENGL_ORIENT), BM_SETSTATE, capowgl->MouseMode()==0, 0);
	SendMessage(GetDlgItem(hDlg,IDC_OPENGL_HEIGHT), BM_SETSTATE, capowgl->MouseMode()==5, 0);
	SendMessage(GetDlgItem(hDlg,IDC_OPENGL_ZOOM), BM_SETSTATE, capowgl->MouseMode()==4, 0);
	SendMessage(GetDlgItem(hDlg,IDC_OPENGL_PAN), BM_SETSTATE, capowgl->MouseMode()==2, 0);
	SendMessage(GetDlgItem(hDlg,IDC_OPENGL_FLY), BM_SETSTATE, capowgl->MouseMode()==6, 0);

	CheckDlgButton(hDlg, IDC_OPENGL_SHOW_GENERATORS,capowgl->ShowGenerators());
	CheckDlgButton(hDlg, IDC_OPENGL_ZERO_PLANE, capowgl->ZeroPlane());
	CheckDlgButton(hDlg, IDC_OPENGL_MAX_PLANE, capowgl->MaxPlane());
	CheckDlgButton(hDlg, IDC_OPENGL_SPIN, capowgl->Spin());
	CheckDlgButton(hDlg, IDC_OPENGL_FLY_SHOW, capowgl->ShowFlyPos());
	CheckDlgButton(hDlg, IDC_OPENGL_ANTIALIASED, capowgl->AntiAliased());
	CheckDlgButton(hDlg, IDC_OPENGL_GLASSES, capowgl->ThreeDGlasses());


	realLabel (hDlg, IDC_OPENGL_TEMPWIDTH, capowgl->tempwidth);
	realLabel (hDlg, IDC_OPENGL_TEMPHEIGHT, capowgl->tempheight);

	GL_active = zoomviewflag && calife_list->FocusCA()->Getviewmode() == IDC_2D_VIEW && capowgl->Type();
	
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_GRAPHTYPES ),zoomviewflag && calife_list->FocusCA()->Getviewmode() == IDC_2D_VIEW );
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_MATERIAL ),GL_active );
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_SURFACE), GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_RESOLUTION), GL_active);

	for (i=	 IDC_OPENGL_MOUSE_CA; i<=IDC_OPENGL_MOUSE_FLY; i++)
		EnableWindow( GetDlgItem( hDlg, i ),GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_SHOW_GENERATORS),GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_ZERO_PLANE),GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_MAX_PLANE),GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_SPIN),GL_active);
	EnableWindow( GetDlgItem( hDlg, IDC_OPENGL_FLY_SHOW), GL_active);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_FLIP_TYPE),calife_list->FocusCA()->Getviewmode() == IDC_2D_VIEW);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_RESET),GL_active);
//	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_PAUSE),GL_active);

	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_ORIENT),GL_active);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_HEIGHT),GL_active);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_ZOOM),GL_active);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_PAN),GL_active&&!capowgl->ThreeDGlasses());
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_FLY),GL_active&&!capowgl->ThreeDGlasses());
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_GLASSES),GL_active);
	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_ANTIALIASED),GL_active);

	EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_TO_VRML), GL_active);


	/* Rudy added the OPENGL_SLEEP control.  The idea is that now rendering is by default
paused when the CAs are paused and you need to use the gl_sleep checkbox to turn
it on if you want it in. 12/2/97.  Had persistent bug problems. */
	/* SendMessage(GetDlgItem(hDlg, IDC_OPENGL_PAUSE), BM_SETSTATE, calife_list->GetSleep()==TRUE,0);
	This was the WRONG thing to do, it meant that whenever ShowOpenGLParams got called, this
	button got toggled, for instance when you clicked on the screen.  Rudy fixed this bug by
	Michael Ling on 5/19/98 ---RR. */
		CheckDlgButton(hDlg, IDC_OPENGL_PAUSE, calife_list->GetSleep());
		CheckDlgButton(hDlg, IDC_OPENGL_GLSLEEP, 1-(calife_list->GetGlSleep()));
		/* The checkbox says "Render While CAs Are Paused," which means that
		gl_sleep is 0, so we set check with the complementary value of gl_sleep.*/
		EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_GLSLEEP), GL_active);
#define GRAY_GLSLEEP
#ifdef GRAY_GLSLEEP
	if (!GL_active || !calife_list->GetSleep()) //Not GL_active or not paused
		EnableWindow(GetDlgItem(hDlg, IDC_OPENGL_GLSLEEP), FALSE);
		/* The only way to enable this checkbox is to go to
		pause.*/
#endif //GRAY_GLSLEEP
}

