// Toolbar functions.
//Status.h
#ifndef TOOLBAR_H
#define TOOLBAR_H

#define NUMBUTTONS                      13
#define BTNWIDTH                        0
#define BTNHEIGHT                       0
#define BMPWIDTH                        21
#define BMPHEIGHT                       21
#define NUMBITMAPS                      NUMBUTTONS


void InitializeToolBarArray ( TBBUTTON ToolAry[] );
LRESULT ToolBarNotify (HWND hwnd, WPARAM wParam, LPARAM lParam);
HWND InitToolBar (HWND hwndParent );

#endif