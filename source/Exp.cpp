#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include <math.h>
#include <string.h>
#include <stdio.h>  //for sprintf
//#include <stdlib.h> //for gcvt
extern char *szMyAppName;
extern HWND hDlgExp, masterhwnd;
extern class CAlist *calife_list;
extern BOOL update_flag;
extern void realLabel (HWND hwnd, int button, Real num);  // display Real only

// void ShowGrid (HWND, int);
void ShowParams (HWND);
int  EqValue (int);
void ShowRange (HWND, Real, Real, Real, Real, int, int);
void RampTable (Real [], Real, Real, int, int, int, int);
void FillGrid (HWND, Real [], Real [], int, int, int);
void GetCurValues (Real [], int, int);
void SetCurValue (Real, int, int);

/* Note: CAs are grouped together for a different combination
			of parmeters into a NUM_GROUP. Thus, it must be
			increment by 1 for any new group. */
#define NUM_GROUP  6

/* Note: these index tables are used to display the initial
			parameter for both the vertical and horizontal
			parameter lists.  If a new group is added,
			make sure to add 0 to both tables. */
int X_Index[NUM_GROUP] = {0,0,0,0,0,0};
int Y_Index[NUM_GROUP] = {0,0,0,0,0,0};

int cur_group;			  // current index into X_Index and Y_Index
int type_ca;			  // type of CA

	static int	scaling;				// type of scaling
	static int CurHorzParam;		// horizontal parameter selection
	static int CurVertParam;      // vertical parameter selection
	static int numCAs;            // total number of CAs
   // l.andrews 11/2/01 "dimension" is now initialized in the constructor for CA
	static int dimension;         // number of CAs per row or column  
/* default min. and max. values for no paramameter, velocity, acceleration,
	max velocity range, max intensity range, heat count, lambda, band count,
	capacitance, resistance, voltage amplitude, voltage frequency, inductance,
	and entropy, score, in the same order as they are in
	the MIN* and MAX* arrays.  */
	static Real MinX[16] = {0.0, 0.3, 0.3, 1.0, 1.0, 2.0, MIN_LAMBDA, 2.0, 0.1, 0.1, 0.1, 0.1, 0.1, 0.0, 0.0};
	//warn 2017 at this line: conversion from 'double' to 'Real' requires a narrowing conversion
	//Don't know why.  Real is float, okay, but MIN_LAMBDA is a Real, so I dunno.  Don't worry about it.
	static Real MaxX[16] = {0.0,1.0,1.0,1000.0,1000.0,256,MAX_LAMBDA,MAX_COLOR,1.0,1.0,1.0,1.0,1.0,0.0,0.0};
	static Real MinY[16] = {0.0,0.3,0.3,1.0,1.0,2.0, MIN_LAMBDA, 0.0,0.1,0.1,0.1,0.1,0.1,0.0,0.0};
	//warn 2017 at this line: conversion from 'double' to 'Real' requires a narrowing conversion
	static Real MaxY[16] = {0.0,1.0,1.0,1000.0,1000.0,256,MAX_LAMBDA,MAX_COLOR,1.0,1.0,1.0,1.0,1.0,0.0,0.0};
	static BOOL oldsleepflag;

//									Message Cracker

#pragma argsused
static BOOL MyWnd_INITDIALOG(HWND hDlg,HWND hwndFocus,LPARAM lParam)
{
				/*Put the program to sleep while this dialog open,
						  otherwise the values get changed while you try to
				type them in.*/

//			oldsleepflag = calife_list->GetSleep();
			scaling = IDC_MULT_SCALE;
			CheckRadioButton(hDlg,IDC_MULT_SCALE,IDC_ADD_SCALE, scaling);
			update_flag = 1;  // needed for calling sc_update
   return TRUE;
}


#pragma argsused
static BOOL MyWnd_PAINT(HWND hDlg)
{
			SendMessage (hDlg, WM_COMMAND, SC_UPDATE, 0L);
   return TRUE;

}

#pragma argsused
static BOOL MyWnd_MOVE(HWND hDlg, int x, int y)
{
			SendMessage (hDlg, WM_COMMAND, SC_UPDATE, 0L);
   return TRUE;

}
#pragma argsused
static BOOL MyWnd_DESTROY(HWND hDlg)
{
	//		calife_list->SetSleep(oldsleepflag);
			hDlgExp = 0;
			InvalidateRect (masterhwnd, NULL, FALSE);
   return TRUE;

}

