

/*******************************************************************************
	FILE:				calist.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains CALIST class function defintions.

	UPDATE LOG:			9-26-97
*******************************************************************************/
//====================INCLUDES===============

#include "ca.hpp"
#include "resource.h"
#include "random.h"
//#include "Graph3D.hpp"
#include <math.h>
#include "userpara.hpp"
#include "capowgl.hpp"  //mike
#include "status.hpp"
//--------------------CONSTANTS used by CAPOWGL.CPP
#include "capowgl.hpp" //For things like FLATCOLOR and SHEET


//====================EXTERNAL DATA===============
//extern Graph3D* graph;
extern char CA_STYLE_NAME[]; //Defined in CAPOW.CPP

extern int  toolBarHeight;
extern int  statusBarHeight;

extern HWND hDlgOpenGL;
extern HWND hwndStatusBar;
extern HWND hDlgFourier;
extern BOOL InMessageProcessing;

extern BOOL zoomviewflag;
extern HWND masterhwnd;
extern CapowGL* capowgl;

extern BOOL statusON;
extern BOOL toolbarON;


//====================GLOBAL DATA===============

int blt_flag = 0; // this is for the timing of bitblting to screen
int StartStates[STATERADIUSCOUNT] = {2,4,8,16,2,4,2,4,2,2};
int StartRadius[STATERADIUSCOUNT] = {1,1,1, 1,2,2,3,3,4,5};

/******************************************************************************/

