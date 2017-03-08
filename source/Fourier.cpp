




#include "ca.hpp"
#include "resource.h"
#include <math.h>

//===============  EXTERNAL VARIABLES, CLASSES, FUNCTIONS

extern BOOL update_flag;
extern HWND hDlgFourier, masterhwnd;
extern short focusflag;

extern class CAlist *calife_list;
extern void numlabel(HWND, int, int);

//========== LOCAL VARIABLES

static HWND hCntrlTrackBar;
static int trackoldpos;
static int TestPoints[9] = { 0 };

void EnableFourierDialogControls (HWND hDlg, BOOL flag );


//==================================================================
//==================================================================
						  
#pragma argsused
static int MyWnd_INITDIALOG(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	if ( calife_list->FocusCA()->Getviewmode() != IDC_POINT_GRAPH )
	{
		EnableFourierDialogControls (hDlgFourier, FALSE );
		return 0;
	}
	EnableFourierDialogControls (hDlgFourier,TRUE );	
	hCntrlTrackBar = GetDlgItem (  hDlg, IDC_FOURIER_SLIDER );
	SendMessage ( hCntrlTrackBar, TBM_SETRANGE, TRUE, MAKELONG ( 0, calife_list->WidthCADisplayWindow() ) );	
	SendMessage ( hCntrlTrackBar, TBM_SETPOS, TRUE, calife_list->FocusCA()->Get_TestPoint() );		
	trackoldpos =   calife_list->FocusCA()->Get_TestPoint();
	SendMessage ( hCntrlTrackBar, TBM_SETPAGESIZE, 0L, 5L );
	
	CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );	
	CheckRadioButton(hDlg,IDC_TIMESCALEHALF,IDC_TIMESCALEDOUBLE,
									(calife_list->FocusCA())->Get_tpviewmode());
	CheckRadioButton(hDlg,IDC_FOURIERGRAPH,IDC_FOURIERSCROLL,
								(calife_list->FocusCA())->Get_tpviewtype());
	CheckRadioButton(hDlg,IDC_SINEONLY,IDC_BOTHSINECOSINE,
									(calife_list->FocusCA())->Get_tpapproxtype());
	CheckDlgButton(hDlg, IDC_VIEWGRAPH,
							(calife_list->FocusCA())->Get_graphflag());
	CheckDlgButton(hDlg, IDC_VIEWAPPROX,
							(calife_list->FocusCA())->Get_approxflag());
	CheckDlgButton(hDlg, IDC_VIEWCOSINE,
							(calife_list->FocusCA())->Get_cosineflag());
	CheckDlgButton(hDlg, IDC_VIEWSINE,
							(calife_list->FocusCA())->Get_sineflag());
	CheckDlgButton(hDlg, IDC_VIEWSPECTRUM,
							(calife_list->FocusCA())->Get_spectrumflag());
	EnableWindow( GetDlgItem( hDlg, IDC_FOURIERSCROLL ), FALSE );
	
	return 0;
}

#pragma argsused
static void MyWnd_DESTROY(HWND hDlg)
{
	int i;

			hDlgFourier = 0;
			if ( !focusflag )
				for (i=0; i<calife_list->Count(); i++)
					(calife_list->GetCA(i))->Set_TempTP();
			else
				(calife_list->FocusCA())->Set_TempTP();
			InvalidateRect(masterhwnd,NULL,FALSE);
			

}

#pragma argsused
static void MyWnd_CLOSE(HWND hDlg)
{ int i;
			if ( !focusflag )
				for (i=0; i<calife_list->Count(); i++)
					(calife_list->GetCA(i))->Set_TempTP();
			else
				(calife_list->FocusCA())->Set_TempTP();
			DestroyWindow(hDlg);


}