#pragma argsused
static BOOL MyWnd_CLOSE(HWND hDlg)
{
	DestroyWindow(hDlg);
   return TRUE;

}

#pragma argsused
static BOOL MyWnd_COMMAND(HWND hDlg,int id,HWND hwndCtl,UINT codeNotify)
{
	Real RampX[50];					// table of horiz ramp values
	Real RampY[50];					// table of vert ramp values
	char   buf[32];               // temporary string buffer
	Real MinMax[2];             // temp array

	switch (id)
	{
		case SC_UPDATE:
			type_ca = calife_list->FocusCA()->Gettype();
			ShowParams (hDlg);
			CurVertParam = EqValue(Y_Index[cur_group]);
				// remark: the number of horz params is one less
				// than the number of vert params, thus we add 1
			CurHorzParam = EqValue(X_Index[cur_group] +1);

			if ((CurHorzParam == IDC_ENTROPY_SCORE ||
				  CurHorzParam == IDC_ENTROPY) &&
				 (CurVertParam == IDC_ENTROPY_SCORE ||
				  CurVertParam == IDC_ENTROPY))
			{
				EnableWindow (GetDlgItem (hDlg, IDC_MULT_SCALE), FALSE);
				EnableWindow (GetDlgItem (hDlg, IDC_ADD_SCALE), FALSE);
				EnableWindow (GetDlgItem (hDlg, IDC_OK), FALSE);
			}
			else
			{
				EnableWindow (GetDlgItem (hDlg, IDC_MULT_SCALE), TRUE);
				EnableWindow (GetDlgItem (hDlg, IDC_ADD_SCALE), TRUE);
				EnableWindow (GetDlgItem (hDlg, IDC_OK), TRUE);
			}
			
			numCAs = calife_list->Count();
			dimension = sqrt(numCAs);  // Must be before ShowRange
					
			ShowRange (hDlg, MinX[CurHorzParam], MaxX[CurHorzParam],
						  MinY[CurVertParam], MaxY[CurVertParam],
						  CurHorzParam, CurVertParam);
			// Get CA values
			GetCurValues (RampX, CurHorzParam, numCAs);
			GetCurValues (RampY, CurVertParam, numCAs);
			// Set CA value to the edit control
			FillGrid (GetDlgItem (hDlg, IDC_DISPLAY),
						 RampX, RampY, dimension, CurHorzParam, CurVertParam);
			break;
		case IDC_HORZMIN:
			strcpy(buf, "");
			GetDlgItemText (hDlg, IDC_HORZMIN, buf, sizeof (buf));
			MinX[CurHorzParam] = atof (buf);
			break;

		case IDC_HORZMAX:
			strcpy (buf, "");
			GetDlgItemText (hDlg, IDC_HORZMAX, buf, sizeof (buf));
			MaxX[CurHorzParam] = atof (buf);
			break;

		case IDC_VERTMIN:
			strcpy(buf, "");
			GetDlgItemText (hDlg, IDC_VERTMIN, buf, sizeof (buf));
			MinY[CurVertParam] = atof (buf);
			break;

		case IDC_VERTMAX:
			strcpy(buf, "");
			GetDlgItemText (hDlg, IDC_VERTMAX, buf, sizeof (buf));
			MaxY[CurVertParam] = atof (buf);
			break;

		case IDC_HORZPARAM:
			if (codeNotify == CBN_SELCHANGE)
			{
				X_Index[cur_group] = (int) SendMessage ((HWND)hwndCtl,
													CB_GETCURSEL, 0, 0L);
					// remark: the number of horz params is one less
					// than the number of ver params, thus we add 1
				CurHorzParam = EqValue (X_Index[cur_group] + 1);
				SendMessage (hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
			break;

		case IDC_VERTPARAM:
			if (codeNotify == CBN_SELCHANGE)
			{
				Y_Index[cur_group] = (int) SendMessage ((HWND)hwndCtl,
														CB_GETCURSEL, 0, 0L);
				CurVertParam = EqValue (Y_Index[cur_group]);
				SendMessage (hDlg, WM_COMMAND, SC_UPDATE, 0L);
			}
			break;

		case IDC_ADD_SCALE:
		case IDC_MULT_SCALE:
			scaling = id;
			break;
		case IDC_RAMP:
			if (CurHorzParam != RAMP_NO_PARAM &&
						 CurHorzParam != IDC_ENTROPY_SCORE &&
						 CurHorzParam != IDC_ENTROPY)
			{
				MinMax[0] = MinX[CurHorzParam];
				MinMax[1] = MaxX[CurHorzParam];
				calife_list->CheckRange (CurHorzParam, MinMax);
				MinX[CurHorzParam] = MinMax[0];
				MaxX[CurHorzParam] = MinMax[1];
				RampTable (RampX, MinX[CurHorzParam], MaxX[CurHorzParam],
						   dimension, IDC_HORZPARAM, CurVertParam, scaling);
				// Set CA value to the edit control
				FillGrid (GetDlgItem (hDlg, IDC_DISPLAY),
						  RampX, RampY, dimension, CurHorzParam, RAMP_NO_PARAM);
			}
			if (CurVertParam != RAMP_NO_PARAM &&
				 CurVertParam != IDC_ENTROPY_SCORE &&
				 CurVertParam != IDC_ENTROPY)
			{
				MinMax[0] = MinY[CurVertParam];
				MinMax[1] = MaxY[CurVertParam];
				calife_list->CheckRange (CurVertParam, MinMax);
				MinY[CurVertParam] = MinMax[0];
				MaxY[CurVertParam] = MinMax[1];
				RampTable (RampY, MinY[CurVertParam], MaxY[CurVertParam],
							  dimension, IDC_VERTPARAM, CurHorzParam, scaling);
				// Set CA value to the edit control
				FillGrid (GetDlgItem (hDlg, IDC_DISPLAY),
						  RampX, RampY, dimension, RAMP_NO_PARAM, CurVertParam);
			}
			break;
		case IDC_OK:
			if (CurHorzParam != RAMP_NO_PARAM &&
						 CurHorzParam != IDC_ENTROPY_SCORE &&
						 CurHorzParam != IDC_ENTROPY)
			{
				GetCurValues (RampX, CurHorzParam, numCAs);
			
				char valueText[80];
				char *pValueText = valueText;
				Real value;
				for(int i = 0; i < numCAs; i++)
				{
					GetWindowText(GetDlgItem(hDlg, IDC_INFO11MIN+i), valueText, 80);
					value = atof(valueText);
					if(value == 0 && (strcmp(valueText, "0") != 0))
					{
						sprintf(pValueText, "%5.5f", RampX[i]);
						SetWindowText(GetDlgItem(hDlg, IDC_INFO11MIN+i), valueText);	
					}
					else RampX[i] = value;
				}
				calife_list->Install_Ramp(CurHorzParam, RampX);
			}
			if (CurVertParam != RAMP_NO_PARAM &&
				 CurVertParam != IDC_ENTROPY_SCORE &&
				 CurVertParam != IDC_ENTROPY)
			{
				GetCurValues (RampY, CurVertParam, numCAs);
			
				char valueText[80];
				char *pValueText = valueText;
				Real value;
				for(int i = 0; i < numCAs; i++)
				{
					GetWindowText(GetDlgItem(hDlg, IDC_INFO11MAX+i), valueText, 80);
					value = atof(valueText);
					if (value == 0 && (strcmp(valueText, "0") != 0))
					{
						sprintf(pValueText, "%5.5f", value);
						SetWindowText(GetDlgItem(hDlg, IDC_INFO11MAX+i), 
									  valueText);	
					}
					else RampY[i] = value;
				}
				calife_list->Install_Ramp(CurVertParam, RampY);
			}
			update_flag = 1;   // redraw other dialogs
			break;
		case IDC_EXIT:
			DestroyWindow (hDlg);
			break;
	}
	return TRUE;
}

extern BOOL CALLBACK ExpProc (HWND hDlg, UINT message,
										  WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg,WM_PAINT,MyWnd_PAINT);
		HANDLE_MSG(hDlg,WM_MOVE,MyWnd_MOVE);
		HANDLE_MSG(hDlg,WM_INITDIALOG,MyWnd_INITDIALOG);
		HANDLE_MSG(hDlg,WM_DESTROY,MyWnd_DESTROY);
		HANDLE_MSG(hDlg,WM_CLOSE,MyWnd_CLOSE);
		HANDLE_MSG(hDlg,WM_COMMAND,MyWnd_COMMAND);
		default:
			return FALSE;
	}
}

