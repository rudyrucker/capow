// ***********************************************************************
// File:		userpara.cpp													
// PROJECT:		CAMCOS CAPOW (Cellular Automata for Power Grid Simulation)
// ENVIRONMENT:	Visual C++ 4.0	Win32
//
// The following codes handle user dialog box.
//
// Date: 4/25/97
//
// Edit:
//
// ***********************************************************************
#include <stdio.h>
#include "userpara.hpp"
#include "ca.hpp"
#include "tweakca.hpp"
#include <COMMCTRL.H>			// This following line must be here 
								// compiling error
extern char *szMyAppName;		// Program name
extern char userDialogName[];	// User Dialog name
extern HWND hUserDialog;		// User Dialog handler 
extern HWND masterhwnd;			// Main program handler
extern HINSTANCE hInst;			// Instance of this program
extern CAlist *calife_list;		// CA list

char userDialogName[] = "Mutate User Rule Params - Double-Click Label to Muate";

// If reCreate == true, then the old column is preserve. It is used so 
// that when users click on another CA Window, the same # of column is 
// create. If false, then it calculate the # of column base on the 
// client width. This will keep the dialog box width about the same.
static BOOL reCreate = FALSE;	

// Use by MyWnd_SIZE to resize the window. This prevent re-executing 
// MyWnd_SIZE again.
static BOOL validUserDialog = FALSE;

vector<HWND> userAddHEdit;		// list of edit box handler,   
vector<HWND> userAddHLabel;		// static box handler, and
vector<HWND> userAddHUpDown;	// edit box handler,   
vector<WNDPROC> userAddHOldProc;// window callback procedure for edit boxes 

HWND	hScrollBar		 = NULL;
HWND	groupBox		 = NULL;
HWND	radioChangeAll	 = NULL;
HWND	radioChangeFocus = NULL;
HWND	randomButton	 = NULL;
WNDPROC randomButtonOld;

static int oldX		 = -1;		// last user dialog coordinate and width 
static int oldY		 = -1;		// and height
static int oldWidth  = -1;
static int oldHeight = -1;				
static int buttonHeight	   = 40;// Heigh of the button rectangle
static int numButtonRow    = 1;	// # of button per row
static int numButtonCol	   = 1;	//			   per column
static int lastScrollPost  = 0;	// For scoll bar uses
static int totalScrollLine = 0;	
static int totalCol = 0;		// Total column, row, and ...
static int totalRow = 0;		
static int cyBorder = 0;
static int cxBorder = 0;
static int labelWidth	  = 0;
static int labelHeight	  = 0;
static int editWidth	  = 0;
static int totalUserParam = 0;
static int vscrollWidth = 0;
static int variancePost = 0;	
static int firstVisible = 0;	// First and last index to visible edit box
static int lastVisible  = 0;	// Used to show and hide edit when scroll
static HFONT	hEditFont;
static BOOL		changeAll = TRUE;

LRESULT CALLBACK EditProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
 static void MyWnd_SIZE(HWND hwnd, UINT state, int cx, int cy);

void removeUserParam(CA *activeCA, BOOL removeVariance)
//
// Remove all user tweak param from a CA. IF removeVariance, then remove it also.
// removeVariance is true on destruction.
//
{	
	for(int a = 0; a < activeCA->userParamAdd.size(); a++)
		if (activeCA->userParamAdd[a] != NULL)
			delete (AdditiveTweakParam *)(activeCA->userParamAdd[a]);
	activeCA->userParamAdd.erase(activeCA->userParamAdd.begin(), 
								 activeCA->userParamAdd.end());
	if (!removeVariance)
		(*activeCA->pAddUserParam)(activeCA, "Mutation Strength (0 to 1)",  0.5);
}