#pragma argsused
static void MyWnd_COMMAND(HWND hDlg,int id, HWND hwndCtl, UINT codeNotify)
{
	int 	i;
	int	oldflag, nterm;

			switch (id)
			{

	    	case RADIO_ALL:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEALLMENU, 0L);
			break;

			case RADIO_FOCUS:
			SendMessage(masterhwnd, WM_COMMAND, IDM_CHANGEFOCUSMENU, 0L);
			break;

			
			case SC_UPDATE:
					CheckRadioButton( hDlg, RADIO_ALL, RADIO_FOCUS, RADIO_ALL+focusflag );	
				//Fix all the checks.
					CheckRadioButton(hDlg,IDC_TIMESCALEHALF,IDC_TIMESCALEDOUBLE,
									(calife_list->FocusCA())->Get_tpviewmode());
					CheckRadioButton(hDlg,IDC_FOURIERGRAPH,IDC_FOURIERSCROLL,
									(calife_list->FocusCA())->Get_tpviewtype());
					CheckRadioButton(hDlg,IDC_SINEONLY,IDC_BOTHSINECOSINE,
									(calife_list->FocusCA())->Get_tpapproxtype());
					numlabel(hDlg ,IDC_NUMTERM,
							(calife_list->FocusCA())->Get_numofterm());
					CheckDlgButton(hDlg, IDC_VIEWGRAPH,
							(calife_list->FocusCA())->Get_graphflag());
					CheckDlgButton(hDlg, IDC_VIEWAPPROX,
							(calife_list->FocusCA())->Get_approxflag());
					CheckDlgButton(hDlg, IDC_VIEWCOSINE,
							(calife_list->FocusCA())->Get_cosineflag());
					CheckDlgButton(hDlg, IDC_VIEWSINE,
							(calife_list->FocusCA())->Get_sineflag());
					CheckDlgButton(hDlg, IDC_VIEWSPECTRUM,
							(calife_list->FocusCA())->Get_spectrumflag());
					break;

				case IDC_FOURIERGRAPH:
				case IDC_FOURIERSCROLL:
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						for (i=0; i<calife_list->Count(); i++)
							(calife_list->GetCA(i))->Set_tpviewtype(id);
					else
						(calife_list->FocusCA())->Set_tpviewtype(id);
					CheckRadioButton(hDlg,IDC_FOURIERGRAPH,IDC_FOURIERSCROLL,
									(calife_list->FocusCA())->Get_tpviewtype());
					//
					//.................//
					//
					break;

				case IDC_SINEONLY:
				case IDC_COSINEONLY:
				case IDC_BOTHSINECOSINE:
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						for (i=0; i<calife_list->Count(); i++)
						{
							(calife_list->GetCA(i))->Set_tpapproxtype(id);
							if ( (calife_list->GetCA(i))->Get_fourierflag() == 1 )
								(calife_list->GetCA(i))->SFT();
						}
					else
					{
						(calife_list->FocusCA())->Set_tpapproxtype(id);
						if ( (calife_list->FocusCA())->Get_fourierflag() == 1 )
							(calife_list->FocusCA())->SFT();
					}
					CheckRadioButton(hDlg,IDC_SINEONLY,IDC_BOTHSINECOSINE,
									(calife_list->FocusCA())->Get_tpapproxtype());
					break;

				case IDC_CHANGESAMPLE:
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						for (i=0; i<calife_list->Count(); i++)
						{
							(calife_list->GetCA(i))->Set_TP();
							(calife_list->GetCA(i))->Reset_tp_array();
							(calife_list->GetCA(i))->Set_fourierflag(0);
						}
					else
					{
						(calife_list->FocusCA())->Set_TP();
						(calife_list->FocusCA())->Reset_tp_array();
						(calife_list->FocusCA())->Set_fourierflag(0);
					}
					CheckDlgButton(hDlg, IDC_VIEWGRAPH, 1);
					CheckDlgButton(hDlg, IDC_VIEWAPPROX, 1);
					break;

					// .fourierflag = 0 means fourier approx hasn't been computed,
					//                 compute fourier approx;
					// .fourierflag = 1 means fourier approx is already computed
					//						 reset to the test point view mode.

					// .When in do-it-to-all mode, what 'Approximate' does
					//  depends on the focus CA.
				case IDC_APPROXIMATE: //The "Analyze" button
					if ( (calife_list->FocusCA())->Get_fourierflag() == 0 )
					{
						SetWindowText(GetDlgItem( hDlg, IDC_APPROXIMATE ), "Record A Time Series");
						// fourier-flag of focus CA is 0, now will be set to 1
					    if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						
							for (i=0; i<calife_list->Count(); i++)
							{
								// fourier-flag of focus CA is 0, but fourier-flag
								// of others may not. Check fourier-flag so that
								// we don't have to recompute SFT
								if((calife_list->GetCA(i))->Get_fourierflag() == 0)
								{
									(calife_list->GetCA(i))->SFT();
									(calife_list->GetCA(i))->Set_fourierflag(1);
								}
							}
						else 	// focus only
						{
							(calife_list->FocusCA())->SFT();
							(calife_list->FocusCA())->Set_fourierflag(1);
						}
					}
					else	// fourier-flag of focus CA is 1, now will be set to 0
					{     // and tp_viewgraph, tp_viewapprox also set to 1
						SetWindowText(GetDlgItem( hDlg, IDC_APPROXIMATE ), "Analyze The Time Series");
						if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						
							for (i=0; i<calife_list->Count(); i++)
							{
								// fourier-flag of focus CA is 1, but fourier-flag
								// of others may not. Check fourier-flag so that
								// we don't unreasonably reset the ones without
								// fourier apporx displayed.
								if((calife_list->GetCA(i))->Get_fourierflag() == 1)
								{
									(calife_list->GetCA(i))->Set_fourierflag(0);
									(calife_list->GetCA(i))->Reset_tp_array();
								}
							}
						else	// focus only
						{
							(calife_list->FocusCA())->Set_fourierflag(0);
							(calife_list->FocusCA())->Reset_tp_array();
						}
	

						CheckDlgButton(hDlg, IDC_VIEWGRAPH, 1);
						CheckDlgButton(hDlg, IDC_VIEWAPPROX, 1);
					
					}
					break;
				case IDC_TIMESCALEHALF:
				case IDC_TIMESCALETRUE:
				case IDC_TIMESCALEDOUBLE:
				
						if ( !focusflag && (calife_list->Getzoomflag() == 0) )
						for (i=0; i<calife_list->Count(); i++)
						{
							(calife_list->GetCA(i))->
								Set_tpviewmode(id);
							if ( (calife_list->GetCA(i))->
								Get_fourierflag() == 1 )
									(calife_list->GetCA(i))->SFT();
						}
					else
					{
						(calife_list->FocusCA())->
							Set_tpviewmode(id);
						if ( (calife_list->FocusCA())->
							Get_fourierflag() == 1 )
								(calife_list->FocusCA())->SFT();
					}
					CheckRadioButton(hDlg,IDC_TIMESCALEHALF,
						IDC_TIMESCALEDOUBLE,
						(calife_list->FocusCA())->Get_tpviewmode());
					break;

				case IDC_VIEWGRAPH:
					oldflag = (calife_list->FocusCA())->Get_graphflag();
					
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					{
						for (i=0; i<calife_list->Count(); i++)
							if ((calife_list->GetCA(i))->Get_graphflag() == oldflag)
								(calife_list->GetCA(i))->Change_graphflag();
					}
					else
						(calife_list->FocusCA())->Change_graphflag();
					CheckDlgButton(hDlg, IDC_VIEWGRAPH,
						(calife_list->FocusCA())->Get_graphflag());
					break;

				case IDC_VIEWAPPROX:
					oldflag = (calife_list->FocusCA())->Get_approxflag();
					
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					{
						for (i=0; i<calife_list->Count(); i++)
							if ((calife_list->GetCA(i))->Get_approxflag() == oldflag)
								(calife_list->GetCA(i))->Change_approxflag();
					}
					else
						(calife_list->FocusCA())->Change_approxflag();

					CheckDlgButton(hDlg, IDC_VIEWAPPROX,
										(calife_list->FocusCA())->Get_approxflag());
					break;

				case IDC_VIEWCOSINE:
					oldflag = (calife_list->FocusCA())->Get_cosineflag();
					
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					{
						for (i=0; i<calife_list->Count(); i++)
							if ((calife_list->GetCA(i))->Get_cosineflag() == oldflag)
								(calife_list->GetCA(i))->Change_cosineflag();
					}
					else
						(calife_list->FocusCA())->Change_cosineflag();

					CheckDlgButton(hDlg, IDC_VIEWCOSINE,
										(calife_list->FocusCA())->Get_cosineflag());
					break;

				case IDC_VIEWSINE:
					oldflag = (calife_list->FocusCA())->Get_sineflag();
					
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					{
						for (i=0; i<calife_list->Count(); i++)
							if ((calife_list->GetCA(i))->Get_sineflag() == oldflag)
								(calife_list->GetCA(i))->Change_sineflag();
					}
					else
						(calife_list->FocusCA())->Change_sineflag();

					CheckDlgButton(hDlg, IDC_VIEWSINE,
										(calife_list->FocusCA())->Get_sineflag());
					break;

				case IDC_VIEWSPECTRUM:
					oldflag = (calife_list->FocusCA())->Get_spectrumflag();
					
					if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					{
						for (i=0; i<calife_list->Count(); i++)
							if ((calife_list->GetCA(i))->Get_spectrumflag() == oldflag)
								(calife_list->GetCA(i))->Change_spectrumflag();
					}
					else
						(calife_list->FocusCA())->Change_spectrumflag();

					CheckDlgButton(hDlg, IDC_VIEWSPECTRUM,
										(calife_list->FocusCA())->Get_spectrumflag());
					break;
			}  // end switch