void GetCurValues (Real curval[], int paramcode, int numCAs)
{
	int theCA_type;
	if (paramcode != RAMP_NO_PARAM)
	{
		for (int i=0; i < numCAs; i++)
		{
			switch (paramcode)
			{
				case RAMP_ACCEL:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(SPACE_STEP_TYPE)->Val();
				break;

				case RAMP_VEL_MULT:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(TIME_STEP_TYPE)->Val();
				break;

				case RAMP_MAX_VEL:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(MAX_VELOCITY_TYPE)->Val();
				break;

				case RAMP_LAMBDA:
					curval[i] = calife_list->GetCA(i)->Getlambda();
				break;

				case RAMP_BAND:
					curval[i] = calife_list->GetCA(i)->Getband();
				break;

				case RAMP_MAX_INTENSITY:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(MAX_INTENSITY_TYPE)->Val();
				break;

				case RAMP_HEAT_COUNT:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(HEAT_INC_TYPE)->Val();
				break;

				case RAMP_FRICTION:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(FRICTION_TYPE)->Val();
				break;

				case RAMP_SPRING:
					theCA_type = calife_list->GetCA(i)->Gettype();
					if (theCA_type == CA_ULAM_WAVE||theCA_type==ALT_CA_ULAM_WAVE)
						curval[i] = calife_list->GetCA(i)->GetTweakParam(NONLINEARITY1_TYPE)->Val();
					else if (theCA_type == CA_AUTO_ULAM_WAVE||theCA_type==CA_CUBIC_ULAM_WAVE)
						curval[i] = calife_list->GetCA(i)->GetTweakParam(NONLINEARITY2_TYPE)->Val();
					else
						curval[i] = calife_list->GetCA(i)->GetTweakParam(SPRING_TYPE)->Val();
				break;

				case RAMP_DRIVER_AMP:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(DRIVER_AMP_TYPE)->Val();
				break;

				case RAMP_DRIVER_FREQ:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(DRIVER_FREQ_TYPE)->Val();
				break;

				case RAMP_INDUCTANCE:
					curval[i] = calife_list->GetCA(i)->GetTweakParam(MASS_TYPE)->Val();
				break;

				case IDC_ENTROPY_SCORE:
					curval[i] = calife_list->GetCA(i)->Getscore();
				break;

				case IDC_ENTROPY:
					curval[i] = calife_list->GetCA(i)->Getentropy();
				break;

			}
		}
	}
}