CAlist::CAlist(HWND myhwnd, int maxcount)
{
	int i;
	HDC hdc = GetDC(myhwnd);
	hwnd = myhwnd;
	_plgpl = (LOGPALETTE *) new char[sizeof(LOGPALETTE) +
		 sizeof(PALETTEENTRY) * 236];
	int bitspixel = GetDeviceCaps(hdc, BITSPIXEL);
	switch (bitspixel)
	{
		case 4:
			_numcolor = 16;
			break;
		case 8:
          	_numcolor = 256;
			break;
		default:
			_numcolor = 1000; //Just use a number not 256.
			break;
	}
	count = START_CAS;
	mutation_strength = START_MUTATION_STRENGTH;
   focus = 0; // l.andrews 11/2/01 since this is not initialized elsewhere
	for (i=0; i<maxcount; i++)
	{
		list[i] = new CA(this);
		if (!list[i])
		{
			MessageBox(hwnd,
				(LPSTR)"Failure in CA Allocation!",
				(LPSTR)"Memory Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			SendMessage(hwnd, WM_DESTROY, 0, 0L );
			return;
		}
	}
	_blt_lines = START_BLT_LINES;
	focus = list[0];
	CA_count_per_edge = (int) sqrt(count);
	if ((CA_count_per_edge*CA_count_per_edge)<count)
		CA_count_per_edge++;
	gl_sleep = sleep = 0;
	zoomflag = 0;
	breedflag = START_BREED_FLAG;
	breedcycle = START_BREED_CYCLE;
	breedcycle_count = 0;
	evolveflag = START_EVOLVEFLAG;
	mutateflag = START_MUTATE_FLAG;
//	scrollflag = START_SCROLL_FLAG;
	stripekillflag = START_STRIPEKILL_FLAG;
	stripeseedflag = START_STRIPESEED_FLAG;
	_hpal = 0;

//	wireflag = START_WIRE_FLAG;  //NOte that wireflag and
//	graphflag = START_GRAPH_FLAG;//graphflag are no longer used. But keep for
		//file consistency, at least for now.  11/95.
	Setcolortable();
	// ---------- Fourier ----------
        tp_dowhich              = IDC_FOURIERALL;
        // -----------------------------
	ReleaseDC(hwnd, hdc);
}

CAlist::~CAlist()
{
	for ( int i = 0; i < MAX_CAS; i++)
	{
		delete list[i];
		list[i] = NULL; //Just to prevent anyone from trying to use them.
	}
	if (_hpal)
		DeleteObject(_hpal);
   if (_plgpl)
		delete [] _plgpl;
}



//This function is no longer used. Locate(void) is used instead.
void CAlist::Locate(int dmaxx, int dmaxy)
{
//Don't do any Clear or Boxfocus in here, leave that to capow.cpp
//	if (!zoomflag)
	if(!zoomviewflag)
		for (int i=0; i<count; i++)
//			list[i]->Locate(i, dmaxx, dmaxy, CA_count_per_edge);
			list[i]->Locate(i, masterhwnd, CA_count_per_edge);
			//Used to have masterhwnd here, but hwnd is a member
			//with the same value, use that instead.
	else
//		focus->Locate(0, dmaxx, dmaxy, 1);
		focus->Locate(0, hwnd, 1);
	blt_flag = 0;

}

void CAlist::Locate()		 //mike 4/97
//this locate function uses hwnd to resize the location
//variables for each of the CAs
{
	RECT rect;

	/*the status bar height is dependent on the current system font size*/
	if (hwndStatusBar) 
	{
		GetWindowRect(hwndStatusBar, &rect);
		statusBarHeight = rect.bottom - rect.top;
	}
	else 
		statusBarHeight = 0;

	if (!zoomviewflag) //if not zoomed, call Locate for each CA
		for (int i=0; i<count; i++)
			list[i] -> Locate(i, masterhwnd, CA_count_per_edge);
	else
		focus->Locate(0, hwnd, 1);
	blt_flag = 0; 
/* Mike left this line out, which caused an ugly
	lip at the bottom of the 1D CAs in scroll view and split view.  Rudy put it
	back in 5/20/97 after three hours of code raking.
	Moral: please don't leave out "extra" lines!!!!*/
}

void CAlist::Update_and_Show(HDC hdc)
{
	int i;
	int x,y;
	HDC glhdc;  //used for drawing to opengl dialog
	HWND hCtrlBlock;  //used for drawing to opengl dialog

#ifdef ONE_AT_A_TIME
	static int next_i = 0;
#endif //ONE_AT_A_TIME

	capowgl->FocusIsActive(!sleep);

	if (sleep)
	{
		if (zoomflag && focus->viewmode==IDC_2D_VIEW && !gl_sleep)
		//the 2D CA view can still be changing even when the CA is 
		//sleeping, so you are allowed to change it IF you turn off gl_sleep.
		//Normally the gl_sleep value mathches the value of sleep.
		{
			if (capowgl->Type())
				capowgl->Draw(hdc, focus);
			else
			{
				StretchBlt(hdc,   //target hdc
					0, (toolbarON)?toolBarHeight:0,  //target corner
				focus->horz_count+2, focus->vert_count,//targ size
				WBM->GetHDC(),                 //source  hdc
				focus->minx, focus->miny,  //source corner
				focus->horz_count_2D, focus->vert_count_2D,//sc size
				SRCCOPY);
			}
				
			if (hDlgOpenGL)  //if open, draw bitmap to opengl dialog
			{
				hCtrlBlock = GetDlgItem(hDlgOpenGL, IDC_OPENGL_FLAT);
				glhdc = GetDC(hCtrlBlock);
				BitBlt(glhdc, 0,0, focus->horz_count_2D, focus->vert_count_2D, WBM->GetHDC(),focus->minx, focus->miny, SRCCOPY);
				ReleaseDC(hCtrlBlock, glhdc);
			}
		}
		return;
	}
	if (!zoomflag)  // not zoomed, do them all
	{
#ifdef ONE_AT_A_TIME
		list[next_i]->Update(hdc);
		next_i++;
		if (next_i < count)
			return;
		next_i = 0;  //you're done, so now you can continue.
#else  //not ONE_AT_A_TIME
		for (i=0; i<count; i++)  //DON'T BLINK OR YOU'LL MISS IT :
			list[i]->Update(hdc);  //HERE'S WHERE THE UPDATE HAPPENS!!!!
#endif //ONE_AT_A_TIME
		blt_flag++;
          for (i=0; i<count; i++)
		switch (list[i]->viewmode)  //9/24/97 Getting exit bug here with THREAD.
		{
			case IDC_DOWN_VIEW:
//Minor bug here, this seems not to show lines at bottom of the box.
				if (!(blt_flag % _blt_lines))
				{
					if (list[0]->row_number == list[0]->miny)
					//In this case you just wrapped row_number to miny
					{  //blt the bottom strip
						WBM->WBMBitBlt(hdc,
							list[i]->minx,
							list[i]->maxy-_blt_lines,
							list[i]->maxx,
							list[i]->maxy); //maxy is row_number - 1 here
					}
					else
                         {
						WBM->WBMBitBlt(hdc,
							list[i]->minx,
							list[i]->row_number-_blt_lines,
							list[i]->maxx,
							list[i]->row_number - 1);
					}
					blt_flag = 0;
				}
				break;
			case IDC_SCROLL_VIEW:
				if (!(blt_flag % _blt_lines))
				{
					WBM->WBMBitBlt(hdc, list[i]->minx, list[i]->miny,
							list[i]->maxx, list[i]->maxy);
					blt_flag = 0;
				}
				break;
			case IDC_WIRE_VIEW:
				break;
			case IDC_2D_VIEW:
/* Later I can add the StretchBlt optoin for all the types for use in
the FIXED_640_480 case.*/
				if (list[i]->maxx_2D == list[i]->maxx &&
					list[i]->maxy_2D == list[i]->maxy)
					WBM->WBMBitBlt(hdc, list[i]->minx, list[i]->miny,
							list[i]->maxx_2D, list[i]->maxy_2D);
				else
					StretchBlt(hdc,   //target hdc
						list[i]->minx, list[i]->miny,  //target corner
						list[i]->horz_count, list[i]->vert_count,//targ size
						WBM->GetHDC(),                 //source  hdc
						list[i]->minx, list[i]->miny,  //source corner
						list[i]->horz_count_2D, list[i]->vert_count_2D,//sc size
						SRCCOPY);
				break;
			case IDC_GRAPH_VIEW:
			//======== Bang-Nguyen ========
			case IDC_POINT_GRAPH:
			//=============================
				WBM->WBMBitBlt(hdc, list[i]->minx, list[i]->miny,
							list[i]->maxx, list[i]->maxy);
				break;
			case IDC_SPLIT_VIEW:
				// SCROLL due case
				if (!(blt_flag % _blt_lines))  //Blt the whole screen
				{
					WBM->WBMBitBlt(hdc, list[i]->minx, list[i]->miny,
							list[i]->maxx, list[i]->maxy);
					blt_flag = 0;
				}
				else //just do the GRAPH strips.  Use the GOACROSS approach.
				{
					WBM->WBMBitBlt(hdc,
						list[i]->minx,
						list[i]->splity+1,
						list[i]->maxx,
						list[i]->maxy);
				}
				break;
		}
		if (breedflag)  //only breed in non zoomflag mode
		{
			++breedcycle_count;
			if (breedcycle_count >= breedcycle)
			{
				Evolve();       // Evolving methods for breeding
				breedcycle_count = 0;
			}
		}
	}// end non zoomflag  case
	else
	{
		focus->Update(hdc);     // We are zoomed, just do the focused one
		blt_flag++;
		switch (focus->viewmode)
		{
			case IDC_2D_VIEW:
				capowgl->FocusIsActive(TRUE);   //tell capowgl focus is not asleep

				if (capowgl->Type())  //if not flat 2-D
					capowgl->Draw(hdc, focus);  //draw 3-D view
				else	//draw the flat 2-D
				{
					StretchBlt(hdc,   //target hdc
						0, (toolbarON)?toolBarHeight:0,  //target corner
					focus->horz_count+2, focus->vert_count,//targ size
					WBM->GetHDC(),                 //source  hdc
					focus->minx, focus->miny,  //source corner
					focus->horz_count_2D, focus->vert_count_2D,//sc size
					SRCCOPY);
				}

				if (hDlgOpenGL)  //if open, draw bitmap to opengl dialog
				{
					hCtrlBlock = GetDlgItem(hDlgOpenGL, IDC_OPENGL_FLAT);
					glhdc = GetDC(hCtrlBlock);
					BitBlt(glhdc, 0,0, focus->horz_count_2D, focus->vert_count_2D, WBM->GetHDC(),focus->minx, focus->miny, SRCCOPY);
					ReleaseDC(hCtrlBlock, glhdc);
				}

				break;
			case IDC_GRAPH_VIEW:
			//======== Bang-Nguyen ========
			case IDC_POINT_GRAPH:
			//=============================
				WBM->WBMBitBlt(hdc, focus->minx, focus->miny,
							focus->maxx, focus->maxy);
				break;

			case IDC_DOWN_VIEW:
				if (!(blt_flag % _blt_lines))
				{
					if (focus->row_number == focus->miny)
					//In this case you just wrapped row_number to miny
					//blt the bottom strip
						WBM->WBMBitBlt(hdc,
							focus->minx,
							focus->maxy-_blt_lines,
							focus->maxx,
							focus->maxy); //maxy is row_number - 1 here
					else
						WBM->WBMBitBlt(hdc,
							focus->minx,
							focus->row_number-_blt_lines,
							focus->maxx,
							focus->row_number - 1);
					blt_flag = 0;
				}
				break;
			case IDC_SCROLL_VIEW:
				if (!(blt_flag % _blt_lines))
				{
					WBM->WBMBitBlt(hdc, focus->minx, focus->miny,
							focus->maxx, focus->maxy);
					blt_flag = 0;
				}
				break;
			case IDC_SPLIT_VIEW:
				//Do the scroll part
				if (!(blt_flag % _blt_lines))
				{
					WBM->WBMBitBlt(hdc, focus->minx, focus->miny,
							focus->maxx, focus->splity);
					blt_flag = 0;
				}
				//Do the graph part
				WBM->WBMBitBlt(hdc, focus->minx, focus->splity+1,
							focus->maxx, focus->maxy);
				break;
			case IDC_WIRE_VIEW:
				break;
		}
	} // end  zoomflag  case
	// Check to see if it is Breeding time
}

void CAlist::Show(HDC hdc, const RECT &rcPaint)
{
	if (Getdimension() == 1)
		BitBlt(hdc, rcPaint.left, rcPaint.top,
			rcPaint.right, rcPaint.bottom, WBM->GetHDC(),
			rcPaint.left, rcPaint.top, SRCCOPY);
	else //(Getdimension() == 2)
	{
		if (!zoomflag)  // not zoomed, do them all
		{
				if (list[0]->maxx_2D == list[0]->maxx &&
					list[0]->maxy_2D == list[0]->maxy)
					WBM->WBMBitBlt(hdc, list[0]->minx, list[0]->miny,
							list[count-1]->maxx_2D, list[count-1]->maxy_2D);
				else
					for (int i=0; i<count; i++)
						StretchBlt(hdc,   //target hdc
							list[i]->minx, list[i]->miny,  //target corner
							list[i]->horz_count, list[i]->vert_count,//targ size
							WBM->GetHDC(),                 //source  hdc
							list[i]->minx, list[i]->miny,  //source corner
							list[i]->horz_count_2D, list[i]->vert_count_2D,//sc size
							SRCCOPY);
		}// end non zoomflag  case
		else
		{

			if (focus->viewmode== IDC_2D_VIEW)
				capowgl->Draw(hdc, focus);				


		/*		if (focus->maxx_2D == focus->maxx &&
					focus->maxy_2D == focus->maxy)
					WBM->WBMBitBlt(hdc, focus->minx, focus->miny,
						focus->maxx_2D, focus->maxy_2D);
				else
					StretchBlt(hdc,   //target hdc
						focus->minx, focus->miny,  //target corner
						focus->horz_count, focus->vert_count,//targ size
						WBM->GetHDC(),                 //source  hdc
						focus->minx, focus->miny,  //source corner
						focus->horz_count_2D, focus->vert_count_2D,//sc size
						SRCCOPY);
		*/
		} // end  zoomflag  case
	} //end Getdimension == 2 case
}

void CAlist::Evolve()
{//This is called from update_and_show every breeding cycle
	int field,i, mother, father;
	CA* oldfocus;

	if (zoomflag) //Don't do anything while zoom is on.
	{
		Resetscores();  // reset all scores
		return;
	}
	Sortfitness();  /* get a list of the best to the worst ca's
		This includes a permute, so no fixed order advantage.*/
	field = count/4;
	/* 1/4th get left alone, 1/2th stay the same, 1/4th get altered.
		Also the very worst guy will be zapped.  Use focus as index.*/
	oldfocus = focus;
	for (i = field*3; i < count-1; i++)
	{
		focus = list[best_list[i]];
		switch (evolveflag)
		{
			case ZAP:
			/* randomize yourself */
				focus->Lambdalookup();
				break;
			case CLONE:
			/* copy a winner */
				focus->CopyCA(
					list[best_list[Random((unsigned short)field)]]);
				break;
			case SEX:
			/* be a child of two winners */
				mother = Random((unsigned short)field);
				father = Random((unsigned short)field);
				if (field > 1)  // more than one winner to be parents?
					while (mother == father)
						// dont want mother to be the father also
						father = Random((unsigned short)field);
				focus->Become_child_of(
					list[best_list[mother]], list[father]);
				break;
			case NONE:
				break;
		}
	}
	focus = oldfocus;       // reset focus to normal
	/* Always zap the biggest loser */
	list[best_list[count-1]]->Lambdalookup();
/*Mutate everyone if that's in the cards*/
	if (mutateflag)
		for (i = 0; i < count; i++)
			list[i]->Mutate(mutation_strength);
	Resetscores();  // reset all scores
	MessageBeep(0); // Beep!
}

void CAlist::Setentropy(int onoff)
{
	for (int i=0; i<count; i++)
		list[i]->Setentropyflag(onoff); //Calls Resetfreq

}

void CAlist::Setwrapflag(int onoff)
{

	for (int i = 0; i < count; i++)
	{
		list[i]->Setwrapflag(onoff);
	}
}

void CAlist::Set_dx_lock(BOOL onoff)
{
	for (int i = 0; i < count; i++)
		list[i]->Set_dx_lock(onoff);
}

void CAlist::Set_smoothflag(BOOL onoff)
{
	for (int i = 0; i < count; i++)
		list[i]->Set_smoothflag(onoff);
}

void CAlist::Reset_smoothsteps()
{
	for (int i = 0; i < count; i++)
		list[i]->Reset_smoothsteps();
}

void CAlist::One_smoothsteps()
{
	for (int i = 0; i < count; i++)
		list[i]->One_smoothsteps();
}

void CAlist::Setgeneratorflag(int onoff)
{

	for (int i = 0; i < count; i++)
		list[i]->Setgeneratorflag(onoff);
}


void CAlist::Boxfocus(HDC hdc, COLORREF color)
{
	if (zoomflag)
		return;
	WBM->PutRectangle(hdc, focus->minx-1, focus->miny-1,
			focus->maxx+1, focus->maxy+1, color);
}

int CAlist::Setfocus(HDC hdc, CA *new_focus)
{
//	if (focus->Getdimension() == 2)
//		capowgl->AdjustHeightFactor(focus);

	if (zoomflag)
		return 0;
	if (new_focus==focus)
		return 1;
	Boxfocus(hdc,RGB(0,0,0));
	focus = new_focus;
	Boxfocus(hdc,RGB(255,255,255));
	
		
	focus->GetCAStyleName ( CA_STYLE_NAME );
	Status_SetText(hwndStatusBar, 1, 0, CA_STYLE_NAME );


	return 0;
}

CA *CAlist::Getfocus(int x, int y)
{
/* I do minx - BORDER instead of minx so that the cursor won't be in a dead
zone all along the left edge of the screen.  Use "BORDER" because in Locate
below, the left and right edges are offset from disp_ptr->minx by 1. So
you would think I could use "1" here instead of "BORDER", but that doesn't
work so what the hey use BORDER which happened to be 3 when I wrote this
comment at calife1.zip.*/
	for (int i=0; i<count; i++)
		if (list[i]->minx - BORDER <= x && x <= list[i]->maxx + BORDER &&
			list[i]->miny - BORDER  <= y && y <= list[i]->maxy + BORDER)
				return list[i];
	return focus;
}

int CAlist::get_focus_index()
{
	int focus_index = 0;
	for (int i=0; i<count; i++)
		if (list[i] == focus)
			focus_index = i;
	return focus_index;
}

int CAlist::Setfocusindex(HDC hdc, int focus_index)
{
	if (focus_index < 0 || focus_index > count)
		focus_index = 0;
	Setfocus(hdc, list[focus_index]);
	return focus_index;
}


int CAlist::Bumpcount(int updown)
{ //Return 0 if no change, else return 1.
	return Changecount((CA_count_per_edge+updown) * (CA_count_per_edge+updown));
}

int CAlist::Changecount(int icount)
{
 //Return 0 if no change happens, else return 1.

	int i;
	RECT rect;
	int oldcount;

	if (icount > MAX_CAS)
	{
		icount = MAX_CAS;
		focus = list[0];
	}
	if (icount < 1)
		icount = 1;
	if (count == icount)
		return 0;
	//First of all zoom out, in case you're zoomed in.
	if (Zoom(0)) //zoom out by copying the WM_RBUTTON code
	{ //if you weren't already zoomed out, do this
		zoomviewflag = FALSE;
		if( hDlgOpenGL )
			InvalidateRect(hDlgOpenGL, NULL, TRUE);
	}
	update_flag = 1;        // need to update dialog boxes
	oldcount = count;
	count = icount;
	CA_count_per_edge = sqrt(count);
 //     assert (CA_count_per_edge*CA_count_per_edge == count);
//      if ((CA_count_per_edge*CA_count_per_edge)<count)
  //            CA_count_per_edge++;
	GetClientRect(hwnd, &rect); //Here is where I emulate WM_SIZE
	Locate(rect.right,rect.bottom - toolBarHeight);//for possible new CAs.
	if (count > oldcount)   // Expand field
		for (i=oldcount; i< count; i++)
		{
			list[i]->Changeradiusandstates(focus->radius, focus->states);
			list[i]->Settype(focus->Gettype());
		}
	else    // Shrink field
		for (i = count; i < oldcount; i++)
			if (focus == list[i])   // if focus being deleted
				focus = list[0];                // set to the first CA
	if (count == 1) //Always act as if zoomed in when count is 1.
		// copy the LBUTTON zoom in code
	{
		if (Zoom(1)) //zoom in
		{ //if you weren't already zoomed in, do this
			zoomviewflag = TRUE;
			capowgl->AdjustHeightFactor(FocusCA());
		}
	}
	SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
	update_flag = 1;        // need to update dialog boxes
	return 1;
}

int CAlist::Zoom(int onoff)
{
	if (onoff == zoomflag)
		return 0;
	zoomflag = onoff;
     for (int i=0; i<count; i++)
		list[i]->Setwrapflag(list[i]->Getwrapflag());
	return 1;
}

void CAlist::DrawDivider(HDC hdc)
{
	if (!zoomflag)
		for (int i = 0; i<count; i++)
			WBM->PutRectangle(hdc, list[i]->minx-3,
				list[i]->miny-3, list[i]->maxx+3, list[i]->maxy+3,
				RGB(128,128,128));
}

void CAlist::Sortfitness()
{
#define RUDYWAY
#ifndef RUDYWAY
	int i, j;
	int used[MAX_CAS];      /* keeps track of which ca's have been recorded
									in the best list */

	for (i = 0; i < count; i++)
		used[i] = 0;

	for (j = 0; j < count; j++)
	{
		best_list[j] = 0;
		while (used[best_list[j]])      // initialize it with the 1st unclaimed ca
			++best_list[j];

		for (i=0; i<count; i++) /* now test it with the rest of the unclaimed
											ca's to see which is the next best */
			if (used[i] == 0)
			{
				if (list[i]->score > list[best_list[j]]->score)
					best_list[j] = i;
				else if (list[i]->score == list[best_list[j]]->score &&
					Random(2)) //To equalize chances of winning.
					best_list[j] = i;
			}
		used[best_list[j]] = 1; // show this ca as claimed
	}
#else
	int i, j, temp;

	jolt_permute_buffer((unsigned short)count); //This function lives in random.cpp
	for (i = 0; i < count; i++)
		best_list[i] = permute_buffer[i]; //buffer is in random.h
	/* permute the order and then do a bubble sort */

	for (j = 0; j < count-1; j++) //Fix the first member, then next, etc.
		for (i=j+1; i<count; i++)
			if (list[best_list[i]]->score > list[best_list[j]]->score)
			{ //swap
				temp = best_list[j];
				best_list[j] = best_list[i];
				best_list[i] = temp;
			}
#endif
}

//---Commands---------------------------------------------------

void CAlist::Changeradiusandstates(int iradius, int istates)
{
	for (int i = 0; i < count; i++)
		list[i]->Changeradiusandstates(iradius, istates);
}

void CAlist::Symmetrize()
{
	for (int i = 0; i < count; i++)
		list[i]->Symmetrize();
}

void CAlist::Reverse()
{
	for (int i = 0; i < count; i++)
		list[i]->Reverse();
}


void CAlist::Seed()
{
	for (short i=0; i<count; i++)
			list[i]->Seed();
}

void CAlist::RandomSeed()
{
	for (short i=0; i<count; i++)
			list[i]->RandomSeed();
}

void CAlist::FourierSeed()
{
	for (short i=0; i<count; i++)
			list[i]->FourierSeed();
}

void CAlist::Oneseed()
{
	for (int i = 0; i < count; i++)
		list[i]->Oneseed();
}
void CAlist::Zeroseed()
{
	for (int i = 0; i < count; i++)
		list[i]->Zeroseed();
}
void CAlist::Halfmaxseed()
{
	for (int i = 0; i < count; i++)
		list[i]->Halfmaxseed();
}
void CAlist::BitmapSeed()
{
	for (int i = 0; i < count; i++)
		list[i]->BitmapSeed();
}
void CAlist::Sineseed()
{
	for (int i = 0; i < count; i++)
		list[i]->Sineseed();
}


void CAlist::Changelambda(Real newlambda)
{
	for (int i = 0; i < count; i++)
		list[i]->Changelambda(newlambda);
}

void CAlist::Setlambda(Real newlambda)
{
	for (int i = 0; i < count; i++)
		list[i]->Setlambda(newlambda);
}

void CAlist::Changeentropy(Real newentropy)
{
	for (int i = 0; i < count; i++)
		list[i]->Changeentropy(newentropy);
}

void CAlist::Lambdalookup()
{
	for (int i = 0; i < count; i++)
	{
		list[i]->Lambdalookup();
	}
}

void CAlist::Mutate()
{
	for (int i = 0; i < count; i++)
		list[i]->Mutate(mutation_strength);
}

void CAlist::Breed()
{
	if (count == 1 || focus == list[0])
		return; // breeding mom with mom does nothing
	for (int i = 1; i < count; i++)
		if (list[i] != focus)
		{
			list[i]->Become_child_of(focus, list[0]);
			list[i]->Seed();
		}
}

void CAlist::Copymutate()
{
	for (int i = 0; i < count; i++)
		if (list[i] != focus)
		{
			list[i]->_usercastyle = focus->_usercastyle; 
			list[i]->_usernabesize = focus->_usernabesize;

			list[i]->Settype(focus->Gettype());
			list[i]->CopyCA(focus);
			list[i]->Mutate(mutation_strength);
		}
}

void CAlist::SetWindowBitmap(WindowBitmap *myWBM)
{
	WBM = myWBM;    // To put the box focus into the bitmap
	for (int i = 0; i < MAX_CAS; i++)
		list[i]->WBM = myWBM;
}

void CAlist::SetCAType(CA *target, int newtype, BOOL fixflag)
{
	
	target->Settype(newtype);
	if (fixflag)
     	target->Full_adjust_params_for_type();
}

void CAlist::SetAllType(int newtype, BOOL fixflag)
{
	CA *catarget;
	for (int i = 0; i<count; i++)
	{
		catarget = list[i];
		SetCAType(catarget, newtype, fixflag);
	}
	SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
}


void CAlist::Setshowmode(int newmode)
{
	for (int i = 0; i < count; i++)
		list[i]->Setshowmode(newmode);
}

void CAlist::BumpCATweakParam(int tweakParamType, int updown)
{
	int i;
	for (i=0; i < count; i++)
		list[i]->BumpTweakParam(tweakParamType, updown);
}

void CAlist::SetTweakParam(int tweakParamType, Real val)
{
	int i;
	for (i=0; i < count; i++)
		list[i]->SetTweakParam(tweakParamType, val);
}

void CAlist::Change_phase(int updown)
{
	for (int i = 0; i < count; i++)
		list[i]->Change_phase(updown);
}

void CAlist::Setshowvelocity(int newshowvelocity, short focusflag)
{
	if (focusflag)
		FocusCA()->Setshowvelocity(newshowvelocity);
	else
		for (int i = 0; i < count; i++)
			list[i]->Setshowvelocity(newshowvelocity);
}

void CAlist::Setfailstripe(int updown)
{
	for (int i = 0; i < count; i++)
		list[i]->Setfailstripe(updown);
}

void CAlist::Setentropybonus(int updown)
{
	for (int i = 0; i < count; i++)
		list[i]->Setentropybonus(updown);
}


void CAlist::Setbreedcycle(int updown)
{
	if (updown > 0)
		breedcycle += 50;
	else
		breedcycle -= 50;
	CLAMP(breedcycle, 50, 10000);
}

void CAlist::Setmutation(int updown)
{
	Real   increment;
  if (mutation_strength >= 0.1)
	increment = 0.1;
  else
	increment = 0.01;
  if (updown > 0)
		mutation_strength += increment;
	else
		mutation_strength -= increment;
	CLAMP(mutation_strength, 0.0, 0.90);
}

void CAlist::Resetscores()
{
	for (int i = 0; i < count; i++)
		list[i]->Resetscore();
}


void CAlist::SyncRows()
{
	for (int i = 0; i < count; i++)
		list[i]->SyncRows();
}

void CAlist::Setviewmode(int newmode)
{
	for (int i = 0; i < count; i++)
     	//Don't change a 2D rule to any view other than IDC_2D_VIEW
     	if (list[i]->Getviewmode() != IDC_2D_VIEW)
			list[i]->Setviewmode(newmode);
}

void CAlist::RandomizeCount(UINT randflag) //******Rong Liu*********
{	
	int i;
	char szBuffer[4];

	int countoptions_count = 0;
	int countoptions_array[3];
	if (randflag & RF_COUNT1) //exactly 1, 2, or 3 of the optoins should hold.
	{
		countoptions_array[countoptions_count] = 1;
		countoptions_count++;
	}
	if (randflag & RF_COUNT4)
	{
		countoptions_array[countoptions_count] = 4;
		countoptions_count++;
	}
	if (randflag & RF_COUNT9)
	{
		countoptions_array[countoptions_count] = 9;
		countoptions_count++;
	}
	//countoptions_count should be 1,2,or3 now.	
	int newcount;
	if (countoptions_count) //The worst that could happen is countoptions is 0.
		newcount = countoptions_array[Random(countoptions_count)];
	else
		newcount = 9;
/* If newcount != count, then Changecount(newcount) calls
	 SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L), and returns TRUE.
	If the count doesn't change we still want to Clear in case we have 
	RF_ALLVIEW because this allows wire and graph view which don't clear
	their backgrounds. */
 	if (!Changecount(newcount) && (randflag & RF_ALLVW)) //2017 put && instead of &
		SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
}	
#define FILE_RANDOMIZE_MUTATION_STRENGTH 0.1
void CAlist::Randomize(UINT randflag)
{

	int type, view, seed;
	int stateradiusindex;
	RECT rect;

	RandomizeCount(randflag);
	//This calls SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L) if count changes
	//or if fRandFlags & RF_ALLVW
	for (int i = 0; i < count; i++)
	{
		if (randflag & RF_FILE) //We LIKE the file so mutate instead of randomizing params.
			list[i]->Mutate(FILE_RANDOMIZE_MUTATION_STRENGTH);  
		else
		{ //Begin randomizing the type
			if (list[i]->type_ca == CA_USER)	// Clear user parameter
			{
				removeUserParam(list[i], FALSE);
				if (focus == list[i])
					recreateUserDialog();  // return immediately if not exist yet
			}
			list[i]->Lambdalookup();  //Randomize all params.
			if (randflag & RF_BOTHVAL)
				type = Random(MAX_TYPE_CA)+ CA_STANDARD;
			else if (randflag & RF_DIGITALVAL)
				type = CA_STANDARD; //Fuck the REVERSIBLE ones
			else // randflag & RF_ANALOGVAL
			{
				type = Random(MAX_TYPE_CA)+ CA_STANDARD;
				if (type == CA_STANDARD || type == CA_REVERSIBLE)
					type = CA_AUTO_ULAM_WAVE; //No digital rules.
			}
			//Note that some of these types aren't used anymore, but
			//CA::settype will switch unusable types to the closest usable one.
			if (randflag & RF_1D)
			{
				if (type == CA_WAVE_2D || type == CA_HEAT_2D || type == CA_USER || type == CA_REVERSIBLE)
				{ //Won't get in here if randflag & RF_DIGITALVAL
					if (randflag & RF_ANALOGVAL)
					{
						if (Random(2))
							type = ALT_CA_WAVE; //Don't get enough of these otherwise.
						else
							type = CA_AUTO_ULAM_WAVE;
					}
					else  //randflag & RF_BOTHVAL
					{
						if (Random(2))
							type = CA_STANDARD; //Don't get enough of these otherwise.
						else
							type = CA_AUTO_ULAM_WAVE;
					}
				}
			}
			else if (randflag & RF_2D)
			{
				if (Random(3))
					type = CA_WAVE_2D; //Don't get enough of these otherwise.
				else
					type = CA_HEAT_2D;
			}
			else //assume (randflag & RF_BOTHD)
			{
				if (type == CA_USER)
					type = CA_AUTO_ULAM_WAVE; //No CA_USER.
				if (!Random(count+1)) //half the time for 1, ~one per screen otherwise
				{
					if (Random(3))
						type = CA_WAVE_2D; //Don't get enough of these otherwise.
					else
						type = CA_HEAT_2D;
				} 
			}
			list[i]->Settype(type);   //Sets viewmode for 2D
			list[i]->Full_adjust_params_for_type();
				//Tweaks params to reasonable vals.
			if (type == CA_STANDARD || type == CA_REVERSIBLE)
			{
				stateradiusindex =  Random(STATERADIUSCOUNT);
				list[i]->Changeradiusandstates(StartRadius[stateradiusindex],
					StartStates[stateradiusindex]);
				list[i]->Setviewmode(IDC_SCROLL_VIEW);
			}
		} //End randomizing the type for the else from RF_FILE.
		if (list[i]->dimension == 1)
		{
			if (randflag & RF_SPLITVW)
					list[i]->Setviewmode(IDC_SPLIT_VIEW);
			else if (randflag & RF_SCROLLVW)
					list[i]->Setviewmode(IDC_SCROLL_VIEW);
			else if (randflag & RF_BOTHVW)
			{
				seed = Random(2);
				if (seed)
					list[i]->Setviewmode(IDC_SPLIT_VIEW);
				else //not seed
					list[i]->Setviewmode(IDC_SCROLL_VIEW);
			} //end RF_BOTHVW
			else //randflag & RF_ALLVW
			{
				view = Random(10);
				switch(view)
				{
					case 0:
					case 1:
						list[i]->Setviewmode(IDC_SCROLL_VIEW);
						break;
					case 2:
						if (Random(2))
							list[i]->Setviewmode(IDC_WIRE_VIEW);
						else
							list[i]->Setviewmode(IDC_GRAPH_VIEW);
						break;
					case 3:
						list[i]->Setviewmode(IDC_POINT_GRAPH);
						break;
					default:
						list[i]->Setviewmode(IDC_SPLIT_VIEW);
						break;
				}
			} //end RF_ALLVIEW
		} //end Viewmode setting for dimension == 1
		if (list[i]->dimension == 2)
		{
			if ((randflag & RF_3D) || ((randflag & RF_3DBOTH) && Random(2)) )
			{
				capowgl->Type(SHEET);
				if (Random(2))
					capowgl->Material(BRASS);
				else
					capowgl->Material(MULTICOLOR);
			}
			else //randflag & RF_3DFLAT or RF_3DBOTH, but you lost the toss.
				capowgl->Type(FLATCOLOR);
		} //end Viewmode setting for dimension == 2
		seed = Random(4);
		switch(seed)
		{
			case 0:
				list[i]->RandomSeed();
				list[i]->_smoothsteps = 3 * list[i]->_startsmoothsteps;
				break;
			case 1:
				list[i]->Sineseed();
				break;
			case 2:
				list[i]->Oneseed();
				break;
			case 3:
				list[i]->FourierSeed();
				break;
		}
		if (randflag & RF_DING)
			list[i]->RandomTouch_CA();
		list[i]->Resetscore();
	}
	if(randflag & RF_COLOR)   //Rong Liu
		SetColors();
	/*This last thing is needed to get the split window views right,
	without a messed up little lip in the bottom of the scroll area.
	I don't really know why this is needed.*/
	GetClientRect(hwnd, &rect);
	Locate(rect.right,rect.bottom - toolBarHeight);
}

void CAlist::CopyCA(CA *ca1, CA *ca2)
{
	if (ca1->Gettype() != ca2->Gettype())
		SetCAType(ca2, ca1->Gettype(), FALSE);
	ca2->CopyCA(ca1);
}

void CAlist::UpdateGenerationCount()
{
	for ( int i=0; i < count; i++ )
			list[i]->BumpAndReturnGenerationCount();
}

void CAlist::ResetAllGenerationCount()
{
	for ( int i=0; i < count; i++ )
			list[i]->ResetGenerationCount();
}

void CAlist::GetFocusRect(RECT *rect)
//fills a RECT structure with the rectangle of the focus, if possible
{
	if (focus)	
	{
		rect->left = focus->minx;
		rect->top = focus->miny;
		rect->right = focus->minx + focus->horz_count_2D;
		rect->bottom = focus->miny + focus->vert_count_2D;
	}
	else
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;
	}
}

void CAlist::set_blt_lines(int linecount)
{
	CLAMP(linecount, 1, 5);
	if (_blt_lines != linecount)
	{
		_blt_lines = linecount;	 
		Locate();
		InvalidateRect(masterhwnd, NULL, FALSE);
		WBM->Clear(masterhwnd, RGB(0,0,0));
		capowgl->Size(hwnd);
	}
}