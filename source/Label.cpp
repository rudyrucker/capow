#include "types.h"
//#include <math.h> // need for ftoa
//#include <stdlib.h>  //for itoa and ltoa

void label(HWND, int, char *, int);
void ulabel( HWND hwnd, int button, char *, unsigned int );
//void dlabel( HWND hwnd, int button, char *, double);
void numlabel( HWND , int , int );

void textLabel (HWND hwnd, int button, char*);  // display text only
void realLabel (HWND hwnd, int button, Real num);  // display Real only
//----------------------CODE--------------------

void textLabel ( HWND hwnd, int button, char* title)
{
	SetWindowText ( GetDlgItem(hwnd, button), (LPSTR)title);
}
#define THREE_DIGITS
#ifdef THREE_DIGITS
void realLabel (HWND hwnd, int button, Real num)
{
	char numString[21];
	char sign =0;
	char integerString[10];
	char fractionString[10];
	long integer;
	long fraction;
	Real temp;

	if (num < 0.0)
	{
		sign = '-';
		num *= -1;
	}

	integer = num; // integer part of num
	temp = num - integer; // temp = fraction part of num
	temp *= 1000;
	fraction = temp; // fraction part of num
	temp -= fraction;
	if (temp > 0.5)  // round up
		fraction++;
	if (fraction > 999) // round up > .999, add to integer
	{
		fraction -= 1000;
		integer ++;
	}
	if (fraction < 0)
		fraction *= -1;  // don't show the fraction part as negative

	wsprintf(integerString, "%ld", integer);
	wsprintf(fractionString, "%ld", fraction);

	if (!sign)
	{
		if (fraction < 10 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.00%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else if (fraction < 100 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.0%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else
			wsprintf((LPSTR)numString, "%s.%s", (LPSTR)integerString,
						(LPSTR)fractionString);
	}
	else
	{
		if (fraction < 10 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.00%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else if (fraction < 100 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.0%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else
			wsprintf((LPSTR)numString, "-%s.%s", (LPSTR)integerString,
						(LPSTR)fractionString);
	}
	SetWindowText(GetDlgItem(hwnd, button), (LPSTR)numString);
}
#else //not THREE_DIGITS, let's do 6
void realLabel (HWND hwnd, int button, Real num)
{
	char numString[21];
	char sign =0;
	char integerString[10];
	char fractionString[10];
	long integer;
	long fraction;
	Real temp;

	if (num < 0.0)
	{
		sign = '-';
		num *= -1;
	}

	integer = num; // integer part of num
	temp = num - integer; // temp = fraction part of num
	temp *= 1000000;
	fraction = temp; // fraction part of num
	temp -= fraction;
	if (temp > 0.5)  // round up
		fraction++;
	if (fraction > 999999) // round up > .999999, add to integer
	{
		fraction -= 1000000;
		integer ++;
	}
	if (fraction < 0)
		fraction *= -1;  // don't show the fraction part as negative

	wsprintf(integerString, "%ld", integer);
	wsprintf(fractionString, "%ld", fraction);

	if (!sign)
	{
		if (fraction < 10 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.00000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 100 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.0000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 1000 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 10000 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.00%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else if (fraction < 100000 && fraction != 0)
			wsprintf((LPSTR)numString, "%s.0%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else
			wsprintf((LPSTR)numString, "%s.%s", (LPSTR)integerString,
						(LPSTR)fractionString);
	}
	else
	{
		if (fraction < 10 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.00000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 100 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.0000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 1000 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.000%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		if (fraction < 10000 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.00%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else if (fraction < 100000 && fraction != 0)
			wsprintf((LPSTR)numString, "-%s.0%s", (LPSTR)integerString,
						(LPSTR)fractionString);
		else
			wsprintf((LPSTR)numString, "-%s.%s", (LPSTR)integerString,
						(LPSTR)fractionString);
	}
	SetWindowText(GetDlgItem(hwnd, button), (LPSTR)numString);
}
#endif //THREE_DIGITS

void label( HWND hwnd, int button, char title[10], int d )
{
	char labeltext[20], numberstring[10];

//	itoa( (int)d, numberstring, (int)10);
	wsprintf (numberstring, "%d", d);
	wsprintf( (LPSTR)labeltext, "%s = %s",
		(LPSTR)title, (LPSTR)numberstring );
	SetWindowText( GetDlgItem( hwnd, button ), (LPSTR)labeltext );
}

void ulabel( HWND hwnd, int button, char title[10], unsigned int d )
{
	char labeltext[20], numberstring[10];
	long ld;

	ld = d;
//	ltoa( ld, numberstring, (int)10);
	wsprintf (numberstring, "%ld", ld);
	wsprintf( (LPSTR)labeltext, "%s = %s",
		(LPSTR)title, (LPSTR)numberstring );
	SetWindowText( GetDlgItem( hwnd, button ), (LPSTR)labeltext );
}
/*
void dlabel(HWND hwnd, int button, char title[10], double d)
{
	char sign=0, labeltext[21], numberstring[10], decimalstring[10];
	long ld,ldd;

	if (d < 0.0)
	{
		sign = '-';
		d *= -1.0;
	}
	ld = d;	// Decimal part of d
	d -= ld;	d *= 1000;
	ldd = d;	// Fractional part of d
	d -= ldd; if (d >= .5) ++ldd;	// Round up
	if (ldd > 999) { ldd -= 1000; ++ld;}	// Round up > .99, add to decimal
	if (ldd < 0) ldd *= -1;	// Don't show the decimal part as negative!

//	ltoa(ld, numberstring, (int)10);
//	ltoa(ldd, decimalstring, (int)10);
	wsprintf (numberstring, "%ld", ld);
	wsprintf (decimalstring, "%ld", ldd);

// don't forget the 0! 
	if (!sign)
	{  if (ldd < 10 && ldd != 0)
			wsprintf((LPSTR) labeltext, "%s %s.00%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
		else if (ldd < 100 && ldd != 0)
			wsprintf((LPSTR) labeltext, "%s %s.0%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
		else
			wsprintf((LPSTR) labeltext, "%s %s.%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
	}
	else //negative number
	{  if (ldd < 10 && ldd != 0)
			wsprintf((LPSTR) labeltext, "%s -%s.00%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
		else if (ldd < 100 && ldd != 0)
			wsprintf((LPSTR) labeltext, "%s -%s.0%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
		else
			wsprintf((LPSTR) labeltext, "%s -%s.%s",
				(LPSTR)title, (LPSTR)numberstring, (LPSTR)decimalstring);
	}
	SetWindowText(GetDlgItem(hwnd, button), (LPSTR)labeltext);
}
*/
void numlabel( HWND hwnd, int button, int d )
{
	char numberstring[10];

	//itoa( (int)d, numberstring, (int)10);
	wsprintf(numberstring, "%d", d);
	SetWindowText( GetDlgItem( hwnd, button ), (LPSTR)numberstring );
}