void SetCurValue (Real curval, int paramcode, int i)
{
	int theCA_type;
	if (paramcode == RAMP_NO_PARAM) return;

	switch (paramcode)
	{
		case RAMP_ACCEL:
			calife_list->GetCA(i)->GetTweakParam(SPACE_STEP_TYPE)->SetVal(curval);
			break;
		case RAMP_VEL_MULT:
			calife_list->GetCA(i)->GetTweakParam(TIME_STEP_TYPE)->SetVal(curval);
			break;
		case RAMP_MAX_VEL:
			calife_list->GetCA(i)->GetTweakParam(MAX_VELOCITY_TYPE)->SetVal(curval);
			break;
		case RAMP_LAMBDA:
			calife_list->GetCA(i)->Setlambda(curval);
			break;
		case RAMP_BAND:
			calife_list->GetCA(i)->Set_band_count(curval);
			break;
		case RAMP_MAX_INTENSITY:
			calife_list->GetCA(i)->GetTweakParam(MAX_INTENSITY_TYPE)->SetVal(curval);
			break;
		case RAMP_HEAT_COUNT:
			calife_list->GetCA(i)->GetTweakParam(HEAT_INC_TYPE)->SetVal(curval);
			break;
		case RAMP_FRICTION:
			calife_list->GetCA(i)->GetTweakParam(FRICTION_TYPE)->SetVal(curval);
			break;
		case RAMP_SPRING:
			theCA_type = calife_list->GetCA(i)->Gettype();
			if (theCA_type == CA_ULAM_WAVE||theCA_type==ALT_CA_ULAM_WAVE)
				calife_list->GetCA(i)->GetTweakParam(NONLINEARITY1_TYPE)->SetVal(curval);
			else if (theCA_type == CA_AUTO_ULAM_WAVE||theCA_type==CA_CUBIC_ULAM_WAVE)
				calife_list->GetCA(i)->GetTweakParam(NONLINEARITY2_TYPE)->SetVal(curval);
			else calife_list->GetCA(i)->GetTweakParam(SPRING_TYPE)->SetVal(curval);
			break;
		case RAMP_DRIVER_AMP:
			calife_list->GetCA(i)->GetTweakParam(DRIVER_AMP_TYPE)->SetVal(curval);
			break;
		case RAMP_DRIVER_FREQ:
			calife_list->GetCA(i)->GetTweakParam(DRIVER_FREQ_TYPE)->SetVal(curval);
			break;
		case RAMP_INDUCTANCE:
			calife_list->GetCA(i)->GetTweakParam(MASS_TYPE)->SetVal(curval);
			break;
		case IDC_ENTROPY_SCORE:
			calife_list->GetCA(i)->Setscore(curval);
			break;
		case IDC_ENTROPY:
			calife_list->GetCA(i)->Changeentropy(curval);
			break;
	}
}