void createEdit( )
//
// Create all edit box and set up it font.
//
{
    CA *activeCA = calife_list->FocusCA();
	int size = activeCA->userParamAdd.size();

	for(int count = 0; count < size; count++)
	{	
		AdditiveTweakParam *temp = (AdditiveTweakParam *)
			                                      activeCA->userParamAdd[count];
		LPCSTR msg = temp->Label();
		HWND hwnd = CreateWindow("static", msg, WS_CHILD | WS_VISIBLE | SS_RIGHT,
								 0, 0, 0, 0, hUserDialog, NULL, hInst, NULL);
		userAddHLabel.push_back(hwnd);
		SendMessage(hwnd, WM_SETFONT, (WPARAM) hEditFont, 0L);
		
		HWND upDown;
		hwnd = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | 
						 WS_TABSTOP, 0, 0, 0, 0, hUserDialog, 
						 (HMENU)(count+USEREDITCODEBASE), hInst, NULL);
		upDown = CreateUpDownControl(WS_CHILD | WS_BORDER | WS_VISIBLE | 
						 UDS_ALIGNRIGHT, 0, 0, 0, 0, hUserDialog, 
						 count+size+USEREDITCODEBASE, hInst, hwnd, 100, 0, 50);	
		userAddHEdit.push_back(hwnd);
		userAddHUpDown.push_back(upDown);
		
		// Change edit font
		SendMessage(hwnd, WM_SETFONT, (WPARAM) hEditFont, 0L);
		
		// Save old window callback procedure of edit control windows
		WNDPROC oldEditProc = (WNDPROC) GetWindowLong(hwnd, GWL_WNDPROC);
		// and set to new window callback procedure of edit control. This
		// way we can process the tab and <return> key
		userAddHOldProc.push_back(oldEditProc);
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG) EditProc);

		// Set edit value
		char valueText[80];
		char *pValueText = valueText;
		Real value = activeCA->userParamAdd[count]->Val();
		sprintf(pValueText, "%5.5f", value);
		SetWindowText(hwnd, valueText);	
	}
}

void moveEdit(int offsetX, int offsetY)
//
// Move all edit control to it proper location. Hide if not visiable.
//
{
	int x = offsetX;
	int y = offsetY;
	int count = 0;

    CA *activeCA = calife_list->FocusCA();

	firstVisible = -1;
	lastVisible = -1;
	// Move the label and edit box to it correct position
	for(int count = 0, count2 = 0; count < activeCA->userParamAdd.size(); 
			  count++)
	{
		if( count >= lastScrollPost && count2 < (totalRow * totalCol))
		{	// current edit box and label is visible
			MoveWindow(userAddHLabel[count], x, y, 
					   labelWidth - LESSLABEL, labelHeight, TRUE);
			MoveWindow(userAddHEdit[count], x + labelWidth, y, 
					   editWidth * PERCENTUPDOWN, labelHeight, TRUE);
			MoveWindow(userAddHUpDown[count], 
					   x + labelWidth+editWidth*PERCENTUPDOWN, y,
					   UPDOWNWIDTH, labelHeight, TRUE);
			if (x == offsetX && y == offsetY)
			{
				SetFocus(userAddHEdit[count]);
				firstVisible = count;
			}
	
			x += columnWidth;
			if ( totalCol == 1 || ((count+1) % totalCol == 0 && count != 0) )
			{
				x  = offsetX;
				y += rowHeight;
			}
			count2++;
		}
		else
		{	// current edit box and label is not visible, this occur if the window
			// can't hold all the label and edit box.
			MoveWindow(userAddHLabel[count], 0, 0, 0, 0, TRUE);
			MoveWindow(userAddHEdit[count], 0, 0, 0, 0, TRUE);
			MoveWindow(userAddHUpDown[count], 0, 0, 0, 0, TRUE);
			if (firstVisible != -1 && lastVisible == -1)
				lastVisible = count - 1;
		}
	}
	if (lastVisible == -1)
		lastVisible = count - 1;
}

void createButton()
//
// Create groupbox, button, radio button.
//
{
	groupBox = CreateWindow("button", "Change Which?",
							WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
							0, 0, 0, 0, hUserDialog, (HMENU)(BUTTONIDBASE),
							hInst, NULL);
	SendMessage(groupBox, WM_SETFONT, (WPARAM) hEditFont, 0L);
	radioChangeAll = CreateWindow("button", "Change All",
								  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
								  0, 0, 0, 0, hUserDialog, 
								  (HMENU)(BUTTONIDBASE+1), hInst, NULL);
	SendMessage(radioChangeAll, WM_SETFONT, (WPARAM) hEditFont, 0L);
	radioChangeFocus = CreateWindow("button", "Change Focus",
								  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
								  0, 0, 0, 0, hUserDialog, 
								  (HMENU)(BUTTONIDBASE+2), hInst, NULL);
	SendMessage(radioChangeFocus, WM_SETFONT, (WPARAM) hEditFont, 0L);
	randomButton = CreateWindow("button", "Mutate Params",
							    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								0, 0, 0, 0, hUserDialog, 
								(HMENU)(BUTTONIDBASE+3), hInst, NULL);
	SendMessage(randomButton, WM_SETFONT, (WPARAM) hEditFont, 0L);
	// Save old window callback procedure of edit control windows
	randomButtonOld = (WNDPROC) GetWindowLong(randomButton, GWL_WNDPROC);
	// and set to new window callback procedure of edit control. This
	// way we can process the tab and <return> key
	SetWindowLong(randomButton, GWL_WNDPROC, (LONG) EditProc);
}

