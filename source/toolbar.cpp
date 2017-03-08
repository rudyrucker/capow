#include "types.h"
#include <commctrl.h>
#include "comcthlp.h"
#include "GUI.hpp"
#include "CA.hpp"
#include "resource.h"


extern HINSTANCE hInst ;
extern short focusflag;
extern int  cursormode;
extern short WhichToolBar;
extern HWND masterhwnd;
HWND hwndToolTip;
HWND hwndTB;



int cyToolBar;

DWORD dwToolBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                        CCS_TOP | CCS_NODIVIDER | TBSTYLE_TOOLTIPS ;

extern HINSTANCE hInst ;

//You can change the button order by shuffle of these 
TBBUTTON OldToolbar[] =
{
	//2017 Commented out a lot of the buttons that aren't much used...to simplify.  Can 
	// still open these dialogs from Control menu.
    BUT_WORLDLARGE,      IDM_WORLD,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	BUT_VIEWLARGE,       IDM_VIEW,       TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,2,
	BUT_3DLARGE,         IDM_OPENGL,     TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,11,	
	BUT_USERDIALOGLARGE, IDM_USERDIALOG, TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,10,
//	BUT_ANALOGLARGE,     IDM_ANALOG,     TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,4,
	BUT_COLORLARGE,      IDM_COLOR,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,1,
//	BUT_DIGITALLARGE,    IDM_DIGITAL,    TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,3,
//	BUT_BREEDINGLARGE,   IDM_CYCLE,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,8,
	// BUT_DIALOG_SWAP,     IDM_SWAP ,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,12, 
	// 2017 Drop  Action	Toolbar
	//	0,                   0,              TBSTATE_ENABLED, TBSTYLE_SEP,   0,0,0,0, // Is this for a space?
//	BUT_RAND,            IDM_CONFIGURE,  TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,11, // Randomize			
//	BUT_ELECTRICLARGE,   IDM_ELECTRIC,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,5,
	BUT_GENERATOR,       IDM_GENERATORS, TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,5,  // Generator
//	BUT_FOURIERLARGE,    IDM_FOURIER,    TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,6,
	BUT_CLONELARGE,      IDM_CELL,       TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,7,
//	BUT_EXPERIMENTLARGE, IDM_EXP,        TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,9,
//	BUT_CLOSELARGE,      IDM_CLOSE,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,12,
	0,                   0,              TBSTATE_ENABLED, TBSTYLE_SEP,   0,0,0,0,


};

TBBUTTON FileSection[] =
{
    STD_FILEOPEN,     IDM_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	//BUT_OPENDLLLARGE, IDM_LOADRULEFOCUS,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	STD_FILESAVE,     IDM_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,


};

TBBUTTON CursorSection[] =
{

	0,                0,                   TBSTATE_ENABLED, TBSTYLE_SEP,   0,0,0,0,
	BUT_POINTZOOMLARGE,  CUR_PICK, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP,0,0,0,0,
	BUT_TOUCHLARGE,      CUR_TOUCH, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP,0,0,0,0,
	STD_COPY,            CUR_ZAP,  TBSTATE_ENABLED, TBSTYLE_CHECKGROUP,0,0,0,0,
	BUT_GENERATORCUR,	 CUR_GENERATOR,  TBSTATE_ENABLED, TBSTYLE_CHECKGROUP,0,0,0,0,
	0,                   0,              TBSTATE_ENABLED, TBSTYLE_SEP,   0,0,0,0,
	BUT_CLOSE,      IDM_CLOSE,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,12,
	BUT_ACTION_SWAP,     IDM_SWAP ,      TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,12,
//	STD_HELP,              IDM_HELP,        TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,

};

TBBUTTON ChangeSection[] =
{

	0,                     0,               TBSTATE_ENABLED, TBSTYLE_SEP,   0,0,0,0,
	BUT_CATYPEMENU,  IDM_CATYPE_MENU,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	BUT_VIEWMENU,  IDM_VIEW_MENU,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	BUT_SEEDMENU,  IDM_SEED_MENU,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
	BUT_CHANGEALLLARGE,    IDM_CHANGEALL,   TBSTATE_ENABLED, TBSTYLE_BUTTON,0,0,0,0,
//	BUT_CHANGEFOCUSLARGE,  IDM_CHANGEFOCUS, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP,0,0,0,0,
};