void ShowParams (HWND hDlg)
{
	HWND hCtrl;

	SendMessage (GetDlgItem (hDlg, IDC_HORZPARAM),
					CB_RESETCONTENT, 0, 0L);
	SendMessage (GetDlgItem (hDlg, IDC_VERTPARAM),
					CB_RESETCONTENT, 0, 0L);
	switch (type_ca)
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
			cur_group = 0;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Lambda");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	 //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Lambda");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;

		case CA_WAVE:
		case CA_WAVE2:
		case ALT_CA_WAVE:
		case ALT_CA_WAVE2:
			cur_group = 1;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	  //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;

		case CA_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case ALT_CA_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
			cur_group = 2;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Nonlinearity");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	  //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Nonlinearity");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;

		case CA_HEATWAVE:
		case CA_HEATWAVE2:
			cur_group = 3;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Heat Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	 //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Heat Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;

		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
		case ALT_CA_OSCILLATOR:
		case ALT_CA_DIVERSE_OSCILLATOR:
			cur_group = 4;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "1/Capacitance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Resistance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Amp.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Freq.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Inductance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	  //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "1/Capacitance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Resistance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Amp.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Freq.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Inductance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;

//		case CA_OSCILLATOR_WAVE:
		case CA_DIVERSE_OSCILLATOR_WAVE:
		case ALT_CA_OSCILLATOR_WAVE:
		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
		case CA_USER:
			cur_group = 5;    // named for this group
			// Set vertical parameters
			hCtrl = GetDlgItem (hDlg, IDC_VERTPARAM);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "1/Capacitance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Resistance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Amp.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Freq.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Inductance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, Y_Index[cur_group], 0L);

			// Set horizontal parameters
			hCtrl = GetDlgItem (hDlg, IDC_HORZPARAM);
	  //		SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "No Param");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Time Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Space Step");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Vel. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Int. Range");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Band Count");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "1/Capacitance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Resistance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Amp.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Volt. Freq.");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Inductance");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Entropy");
			SendMessage (hCtrl, CB_ADDSTRING, 0, (LONG) "Score");
			SendMessage (hCtrl, CB_SETCURSEL, X_Index[cur_group], 0L);
			break;
	}
}

