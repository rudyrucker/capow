#define BUT_OPENDLLLARGE     15
#define BUT_CATYPEMENU       16
#define BUT_POINTZOOMLARGE   17
#define BUT_TOUCHLARGE       18
#define BUT_CHANGEALLLARGE   19
#define BUT_CHANGEFOCUSLARGE 20
#define BUT_VIEWMENU		 21
#define BUT_CLOSE		     22
#define BUT_SEEDMENU	     23
#define BUT_ACTION_SWAP      24
#define BUT_GENERATORCUR	 25

#define BUT_WORLDLARGE       0
#define BUT_COLORLARGE       1
#define BUT_VIEWLARGE        2
#define BUT_DIGITALLARGE     3
#define BUT_ANALOGLARGE      4
#define BUT_ELECTRICLARGE    5
#define BUT_FOURIERLARGE     6
#define BUT_CLONELARGE       7
#define BUT_BREEDINGLARGE    8
#define BUT_EXPERIMENTLARGE  9
#define BUT_USERDIALOGLARGE  10
#define BUT_3DLARGE          11
#define BUT_CLOSELARGE       12
#define BUT_DIALOG_SWAP      13
#define BUT_RAND			 14	
#define BUT_GENERATOR		 15


#define CHANGEALLFOCUS_BUTTON 6

/// GUI.hpp  contains prototypes and information related to
// toolbar, statusbar, and tooltips.

// Toolbar functions.
HWND InitActionToolBar (HWND hwndParent) ;
HWND InitDialogToolBar (HWND hwndParent) ;
HWND RebuildToolBar (HWND hwndParent, WORD wFlag);
void ToolBarMessage (HWND hwndTB, WORD wMsg) ;
LRESULT ToolBarNotify (HWND hwnd, WPARAM wParam, LPARAM lParam) ;

// Tooltip functions.
BOOL InitToolTip (HWND hwndToolBar, HWND hwndComboBox) ;
BOOL RelayToolTipMessage (LPMSG pMsg) ;
void CopyToolTipText (LPTOOLTIPTEXT lpttt) ;

// Status bar functions.
HWND InitStatusBar (HWND hwndParent) ;
HWND RebuildStatusBar (HWND hwndParent, WORD wFlag) ;
void StatusBarMessage (HWND hwndSB, WORD wMsg) ;
LRESULT Statusbar_MenuSelect (HWND, WPARAM, LPARAM) ;

