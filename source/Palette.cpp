#include "ca.hpp"


const int goodVGAcolorcount = 13;
COLORREF goodVGAcolor[13] =   //Don't allow any grays.
{RGB(000,000,000), RGB(000,000,128), RGB(000,128,000), RGB(000,128,128),
RGB(128,000,000), RGB(128,000,128), RGB(128,128,000), RGB(000,000,255),
RGB(000,255,000), RGB(000,255,255), RGB(255,000,000), RGB(255,000,255),
RGB(255,255,000)};

//------------CA things

COLORREF CA::Color_look(int numb)
{
	return colortable[lookup[numb]];
}

void CA::SetSamecolortable(COLORREF *color, int bc)
{
	for (int i = 0;i<MAX_COLOR;i++)
	{
		band_count = bc;
		colortable[i] = color[i];
	}
	if (Gettype() == CA_STANDARD || Gettype() == CA_REVERSIBLE)
	{
		Setoldcolor();
	}
}

void CA::SetColors()
// NEW 2-27-96, DDC
/* PURPOSE: Sets the anchor colors for the bands. This keeps the
**          band colors consistent when changing band number or
**          number of colors.
** REMARKS: NOTE - this function needs to be called upon construction
**          of the CA!
*/
{
	for (int k=0;k<MAX_BAND_COUNT;k++){
	do{
		_anchor_color[k] = RGB(Random(255), Random(255), Random(255));
	}while (_anchor_color[k] == BLACK); }

	 if (Get_monochromeflag())
			Setmonochromecolortable();
	else
		Setcolortable();
}


void CA::Setcolortable()
{
	int k, i, j;
	HDC hdc;
	hdc = GetDC(hwnd);

	if (band_count > MAX_BAND_COUNT)
		band_count = START_BAND_COUNT;
	if (band_count < 2) //Can't allow band_count-1 to be 0.
		band_count = 2;

	/* This is a brute force fix to a bug marked Oct 27, 96 a page below
	here */

	if (calist_ptr->numcolor() == 16)
	{
		for (i=0;i<band_count;i++)
		{
					COLORREF VGA_anchor_color = goodVGAcolor[
				Random(goodVGAcolorcount)];
			for (j=0;j<(MAX_COLOR-1)/band_count;j++)
				colortable[(i*((MAX_COLOR-1)/band_count)) + j] =
					 VGA_anchor_color;
		}
	} //end 16 color case
	else     //256 color card or thousands of colors card
	{ //First set calist_ptr->hpal in case you are the focus CA in 256 col case
		if (this == (calist_ptr->FocusCA()))
			calist_ptr->SetRandomLogPalette(_anchor_color, band_count);
		for (k=0;k<(band_count-1);k++)  // ramp the colors
			for (j=0;j<MAX_COLOR/(band_count-1);j++)
			{
				double dr = GetRValue(_anchor_color[k+1]) -
					GetRValue(_anchor_color[k]);
				double dg = GetGValue(_anchor_color[k+1]) -
					GetGValue(_anchor_color[k]);
				double db = GetBValue(_anchor_color[k+1]) -
					GetBValue(_anchor_color[k]);
/* Oct 27, 96.  We are getting a crash here . j is 0,
 k is 289, MAX_COLOR is 1024, band_count is 364 one time, the next
 time it is 1024, it looks like band_count is not initialized.  When you
 do a new Build All this usually goes away. Let's put in a brute force
fix at the start of this call by clamping band_count. */
				BYTE r = GetRValue(_anchor_color[k]) +
					((dr/((MAX_COLOR-1)/(band_count-1))) *j);
				BYTE g = GetGValue(_anchor_color[k]) +
					((dg/((MAX_COLOR-1)/(band_count-1))) *j);
				BYTE b = GetBValue(_anchor_color[k]) +
					((db/((MAX_COLOR-1)/(band_count-1))) *j);
				if (calist_ptr->numcolor() == 256)
									colortable[
						j+(k*((MAX_COLOR-1)/(band_count-1)))]=
						PALETTEINDEX(GetNearestPaletteIndex(
						calist_ptr->hpal(),RGB(r,g,b)));
				else //More than 256 colors, like thousands
					colortable[
						j+(k*((MAX_COLOR-1)/(band_count-1)))] =
						GetNearestColor(hdc, RGB(r,g,b));
			}
		j = MAX_COLOR / (band_count - 1); //2017 One more step for top end.
		if ( j+(k*((MAX_COLOR-1)/(band_count-1))) < MAX_COLOR )
			for (i=j+(k*((MAX_COLOR-1)/(band_count-1)));
				i<(MAX_COLOR-1);i++)
				colortable[i] = colortable[i-1];
	}  //end 256 or thousands case
	//The max color value gets missed in the ramping roundoff
	colortable[MAX_COLOR-1] = RGB(128,0,0); //Dull red
	if (Gettype() == CA_STANDARD || Gettype() == CA_REVERSIBLE)
		Setoldcolor(); //Use some standard colors for 0-16
	else //analog case likes blue at the bottom.
		colortable[0] = RGB(0,0,128); //Dull Blue
	ReleaseDC(hwnd, hdc);
}

