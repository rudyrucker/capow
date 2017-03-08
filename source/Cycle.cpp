/*******************************************************************************
	FILE:				cycle.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions and data to control
						the cycle dialog box and its features.

	UPDATE LOG:			
*******************************************************************************/

//====================INCLUDES===============

#include <windowsx.h>
#include "ca.hpp"
#include "resource.h"

extern char *szMyAppName;
extern HWND hDlgCycle, hDlgLookup, masterhwnd;
extern class CAlist *calife_list;
extern void realLabel (HWND, int, Real);
extern BOOL entropy_flag, update_flag;

void showcycleparams(HWND);

double entropy;
unsigned char drop3, drop4;
static short changeentropy;

/*----------------------------------------------------------------------------------------*/
// 							Messager Cracker


#pragma argsused
static int MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
	HWND hCntl;


			if (calife_list->Getbreedflag())
				CheckDlgButton(hDlg, IDC_BREED_CYCLE,1);
			else
				CheckDlgButton(hDlg, IDC_BREED_CYCLE,0);

			if (calife_list->Getmutateflag())
				CheckDlgButton(hDlg, MUTATE_BREED, 1);
			else
				CheckDlgButton(hDlg, MUTATE_BREED, 0);

			if (calife_list->Getstripekillflag())
				CheckDlgButton(hDlg, STRIPEKILL, 1);
			else
				CheckDlgButton(hDlg, STRIPEKILL, 0);

			if (calife_list->Getstripeseedflag())
				CheckDlgButton(hDlg, STRIPESEED, 1);
			else
				CheckDlgButton(hDlg, STRIPESEED, 0);

			entropy = calife_list->FocusCA()->Gettargetentropy();
			realLabel (hDlg, IDC_TARGET_ENTROPY, entropy);
			realLabel (hDlg, IDC_CURRENT_ENTROPY, calife_list->FocusCA()->Getentropy());
			changeentropy = 0;

			// Set EVOLVE type Combo Box
			hCntl = GetDlgItem( hDlg, IDC_EVOLVE);
			SendMessage( hCntl, CB_ADDSTRING, CA_ZAP, (LONG)"Zap");
			SendMessage( hCntl, CB_ADDSTRING, CA_CLONE, (LONG)"Clone");
			SendMessage( hCntl, CB_ADDSTRING, CA_SEX, (LONG)"Breed");

			showcycleparams(hDlg);

			return 0 ;


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
			
}


static void MyWnd_DESTROY(HWND hDlg)
{
			hDlgCycle = 0;
			InvalidateRect(masterhwnd,NULL,FALSE);
			

}


#pragma argsused
static void MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
	
}

#pragma argsused
static void MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
	int comboint;
			switch (id)
			{
				case IDCANCEL:
					DestroyWindow(hDlg);
					break;

				case SC_UPDATE:
					if (calife_list->Getbreedflag())
						CheckDlgButton(hDlg, IDC_BREED_CYCLE,1);
					else
						CheckDlgButton(hDlg, IDC_BREED_CYCLE,0);

					if (calife_list->Getmutateflag())
						CheckDlgButton(hDlg, MUTATE_BREED,1);
					else
						CheckDlgButton(hDlg, MUTATE_BREED,0);

					if (calife_list->Getstripekillflag())
						CheckDlgButton(hDlg, STRIPEKILL, 1);
					else
						CheckDlgButton(hDlg, STRIPEKILL, 0);

					if (calife_list->Getstripeseedflag())
						CheckDlgButton(hDlg, STRIPESEED, 1);
					else
						CheckDlgButton(hDlg, STRIPESEED, 0);

					showcycleparams(hDlg);
					break;

				case IDC_BREED_CYCLE:
					if (!calife_list->Getbreedflag())
						calife_list->Setbreedflag(1);
					else
						calife_list->Setbreedflag(0);
					calife_list->Resetscores();	// Reset all the scores
					update_flag = 1;
					if (calife_list->Getbreedflag())
					{
						CheckDlgButton(hDlg, IDC_BREED_CYCLE,1);
						EnableWindow(GetDlgItem(hDlg, BREEDING_STEPS), 1);
					}
					else
					{
						CheckDlgButton(hDlg, IDC_BREED_CYCLE,0);
						EnableWindow(GetDlgItem(hDlg, BREEDING_STEPS), 0);
					}
					break;

				case MUTATE_BREED:
					if (!calife_list->Getmutateflag())
						calife_list->Setmutateflag(1);
					else
						calife_list->Setmutateflag(0);
			if (calife_list->Getmutateflag())
				CheckDlgButton(hDlg, MUTATE_BREED, 1);
			else
				CheckDlgButton(hDlg, MUTATE_BREED, 0);
					break;

				case STRIPEKILL:
					if (!calife_list->Getstripekillflag())
						calife_list->Setstripekillflag(1);
					else
						calife_list->Setstripekillflag(0);
			if (calife_list->Getstripekillflag())
				CheckDlgButton(hDlg, STRIPEKILL, 1);
			else
				CheckDlgButton(hDlg, STRIPEKILL, 0);
					break;

				case STRIPESEED:
					if (!calife_list->Getstripeseedflag())
						calife_list->Setstripeseedflag(1);
					else
						calife_list->Setstripeseedflag(0);
			if (calife_list->Getstripeseedflag())
				CheckDlgButton(hDlg, STRIPESEED, 1);
			else
				CheckDlgButton(hDlg, STRIPESEED, 0);
					break;
				
				case IDC_EVOLVE:
					if (SendMessage( (HWND)hwndCtl, CB_GETDROPPEDSTATE, 0, 0L)
						&& drop3 == 0)
					{
						drop3 = 1;
						break;
					}
					if (!SendMessage( (HWND)hwndCtl, CB_GETDROPPEDSTATE, 0, 0L)
						&& drop3 == 1)
					{
						comboint = CA_ZAP +(unsigned char)
							SendMessage((HWND)hwndCtl, CB_GETCURSEL, 0, 0L);

						calife_list->SetEvolve(comboint);
						SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
						drop3 = 0;
					}
					break;
				}


}

