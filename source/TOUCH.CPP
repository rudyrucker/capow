/* touch.cpp by James Kroutch for the SJSU CAMCOS Project. Fall '96

	Specification :
	This module contains the code to impliment a touch feautre to the Capow
	program.  This feature will allow users,  once certain conditions are
	met, to use their cursor to "touch" the currently running experiment.

	Requirements:
	For the touch feature to be enabled the following criteria must be met:
	1. The user must either have one CA focused and zoomed or be running
		capowlow.exe(only one CA running.)  The touch mode menu choice
		will be grayed out until this first criteria is met.

	2. The user must select touch mode - A choice availible from the
		Tools/Cursors menu once the first criteria is satisfied.
		When the user selects this option from the menu the cursor will change
		to signify to the user that they are now in touch mode. The menu
		option will also be checked when the user is in touch mode.

	Log:
	9/13/96 - started first prototype of the touch feature.  First rendition
	will only work if #LITE is defined.  This guarantees that the user will
	only have one active CA. The dialog box will be added later.
	9/17/96 - decided to workout nowrap case 1st.  Will warp ends to
	ensure continuous values.  Potential problems with stadard and reversable
	CA's.  ask rucker about some specific calls involving these calls
	9/19/96 - Finished initial implimentation of 1-D touch feature
   9/20/96 - Took out scaling of touch at ends of table because it causes
	too much instability in the wave.  Added Right click as a bump down.
	9/22/96 - Began implimentation of 2D touch feature
	9/23/96 - Added Cursor for Touch Mode and finished basic 2D touch.  Will
	move on to getting touch feature wotking in Real (16 & 32-bit) Capow.
	10/1/96 - Main decision needs to be made on how to do multi CA
	implementation of touch.  My decision is not to change focus when user
	picks point to tweak in CA.
	10/5/96 - CAlist::Touch_CA now modified to work with Multi CA
	Experiments.
   10/12/96 - CA::Touch_CA now modified to work with Multi CA Experiments.
   Slight problem with value translation in 2D CA's need to fix!!
	*/

// Preprocessor Directives
#include <math.h>
#include "ca.hpp"
#include "random.h" // random.h will be needed later for Rain effect
						  // so I'll just add it now.
//#include "Graph3D.hpp"
#include "capowgl.hpp"

// Defines and module globals

//extern Graph3D* graph;
extern CapowGL* capowgl;
// Functions
void CA::Touch_CA(int cx, int cy, int sender)
{
	/* Inputs:  cx, cy - the x & y coord the user wanted to touch 
					sender - the msg that triggered the call
		Outputs: none

	*/

	int touchsize = 40;  // Default half width of touch
	int i, j, start_x, start_y, end_x, end_y, direction;
	int ni, nj; // Used in wrapping touch
	int trans_x, trans_y;  // For touch location when using 2D touch
	Real distance;
	int n;
	int current_horz_count;
	unsigned long xxx, yyy;  //temp values used in calc. coordinate translation

	if (dimension == 1)  // 1-d Case
	{

   	// Zoom flag check added for multi-ca touch mode
		if (calist_ptr->Getzoomflag())
		{
			// In this case we are zoomed so just like LITE case
			// Size Check
			if (touchsize > horz_count/2)
				touchsize = horz_count/4;
			current_horz_count = horz_count;
			start_x = cx - touchsize;
			end_x = cx + touchsize;
		}
		else // Otherwise we are not zoomed
		{
			// Size Check
			if (touchsize > (maxx - minx)/2 )
				touchsize = (maxx - minx)/4;
         // Start_x and End_x modified
			start_x = (cx - minx) - touchsize;
			end_x = (cx - minx) + touchsize;
			current_horz_count = maxx - minx + 1;
         cx = cx - minx;
		}


	// direction lets left click touch up and right click touch down

		if (sender == WM_LBUTTONDOWN)
			direction = 1;
		else // sender = WM_RBUTTONDOWN
			direction = -1;

		for (i = start_x; i < end_x; i++)
		{
			ni = i;
			if (i < 0)
				ni = i + current_horz_count;

			if (i >= current_horz_count)
				ni = i - current_horz_count;
			// oops mistake to use ni in (ni - cx) No wonder it wave
			// gets so unstable, duh!
			wave_source_row[ni].intensity =
			wave_past_row[ni].intensity = wave_past_row[ni].intensity +
				direction * (_max_intensity.Val() * ( 1.0 - fabs(i - cx)/ (Real)(touchsize)));
		} // closes for loop on i
		Resetfreq();
		generator_ptr->Seed();
		//generatorlist.Seed();  //no seed function defined yet
	}
	else // 2-d Case
	{
		// Size Check not necessary 2D waves always are either 120X90 or 10X10
		// set up default values
		touchsize = 20;
		//Get Current Screen Size from  horz_count & vert_count to use as
		//traslation values to translate from screen coordinates to
		//correct bitmap coordinates

		if (calist_ptr->Getzoomflag())
		{
		// had to use unsigned long ,  because 16-bit int's flip on big values
			xxx = (unsigned long)horz_count_2D *(unsigned long) cx;
			yyy = (unsigned long)vert_count_2D *(unsigned long) cy;
			trans_x = (int)(xxx / horz_count);
			trans_y = (int)(yyy / vert_count);

		// also need to set end wrapping values
		}
		else // Not zoomed
		{
			xxx = (unsigned long)horz_count_2D * (unsigned long) (cx - minx);
         yyy = (unsigned long)vert_count_2D * (unsigned long) (cy - miny);
         trans_x = (int) (xxx / horz_count);
         trans_y = (int) (yyy / vert_count);
		}
		//  Set beginning and ending values for loops
		start_x = trans_x - touchsize;
		end_x = trans_x + touchsize;
		start_y = trans_y - touchsize;
		end_y = trans_y + touchsize;

      // No multi CA modifications here because 2D CA is always stored as a
      // CX_2D by CY_2D  bitmap
		for (j = start_y; j < end_y; j++)
		{
			for (i = start_x; i < end_x; i++)
			{
				ni = i;
				nj = j;
				// do touch wrapping here
				if (i < 0)
					ni = ni + CX_2D;
				if (i >= CX_2D)
					ni = ni - CX_2D;
				if (j < 0)
					nj = nj + CY_2D;
				if (j >= CY_2D)
					nj = nj - CY_2D;

				n = index(ni,nj);

				// set direction using sender
				if (sender == WM_LBUTTONDOWN)
					direction = 1;
				else // sender = WM_RBUTTONDOWN
					direction = -1;

#ifdef TAXI_CAB_METRIC
				// Left this switch here will do later if relevant
#else // if not TAXI_CAB_METRIC use Euclidean metric.
				distance = sqrt( ((i - trans_x) * (i - trans_x)) +
									  ((j - trans_y) * (j - trans_y)) );
				distance /= touchsize;
				if ( distance <= 1.0)
				{
					wave_source_plane[n].intensity =
					wave_past_plane[n].intensity = wave_past_plane[n].intensity +
						(direction * ((_max_intensity.Val() *.7) * (1.0 - distance)));

				} // closes distance if
#endif // TAXI_CAB_METRIC
			} // closes loop on x
		} // closes loop on y

	} // closes dimension if

} // Closes CA::Touch_CA