//#define ABRUPT
/* If ABRUPT is defined, then the monochrome color scale has bands which
each go from black to white, so there are abrupt changes as you pass from
band to band.  If ABRUPT is not defined, then the even bands go black to
white, but the odd ones go white to black, which smooths the joins.*/
void CA::Setmonochromecolortable()
{
	int k, i, j;
	HDC hdc;
	BYTE wht;   //whiteness, not r, g, b needed here.

	hdc = GetDC(hwnd);
	if (calist_ptr->numcolor() == 16)
	{
		for (i=0;i<band_count;i++)
		{
					COLORREF VGA_anchor_color = goodVGAcolor[
				Random(goodVGAcolorcount)];
			for (j=0;j<(MAX_COLOR-1)/band_count;j++)
				colortable[(i*((MAX_COLOR-1)/band_count)) + j] =
					 VGA_anchor_color;
		}
	} //end 16 color case
	else     //256 color card or thousands of colors card
	{
		if (this == (calist_ptr->FocusCA()))
		//fix calist_ptr->hpal for 256 case
			calist_ptr->SetMonochromeLogPalette();
		for (k=0;k<(band_count-1);k++)  // ramp the colors
			for (j=0;j<MAX_COLOR/(band_count-1);j++)
			{
				wht = (BYTE)((255.0 * (Real)j * (band_count-1))/(MAX_COLOR-1));
#ifndef ABRUPT
				if (k%2)
					wht = (BYTE)(255 - wht);
#endif //ABRUPT
				if (calist_ptr->numcolor() == 256)
									colortable[
						j+(k*((MAX_COLOR-1)/(band_count-1)))]=
						PALETTEINDEX(GetNearestPaletteIndex(
						calist_ptr->hpal(),RGB(wht, wht, wht)));
				else //More than 256 colors, like thousands
					colortable[
						j+(k*((MAX_COLOR-1)/(band_count-1)))] =
						GetNearestColor(hdc, RGB(wht, wht, wht));
			}
		if ( j+(k*((MAX_COLOR-1)/(band_count-1))) < MAX_COLOR )
			for (i=j+(k*((MAX_COLOR-1)/(band_count-1)));
				i<(MAX_COLOR-1);i++)
				colortable[i] = colortable[i-1];
	}  //end 256 or thousands case
	//The max color value gets missed in the ramping roundoff
	colortable[MAX_COLOR-1] = RGB(255,255,255); //WHITE
	if (Gettype() == CA_STANDARD || Gettype() == CA_REVERSIBLE)
		Setoldcolor(); //Use some standard colors for 0-16
	else //analog case likes blue at the bottom.
		colortable[0] = RGB(0,0,0); //BLACK
	ReleaseDC(hwnd, hdc);
}

void CA::Setoldcolor()
{
	colortable[0] = RGB(0,0,0);
	colortable[1] = RGB(0,0,255);
	colortable[2] = RGB(0,255,0);
	colortable[3] = RGB(0,255,255);
	colortable[4] = RGB(255,0,0);
	colortable[5] = RGB(255,0,255);
	colortable[6] = RGB(255,255,0);
	colortable[7] = RGB(100,100,100);
	colortable[8] = RGB(40,40,40);
	colortable[9] = RGB(100,100,255);
	colortable[10] = RGB(100,255,100);
	colortable[11] = RGB(100,255,255);
	colortable[12] = RGB(255,100,100);
	colortable[13] = RGB(255,100,255);
	colortable[14] = RGB(255,255,100);
	colortable[15] = RGB(255,255,255);
	if (states == 2)
		colortable[1] = RGB(255,255,255);
}

void CA::Change_band_count(int updown)
{
	if (band_count<=1) return;
	if (updown >0)
		band_count *= 1.5;
	else
		band_count /= 1.5;
	CLAMP(band_count, 2, MAX_BAND_COUNT);
	if (!monochromeflag)
		Setcolortable();
	else
		Setmonochromecolortable();
}


void CA::Set_monochromeflag(int onoff)
{
	if (monochromeflag == onoff)
		return;
	monochromeflag = onoff;
	if (!monochromeflag)
		Setcolortable();
	else
		Setmonochromecolortable();
}


//---------CAlist things

void CAlist::SetColors()
{
	for (int i=0; i<count; i++)
		list[i]->SetColors();
}