LRESULT ToolBarNotify(HWND hwnd, int idForm, NMHDR  * pnmhdr)
{
 	switch(pnmhdr->code)
    {
  	case TTN_NEEDTEXT:
      	LPTOOLTIPTEXT ToolTip = LPTOOLTIPTEXT(pnmhdr);
         switch(idForm)
         {
		 case IDM_GENERATORS:
			 	ToolTip->lpszText = "Generators";
				break;
		case IDM_CONFIGURE:
			 	ToolTip->lpszText = "Randomize";
				break;

			case IDM_SEED_MENU:
			 	ToolTip->lpszText = "Seed CAs";
				break;

			case IDM_VIEW_MENU:
			 	ToolTip->lpszText = "Change View";
				break;

			case IDM_CATYPE_MENU:
			 	ToolTip->lpszText = "Change CA Type";
				break;
			case IDM_SWAP:
			 //	ToolTip->lpszText = "Swap Toolbars"; // 2017 Drop the Action Toolbar
				break;
		     case IDM_HELP:
				ToolTip->lpszText = "Help";
				break;
			case 	 CUR_ZAP:
				ToolTip->lpszText = "Copy Mutate Cursor";
				break;
			case IDM_RANDOMIZE:
				ToolTip->lpszText = "Randomize CAs";
				break;
			case CUR_PICK:
				ToolTip->lpszText = "Pick & Zoom Cursor";
				break;
			case CUR_TOUCH:
				ToolTip->lpszText = "Touch Cursor";
				break;
			case IDM_CHANGEALL:
 				ToolTip->lpszText = "Change All";
				break;
			case IDM_CHANGEFOCUS:
 				ToolTip->lpszText = "Change Focus";
				break;
			case IDM_LOADRULEFOCUS:
				ToolTip->lpszText = "Load User Rule";
				break;
   			case IDM_OPEN_INDIVIDUAL:
               ToolTip->lpszText = "Open CA";
               break;
			case IDM_SAVE_INDIVIDUAL:
               ToolTip->lpszText = "Save CA";
               break;
			
			case IDM_CYCLE:
            	ToolTip->lpszText = "Breeding";
               break;
          	case IDM_COLOR:
            	ToolTip->lpszText = "Color";
               break;
          	case IDM_EXP:
            	ToolTip->lpszText = "Grid Info";
               break;
          	case IDM_FOURIER:
            	ToolTip->lpszText = "Fourier";
               break;
          	case IDM_ANALOG:
            	ToolTip->lpszText = "Analog";
               break;
          	case IDM_ELECTRIC:
            	ToolTip->lpszText = "Electric";
               break;
          	case IDM_DIGITAL:
            	ToolTip->lpszText = "Digital";
               break;
          	case IDM_CELL:
            	ToolTip->lpszText = "Clone";
               break;
          	case IDM_VIEW:
            	ToolTip->lpszText = "View";
               break;
          	case IDM_WORLD:
            	ToolTip->lpszText = "World";
               break;
			case IDM_USERDIALOG:
				ToolTip->lpszText = "User Dialog";
               break;
          	case IDM_CLOSE:
            	ToolTip->lpszText = "Close Dialog";
				break;
			case IDM_OPENGL:
				ToolTip->lpszText = "3-D View Controls";
               break;
         }
	}
   return 1;
}
//-------------------------------------------------------------------
/*
HWND InitToolBar (HWND hwndParent )
{
	if ( WhichToolBar )  // We want the new Toolbar 
	{
		RECT r;
		int x,y,cx,cy,boxindex;
		LPTBBUTTON newTBSection1, newTBSection2, newTBSection3;
		TBADDBITMAP  TBsection1 = { HINST_COMMCTRL, IDB_STD_LARGE_COLOR, };
		TBADDBITMAP  TBsection2 = { hInst, IDB_BITMAP3, };
	
		newTBSection1 = &FileSection[0];
		newTBSection2= &CursorSection[0];
		newTBSection3= &ChangeSection[0];

		hwndTB = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
				  0,0,0,0, hwndParent, (HMENU) 1, hInst, 0 );                
		ToolBar_ButtonStructSize(hwndTB);
	
		int butcount1, butcount2, butcount3;
		butcount1 = sizeof ( FileSection ) / sizeof ( TBBUTTON );
		butcount2 = sizeof ( CursorSection ) / sizeof ( TBBUTTON );
		butcount3 = sizeof ( ChangeSection ) / sizeof ( TBBUTTON );
		
		ToolBar_AddBitmap(hwndTB, 14, &TBsection1 );
		ToolBar_AddBitmap(hwndTB, 5, &TBsection2 );
		ToolBar_AddButtons(hwndTB, butcount1, newTBSection1);		    
		ToolBar_AddButtons(hwndTB, butcount3, newTBSection3);		    
		ToolBar_AddButtons(hwndTB, butcount2, newTBSection2);		    

		if ( focusflag )
		{
			ToolBar_ChangeBitmap(hwndTB, IDM_CHANGEFOCUS, BUT_CHANGEFOCUSLARGE);
			ToolBar_SetCmdID(hwndTB, CHANGEALLFOCUS_BUTTON, IDM_CHANGEFOCUS);
		}
		else
		{
			ToolBar_SetCmdID(hwndTB, CHANGEALLFOCUS_BUTTON, IDM_CHANGEALL);
			ToolBar_ChangeBitmap(hwndTB, IDM_CHANGEALL, BUT_CHANGEALLLARGE);
		
		}
		switch ( cursormode	)
		{
			case CUR_PICK:
				ToolBar_CheckButton(hwndTB, CUR_PICK, TRUE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, FALSE );
				break;
		
			case CUR_TOUCH:
				ToolBar_CheckButton(hwndTB, CUR_PICK, FALSE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, TRUE );
				break;

			case CUR_ZAP:
			case CUR_COPY:
			case CUR_GENERATOR:	
				ToolBar_CheckButton(hwndTB, CUR_PICK, FALSE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, FALSE );
				break;
		}  // End of cursor switch 
			
	}  // END of NEW toolbar creation.
	else  // WE WANT THE OLD TOOLBAR
	{
		int butcount = sizeof ( OldToolbar ) / sizeof (TBBUTTON );
		hwndTB = CreateToolbarEx(hwndParent, dwToolBarStyles,
        ID_TOOLBAR, butcount,
        hInst,
        TOOLBARBMP, OldToolbar, butcount,
        0, 0, 24, 24,
        sizeof(TBBUTTON));
	
	}



return hwndTB ;
}
*/
//-------------------------------------------------------------------