int EqValue(int NewValue)
{
	switch (type_ca)
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_LAMBDA;
			if (NewValue == 2)
				return IDC_ENTROPY;
			if (NewValue == 3)
				return IDC_ENTROPY_SCORE;

		case CA_WAVE:
		case CA_WAVE2:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_VEL_MULT;
			if (NewValue == 2)
				return RAMP_ACCEL;
			if (NewValue == 3)
				return RAMP_MAX_VEL;
			if (NewValue == 4)
				return RAMP_MAX_INTENSITY;
			if (NewValue == 5)
				return RAMP_BAND;
			if (NewValue == 6)
				return IDC_ENTROPY;
			if (NewValue == 7)
				return IDC_ENTROPY_SCORE;

		case CA_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case ALT_CA_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_VEL_MULT;
			if (NewValue == 2)
				return RAMP_ACCEL;
			if (NewValue == 3)
				return RAMP_MAX_VEL;
			if (NewValue == 4)
				return RAMP_MAX_INTENSITY;
			if (NewValue == 5)
				return RAMP_BAND;
			if (NewValue == 6)
				return RAMP_SPRING;
			if (NewValue == 7)
				return IDC_ENTROPY;
			if (NewValue == 8)
				return IDC_ENTROPY_SCORE;

		case CA_HEATWAVE:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_VEL_MULT;
			if (NewValue == 2)
				return RAMP_ACCEL;
			if (NewValue == 3)
				return RAMP_MAX_VEL;
			if (NewValue == 4)
				return RAMP_MAX_INTENSITY;
			if (NewValue == 5)
				return RAMP_HEAT_COUNT;
			if (NewValue == 6)
				return RAMP_BAND;
			if (NewValue == 7)
				return IDC_ENTROPY;
			if (NewValue == 8)
				return IDC_ENTROPY_SCORE;

		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_VEL_MULT;
			if (NewValue == 2)
				return RAMP_MAX_VEL;
			if (NewValue == 3)
				return RAMP_MAX_INTENSITY;
			if (NewValue == 4)
				return RAMP_BAND;
			if (NewValue == 5)
				return RAMP_SPRING;
			if (NewValue == 6)
				return RAMP_FRICTION;
			if (NewValue == 7)
				return RAMP_DRIVER_AMP;
			if (NewValue == 8)
				return RAMP_DRIVER_FREQ;
			if (NewValue == 9)
				return RAMP_INDUCTANCE;
			if (NewValue == 10)
				return IDC_ENTROPY;
			if (NewValue == 11)
				return IDC_ENTROPY_SCORE;

	 //	case CA_OSCILLATOR_WAVE:
		case CA_DIVERSE_OSCILLATOR_WAVE:
			if (NewValue == 0)
				return RAMP_NO_PARAM;
			if (NewValue == 1)
				return RAMP_VEL_MULT;
			if (NewValue == 2)
				return RAMP_ACCEL;
			if (NewValue == 3)
				return RAMP_MAX_VEL;
			if (NewValue == 4)
				return RAMP_MAX_INTENSITY;
			if (NewValue == 5)
				return RAMP_BAND;
			if (NewValue == 6)
				return RAMP_SPRING;
			if (NewValue == 7)
				return RAMP_FRICTION;
			if (NewValue == 8)
				return RAMP_DRIVER_AMP;
			if (NewValue == 9)
				return RAMP_DRIVER_FREQ;
			if (NewValue == 10)
				return RAMP_INDUCTANCE;
			if (NewValue == 11)
				return IDC_ENTROPY;
			if (NewValue == 12)
				return IDC_ENTROPY_SCORE;

		default:
			return NewValue;
	}
}