void CAlist::SetRandomLogPalette(COLORREF *_anchor_color, int band_count)
{  //Only need this for 256 color mode
	int i,j,k;

	if (_hpal)
	{
		DeleteObject(_hpal);    //old palway BUG?
		_hpal = 0;
	}
	_plgpl->palNumEntries = 236;
	_plgpl->palVersion = 0x300;
	i = 0;
	for (k=0;k<(band_count-1);k++)  // ramp the colors
		for (j=0;j<234/(band_count-1);j++)
		{
			double dr = GetRValue(_anchor_color[k+1]) - GetRValue(_anchor_color[k]);
			double dg = GetGValue(_anchor_color[k+1]) - GetGValue(_anchor_color[k]);
			double db = GetBValue(_anchor_color[k+1]) - GetBValue(_anchor_color[k]);
			_plgpl->palPalEntry[i].peRed = GetRValue(_anchor_color[k])+((dr/(234/(band_count-1))) *j);
			_plgpl->palPalEntry[i].peGreen = GetGValue(_anchor_color[k])+((dg/(234/(band_count-1))) *j);
			_plgpl->palPalEntry[i].peBlue = GetBValue(_anchor_color[k])+((db/(234/(band_count-1))) *j);
			_plgpl->palPalEntry[i].peFlags = PC_RESERVED;
			i++;
		}
	_plgpl->palPalEntry[235].peRed = 0;
	_plgpl->palPalEntry[235].peGreen = 0;
	_plgpl->palPalEntry[235].peBlue = 0;
	_plgpl->palPalEntry[235].peFlags = PC_RESERVED;
	_plgpl->palPalEntry[234].peRed = 255;
	_plgpl->palPalEntry[234].peGreen = 255;
	_plgpl->palPalEntry[234].peBlue = 255;
	_plgpl->palPalEntry[234].peFlags = PC_RESERVED;
	_hpal = CreatePalette(_plgpl);
}

void CAlist::SetMonochromeLogPalette()
{   //Only need this for 256 color mode
	int i;

	if (_hpal)
	{
		DeleteObject(_hpal);    //old palway BUG?
		_hpal = 0;
	}
	_plgpl->palNumEntries = 236;
	_plgpl->palVersion = 0x300;
	for (i=0;i<234;i++)
		{
			_plgpl->palPalEntry[i].peRed = (BYTE)(21 + i);
			_plgpl->palPalEntry[i].peGreen = (BYTE)(21+i);
			_plgpl->palPalEntry[i].peBlue = (BYTE)(21+i);
			_plgpl->palPalEntry[i].peFlags = PC_RESERVED;
		}
	_plgpl->palPalEntry[235].peRed = 0;
	_plgpl->palPalEntry[235].peGreen = 0;
	_plgpl->palPalEntry[235].peBlue = 0;
	_plgpl->palPalEntry[235].peFlags = PC_RESERVED;
	_plgpl->palPalEntry[234].peRed = 255;
	_plgpl->palPalEntry[234].peGreen = 255;
	_plgpl->palPalEntry[234].peBlue = 255;
	_plgpl->palPalEntry[234].peFlags = PC_RESERVED;
	_hpal = CreatePalette(_plgpl);
}


void CAlist::Change_band_count(int updown)
{
	if (_numcolor == 256)
	{
		list[0]->Change_band_count(updown);
		for (int i = 1; i < count; i++)
			list[i]->SetSamecolortable(list[0]->GetColortable(), list[0]->Getband());
	}
	else
		for (int i = 0; i < count; i++)
			list[i]->Change_band_count(updown);
}

void CA::Set_band_count(Real band)
{
	band_count = band;
	CLAMP(band_count, 2, MAX_BAND_COUNT);
	if (!monochromeflag)
		Setcolortable();
	else
		Setmonochromecolortable();
}

void CAlist::Set_monochromeflag(int onoff)
{
	for (int i=0; i< count; i++)
		list[i]->Set_monochromeflag(onoff);
}

void CAlist::Setcolortable()
{
	if (_numcolor == 256)
	{
		if (!focus->Get_monochromeflag())
			focus->Setcolortable();
		else
			focus->Setmonochromecolortable();
		for (int i = 0; i < count; i++)
			list[i]->SetSamecolortable(focus->GetColortable(),
				focus->Getband());
	}
	else
		for (int i = 0; i < count; i++)
		{
			if (!list[i]->Get_monochromeflag())
				list[i]->Setcolortable();
			else
				list[i]->Setmonochromecolortable();
		}
}

void CAlist::Setnumcolor(int newnumcolor)
{
	if (_numcolor == newnumcolor)
		return;
	_numcolor = newnumcolor;
	Setcolortable();
}