if( hDlgFourier )
		SendMessage( hDlgFourier, WM_INITDIALOG, 0, 0L );

}

BOOL HandleUpDownControlFourier(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDC_SPIN_FOURIER_TERMS: // do for focus CA first
			if ( pnmud->iDelta > 0 )
				(calife_list->FocusCA())->Change_numofterm(-1);
			else
				(calife_list->FocusCA())->Change_numofterm(+1);

			if ( (calife_list->FocusCA())->Get_fourierflag() == 1 )
				(calife_list->FocusCA())->SFT();
					
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
			{
				int nterm = (calife_list->FocusCA())->Get_numofterm();
				for (int i=0; i<calife_list->Count(); i++)
				{
					(calife_list->GetCA(i))->Set_numofterm(nterm);
					if ( (calife_list->GetCA(i))->Get_fourierflag() == 1 )
							(calife_list->GetCA(i))->SFT();
				}
			}
			numlabel(hDlg ,IDC_NUMTERM,(calife_list->FocusCA())->Get_numofterm());
			break;	
	}

	return TRUE;
}

static void MyWnd_HSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	
	switch ( code )
	{
					
		case TB_LINEDOWN:
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
				for (int i=0; i < calife_list->Count(); i++)
					( calife_list->GetCA(i))->Change_TempTP(+1);
			else
				(calife_list->FocusCA())->Change_TempTP(+1);
			break;
		
		case TB_LINEUP:
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					for (int i=0; i<calife_list->Count(); i++)
						(calife_list->GetCA(i))->Change_TempTP(-1);
			else
				(calife_list->FocusCA())->Change_TempTP(-1);
			break;

		
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:
		
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					for (int i=0; i<calife_list->Count(); i++)
						(calife_list->GetCA(i))->Change_TempTP(pos-trackoldpos);
			else
				(calife_list->FocusCA())->Change_TempTP(pos-trackoldpos);
			break;
	
			

		case TB_PAGEDOWN:
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					for (int i=0; i<calife_list->Count(); i++)
						(calife_list->GetCA(i))->Change_TempTP((int)SendMessage ( hCntrlTrackBar, TBM_GETPAGESIZE, 0L, 0L ) );
			else
				(calife_list->FocusCA())->Change_TempTP((int)SendMessage ( hCntrlTrackBar, TBM_GETPAGESIZE, 0L, 0L ) );
			break;
	

		
		case TB_PAGEUP:
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					for (int i=0; i<calife_list->Count(); i++)
						(calife_list->GetCA(i))->Change_TempTP(-((int)SendMessage ( hCntrlTrackBar, TBM_GETPAGESIZE, 0L, 0L ) ));

			else
				(calife_list->FocusCA())->Change_TempTP(-((int)SendMessage ( hCntrlTrackBar, TBM_GETPAGESIZE, 0L, 0L ) ));
			break;
	


		case TB_BOTTOM:
		case TB_TOP:
			
			if ( !focusflag && (calife_list->Getzoomflag() == 0) )
					for (int i=0; i<calife_list->Count(); i++)
						(calife_list->GetCA(i))->Change_TempTP(pos-trackoldpos);
			else
				(calife_list->FocusCA())->Change_TempTP(pos-trackoldpos);
			break;
	
	}
	trackoldpos = pos;
}
			