void ShowRange (HWND hDlg, Real minx, Real maxx,
					 Real miny, Real maxy, int hparam, int vparam)
{
	char buf[32];

	// horz values
	strcpy (buf, "");
	if ((hparam != RAMP_NO_PARAM) &&
		 (hparam != IDC_ENTROPY_SCORE) &&
		 (hparam != IDC_ENTROPY))
	{
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMIN), TRUE);
		realLabel (hDlg, IDC_HORZMIN, minx);
	}

	strcpy (buf, "");
	if (hparam != RAMP_NO_PARAM &&
		 hparam != IDC_ENTROPY_SCORE &&
		 hparam != IDC_ENTROPY)
	{
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMAX), TRUE);
		realLabel (hDlg, IDC_HORZMAX, maxx);
		//wsprintf (buf, "%.2f", maxx);
		//SendMessage (GetDlgItem (hDlg, IDC_HORZMAX), WM_SETTEXT, 0,	(LONG) buf);
	}

	//  vert values
	strcpy (buf, "");
	if (vparam != RAMP_NO_PARAM &&
		 vparam != IDC_ENTROPY_SCORE &&
		 vparam != IDC_ENTROPY)
	{
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMIN), TRUE);
		realLabel (hDlg, IDC_VERTMIN, miny);
        //wsprintf (buf, "%.2f", miny);
		//SendMessage (GetDlgItem (hDlg, IDC_VERTMIN), WM_SETTEXT, 0,	(LONG) buf);
	}

	strcpy (buf, "");
	if (vparam != RAMP_NO_PARAM &&
		 vparam != IDC_ENTROPY_SCORE &&
		 vparam != IDC_ENTROPY)
	{
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMAX), TRUE);
		realLabel (hDlg, IDC_VERTMAX, maxy);
		//wsprintf (buf, "%.2f", maxy);
		//SendMessage (GetDlgItem (hDlg, IDC_VERTMAX), WM_SETTEXT, 0,	(LONG) buf);
	}

	if (vparam == RAMP_NO_PARAM )
	{
		strcpy (buf, "");
		SendMessage (GetDlgItem (hDlg, IDC_VERTMIN), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMIN), FALSE);
		SendMessage (GetDlgItem (hDlg, IDC_VERTMAX), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMAX), FALSE);
		for(int i = 0; i < numCAs; i++)
		{
			SendMessage (GetDlgItem (hDlg, IDC_INFO11MAX+i), WM_SETTEXT, 0,	(LONG) buf);
			EnableWindow (GetDlgItem (hDlg, IDC_INFO11MAX+i), FALSE);
		}
	}
	else 
	{
		for(int row = 0; row < 3; row++)	// Row
			for(int col = 0; col < 3; col++)	// column
				if (row < dimension && col < dimension)
				{					
					SendMessage (GetDlgItem (hDlg, IDC_INFO11MAX+(row*3)+col), WM_SETTEXT, 0,	(LONG) buf);
					EnableWindow (GetDlgItem (hDlg, IDC_INFO11MAX+(row*3)+col), TRUE);
				}
				else
				{
					SendMessage (GetDlgItem (hDlg, IDC_INFO11MAX+(row*3)+col), WM_SETTEXT, 0,	(LONG) buf);
					EnableWindow (GetDlgItem (hDlg, IDC_INFO11MAX+(row*3)+col), FALSE);
				}
	}

	if (hparam == RAMP_NO_PARAM )
	{
		strcpy (buf, "");
		SendMessage (GetDlgItem (hDlg, IDC_HORZMIN), WM_SETTEXT, 0,	(LONG) buf );
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMIN), FALSE);
		SendMessage (GetDlgItem (hDlg, IDC_HORZMAX), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMAX), FALSE);
		for(int i = 0; i < numCAs; i++)
		{
			SendMessage (GetDlgItem (hDlg, IDC_INFO11MIN+i), WM_SETTEXT, 0,	(LONG) buf);
			EnableWindow (GetDlgItem (hDlg, IDC_INFO11MIN+i), FALSE);
		}
	}
	else 
	{
		for(int row = 0; row < 3; row++)	// Row
			for(int col = 0; col < 3; col++)	// column
				if (row < dimension && col < dimension)
				{					
					SendMessage (GetDlgItem (hDlg, IDC_INFO11MIN+(row*3)+col), WM_SETTEXT, 0,	(LONG) buf);
					EnableWindow (GetDlgItem (hDlg, IDC_INFO11MIN+(row*3)+col), TRUE);
				}
				else
				{
					SendMessage (GetDlgItem (hDlg, IDC_INFO11MIN+(row*3)+col), WM_SETTEXT, 0,	(LONG) buf);
					EnableWindow (GetDlgItem (hDlg, IDC_INFO11MIN+(row*3)+col), FALSE);
				}
	}

	if (hparam == IDC_ENTROPY_SCORE || hparam == IDC_ENTROPY)
	{
		strcpy (buf, "");
		SendMessage (GetDlgItem (hDlg, IDC_HORZMIN), WM_SETTEXT, 0,	(LONG) buf );
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMIN), FALSE);
		SendMessage (GetDlgItem (hDlg, IDC_HORZMAX), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_HORZMAX), FALSE);
	}

	if (vparam == IDC_ENTROPY_SCORE || vparam == IDC_ENTROPY)
	{
		strcpy (buf, "");
		SendMessage (GetDlgItem (hDlg, IDC_VERTMIN), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMIN), FALSE);
		SendMessage (GetDlgItem (hDlg, IDC_VERTMAX), WM_SETTEXT, 0,	(LONG) buf);
		EnableWindow (GetDlgItem (hDlg, IDC_VERTMAX), FALSE);
	}
}

void FillGrid (HWND hwnd, Real rampX[], Real rampY[], int d,
					int hparam, int vparam)