HWND RebuildToolBar (HWND hwndParent, WORD wFlag)
{
     HWND hwndTB ;
     RECT r ;

	 if ( WhichToolBar )
		 hwndTB = InitActionToolBar (hwndParent) ;
	 else
		 hwndTB = InitDialogToolBar (hwndParent) ;

     // Post parent a WM_SIZE message to resize children
     GetClientRect (hwndParent, &r) ;
     PostMessage (hwndParent, WM_SIZE, 0, 
                  MAKELPARAM (r.right, r.bottom)) ;
	
     return hwndTB ;
}

//===================================================
LRESULT CALLBACK toolbarProc(HWND hwndtoolbar, UINT message, WPARAM wParam, 
								LPARAM lParam )
{
	switch( message )
	{
		case WM_COMMAND:
			//return (BOOL) !HANDLE_WM_COMMAND(hDlg,wParam,lParam,MyWnd_COMMAND);
			MessageBox ( masterhwnd, "D","D", MB_OK );
			break;
		
	}
	return DefWindowProc (hwndtoolbar, message, wParam, lParam) ;
}

//====================================================


#if 0
//-------------------------------------------------------------------
void ToolBarMessage (HWND hwndTB, WORD wMsg)
     {
     switch (wMsg)
          {
          case IDM_TB_CHECK :
               {
               int nState = ToolBar_GetState (hwndTB, 1) ;
               BOOL bCheck = (!(nState & TBSTATE_CHECKED)) ;
               ToolBar_CheckButton (hwndTB, 1, bCheck ) ;
               break ;
               }

          case IDM_TB_ENABLE :
               {
               int nState = ToolBar_GetState (hwndTB, 2) ;
               BOOL bEnabled = (!(nState & TBSTATE_ENABLED)) ;
               ToolBar_EnableButton (hwndTB, 2, bEnabled) ;
               break ;
               }

          case IDM_TB_HIDE :
               {
               int nState = ToolBar_GetState (hwndTB, 3) ;
               BOOL bShow = (!(nState & TBSTATE_HIDDEN)) ;
               ToolBar_HideButton (hwndTB, 3, bShow) ;
               break ;
               }

          case IDM_TB_INDETERMINATE :
               {
               int nState = ToolBar_GetState (hwndTB, 4) ;
               BOOL bInd = (!(nState & TBSTATE_INDETERMINATE)) ;
               ToolBar_Indeterminate (hwndTB, 4, bInd) ;
               break ;
               }

          case IDM_TB_PRESS :
               {
               int nState = ToolBar_GetState (hwndTB, 5) ;
               BOOL bPress = (!(nState & TBSTATE_PRESSED)) ;
               ToolBar_PressButton (hwndTB, 5, bPress) ;
               break ;
               }

          case IDM_TB_BUTTONCOUNT :
               {
               int nButtons = ToolBar_ButtonCount (hwndTB) ;
               char ach[80] ;
               wsprintf (ach, "Button Count = %d", nButtons) ;
               MessageBox (GetParent (hwndTB), ach, 
                           "TB_BUTTONCOUNT", MB_OK) ;
               break ;
               }

          case IDM_TB_GETROWS :
               {
               int nRows = ToolBar_GetRows (hwndTB) ;
               char ach[80] ;
               wsprintf (ach, "Row Count = %d", nRows) ;
               MessageBox (GetParent (hwndTB), ach, 
                           "TB_GETROWS", MB_OK) ;
               break ;
               }
          case IDM_TB_CUSTOMIZE :
               // ToolBar_Customize (hwndTB) ;
               SendMessage (hwndTB, TB_CUSTOMIZE, (LPARAM) &tbb[25], 5) ;
               break ;
          }
     }