#pragma argsused
BOOL CALLBACK FourierProc (HWND hDlg, UINT message,
													WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:
			return (BOOL) !HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, MyWnd_INITDIALOG);
		case WM_COMMAND:
			return (BOOL) !HANDLE_WM_COMMAND(hDlg, wParam, lParam, MyWnd_COMMAND);
		case WM_CLOSE:
			return (BOOL) !HANDLE_WM_CLOSE(hDlg, wParam, lParam, MyWnd_CLOSE);
		case WM_NOTIFY:
			return HandleUpDownControlFourier(hDlg, message, wParam, lParam);
		case WM_HSCROLL:
			return (BOOL) !HANDLE_WM_HSCROLL(hDlg, wParam, lParam, MyWnd_HSCROLL);
		case WM_DESTROY:
			return (BOOL) !HANDLE_WM_DESTROY(hDlg, wParam, lParam, MyWnd_DESTROY);
		default:
			return FALSE;
	}
}


void CA::Change_TempTP(int disp)
{

	if ( Getviewmode() != IDC_POINT_GRAPH )
		return;
	temp_test_point += disp;
	if (temp_test_point < 1)
		temp_test_point = 1;
	if (temp_test_point >= (maxx - minx))
		temp_test_point = maxx - minx - 1;
}


void CA::Set_TP()
{

	if (maxx == minx) maxx = minx+1;  //Rudy, protect against 1/0.

	test_point = temp_test_point;
	rel_test_point = (Real)(test_point) / (Real)(maxx - minx);
}


void CA::Change_numofterm(int num)
{
	int	limit_number;

	if ( Getviewmode() != IDC_POINT_GRAPH ) // Don't change if N/A
		return;

	limit_number = ( ((maxx-minx) > MAXTERM)? MAXTERM : (maxx - minx - 1) );
	numofterm += num;
	if (numofterm < 2) //Rudy says avoid crash at low end.
		numofterm = 2;
	if (numofterm > limit_number)
		numofterm = limit_number;
}


void CA::Reset_tp_array(void)
{
	tp_startpos		= 0;
	tp_endpos   	= 0;
	tp_graphflag	= 1;
	tp_approxflag	= 1;
}


void CA::Reset_fourier_array(void)
{
   int i;
	for(i = 0; i<MAXTERM; i++)
	{
		fourier_a[i] = 0;
		fourier_b[i] = 0;
	}
	for(i=0; i<(int)(2.0*_max_horz_count); i++)
		fourier_approx[i] = 0;
}

void CA::Reset_tp_all(void)
{
	if (maxx == minx) maxx = minx+1;  //Rudy, protect against 1/0.

	fourierflag			= 0;
	tp_graphflag		= 1;
	tp_approxflag		= 1;
	test_point = (int)(rel_test_point * (maxx-minx));
	temp_test_point	= test_point;
	Reset_fourier_array();
	Reset_tp_array();
	if( hDlgFourier )
		SendMessage( hDlgFourier, WM_INITDIALOG, 0, 0L );
}


