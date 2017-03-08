//Status.h
#ifndef STATUS_H
#define STATUS_H

//-------------------------------------------------------------------
// Status Bar Helper Macros   Petzold Chap. 12
//-------------------------------------------------------------------
#define Status_GetBorders(hwnd, aBorders) \
    (BOOL)SendMessage((hwnd), SB_GETBORDERS, 0, (LPARAM) (LPINT) aBorders)

#define Status_GetParts(hwnd, nParts, aRightCoord) \
    (int)SendMessage((hwnd), SB_GETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aRightCoord)

#define Status_GetRect(hwnd, iPart, lprc) \
    (BOOL)SendMessage((hwnd), SB_GETRECT, (WPARAM) iPart, (LPARAM) (LPRECT) lprc)

#define Status_GetText(hwnd, iPart, szText) \
    (DWORD)SendMessage((hwnd), SB_GETTEXT, (WPARAM) iPart, (LPARAM) (LPSTR) szText)

#define Status_GetTextLength(hwnd, iPart) \
    (DWORD)SendMessage((hwnd), SB_GETTEXTLENGTH, (WPARAM) iPart, 0L)

#define Status_SetMinHeight(hwnd, minHeight) \
    (void)SendMessage((hwnd), SB_SETMINHEIGHT, (WPARAM) minHeight, 0L)

#define Status_SetParts(hwnd, nParts, aWidths) \
    (BOOL)SendMessage((hwnd), SB_SETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aWidths)

#define Status_SetText(hwnd, iPart, uType, szText) \
    (BOOL)SendMessage((hwnd), SB_SETTEXT, (WPARAM) (iPart | uType), (LPARAM) (LPSTR) szText)

#define Status_Simple(hwnd, fSimple) \
    (BOOL)SendMessage((hwnd), SB_SIMPLE, (WPARAM) (BOOL) fSimple, 0L)



HWND InitStatusBar ( HWND hwndParent );
HWND RebuildStatusBar ( HWND hwndParent, WORD wFLAG );
void StatusBarMessage ( HWND hwndSB, WORD wMsg );
LRESULT Statusbar_MenuSelect (HWND hwnd, WPARAM wParam, LPARAM lParam);
#endif  // STATUS_H