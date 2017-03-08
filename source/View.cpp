#include "ca.hpp"
#include "resource.h"
#include "status.hpp"
extern char *szMyAppName;
extern HWND hDlgView, masterhwnd;
extern class CAlist *calife_list;
extern int zoomviewflag;
extern HWND hwndStatusBar; 

static void showparams(HWND);
extern short focusflag;


/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
// 							Message Cracker
 
#pragma argsused
static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	HINSTANCE hInstance;
	
//	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );
	hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
	showparams( hDlg );
	return 0;
}

#pragma argsused
static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgView = 0;
			InvalidateRect( masterhwnd, NULL, FALSE );

}


#pragma argsused
static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	
}



#pragma argsused
static void MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
			switch( id )
			{
		     	case RADIO_ALL:
					SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEALLMENU, 0L);
					break;

				case RADIO_FOCUS:
					SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEFOCUSMENU, 0L);
					break;
	
				case SC_UPDATE:
					showparams( hDlg );
					break;
				case RADIO_WIRE_VIEW:
					SendMessage(masterhwnd, WM_COMMAND, IDM_CLEAR, 0L);
				case RADIO_DOWN_VIEW:
				case RADIO_SCROLL_VIEW:
				case RADIO_GRAPH_VIEW:
				case RADIO_SPLIT_VIEW:
				case RADIO_POINT_VIEW:
						if( focusflag )
						calife_list->FocusCA()->
						Setviewmode( (id - RADIO_DOWN_VIEW) +
						IDC_DOWN_VIEW ); //The IDC_?_VIEW are in resource.h
						//I could have just used the RADIO_?_VIEW for these
						//id numbers, and then I could just use Setviewmode(id).
						//But I didn't, and now we don't want to change because
						//we want old file params to still mean the same thing.
					else
						calife_list->
						Setviewmode( (id - RADIO_DOWN_VIEW) +
						 IDC_DOWN_VIEW );
//the following line is bad bad bad for capowgl! It fools capowgl into thinking
//the window size is 0, so it causes the 3d view to appear to freeze.
//To prove my point, uncomment the line, go look at a 2D wave in 3D, then
//select a view, like Split View from the toolbar menu.  It should 'freeze'.
//Commenting the SendMessage() fixes this bug.
//The status bar visibility is handled differently now.   mike 11/18/97
//	  				SendMessage(masterhwnd, WM_SIZE, 0, 0L);// to give us our status bar back
					calife_list->SyncRows();
					showparams( hDlg );
					break;
				case RADIO_SHOW_CHARGE:
				case RADIO_SHOW_CURRENT: ///Set the CA's showvelocity flag to 0 for charge or 1 for velocity
					//Note that "velocity" is the inhibitor field in Reaction Diffusion rules.
					calife_list->Setshowvelocity( id -
						RADIO_SHOW_CHARGE, focusflag );
					showparams( hDlg );
					break;

				case RADIO_BOTH_SHOW:
				case RADIO_ODD_SHOW:
				case RADIO_EVEN_SHOW:
					if( focusflag )
						calife_list->FocusCA()->Setshowmode( id - RADIO_BOTH_SHOW );
					else
						calife_list->Setshowmode( id - RADIO_BOTH_SHOW );
					showparams( hDlg );
					break;
				case RADIO_VIEW_ONE:
					calife_list->Changecount(1);
					break;

				case RADIO_VIEW_FOUR:
					calife_list->Changecount(4); 
					break;

				case RADIO_VIEW_NINE:
					calife_list->Changecount(9);
					break;
			   
		
			} // switch wParam
			
			
}			

static void MyWnd_MOVE(HWND hDlg,int x, int y)
{
	RECT rect;
	char buf[32];

	GetWindowRect(hDlg, &rect);
	wsprintf((LPSTR)buf,"%i",rect.left);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"VIEWX",(LPSTR)buf);
	wsprintf((LPSTR)buf,"%i",rect.top);
	WriteProfileString((LPSTR)szMyAppName,(LPSTR)"VIEWY",(LPSTR)buf);
			
}


//-----------------------------------------------------------------------------//
#pragma argsused	// To disable "Parameter 'lParam' is never used" warning

extern BOOL CALLBACK ViewProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
/*All the message-crackers other than HANDLE_WM_INITDIALOG return by
default a void, which comes through as FALSE, but we want to return
a TRUE for messages which really are handled, so we do a (BOOL) ! cast
on those.  We make HANDLE_WM_INITDIALOG  return a 0 for success, so cast
it the same way.*/
	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, MyWnd_MOVE);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}

} // ViewProc */

/*----------------------------------------------------------------------------------------*/

void showparams( HWND hDlg )
{ 	
	int numcas=9;
	int viewmode = (calife_list->FocusCA()->Getviewmode() - IDC_DOWN_VIEW);
	CheckRadioButton( hDlg, RADIO_DOWN_VIEW, RADIO_POINT_VIEW,
	  RADIO_DOWN_VIEW + viewmode );
	CheckRadioButton( hDlg, RADIO_BOTH_SHOW, RADIO_EVEN_SHOW,
	  RADIO_BOTH_SHOW + calife_list->FocusCA()->Getshowmode() );
	CheckRadioButton( hDlg, RADIO_SHOW_CHARGE, RADIO_SHOW_CURRENT,
	  RADIO_SHOW_CHARGE + calife_list->FocusCA()->Getshowvelocity() );
	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );	
	switch ( calife_list->Count() ) // Number of CAS
	{
		case 1:
			numcas = RADIO_VIEW_ONE;
			break;
		case 4:
			numcas = RADIO_VIEW_FOUR;
			break;
		case 9:
			numcas = RADIO_VIEW_NINE;
			break;
	}
	CheckRadioButton( hDlg, RADIO_VIEW_ONE, RADIO_VIEW_NINE,
	  numcas );
	
	for( int i = RADIO_DOWN_VIEW; i <= RADIO_POINT_VIEW; i++ )
		EnableWindow( GetDlgItem( hDlg, i ),
	     calife_list->FocusCA()->Getviewmode() != IDC_2D_VIEW );
	for (int i = RADIO_FLATCOLOR; i<= RADIO_OPENGL; i++)				//for 3-d graph
		EnableWindow(GetDlgItem(hDlg, i),
		 calife_list->FocusCA()->Getviewmode()==IDC_2D_VIEW);
} // showparams */