void moveButton(int x, int y)
//
// Move button and group to it proper location.
//
{
	RECT		rect;
    GetClientRect(hUserDialog, &rect);
	
	MoveWindow(groupBox, x, y, GROUPBOXWIDTH, GROUPBOXHEIGHT, TRUE);
	MoveWindow(radioChangeAll, 10 + x, 22 + y, 110, labelHeight, TRUE);
	MoveWindow(radioChangeFocus, 10 + x, 24 + labelHeight+y, 110, 
			   labelHeight, TRUE);
	x = ((rect.right - GROUPBOXWIDTH) - BUTTON_WIDTH) / 2;
	MoveWindow(randomButton, x+GROUPBOXWIDTH,  
			   y + (GROUPBOXHEIGHT - BUTTON_HEIGHT) / 2, 
			   BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
}

void recreateUserDialog()
// 
// Recreate user dialog. Will be call when user select a different CA window.
//
{
	if (!hUserDialog) return;	// No user dialog box yet

	validUserDialog = FALSE;	// Don't process resize message yet
	// Free resource from last user dialog box
	for(int i = 0; i < userAddHEdit.size(); i++)
	{	// Free previous window of edit and static control windows
		DestroyWindow(userAddHEdit[i]);
		DestroyWindow(userAddHLabel[i]);
		DestroyWindow(userAddHUpDown[i]);
	}		
	
	// Initialize storage
	userAddHEdit.erase(userAddHEdit.begin(), userAddHEdit.end());
	userAddHLabel.erase(userAddHLabel.begin(), userAddHLabel.end());
	userAddHOldProc.erase(userAddHOldProc.begin(), userAddHOldProc.end());
	userAddHUpDown.erase(userAddHUpDown.begin(), userAddHUpDown.end());

	CA *activeCA = calife_list->FocusCA();

    TEXTMETRIC  tm;
	HDC hDC			= GetDC(hUserDialog);
	hEditFont		= (HFONT) GetStockObject(ANSI_VAR_FONT);
	HFONT oldHFont  = (HFONT) SelectObject(hDC, hEditFont);

	GetTextMetrics(hDC, &tm);
	
	// Calculate the largest width
	labelWidth = MINLABELWIDTH;
	for(int count1 = 0; count1 < activeCA->userParamAdd.size(); count1++)
	{
		AdditiveTweakParam *temp = (AdditiveTweakParam *)
											activeCA->userParamAdd[count1];
		LPCSTR msg = temp->Label();
		int t1 = lstrlen(msg);
		SIZE size;
		GetTextExtentPoint32(hDC, msg, t1, &size);
		if (size.cx+tm.tmAveCharWidth > labelWidth)
			labelWidth = size.cx + tm.tmAveCharWidth;
	}
	ReleaseDC(hUserDialog, hDC);
	
	// Create all edit box with currently select CA
	createEdit();

	// resize the new created user dialog by calling MyWnd_SIZE
	RECT		rect;
    GetClientRect(hUserDialog, &rect);
	
	// Indicate that user dialog is valid
	validUserDialog = TRUE;
	
	reCreate = TRUE;	
	MyWnd_SIZE(hUserDialog, SIZE_RESTORED, rect.right + 1, rect.bottom + 1);
	reCreate = FALSE;
	// Set focus back to master window. This happen when 
	// user change focus CA window
	SetFocus(masterhwnd);	
}

void createUserDialog()
//
// Create user dialog.
//
{
	validUserDialog = FALSE;	// Do start processing WM_RESIZE
	if (hUserDialog)
	{	
		DestroyWindow(hUserDialog);
		hUserDialog = 0;
		return;
	}

    CA *activeCA = calife_list->FocusCA();

	// Get system measurement and calcute some parameter
    TEXTMETRIC  tm;
	HDC hDC			=  GetDC(hUserDialog);
	hEditFont		= (HFONT) GetStockObject(ANSI_VAR_FONT);
	HFONT oldHFont	= (HFONT) SelectObject(hDC, hEditFont);

	GetTextMetrics(hDC, &tm);
	
	// Calculate the largest width of label
	labelWidth = MINLABELWIDTH;
	for(int count1 = 0; count1 < activeCA->userParamAdd.size(); count1++)
	{
		AdditiveTweakParam *temp = (AdditiveTweakParam *)
											activeCA->userParamAdd[count1];
		LPCSTR msg = temp->Label();
		int t1 = lstrlen(msg);
		SIZE size;
		GetTextExtentPoint32(hDC, msg, t1, &size);
		if (size.cx + tm.tmAveCharWidth > labelWidth)
			labelWidth = size.cx + tm.tmAveCharWidth;
	}
	ReleaseDC(hUserDialog, hDC);
	// Calculate system some value
	totalUserParam = activeCA->userParamAdd.size();
	if (totalUserParam == 0)
		labelWidth  = tm.tmAveCharWidth * USERCHARLABELWIDTH;
	labelHeight = tm.tmHeight + 4;
	editWidth   = tm.tmAveCharWidth * USERCHAREDITWIDTH;
	cxBorder = 2 * (GetSystemMetrics(SM_CXBORDER) +
				   GetSystemMetrics(SM_CXSIZEFRAME)) - 2 + 2 * STARTX;
	cyBorder = 2 * (GetSystemMetrics(SM_CYBORDER) + 
	               GetSystemMetrics(SM_CYSIZEFRAME)) - 2 +
		           GetSystemMetrics(SM_CYCAPTION) + 3 * STARTY;
	vscrollWidth = GetSystemMetrics(SM_CXVSCROLL);
	
	lastScrollPost = 0;
	if (oldX == -1)
	{	// First time the program create a user dialog box
		if (totalUserParam == 0)
		{	// no user param, so use default
			totalCol = 1; 
			totalRow = 1;
			totalScrollLine = lastScrollPost = 0;
			buttonHeight	= GROUPBOXHEIGHT;
		}
		else
		{	// construct a user dialog box with TOTALSTARTCOL column
			totalCol = TOTALSTARTCOL;
			totalRow = totalUserParam / totalCol;
			if(totalUserParam % totalCol != 0) totalRow++;
			int screenLine = (GetSystemMetrics(SM_CYSCREEN) - cyBorder) / 
																	labelHeight;
			if (totalRow > screenLine)
			{	// more row of user dialog edit box than the screen 
				// can hold.
				lastScrollPost = 0;
				totalScrollLine = totalUserParam - (totalRow * totalCol);
				if (totalScrollLine <= 0)  // This isn't necessary.
					totalScrollLine = 0;
				totalRow = screenLine;
			}
			// Calculate the height when all button added
			numButtonCol = (totalCol * columnWidth) / BUTTON_WIDTH;
			numButtonRow = TOTALBUTTON / numButtonCol;
			if ( TOTALBUTTON % numButtonCol != 0)
				numButtonRow++;
			buttonHeight = GROUPBOXHEIGHT;
		}
	}
	else if (totalUserParam == 0)
	{	// Not first time, set to default value
		totalScrollLine = 0;
		lastScrollPost  = 0;
	}
	else 
	{
		// Use old totalCol only
		// construct a user dialog box with TOTALSTARTCOL column
		int screenLine = float(oldHeight - cyBorder) / labelHeight;
		int temp = (totalUserParam / totalCol);
		if (totalUserParam % totalCol != 0)
			temp++;
		if (temp > screenLine)
		{	// more row of user dialog edit box than the screen can hold
			totalRow = screenLine;
			lastScrollPost = 0;
			totalScrollLine = totalUserParam - (totalRow * totalCol);
			if (totalScrollLine <= 0)  // This isn't necessary.
				totalScrollLine = 0;
		}
		// Calculate the height when all button added
		numButtonCol = (totalCol * columnWidth) / BUTTON_WIDTH;
		numButtonRow = TOTALBUTTON / numButtonCol;
		if ( TOTALBUTTON % numButtonCol != 0)
			numButtonRow++;
		buttonHeight = GROUPBOXHEIGHT;
	}
	
	// Calculate window width and height
	oldWidth  = cxBorder + totalCol * columnWidth;
	oldHeight = cyBorder + totalRow * rowHeight + buttonHeight;
	if (totalScrollLine != 0)		
		oldWidth += vscrollWidth;	// if Vertical scroll bar added, 
									//   add the extra width of the scroll bar
	if (oldX == -1)	// Use default x, y or old value
		hUserDialog = CreateWindow(userDialogName, userDialogName,  
					WS_POPUPWINDOW | WS_VISIBLE |
					WS_CAPTION | WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 
					oldWidth, oldHeight, masterhwnd, NULL, hInst, NULL);
	else hUserDialog = CreateWindow(userDialogName, userDialogName,  
					WS_POPUPWINDOW | WS_VISIBLE |
					WS_CAPTION | WS_THICKFRAME, oldX, oldY, 
					oldWidth, oldHeight, masterhwnd, NULL, hInst, NULL);
	// Create vertical scroll bar
	RECT rect;
	GetClientRect(hUserDialog, &rect);
	hScrollBar = CreateWindow("Scrollbar", NULL,  
					WS_CHILD | WS_VISIBLE | SBS_VERT | // WS_TABSTOP |
					SBS_RIGHTALIGN | SBS_TOPALIGN,
					rect.right - vscrollWidth - STARTX, STARTY, vscrollWidth, 
					totalRow * rowHeight, hUserDialog, NULL, hInst, NULL);

	if (totalScrollLine == 0)	// hide scroll bar
		ShowWindow(hScrollBar, SW_HIDE);
	
	ShowWindow(hUserDialog, SW_SHOW);
	UpdateWindow(hUserDialog);

	createEdit();
	createButton();
	moveEdit(STARTX, STARTY);
	moveButton(STARTX / 2, rect.bottom - GROUPBOXHEIGHT - labelHeight);
	validUserDialog = TRUE;		// OK to process WM_RESIZE
	if (changeAll)
		CheckRadioButton(hUserDialog, BUTTONIDBASE+1,BUTTONIDBASE+2, 
						 BUTTONIDBASE+1);
	else CheckRadioButton(hUserDialog, BUTTONIDBASE+1,BUTTONIDBASE+2, 
						  BUTTONIDBASE+2);
}

static void MyWnd_SIZE(HWND hwnd, UINT state, int cx, int cy)
//
// Process the resize command. This is complex!!!
//
{
	// current user dialog box is invalid so return. This function get call
	// when the user resize the user dialog box.  I want to resize 
	// them in a certain way. When I, this program call resize 
	// (not the operating system), I just want the resize function to return.
	if (!validUserDialog) return;
	
	// set vaildUserDialog to false so that this function return immediately.
	// We do not want it to execute again. SetWindowPos will send a WM_SIZE 
	// message. This protect call MyWnd_SIZE twice.
	validUserDialog = FALSE;

	CA *activeCA = calife_list->FocusCA();
	if (activeCA == NULL) return;
	
	totalUserParam = activeCA->userParamAdd.size();
	
	// Calcalute new column and row that can be fix with the new resize window
	int newTotalCol;
	if (totalCol == 0 || reCreate == FALSE)
	{	// No user parameter or user resize the dialog box, 
		// so calculate new column
		newTotalCol = (cx - 2 * STARTX) / columnWidth;
		while( newTotalCol * columnWidth < MINWIDTH)
			newTotalCol ++;
	}
	else // use old column value because user either load new rule
		 newTotalCol = totalCol;	
	if (newTotalCol == 0) newTotalCol++;	// total column can't be 0
	// Calculate new total row
	int newTotalRow = totalUserParam / newTotalCol;
	if (totalUserParam % newTotalCol != 0) newTotalRow++;
	if (newTotalRow == 0) newTotalRow++;	// total row can't be 0

	// Fix scroll bar
	lastScrollPost = 0;
	if (newTotalRow * rowHeight > cy - 2 * STARTY - buttonHeight)
	{	// more row than client window can hold, readjust total row
		newTotalRow = (cy - 2 * STARTY - buttonHeight) / rowHeight;
		if (newTotalRow <= 0) newTotalRow = 1;
		// Calculate scroll bar range
		totalScrollLine = totalUserParam - (newTotalCol * newTotalRow);
		if (totalScrollLine <= 0)
			totalScrollLine = 0;
	}
	else totalScrollLine = 0;	// set to 0 to indicate no scroll bar

	// Calculate the height when all button added
	numButtonCol = (newTotalCol * columnWidth) / BUTTON_WIDTH;
	if (numButtonCol == 0)
		numButtonCol = 1;
	numButtonRow = TOTALBUTTON / numButtonCol;
	if ( TOTALBUTTON % numButtonCol != 0) numButtonRow++;
	buttonHeight = GROUPBOXHEIGHT;

	// Update new value
	totalRow = newTotalRow;
	totalCol = newTotalCol;

	// Hide or show vertical scroll bar
	if (totalScrollLine == 0)
		ShowWindow(hScrollBar, SW_HIDE);
	else 
	{
		ShowWindow(hScrollBar, SW_SHOW);
		SetScrollRange(hScrollBar, SB_CTL, 0, totalScrollLine, TRUE);
		SetScrollPos(hScrollBar, SB_CTL, lastScrollPost, TRUE);
	}
	// Save new location of the user dialog, so that when user resize on the
	// upper left corner, the dialog keep the upper left coordinate but adjust
	// the lower right coordinate
	RECT rect;
	GetWindowRect(hUserDialog, &rect);
	oldX = rect.left;
	oldY = rect.top;
	
	// Calculate the entire dialog width and height
	oldWidth  = cxBorder + totalCol * columnWidth;
	oldHeight = cyBorder +  totalRow * rowHeight + buttonHeight;

	if (totalScrollLine != 0)
		oldWidth += vscrollWidth; // If scrollbar add, account for it width
	
	// Readjust the user dialog box, it will not call this function again because
	// set validUserDialog = FALSE
	SetWindowPos(hUserDialog, HWND_BOTTOM, rect.left, rect.top, 
				 oldWidth, oldHeight, 
				 SWP_NOCOPYBITS | SWP_NOZORDER);

	// Move scroll bar
	RECT rect2;
	GetClientRect(hUserDialog, &rect2);
	MoveWindow(hScrollBar, rect2.right - vscrollWidth - STARTX, STARTY, 
			   vscrollWidth, totalRow * rowHeight, TRUE);
	// Move edit box, label, buttons, and etc.
	moveEdit(STARTX, STARTY);
	moveButton(STARTX / 2, rect2.bottom - GROUPBOXHEIGHT  - labelHeight);
	validUserDialog = TRUE;	// OK to process WM_RESIZE or this procedure again
}

static LRESULT CALLBACK EditProc(HWND hwnd, UINT message, 
										UINT wParam, LONG lParam)
//
// This function replace the default edit box call back function. This
// way we can process the TAB and RETURN key.
// 
{
	short n = (short) GetWindowLong(hwnd, GWL_ID);
	if (n == BUTTONIDBASE+3)
	{	// It is the randomize button
		switch (message)
		{
			case WM_KEYDOWN:
				if (wParam == VK_TAB)
				{
					// Set focus to next edit control -- currently on buttons
					SetFocus(userAddHEdit[firstVisible]);
					return 0;
				}
				break;
		}
		return CallWindowProc((WNDPROC) randomButtonOld, 
							  hwnd, message, wParam, lParam);
	}
	else 
	{	// Must be an edit box control
		n -= USEREDITCODEBASE;
		switch (message)
		{
			case WM_KEYDOWN:
				if (wParam == VK_TAB)
				{	// Set focus to next edit control
					if (n == lastVisible)
						// SetFocus(userAddHButton[0]);
						SetFocus(randomButton);
					else SetFocus(userAddHEdit[(n+1) % userAddHEdit.size()]);
				}
				else if (wParam == VK_RETURN)
				{	// Enter press, sent an EN_KILLFOCUS message as signal of 
					//	update
					SendMessage(hUserDialog, WM_COMMAND, 
								MAKEWPARAM(n + USEREDITCODEBASE, EN_KILLFOCUS), 
								LONG(hwnd));
					return 0;
				}
				break;
		}
		if (n < userAddHOldProc.size())
			return CallWindowProc( (WNDPROC) userAddHOldProc[n], hwnd, message,
								   wParam, lParam);
		else return 0;
	}
}

static void MyWnd_DESTROY(HWND hDlg)
//
// Release all resources
//
{
	for(int i = 0; i < userAddHEdit.size(); i++)
	{
		DestroyWindow(userAddHEdit[i]);
		DestroyWindow(userAddHLabel[i]);
		DestroyWindow(userAddHUpDown[i]);
	}
	
	userAddHEdit.erase(userAddHEdit.begin(), userAddHEdit.end());
	userAddHLabel.erase(userAddHLabel.begin(), userAddHLabel.end());
	userAddHOldProc.erase(userAddHOldProc.begin(), userAddHOldProc.end());
	userAddHUpDown.erase(userAddHUpDown.begin(), userAddHUpDown.end());

	DestroyWindow(hScrollBar);
	DestroyWindow(groupBox);
	DestroyWindow(radioChangeAll);
	DestroyWindow(radioChangeFocus);
	DestroyWindow(randomButton);

	hUserDialog		= 0;
	validUserDialog = FALSE;	// Don't process resize message
}

static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
}