void CA::Fourier_init(void)
{
	fourierflag	= 0;
	tp_viewmode	= IDC_TIMESCALETRUE;
	tp_viewtype	= IDC_FOURIERGRAPH;
	tp_approxtype 	= IDC_BOTHSINECOSINE;
	tp_graphflag	= 1;
	tp_approxflag	= 1;
	tp_cosineflag	= 0;
	tp_sineflag	= 0;
	tp_spectrumflag= 0;
	numofterm		= 4;
	rel_test_point = 0.65791; //Don't want 0.5 as this is often stationary
		//in the case where you are seeding with sinewaves.
	//Reset_fourier_array();
	//Reset_tp_array();	Reset_tp_all();
	Reset_tp_all();
	if( hDlgFourier )
		SendMessage( hDlgFourier, WM_INITDIALOG, 0, 0L );
}


void CA::SFT(void) 							// slow fourier transform
{
	if ( Getviewmode() != IDC_POINT_GRAPH ) // Don't do if N/A
		return;
		
	int Nterm, Mvalue;
	int startpos, endpos, limit;

	Reset_fourier_array();

	switch (tp_viewmode)
	{
		case IDC_TIMESCALEHALF:
			limit = 2*horz_count;
			break;

		case IDC_TIMESCALETRUE:
			limit = horz_count;
			break;

		case IDC_TIMESCALEDOUBLE:
			limit = (int)(horz_count/2);
			break;
	}

	// If enough data, then do with the latest data;
	// else do from the beginning.
	startpos = (tp_endpos >= limit)? (tp_endpos - limit) : tp_startpos;
	endpos   = tp_endpos;
	Nterm 	= numofterm - 1;

	switch(tp_approxtype)
	{
		case IDC_SINEONLY:
			Mvalue = endpos - startpos;
			Nterm = (Nterm < Mvalue)? Nterm : Mvalue-1 ;
			SineSFT(Nterm, Mvalue, startpos);
			break;

		case IDC_COSINEONLY:
			Mvalue = endpos - startpos;
			Nterm = (Nterm < Mvalue)? Nterm : Mvalue-1 ;
			CosineSFT(Nterm, Mvalue, startpos);
			break;

		case IDC_BOTHSINECOSINE:
			Mvalue = (int)((endpos - startpos)/2);
			if ( Mvalue == 0 )
				return;
			Nterm = (Nterm < Mvalue)? Nterm : Mvalue-1 ;
			BothSFT(Nterm, Mvalue, startpos);
			break;
	}
}


void CA::SineSFT(int N, int M, int startpos)
{
	Real *x, sum;
   int j,k;

//Rudy added this to prevent divide by 0.
	if (M <= 1)
		M = 1;

	x = new Real [2*M];
	for( j = 0; j < 2*M; j++)				// transform [0,M] into [-PI,PI]
		x[j] = PI * ((Real)j/(Real)M - 1.0);//---- xj = PI *(j/M -1)  ----
														//----  j = M*(1 + xj/PI) ----

	// compute bk, k=0,1,...,N-1
	//      b0 = 0,
	//  	  bk = (1/M) * sumof(yj * sin(k*xj)),	j=0,1,..,2*M-1
	fourier_b[0] = 0.0;
	for(k = 1; k <= N-1; k++ )
	{
		sum = 0.0;
		for(j = 0; j < M - 1; j++)
			sum += tp_real_array[startpos + j] *
						( sin((Real)k * x[j]) - sin((Real)k * x[2*M - 1 - j]) );
		fourier_b[k] = sum / (Real)M;
	}

	// compute values of the Fourier approximation at xj's
	//			SN(xj) = a0/2 + aN*cos(N*xj) +
	//						sumof( ak*cos(k*xj) + bk*sin(k*xj) ),  k = 1,..,N-1
	for( j = 0; j < 2*M ; j++)
	{
		sum = 0.0;
		for(k = 1; k <= N-1; k++)
			sum += fourier_b[k] * sin((Real)k * x[j]);
		fourier_approx[j] = sum;
	}
	delete x;
}


void CA::CosineSFT(int N, int M, int startpos)
{
	Real *x, sum;
   int j,k;

//Rudy added this to prevent divide by 0.
	if (M < 1)
		M = 1;

	x = new Real [2*M];
	for(j = 0; j < 2*M; j++)				// transform [0,M] into [-PI,PI]
		x[j] = PI * ((Real)j/(Real)M - 1.0);//---- xj = PI *(j/M -1)  ----
														//----  j = M*(1 + xj/PI) ----

	// compute ak, k=0,1,...,N
	//  	  ak = (1/M) * sumof(yj * cos(k*xj)),	j=0,1,..,2*M-1
	for(k = 0; k <= N; k++ )
	{
		sum = 0.0;
		for(j = 0; j < M - 1; j++)
			sum += tp_real_array[startpos + j] *
						( cos((Real)k * x[j]) + cos((Real)k * x[2*M - 1 - j]) );
		fourier_a[k] = sum / (Real)M;
	}

	// compute values of the Fourier approximation at xj's
	//			SN(xj) = a0/2 + aN*cos(N*xj) +
	//						sumof( ak*cos(k*xj) + bk*sin(k*xj) ),  k = 1,..,N-1
	for( j = 0; j < 2*M ; j++)
	{
		sum  = fourier_a[0] / 2.0;
		for(k = 1; k <= N; k++)
			sum += fourier_a[k] * cos((Real)k * x[j]);
		fourier_approx[j] = sum;
	}
	delete x;
}


