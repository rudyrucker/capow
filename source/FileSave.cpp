/*************************************************************************
 *		FILE:				filesave.cpp
 *		PURPOSE:			provides a dialog for selecting CA save options
 *		PROJECT:			CAMCOS CAPOW
 *		PROGRAMMER:			T. Colbert (tc)
 *		DATE:				19 October 1997
 *		ENVIRONMENT:	Microsoft Visual C++ 5.0/WinNT/Win95
 *		NOTES:			use _export, even though is slower than putting info
 *							in the .def file.  So what--it's an about box.
*************************************************************************/
#include "ca.hpp"
#include "resource.h"
#pragma hdrstop

extern class CAlist *calife_list;
extern BOOL compressFile; 

static void showparams(HWND hDlg); // function prototype


extern BOOL load_save_cells_flag;


static  char szFileName[MAXFILENAME];
static	char szFileTitle[MAXFILENAME];
static	OPENFILENAME ofn;
	
	//CA and CAS file load save
static	char szFilterSpecCA [128] =
		"CA Files (*.CA)\0All Files (*.*)\0*.*\0";
static	char szFilterSpecCAS [128] =
		"Experiment Files (*.CAS)\0All Files (*.*)\0*.*\0";

//------------ start with focus on all--------------------
static BOOL focusflag = FALSE; 



//------------------------ Message Processing -----------------------//


static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	if(focusflag == FALSE) //focus on all cas
	{
		strcpy(szFileName,"*.CAs");	
		ofn.lpstrDefExt   = "CAS";  
		ofn.lpstrFilter	  = szFilterSpecCAS; 
	}
	else //focus on single ca
	{
		strcpy(szFileName,"*.CA");		
		ofn.lpstrDefExt     = "CA";		
		ofn.lpstrFilter		= szFilterSpecCA;
	}
	ofn.lpstrFile     = szFileName;

	// fill in non-variant fields of OPENFILENAME struct.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner	      = hDlg;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	  = 0;
	ofn.nFilterIndex	  = 1;
	ofn.nMaxFile	      = MAXFILENAME;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrFileTitle    = szFileTitle;
	ofn.nMaxFileTitle     = MAXFILENAME;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

	showparams(hDlg);
	return 0; //if successful
}


static void  MyWnd_COMMAND(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
		case IDC_FILE_COMPRESSION:
			compressFile = !compressFile;
			break;
		
		case IDC_SAVE_PARAMETERS_ONLY:
			load_save_cells_flag = FALSE;
			break;

		case IDC_SAVE_PARAMETERS_AND_IMAGE:
			load_save_cells_flag = TRUE;
			break;

		case IDC_SAVE_FOCUS_CA:
			focusflag = TRUE;
			strcpy(szFileName,"*.CA");	
			ofn.lpstrFile		= szFileName;
			ofn.lpstrFilter		= szFilterSpecCA;
			ofn.lpstrDefExt     = "CA";		
			break;

		case IDC_SAVE_ALL_CAS:
			focusflag = FALSE;
			strcpy(szFileName,"*.CAs");	
			ofn.lpstrFile     = szFileName;
			ofn.lpstrFilter	  = szFilterSpecCAS;  
			ofn.lpstrDefExt   = "CAS";  
			break;		
			
		case IDOK:  //Save button
			if (focusflag) //save focus
			{
				if( GetSaveFileName((LPOPENFILENAME)&ofn) )
				{
						SetCursor(LoadCursor(NULL, IDC_WAIT)); // Wait, I'm working!
						calife_list->Saveindividual(szFileName,calife_list->FocusCA());
						SetCursor(LoadCursor(NULL, IDC_ARROW)); //I'm done!
				}
			}
			else  //save all
			{
				if( GetSaveFileName((LPOPENFILENAME)&ofn) )
				{
						SetCursor(LoadCursor(NULL, IDC_WAIT)); // Wait, I'm working!
						calife_list->Saveall(szFileName);
						SetCursor(LoadCursor(NULL, IDC_ARROW)); //I'm done!
				}
			}
			
			EndDialog(hDlg, 0);
			return ;
				

		case IDCANCEL:
		case IDIGNORE:
			EndDialog(hDlg, 0);
			return;
	}
	showparams(hDlg);
}



static BOOL MyWnd_DESTROY(HWND hDlg)
{
	return TRUE;
}

static BOOL MyWnd_CLOSE(HWND hDlg)
{
	EndDialog(hDlg,TRUE);
	return TRUE;
}

/*static BOOL MyWnd_MOVE(HWND hDlg, int x, int y)
{
	InvalidateRect(hDlg,NULL,FALSE);
	UpdateWindow(hDlg);
	return TRUE;
}
*/

static void MyWnd_RBUTTONDOWN(HWND hDlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
			DestroyWindow(hDlg);
}




BOOL CALLBACK SaveFileProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
//		case WM_MOVE:
//			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, MyWnd_MOVE);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_RBUTTONDOWN:
			return (BOOL) !HANDLE_WM_RBUTTONDOWN(hDlg, wParam, lParam, MyWnd_RBUTTONDOWN);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
}

static void showparams(HWND hDlg)
{

	CheckDlgButton(hDlg,IDC_FILE_COMPRESSION, compressFile);

	CheckRadioButton(hDlg,IDC_SAVE_FOCUS_CA,IDC_SAVE_ALL_CAS, 
		(focusflag== TRUE)?IDC_SAVE_FOCUS_CA:IDC_SAVE_ALL_CAS);

	CheckRadioButton(hDlg,IDC_SAVE_PARAMETERS_ONLY,IDC_SAVE_PARAMETERS_AND_IMAGE,
		(load_save_cells_flag==FALSE)?IDC_SAVE_PARAMETERS_ONLY:IDC_SAVE_PARAMETERS_AND_IMAGE);
}

