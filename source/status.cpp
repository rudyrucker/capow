/*-------------------------------------------
   STATBAR.C -- Status bar helper functions.
                (c) Paul Yao, 1996
  -------------------------------------------*/
#include "types.h"
#include <commctrl.h>
#include "status.hpp"
#include "resource.h"

typedef struct tagPOPUPSTRING
     {
     HMENU hMenu ;
	 UINT uiString ;
     } POPUPSTRING ;

#define MAX_MENUS 4
#define NPARTS 3

static POPUPSTRING popstr[MAX_MENUS] ;

DWORD dwStatusBarStyles = WS_CHILD | WS_VISIBLE | 
                          WS_CLIPSIBLINGS | CCS_BOTTOM |
                          SBARS_SIZEGRIP ;

extern HINSTANCE hInst ;
extern HWND hwndStatusBar ;

//-------------------------------------------------------------------
HWND InitStatusBar (HWND hwndParent)
     {
     HWND hwndSB ;

     // Initialize values for WM_MENUSELECT message handling
     HMENU hMenu = GetMenu (hwndParent) ;
     HMENU hMenuFile  = GetSubMenu (hMenu, 0) ;
     HMENU hMenuTools = GetSubMenu (hMenu, 4) ;

	 popstr[0].hMenu    = 0 ;
     popstr[0].uiString = 0 ;

     popstr[1].hMenu    = hMenu ;
     popstr[1].uiString = IDS_MAIN_MENU;

     popstr[2].hMenu    = hMenuFile;
     popstr[2].uiString = IDS_FILE_MENU;

     popstr[3].hMenu    = hMenuTools;
     popstr[3].uiString = IDS_TOOLS_MENU;

     hwndSB = CreateStatusWindow (dwStatusBarStyles,
                                  "Ready",
                                  hwndParent,
                                  2) ;

	 StatusBarMessage (hwndSB, SB_SETPARTS);
     
	 return hwndSB ;
     }

//-------------------------------------------------------------------


void static FlipStyleFlag (LPDWORD dwStyle, DWORD flag)
     {
     if (*dwStyle & flag)  // Flag on -- turn off
          {
          *dwStyle &= (~flag) ;
          }
     else                  // Flag off -- turn on
          {
          *dwStyle |= flag ;
          }
     }
//-------------------------------------------------------------------
HWND RebuildStatusBar (HWND hwndParent, WORD wFlag)
     {
     HWND hwndSB ;
     RECT r ;
/*
     switch (wFlag)
          {
          }
*/
     hwndSB = InitStatusBar (hwndParent) ;

     // Post parent a WM_SIZE message to resize children
     GetClientRect (hwndParent, &r) ;
     PostMessage (hwndParent, WM_SIZE, 0, 
                  MAKELPARAM (r.right, r.bottom)) ;

     return hwndSB ;
     }

void StatusBarMessage (HWND hwndSB, WORD wMsg)
{
	switch (wMsg)
    {
		case SB_SETPARTS:
			{
				int aWidths[NPARTS]; // -1 is default for a part
			
				//These give the right hand edges of the little boxes.
				aWidths[0] = 70; //70
				aWidths[1] = 300; //was 200, not wide enough
				aWidths[2] = 400; //was 300, keep 100 more than aWidths[1].
                Status_SetParts(hwndSB, NPARTS, aWidths);
            }
			break ;
            

		case SB_SIMPLE:
			{
				static BOOL bSimple = TRUE ;
				Status_Simple (hwndSB, bSimple) ;
				bSimple = (!bSimple) ;
				break ;
            }
	} // End Switch
}

//-------------------------------------------------------------------
LRESULT 
Statusbar_MenuSelect (HWND hwnd, WPARAM wParam, LPARAM lParam)
     {
     UINT fuFlags = (UINT) HIWORD (wParam) ;
     HMENU hMainMenu = NULL ;
     int iMenu = 0 ;

     // Handle non-system popup menu descriptions.
     if ((fuFlags & MF_POPUP) &&
         (!(fuFlags & MF_SYSMENU)))
          {
          for (iMenu = 1 ; iMenu < MAX_MENUS ; iMenu++)
               {
               if ((HMENU) lParam == popstr[iMenu].hMenu)
                    {
                    hMainMenu = (HMENU) lParam ;
                    break ;
                    }
               }
          }

     // Display helpful text in status bar
     MenuHelp (WM_MENUSELECT, wParam, lParam, hMainMenu, hInst, 
               hwndStatusBar, (UINT *) &popstr[iMenu]) ;

     return 0 ;
     }