void CA::BothSFT(int N, int M, int startpos)
{
	Real *x, sum;
   int j,k;

//Rudy added this to prevent divide by 0.
	if (M < 1)
		M = 1;

	x = new Real [2*M];
	for(j = 0; j < 2*M; j++)				// transform [0,M] into [-PI,PI]
		x[j] = PI * ((Real)j/(Real)M - 1.0);//---- xj = PI *(j/M -1)  ----
														//----  j = M*(1 + xj/PI) ----

	// compute ak, k=0,1,...,N
	//  	  ak = (1/M) * sumof(yj * cos(k*xj)),	j=0,1,..,2*M-1
	for( k = 0; k <= N; k++ )
	{
		sum = 0.0;
		for(j = 0; j < 2*M - 1; j++)
			sum += tp_real_array[startpos + j] * cos( (Real)k * x[j] );
		fourier_a[k] = sum / (Real)M;
	}

	// compute bk, k=0,1,...,N-1
	//      b0 = 0,
	//  	  bk = (1/M) * sumof(yj * sin(k*xj)),	j=0,1,..,2*M-1
	fourier_b[0] = 0.0;
	for( k = 1; k <= N-1; k++ )
	{
		sum = 0.0;
		for( int j = 0; j < 2*M - 1; j++)
			sum += tp_real_array[startpos + j] * sin( (Real)k * x[j] );
		fourier_b[k] = sum / (Real)M;
	}

	// compute values of the Fourier approximation at xj's
	//			SN(xj) = a0/2 + aN*cos(N*xj) +
	//						sumof( ak*cos(k*xj) + bk*sin(k*xj) ),  k = 1,..,N-1
	for( j = 0; j < 2*M ; j++)
	{
		sum  = fourier_a[0] / 2.0;
		sum += fourier_a[N] * cos((Real)N * x[j]);
		for(k = 1; k <= N-1; k++)
			sum += (fourier_a[k] * cos((Real)k * x[j])
					  + fourier_b[k] * sin((Real)k * x[j]));
		fourier_approx[j] = sum;
	}
	delete x;
}