static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	return 0;
}

static void MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//
// When edit box lost the focus, we want to update the CA tweak value.
// It is done as follow:
//   When an edit box lost focus it set a message to the parent.
//	 all edit box is defined start from USEREDITCODEBASE. (This base value
//	 is define save from window reserved value.)
//   codeNotify will be the EN_KILLFOCUS and id is the edit window ID.	 	 
//
{
	CA *activeCA = calife_list->FocusCA();
	if (activeCA == NULL) return;
	
	// to avoid general protection fault, size must check first
	if (hwndCtl == randomButton) 
	{	// It is from randomize button
		char valueText[80];
		Real clampedvariance = activeCA->userParamAdd[0]->Val(); //Get the variance under control
		CLAMP(clampedvariance, MIN_VARIANCE, MAX_VARIANCE);
		activeCA->userParamAdd[0]->SetVal(clampedvariance);
		for(int count = 0; count < calife_list->Count(); count++)
		{
			CA *curCA = calife_list->GetCA(count);
		 	  if (changeAll || (!changeAll &&  curCA == activeCA))
			  {
				  curCA->userParamAdd[0]->SetVal(clampedvariance); //Put good variance in all of them.
				  for (int index = 1; index < curCA->userParamAdd.size(); index++) //Skip index 0 which is where variance lives.
				  {  //2017 had activeCA at start of next line which seems wrong..  Changed to curCA.
					  curCA->RandomizeTweakParamPercent(curCA->userParamAdd[index], curCA->userParamAdd[index]->Val(), clampedvariance);
					  sprintf(valueText, "%5.5f", curCA->userParamAdd[index]->Val());
					  SetWindowText(userAddHEdit[index], valueText);
				  }
		  }
		}
	}
	else if (hwndCtl == radioChangeAll)
	{
		changeAll = TRUE;		
	}
	else if (hwndCtl == radioChangeFocus)
	{
		changeAll = FALSE;
	}
	else // to avoid general protection fault, size must check first
		if (id >= USEREDITCODEBASE && 
			id < USEREDITCODEBASE + activeCA->userParamAdd.size() )
	{	// it is an edit box 
		if (codeNotify == EN_KILLFOCUS)
		{	// focus lost
			char valueText[80];
			char *pValueText = valueText;
			
			// Set to new value
			GetWindowText(hwndCtl, pValueText, 79);
			Real value = atof(valueText);
			TweakParam *temp2 = activeCA->userParamAdd[id-USEREDITCODEBASE];
			if (value == 0 && (strcmp(valueText, "0") != 0))
			{
				// Invalid input
			}
			else temp2->SetVal(value);	// try to set to new value
			value = temp2->Val();	// get the actual value
			sprintf(pValueText, "%5.5f", value);
			// updown the actual value to edit box
			SetWindowText(hwndCtl, valueText);	
			if (changeAll)
				for(int count = 0; count < calife_list->Count(); count++)
				{
					CA *curCA = calife_list->GetCA(count);
					if (curCA == calife_list->FocusCA()) continue;
					if (id - USEREDITCODEBASE < curCA->userParamAdd.size())
						curCA->userParamAdd[id-USEREDITCODEBASE]->SetVal(value);	
				}
		}
	}
}

