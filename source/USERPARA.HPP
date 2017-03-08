// ***********************************************************************
// File:		userpara.hpp													
// PROJECT:		CAMCOS CAPOW
// ENVIRONMENT:	Visual C++ 4.0	Win32
//
// hpp file for userpara.hpp.
//
// Date: 4/25/97
//
// Edit:
//
// ***********************************************************************

#ifndef USERPARA_HPP

#define USERPARA_HPP

#include "types.h"
#include "tweak.hpp"

#define USERTEXTLENGTH		60 //2017 was 40. Wanted bigger font.
#define USEREDITCODEBASE	(WM_USER + 1000)

// Offset of client area. Note - left, right, top and bottom 
#define STARTX	20	
#define STARTY	20	

// Initail column size 
#define TOTALSTARTCOL	3	

// Width and height of a column in pixel */
#define columnWidth		(labelWidth+editWidth)
#define rowHeight		(labelHeight + 5)

// Percent less of edit width this percentage less is use to display the 
// updown control. Use for spacing between updown control and label box.
#define PERCENTUPDOWN	(0.7)	
#define UPDOWNWIDTH		(17)

// width of static text less, spacing purpose between label and edit box
#define LESSLABEL		5

#define TOTALBUTTON		1
#define BUTTON_HEIGHT	25			/* # of pixel */
#define BUTTON_WIDTH	100			/* # of pixel */
#define BUTTONIDBASE	(USEREDITCODEBASE + 5000)

#define MINLABELWIDTH		4		/* # of char  */
#define USERCHARLABELWIDTH	15		/* # of char  */
#define USERCHAREDITWIDTH	12		/* # of  char */
#define GROUPBOXWIDTH		140
#define GROUPBOXHEIGHT		(labelHeight * 4)

// Min width for the user dialog box
#define MINWIDTH			(GROUPBOXWIDTH + BUTTON_WIDTH + 20)

void CreateUserDialog();
void recreateUserDialog();
void removeUserParam(CA *activeCA, BOOL removeVariance = TRUE);

#endif