void CA::Showpointgraph(HDC hdc)
{
	int xGen,yGen;
	int vert_count1;
	int maxy1, maxy2, miny1, miny2;
	int startpos, fendpos, x_coord, y_coord;
	int i, j, k, M, nterm, xstep, ystep, spec_interval;
	Real *x, max_value;

	WBM->ClearSection(minx, miny, maxx, maxy);
	vert_count1 = (int)(vert_count/2);
	miny1 = miny;
	maxy1 = miny1 + vert_count1;
	miny2 = maxy1 + 1;
	maxy2 = maxy;

	if (states < 1) states = 1; //Rudy prevent 1/0
	if (type_ca == CA_STANDARD || type_ca == CA_REVERSIBLE)
	{
		/////////////// NEED WORK //////////////
		for (i = 0; i < horz_count; ++i)
			WBM->WBMOnlyPutPixel(minx + i,
							maxy - (int)((vert_count-1) *
							((Real)(target_row[i])/ (states-1))),
							RGB(255, 255, 255));
		////////////////////////////////////////

		//DRAW RED GENERATORS //mike 11-25-97
		for (i=0; i < generatorlist.Count(); i++)
		{
			if (generatorlist.Location(i)<horz_count)
			{
				xGen = minx + generatorlist.Location(i);
				yGen = maxy - (int)((vert_count-1) *
							((Real)(target_row[generatorlist.Location(i)])/ (states-1)));
				WBM->PutRectangle(hdc, xGen - 1, yGen - 1, xGen + 1, yGen + 1, RGB(255, 0, 0));
			}							
		}

		

	}
	else
	{
		//-------- show graph of the whole thing
		for (i = 0; i < horz_count; ++i)
			WBM->WBMOnlyPutPixel(minx + i, maxy1 - (int)((vert_count1-1) *
							((float)(colorindex_target_row[i])/(MAX_COLOR-1))),
							RGB(255, 255, 255));

		//DRAW RED GENERATORS  //mike 11-25-97
		for (i=0; i < generatorlist.Count(); i++)
		{
			if (generatorlist.Location(i)<horz_count)
			{
				xGen = minx + generatorlist.Location(i);
				yGen = maxy1 - (int)((vert_count1-1) *
							((float)(colorindex_target_row[generatorlist.Location(i)])/(MAX_COLOR-1)));
				WBM->PutRectangle(hdc, xGen - 1, yGen - 1, xGen + 1, yGen + 1, RGB(255, 0, 0));
			}							
		}

		// show test point markers
		WBM->PutRectangle(hdc, minx + temp_test_point, miny1,
									  minx + temp_test_point, maxy1,
									  RGB(0, 100, 0));
		WBM->PutRectangle(hdc, minx + test_point, miny1,
									  minx + test_point, maxy1,
									  RGB(0, 255, 0));

		// show divider
		WBM->PutRectangle(hdc, minx, maxy1, maxx, maxy1, RGB(255, 255, 255));

		//---- show graph of the test point
		if(!fourierflag)
		{
			// wrap to the beginning of the array tp_real_array
			if( tp_endpos == (4 * horz_count) )
			{
				tp_startpos = 0;
				tp_endpos   = 2*horz_count;
			}

			// save the value of the test point
			if (!showvelocity)
				tp_real_array[tp_endpos] = wave_target_row[test_point].intensity;
			else
				tp_real_array[tp_endpos] = wave_target_row[test_point].velocity;
		}

		switch (tp_viewmode)
		{
			case IDC_TIMESCALETRUE:     	// normal view mode
				ystep = 1;
				xstep = 1;
				startpos = (tp_endpos >= horz_count)?
								(tp_endpos - horz_count) :	tp_startpos;
				break;

			case IDC_TIMESCALEHALF:
				ystep = 2;
				xstep = 1;
				startpos = (tp_endpos >= 2*horz_count)?
								2 * (int)((tp_endpos - 2*horz_count)/2) + 1 :
								tp_startpos;
				break;

			case IDC_TIMESCALEDOUBLE:
				ystep = 1;
				xstep = 2;
				startpos = (tp_endpos >= (int)(horz_count/2))?
								(tp_endpos - (int)(horz_count/2)) : tp_startpos;
				break;
		}
		M = (int)((tp_endpos - startpos)/2);

//Rudy added this to prevent divide by 0.
	if (M < 1)
		M = 1;

		fendpos = 2 * M;     // end position of fourier-array
		nterm = numofterm - 1;
		nterm = (nterm<M)? nterm : M-1 ;
		if (nterm < 1) nterm = 1; //Rudy prevent 1/0
		max_value = (!showvelocity)? _max_intensity.Val() : _max_velocity.Val();


		// draw graph of test point first
		if(tp_graphflag)
		{
			WBM->Getpen(hdc, PS_SOLID, 0, RGB(200, 200, 200));
			y_coord = Screen_yvalue(tp_real_array[startpos],
				max_value, miny2, maxy2);
			WBM->WBMOnlyMoveTo(minx, y_coord);
			x_coord = minx + xstep;
			for (i = startpos + ystep; i < tp_endpos; i += ystep)
			{
				y_coord = Screen_yvalue(tp_real_array[i],
					max_value, miny2, maxy2);
				WBM->WBMOnlyLineTo(x_coord, y_coord);
				x_coord += xstep;
			}
			WBM->Discardpen(hdc);
		}

		// if fourier approx is computed, then display it
		if(fourierflag)
		{
			x = new Real [2*M];
			if (tp_approxtype == IDC_BOTHSINECOSINE)
				for(j = 0; j < 2*M; j++)
					x[j] = PI * ((Real)j/(Real)M - 1.0);
			else  // tp_approxtype == IDC_SINEONLY or IDC_COSINEONLY
				for(j = 0; j < 2*M; j++)
					x[j] = PI * ((Real)j/((Real)M * 2.0) - 1.0);

			// draw the x-axis
			WBM->Getpen( hdc, PS_SOLID, 1, RGB(100, 100, 100) );
			y_coord = Screen_yvalue(0, max_value, miny2, maxy2);
			WBM->WBMOnlyMoveTo(minx, y_coord);
			WBM->WBMOnlyLineTo(maxx, y_coord);
			WBM->Discardpen(hdc);

			if(tp_cosineflag)
			{
				//---- Draw the cosine components  ak*cos(kx), k=0,..,nterm
				WBM->Getpen(hdc, PS_SOLID, 0, RGB(64, 64, 255));

				// Draw the constant term first
				y_coord = Screen_yvalue((fourier_a[0]/2.0),
															max_value, miny2, maxy2);
				WBM->WBMOnlyMoveTo(minx, y_coord);
				WBM->WBMOnlyLineTo(minx + (int)(fendpos/ystep)*xstep, y_coord);

				// then draw the other terms
				for (k = 1 ; k <= nterm; k++)
				{
					y_coord = Screen_yvalue((fourier_a[k] *
								cos((double)k * x[0])), max_value, miny2, maxy2);
					WBM->WBMOnlyMoveTo(minx, y_coord);
					x_coord = minx + xstep;
					for (j = ystep; j < fendpos; j += ystep)
					{
						y_coord = Screen_yvalue((fourier_a[k] *
							 cos((double)k * x[j])), max_value, miny2, maxy2);
						WBM->WBMOnlyLineTo(x_coord, y_coord);
						x_coord += xstep;
					}
				}
				WBM->Discardpen(hdc);
			}

			if(tp_sineflag)
			{
				//---- Draw the sine components  bk*sin(kx), k=1,..,nterm-1
				WBM->Getpen(hdc, PS_SOLID, 0, RGB(64, 255, 64));
				// go from term to term
				for (k = 0 ; k <= nterm-1; k++)
				{
					y_coord = Screen_yvalue((fourier_b[k] *
								sin((double)k * x[0])), max_value, miny2, maxy2);
					WBM->WBMOnlyMoveTo(minx, y_coord);
					x_coord = minx + xstep;
					for (j = ystep; j < fendpos; j += ystep)
					{
						y_coord = Screen_yvalue((fourier_b[k] *
									sin((double)k * x[j])), max_value, miny2, maxy2);
						WBM->WBMOnlyLineTo(x_coord, y_coord);
						x_coord += xstep;
					}
				}
				WBM->Discardpen(hdc);
			}

			if(tp_spectrumflag)
			{
				//-------- Draw Spectrum --------
				spec_interval = (int)((maxx - minx - 3)/(nterm));

				// Draw the cosine components  ak, k=0,..,nterm
				if (tp_approxtype != IDC_SINEONLY)
				{
					WBM->Getpen(hdc, PS_SOLID, 2, RGB(127, 127, 255));
					x_coord = minx + 1;
					y_coord = Screen_yvalue(0, max_value, miny2, maxy2);
					WBM->WBMOnlyMoveTo(x_coord, y_coord);
					WBM->WBMOnlyLineTo(x_coord, y_coord);
					y_coord = Screen_yvalue((fourier_a[0]/2.0),
														max_value, miny2, maxy2);
					WBM->WBMOnlyLineTo(x_coord, y_coord);

					x_coord += spec_interval;
					for(k = 1; k <= nterm; k++)
					{
						y_coord = Screen_yvalue(0, max_value, miny2, maxy2);
						WBM->WBMOnlyMoveTo(x_coord, y_coord);
						y_coord = Screen_yvalue(fourier_a[k], max_value,
																				miny2, maxy2);
						WBM->WBMOnlyLineTo(x_coord, y_coord);
						x_coord += spec_interval;
					}
					WBM->Discardpen(hdc);
				}

				// Draw the sine components  bk, k=0,..,nterm
				if (tp_approxtype != IDC_COSINEONLY)
				{
					WBM->Getpen(hdc, PS_SOLID, 2, RGB(63, 255, 63));
					x_coord = minx + 3;
					for(k = 0; k <= nterm; k++)
					{
						y_coord = Screen_yvalue(0, max_value, miny2, maxy2);
						WBM->WBMOnlyMoveTo(x_coord, y_coord);
						y_coord = Screen_yvalue(fourier_b[k], max_value,
																				miny2, maxy2);
						WBM->WBMOnlyLineTo(x_coord, y_coord);
						x_coord += spec_interval;
					}
					WBM->Discardpen(hdc);
				}
			}

			if(tp_approxflag)
			{
				// -------- Draw fourier approx --------
				WBM->Getpen(hdc, PS_SOLID, 0, RGB(255, 64, 64));
				y_coord = Screen_yvalue(fourier_approx[0],
															max_value, miny2, maxy2);
				WBM->WBMOnlyMoveTo(minx, y_coord);
				x_coord = minx + xstep;
				for (i = ystep; i < fendpos; i += ystep)
				{
					y_coord = Screen_yvalue(fourier_approx[i],
																max_value, miny2, maxy2);
					WBM->WBMOnlyLineTo(x_coord, y_coord);
					x_coord += xstep;
				}
				WBM->Discardpen(hdc);
			}
			delete x;
		}
		else
		// copy current value to another position for later wrapping
		// do this only when fourier approx is not displayed
		{
			if((tp_endpos - tp_startpos) == 2*horz_count)
			{
				tp_real_array[tp_startpos] = tp_real_array[tp_endpos];
				tp_startpos++;
			}
			tp_endpos++;
		}
	}
}