static void MyWnd_MOVE(HWND hwnd, int x, int y)
//
// User dialog box moved, so update it last position. Last position is 
// used to recreating the dialog box if the user close it and open again.
//
{
	if (validUserDialog)
	{
		RECT rect;
		// Must use this function x & y are in client coordinate
		GetWindowRect(hUserDialog, &rect);	
		oldX = rect.left;
		oldY = rect.top;
	}
}

static void MyWnd_VSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos)
//
// User dialog box got scrolled. 
//
{
	for(int index = 0; index < userAddHUpDown.size(); index ++)
	{	// If it is the message send by an updown control, ignore it
		if (hwndCtl == userAddHUpDown[index]) return;
	}
	switch(code)
	{
		case SB_LINEUP:
			// Scroll up by one row, which is "totalCol"
			if (lastScrollPost != 0)
				lastScrollPost = lastScrollPost - totalCol;
			else return;
			break;
		case SB_LINEDOWN:
			// Scroll down by one row, which is "totalCol"
			if (lastScrollPost < totalScrollLine)
				lastScrollPost = lastScrollPost + totalCol;
			else return;
			break;
		case SB_PAGEUP:
			// Scroll up by one page, which is "totalCol * totalRow"
			if ((lastScrollPost - (totalRow*totalCol)) >= 0)
				lastScrollPost -= totalRow*totalCol;
			else lastScrollPost = 0;
			break;
		case SB_PAGEDOWN:
			// Scroll down by one page, which is "totalCol * totalRow"
			if ((lastScrollPost + (totalRow*totalCol)) < totalScrollLine)
				lastScrollPost += totalRow*totalCol;
			else lastScrollPost = totalScrollLine;
			break;
		case SB_THUMBPOSITION:
			// Thumb get drag, change appropriately
			if (pos == lastScrollPost) 
				return;
			if (pos > lastScrollPost)
			{	// move down
				int displace = pos - lastScrollPost;
				if (totalCol != 1)
					displace %= totalCol;
				if (displace > 0)
					lastScrollPost += displace * totalCol;
				else return;		
			}
			else
			{	// move up
				int displace = - pos + lastScrollPost;
				if (totalCol != 1)
					displace %= totalCol;
				if (displace > 0)
					lastScrollPost -= displace * totalCol;
				else return;		
			}
			break;			
		default:
			return;
	}
	SetScrollPos(hScrollBar, SB_CTL, lastScrollPost, TRUE);
		
	CA *activeCA = calife_list->FocusCA();

	SetFocus(hUserDialog);

	moveEdit(STARTX, STARTY);
	SetScrollPos(hwnd, SB_VERT, lastScrollPost, TRUE);
}