/* increasing y is going down; increasing x is going left */
{
	if (hparam == RAMP_NO_PARAM &&
		 vparam == RAMP_NO_PARAM)
		 // do nothing
		  ;
	else if (hparam == RAMP_NO_PARAM)
	{	// Need parathensis
		int index = 0;
		for(int row = 0; row < 3; row++)	// Row
			for(int col = 0; col < 3; col++)	// column
				if (row < d && col < d)
				{					
					char valueText[80];
					sprintf(valueText, "%.3f", rampY[index]);
					SetWindowText(GetDlgItem(hDlgExp, IDC_INFO11MAX + (row*3) + col), valueText);
					index++;
				}
	}
	else if (vparam == RAMP_NO_PARAM)
	{	// Need parathensis
		int index = 0;
		for(int row = 0; row < 3; row++)	// Row
			for(int col = 0; col < 3; col++)	// column
				if (row < d && col < d)
				{					
					char valueText[80];
					sprintf(valueText, "%.3f", rampX[index]);
					SetWindowText(GetDlgItem(hDlgExp, IDC_INFO11MIN + (row*3) + col), valueText);
					index++;
				}

	}
	else
	{
		int index = 0;
		for(int row = 0; row < 3; row++)	// Row
			for(int col = 0; col < 3; col++)	// column
				if (row < d && col < d)
				{					
					char valueText[80];
					sprintf(valueText, "%.3f", rampX[index]);
					SetWindowText(GetDlgItem(hDlgExp, IDC_INFO11MIN + (row*3) + col), valueText);
		
					sprintf(valueText, "%.3f", rampY[index]);
					SetWindowText(GetDlgItem(hDlgExp, IDC_INFO11MAX + (row*3) + col), valueText);
					index++;
				}
	}
}

void RampTable (Real ramp[], Real min, Real max,
					 int dim, int hvparam, int v_h_param, int scaling)
/* if v_h_param is a No Param, we want to ramp only
	on the side of hvparam */
{
	int numCAs = dim * dim;
	int i = 0;
	int steps;

	if (hvparam == IDC_HORZPARAM)
	{	if (v_h_param == RAMP_NO_PARAM)    // if a vert param is a No Param
		{  if (numCAs == 1)
				steps = 1;
			else
				steps = numCAs - 1;
			for (int j = 0; j < numCAs; j++)
				if (scaling == IDC_MULT_SCALE)       // multiplicative scaling
					ramp[j] = min * pow(exp((log(max) -
						log(min)) / steps), j);
				else // scaling == IDC_ADD_SCALE)	// additive scaling
					ramp[j] = min + j * ((max - min) / steps);
		}
		else
		{
			if (dim == 1)
				steps = 1;
			else
				steps = dim - 1;
			while (i <= (numCAs - dim))
			{
				for (int j = 0; j < dim; j++)
					if (scaling == IDC_MULT_SCALE)    // multiplicative scaling
						ramp[i + j] = min * pow(exp((log(max) -
							log(min)) / steps), j);
					else // scaling == IDC_ADD_SCALE   // additive scaling
						ramp[i + j] = min + j * ((max - min) / steps);
				i = i + dim;
			}
		}
	}
	else // (hvparam == IDC_VERTPARAM)
	{
		if (v_h_param == RAMP_NO_PARAM)  // if horiz param is a No Param
		{  if (numCAs == 1)
				steps = 1;
			else
				steps = numCAs - 1;
			for (int j = 0; j < numCAs; j++)
				if (scaling == IDC_MULT_SCALE)       // multiplicative scaling
					ramp[j] = min * pow(exp((log(max) -
						log(min)) / steps), j);
				else // scaling == IDC_ADD_SCALE)	// additive scaling
					ramp[j] = min + j * ((max - min) / steps);
		}
		else
		{
			if (dim == 1)
				steps = 1;
			else
				steps = dim - 1;
			Real temp[50];
			for (int j = 0; j < dim; j++)
				if (scaling == IDC_MULT_SCALE)	// multiplicative scaling
					temp[j] = min * pow(exp((log(max) -
						log(min)) / steps), j);
				else // scaling == IDC_ADD_SCALE		// additive scaling
					temp[j] = min + j * ((max - min) / steps);

			int k = dim - 1;
			while (i <= (numCAs - dim))
			{
				for (int j = 0; j < dim; j++)
					ramp[i + j] = temp[k];
				i = i + dim;
				k--;
			}
		}
	}
}