int CA::Screen_yvalue(Real val, Real max, int min_y, int max_y)
{
	int vertical, yval;

//Rudy added this to prevent divide by 0.
	if (max == 0)
		max = 1;

	vertical = max_y - min_y - 1;
	yval = max_y - (int)((Real)vertical * ((val + max) / (2.0 * max)));
	yval = (max_y < yval) ? max_y : ((min_y > yval) ? min_y : yval);

	return yval;
}

void EnableFourierDialogControls (HWND hDlg, BOOL flag )
{
	EnableWindow ( GetDlgItem( hDlg, IDC_APPROXIMATE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_CHANGESAMPLE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_FOURIER_SLIDER ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_SINEONLY ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_BOTHSINECOSINE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_COSINEONLY ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_FOURIERSCROLL ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_FOURIERGRAPH ), flag );
	EnableWindow ( GetDlgItem( hDlg, RADIO_ALL ), flag );
	EnableWindow ( GetDlgItem( hDlg, RADIO_FOCUS ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_NUMTERM ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_SPIN_FOURIER_TERMS ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_VIEWGRAPH ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_VIEWAPPROX ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_VIEWCOSINE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_VIEWSINE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_VIEWSPECTRUM ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_TIMESCALEHALF ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_TIMESCALETRUE ), flag );
	EnableWindow ( GetDlgItem( hDlg, IDC_TIMESCALEDOUBLE ), flag );
}