BOOL HandleUpDownControlUserParam(HWND hDlg, UINT message, WPARAM wParam, 
								  LPARAM lParam)
//
// Up down control work as follow:
//		alway remain at 50
//		the value or variable change only.
//
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;

	for(int count = 0; count < userAddHUpDown.size(); count++)
		if (pnmud->hdr.hwndFrom == userAddHUpDown[count])
		{	// find the updown HWND
			char valueText[80];
			char *pValueText = valueText;
			
			// Set to new value
			GetWindowText(userAddHEdit[count], pValueText, 79);
			Real value = atof(valueText);
			if (pnmud->iDelta > 0)
				// Click up arrow
				if (value == 0)
					value += 0.1;
				else value *= 2;
			else
				// Click down arrow
				if (value == 0)
					value -= 0.1;
				else value /= 2;
			
			TweakParam *temp2 = calife_list->FocusCA()->userParamAdd[count];
			temp2->SetVal(value);	// Try to set to new value
			value = temp2->Val();	// Get the actual value

			// Update the actual value of the edit box control
			sprintf(pValueText, "%5.5f", value);
			SetWindowText(userAddHEdit[count], valueText);	
			break;
		}
	// We must return true so that the position remain the same value
	return TRUE;
}

void MyWnd_LBUTTONDBCLK(HWND hwnd, BOOL fDoubleClick, int x, int y, 
						UINT keyFlags)