#endif


HWND InitActionToolBar (HWND hwndParent )
{
		RECT r;
		int x,y,cx,cy,boxindex;
		LPTBBUTTON newTBSection1, newTBSection2, newTBSection3;
		TBADDBITMAP  TBsection1 = { HINST_COMMCTRL, IDB_STD_LARGE_COLOR, };
		TBADDBITMAP  TBsection2 = { hInst, IDB_BITMAP3, };
	
		newTBSection1 = &FileSection[0];
		newTBSection2= &CursorSection[0];
		newTBSection3= &ChangeSection[0];

		hwndTB = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
				  0,0,200,TOOLBARHEIGHT, hwndParent, (HMENU) 1, hInst, 0 );                
		ToolBar_ButtonStructSize(hwndTB);
	
		int butcount1, butcount2, butcount3;
		butcount1 = sizeof ( FileSection ) / sizeof ( TBBUTTON );
		butcount2 = sizeof ( CursorSection ) / sizeof ( TBBUTTON );
		butcount3 = sizeof ( ChangeSection ) / sizeof ( TBBUTTON );
		
		ToolBar_AddBitmap(hwndTB, 14, &TBsection1 );
		ToolBar_AddBitmap(hwndTB, 5, &TBsection2 );
		ToolBar_AddButtons(hwndTB, butcount1, newTBSection1);		    
		ToolBar_AddButtons(hwndTB, butcount3, newTBSection3);		    
		ToolBar_AddButtons(hwndTB, butcount2, newTBSection2);		    

		if ( focusflag )
		{
			ToolBar_ChangeBitmap(hwndTB, IDM_CHANGEFOCUS, BUT_CHANGEFOCUSLARGE);
			ToolBar_SetCmdID(hwndTB, CHANGEALLFOCUS_BUTTON, IDM_CHANGEFOCUS);
		}
		else
		{
			ToolBar_SetCmdID(hwndTB, CHANGEALLFOCUS_BUTTON, IDM_CHANGEALL);
			ToolBar_ChangeBitmap(hwndTB, IDM_CHANGEALL, BUT_CHANGEALLLARGE);
		
		}
		switch ( cursormode	)
		{
			case CUR_PICK:
				ToolBar_CheckButton(hwndTB, CUR_PICK, TRUE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, FALSE );
				break;
		
			case CUR_TOUCH:
				ToolBar_CheckButton(hwndTB, CUR_PICK, FALSE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, TRUE );
				break;

			case CUR_ZAP:
			case CUR_COPY:
			case CUR_GENERATOR:	
				ToolBar_CheckButton(hwndTB, CUR_PICK, FALSE );
				ToolBar_CheckButton(hwndTB, CUR_TOUCH, FALSE );
				break;
		}  // End of cursor switch 

	ShowWindow (hwndTB, SW_HIDE) ;
	return hwndTB ;
}

HWND InitDialogToolBar (HWND hwndParent )
{
		int butcount = sizeof ( OldToolbar ) / sizeof (TBBUTTON );
		hwndTB = CreateToolbarEx(hwndParent, dwToolBarStyles,
        ID_TOOLBAR, butcount,
        hInst,
        TOOLBARBMP, OldToolbar, butcount,
        0, 0, 24, 24,
        sizeof(TBBUTTON));
		ShowWindow (hwndTB, SW_HIDE) ;
		return hwndTB ;
}