BOOL HandleUpDownControlCycle(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//
// Up down control work as follow:
//		alway remain at 50
//		the value or variable change only.
//
{
	NM_UPDOWN *pnmud = (NM_UPDOWN FAR *) lParam;
    
	if (pnmud->hdr.code != UDN_DELTAPOS)	// if no change then return
		return FALSE;
 
 
switch ( pnmud->hdr.idFrom )
{
	case IDC_SPIN_BREED_BREED:
		if ( pnmud->iDelta < 0 )
		{
			if (calife_list->Getbreedflag())
			{
				calife_list->Setbreedcycle(+1);
				SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
		}
		else
		{
			if (calife_list->Getbreedflag())
			{
				calife_list->Setbreedcycle(-1);
				SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
		}
		break;

	case IDC_SPIN_BREED_FAIL:
		if ( pnmud->iDelta < 0 )
		{
			calife_list->Setfailstripe(+1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		else
		{		
			calife_list->Setfailstripe(-1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		break;

	case IDC_SPIN_BREED_GOAL:
		if ( pnmud->iDelta < 0 )
		{
			changeentropy = 1;
			entropy += 0.1;
			if (entropy > 0.9)
				entropy = 0.9;

			calife_list->Changeentropy(entropy);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		else
		{	
			changeentropy = 1;
			entropy -= 0.1;
			if (entropy < 0.1)
				entropy = 0.1;

			calife_list->Changeentropy(entropy);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		break;

	case IDC_SPIN_BREED_SCORE:
		if ( pnmud->iDelta < 0 )
		{		
			calife_list->Setentropybonus(+1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		else
		{			
			calife_list->Setentropybonus(-1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		break;
/*  MOVED TO CELL.CPP

	case IDC_SPIN_BREED_MUTATE:
		if ( pnmud->iDelta < 0 )
		{		
			calife_list->Setmutation(+1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		else
		{
			calife_list->Setmutation(-1);
			SendMessage(hDlg, WM_COMMAND, SC_UPDATE, 0L);
		}
		break;
*/


} // Switch
	// We must return true so that the position remain the same value
	return TRUE;
}



extern BOOL CALLBACK CycleProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_MOVE:
			return (BOOL) !HANDLE_WM_MOVE(hDlg, wParam, lParam, MyWnd_MOVE);
		case WM_NOTIFY:
			return HandleUpDownControlCycle(hDlg, message, wParam, lParam);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE; ;
	}

}

void showcycleparams(HWND hDlg)
{
	entropy = calife_list->FocusCA()->Gettargetentropy();

	if (calife_list->Getbreedflag())
	{
		CheckDlgButton(hDlg, IDC_BREED_CYCLE,1);
		EnableWindow(GetDlgItem(hDlg, BREEDING_STEPS), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_SPIN_BREED_BREED), TRUE);
	}
	else
	{
		CheckDlgButton(hDlg, IDC_BREED_CYCLE,0);
		EnableWindow(GetDlgItem(hDlg, BREEDING_STEPS), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_SPIN_BREED_BREED), FALSE);
	}
	if (!SendMessage( GetDlgItem( hDlg, IDC_EVOLVE), CB_GETDROPPEDSTATE, 0, 0L))
		SendMessage(GetDlgItem( hDlg, IDC_EVOLVE),
			CB_SETCURSEL, (WORD)calife_list->Getevolve(), 0);

	//  Function moved to Cell.cpp
	//	realLabel (hDlg,IDC_MUTATION,100.0 * calife_list->Getmutation());
	realLabel (hDlg, IDC_TARGET_ENTROPY, entropy);
    realLabel (hDlg, IDC_CURRENT_ENTROPY, calife_list->FocusCA()->Getentropy());
	realLabel (hDlg, IDC_FAILSTRIPE, calife_list->FocusCA()->Getfailstripe());
	realLabel (hDlg, IDC_ENTROPY_SCORE, calife_list->FocusCA()->Getentropybonus());
	realLabel (hDlg, IDC_BREEDING_STEPS, calife_list->Getbreedingsteps());
		
	
}