//
// User double click. Check if double on label. If so, randomize.
//
{
	int onCol;
	int onRow;
	RECT		rect;
    GetClientRect(hwnd, &rect);
	onCol = (x - STARTX) / columnWidth;
	onRow = (y - STARTY) / rowHeight;
	int index = onRow * totalCol;
	index += onCol;
	index += firstVisible;	// Skip invisible 
	if (index == variancePost) return;

	CA *activeCA = calife_list->FocusCA();
	for(int count = 0; count < calife_list->Count(); count++)
	{
	  CA *curCA = calife_list->GetCA(count);
	  if (changeAll || (!changeAll &&  curCA == activeCA))
	  {	
		if (index < curCA->userParamAdd.size())
		{
			char valueText[80];
			char *pValueText = valueText;
			// Set to new value
			GetWindowText(userAddHEdit[index], pValueText, 79);
			Real value = atof(valueText);
	
			curCA ->RandomizeTweakParamPercent(curCA ->userParamAdd[index], 
								curCA ->userParamAdd[index]->Val(),
								curCA ->userParamAdd[variancePost]->Val());
			sprintf(valueText, "%5.5f", curCA ->userParamAdd[index]->Val());
			SetWindowText(userAddHEdit[index], valueText);	
		}
	  }
	}
}