#define TOUCH_BUFF 10
void CA::RandomTouch_CA( )
{
	int x, y;

	
	if (dimension == 1)
	{
		x = minx + TOUCH_BUFF + Random(horz_count - 2*TOUCH_BUFF);
		CLAMP(x, minx, maxx);
		y = (miny + maxy ) / 2;
	}
	else //dimension 2
	{
		x = minx + TOUCH_BUFF + Random(horz_count - 2*TOUCH_BUFF);
		CLAMP(x, minx, maxx);
		y = miny + TOUCH_BUFF + Random(vert_count - 2*TOUCH_BUFF);
		CLAMP(y, miny, maxy);
	}
	if (Random(2))
		Touch_CA(x, y, WM_LBUTTONDOWN);
	else
		Touch_CA(x, y, WM_RBUTTONDOWN);
}

void CAlist::RandomTouch_CA( )
{
	if (zoomflag)
		focus->RandomTouch_CA();
	else
		for (int i=0; i< count; i++)
			list[i]->RandomTouch_CA();
}

void CAlist::Touch_CA(int x, int y, int sender)
{
/* For this implimentation we only want to send one CA to CA::Touch_CA
	so we need to determine which one was touched and where */

	if (zoomflag)
	{
	/* this is the case where user has picked a CA and zoomed on it
		so take the focused one and touch it */

		focus->Touch_CA(x, y, sender);
	}
	else
	{
	/* use a method similar to Getfocus to determine which Ca user touched
		with the BORDER being counted as dead space */
		for (short i = 0; i < count; i++)
			if (list[i]->minx - BORDER <= x && x <= list[i]->maxx + BORDER &&
				list[i]->miny - BORDER  <= y && y <= list[i]->maxy + BORDER)
					list[i]->Touch_CA(x, y, sender);
	}
} // Closes CAlist::Touch_CA


void CAlist::LocateNewGenerator(int x, int y, int sender)   //mike 4/97  ,basically a copy of Touch_CA
{
	if (zoomflag)
	{
		if (focus->dimension==1)
			focus->generatorlist.Add(x);
		else //2 dimensions
//			if(graph->GraphType()==0)
//			focus->generatorlist.Add((int)(CX_2D* x/graph->GraphRect().right),(int) (CY_2D*(y-33)/(graph->GraphRect().bottom-33)));
		if (capowgl->Type()==0)
			focus->generatorlist.Add((int)(CX_2D*x/focus->horz_count),(int)(CY_2D*(y-focus->miny)/focus->vert_count));
			
	
	}
	else
	{
		//Mike 2/98 generator cursor now handles non-zoom case
		//For 2D cases, it's hard to spot the generator locations
		for (short i = 0; i < count; i++)
			if (list[i]->minx - BORDER <= x && x <= list[i]->maxx + BORDER &&
				list[i]->miny - BORDER  <= y && y <= list[i]->maxy + BORDER)
					if (list[i]->dimension ==1)
						list[i]->generatorlist.Add(x-list[i]->minx);
					else
						list[i]->generatorlist.Add( (int)(CX_2D*(x-list[i]->minx)/(list[i]->maxx-list[i]->minx)),
													(int)(CY_2D*(y-list[i]->miny)/(list[i]->maxy-list[i]->miny)) );
	}
}