LRESULT CALLBACK userDialogProc(HWND hDlg, UINT message, WPARAM wParam, 
								LPARAM lParam )
{
	switch( message )
	{
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg,wParam,lParam,MyWnd_DESTROY);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg,wParam,lParam,MyWnd_CLOSE);
		case WM_SIZE:
			return (BOOL) !HANDLE_WM_SIZE(hDlg,wParam,lParam,MyWnd_SIZE);
		case WM_CREATE:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg,wParam,lParam,
												MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg,wParam,lParam,MyWnd_COMMAND);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg,wParam,lParam,MyWnd_MOVE);
		case WM_VSCROLL:
			return (BOOL) !HANDLE_WM_VSCROLL(hDlg, wParam, lParam, 
										     MyWnd_VSCROLL);
		case WM_NOTIFY:
			return HandleUpDownControlUserParam(hDlg, message, wParam, lParam);
		case WM_LBUTTONDBLCLK:
			return HANDLE_WM_LBUTTONDBLCLK(hDlg, wParam, lParam, 
										   MyWnd_LBUTTONDBCLK);
		case WM_PAINT:
		  {	// Draw rectangle around Label and Edit control
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hDlg, &ps);
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			
			SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));
			SetBkColor(hDC, RGB(192, 192, 192));	
				
			RECT rect;
			GetClientRect(hDlg, &rect);
			Rectangle(hDC, STARTX / 2, STARTY / 2, 
					  rect.right - rect.left - (STARTX /2), 
					  STARTY + totalRow * rowHeight + STARTY / 2);
			EndPaint(hDlg, &ps);
			return 0;
		  }
		default:
			return DefWindowProc (hDlg, message, wParam, lParam) ;
	}
}
