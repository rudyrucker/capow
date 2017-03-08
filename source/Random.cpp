#include <stdlib.h>
#include <time.h>
#include "random.h"
#include "math.h" //*for sqrt in Randomunitpair
//#define MYMACROS 

/* The Borland stdlib.h defines long rotate left and long rotate right
functions called _lrotr and _lrotl.  We need to call _lrotr(x,1) and
_lrotl(x,1) in the thirtytwobits function. The Borland stdlib.h also
defines rotate left and rotate right functions called _rotr and _rotl.
 We need to call _rotr(x,1) and_rotl(x,1) in sixteenbits.
 If your compiler's stdlib.h doesn't have these functions, you can
 define similar macro functions yourself as by commenting in the line
#define MYMACROS above. */

/* This is a set of randomizing functions written by Rudy Rucker,
rucker@mathcs.sjsu.edu.  The randomizing algorithm used is based on
the one-dimensional cellular automaton called Rule 30.  Stephen Wolfram
discovered that this rule makes a good randomizer, see his paper on
the subject in his anthology, Theory and Applications of Cellular
Automata, World Scientific, 1986.  See also Wolfram's Patent #4,691,291
on this rule, and be warned that use of Rule 30 as a randomizer may
be in violation of the patent.  By way of mitigation of this warning,
do note that Rucker has significantly altered the randomizer by adding
the "count" register, and that Rucker wishes to place the so-modified
Wolfram-Rucker randomizer in the public domain. */

/* Since the random code works with register variables, it makes a
difference whether the registers are 16 bit or 32 bit.  When you do
a 32 bit build, the constant __FLAT__ is defined.  Note that __FLAT__ has
two underscores before and after.  We write two versions of most of the
functions in random.cpp, and have a big switch on __FLAT__ to choose the
appropriate block of code.  The "derived" second-order functions that
don't have calls to sixteenbit or thirtytwobit are at the end, and are
the same for the two memory models. */

#ifndef __BORLANDC__ //Microsoft doesn't define __FLAT__, assume Microsoft is for 32 bit
#define __FLAT__
#endif //__BORLANDC__

#ifdef __FLAT__

//-----------------------------------BEGIN 32 BIT VERSION------------

#ifdef MYMACROS
	#define MYLROTR1(x) ( ( (x) >> 1 ) |  ( (x) << 31 ) )
	#define MYLROTL1(x) ( ( (x) << 1 ) |  ( (x) >> 31 ) )
#endif //MYMACROS

static unsigned long shiftregister = 1946; // Rucker's birthday
static unsigned long count = 0;

unsigned long thirtytwobits(unsigned long seed)
{
/*  In use, we call sixteenbits(seed) once
 with some nonzero seed, and from then on call it
 with seed value zero to get the next random value.  It also works fine
 if you don't seed it it all, but for short runs you may want to start
 from different seeds. Using the same seed twice produces the same
 sequence of numbers.*/

	register unsigned long l,c,r;

	if (seed)
	{
		shiftregister = seed;
		count = 0;
		return seed;
	}
	l = r = c = shiftregister;
#ifndef MYMACROS  //Borland compiler
	l = _lrotr(l, 1);/* bit i of l equals bit just left of bit i in c */
	r = _lrotl(r, 1);/* bit i of r euqals bit just right of bit i in c */
#else  //other compiler
	l = MYLROTR(l);/* bit i of l equals bit just left of bit i in c */
	r = MYLROTL(r);/* bit i of r euqals bit just right of bit i in c */
#endif //MYMACROS
	c |= r;
	c ^= l;		/* c = l xor (c or r), aka rule 30, named by wolfram */
	c ^= count;	/* rucker's trick to  make reaction self-sustaining */
	count++;
	shiftregister = c;
	return c;
}

void rseed(unsigned long n)
{
/* This installs a specific seed, so randomizers will repeat. */
	thirtytwobits(n);
}

unsigned int Randomize() //We're in 32 bit so int means long.
{
/* This uses the time to install a random seed */
/* Return the seed in case you want it for a regression test.*/
	time_t timeslot; //2017 wants special type  
	time(&timeslot);	/* unix time function is seconds since 1970*/
	rseed( (unsigned long)timeslot );
	return (unsigned int)timeslot;
}

unsigned short Randomshort(unsigned short n)
{
/* This returns a random integer from 0 to n - 1. */
/* Bail 0 if n <= 1 to avoid weirdness with modulo operator.*/
	if (n <= 0)
		return 0;
	return ( (unsigned short)((unsigned short)(thirtytwobits(0)) % n) );
}

unsigned long Randomlong(unsigned long n)
{
/* This returns a random integer from 0 to n - 1. */
/* Bail 0 if n <= 1 to avoid weirdness with modulo operator.*/
	if (n <= 0)
		return 0;
	return ( (unsigned long)(thirtytwobits(0) % n) );
}

unsigned int Random(unsigned int n)
{  //Here in the __FLAT__ case, an int is 32 bits.
	return (unsigned int)Randomlong(n);
}

unsigned char Randombyte(void)
{
	return (unsigned char)( thirtytwobits(0) & 0x00FF );
}

Real Randomreal()
{
/* This returns a random real between 0 and 1. */
	return ((Real)(thirtytwobits(0))) / 0xFFFFFFFFUL;
}

Real Randomsign(void)
{
	if (thirtytwobits(0) & 1)
		return 1.0f;
	else
		return -1.0f;
}
//----------------------------------END 32 BIT VERSION------------

#else

//-----------------------------------BEGIN 16 BIT VERSION------------


#ifdef MYMACROS
	#define MYROTR1(x) ( ( (x) >> 1 ) |  ( (x) << 15 ) )
	#define MYROTL1(x) ( ( (x) << 1 ) |  ( (x) >> 15 ) )
#endif //MYMACROS

static unsigned short shiftregister = 1946; // Rucker's birth year
static unsigned short count = 0;

unsigned short sixteenbits(unsigned short seed)
{
/*  In use, we call sixteenbits(seed) once
 with some nonzero seed, and from then on call it
 with seed value zero to get the next random value.  It also works fine
 if you don't seed it it all, but for short runs you may want to start
 from different seeds. Using the same seed twice produces the same
 sequence of numbers.*/

	register unsigned short l,c,r;

	if (seed)
	{
		shiftregister = seed;
		count = 0;
		return seed;
	}
	l = r = c = shiftregister;
#ifndef MYMACROS  //Borland compiler
	l = _rotr(l, 1);/* bit i of l equals bit just left of bit i in c */
	r = _rotl(r, 1);/* bit i of r euqals bit just right of bit i in c */
#else  //other compiler
	l = MYROTR(l);/* bit i of l equals bit just left of bit i in c */
	r = MYROTL(r);/* bit i of r euqals bit just right of bit i in c */
#endif //MYMACROS
	c |= r;
	c ^= l;		/* c = l xor (c or r), aka rule 30, named by wolfram */
	c ^= count;	/* rucker's trick to  make reaction self-sustaining */
	count++;
	shiftregister = c;
	return c;
}

void rseed(unsigned int n)
{
/* This installs a specific seed, so randomizers will repeat. */
	sixteenbits(n);
}

unsigned int Randomize()
{
/* This uses the time to install a random seed */
/* Return the seed in case you want it for a regression test.*/
	long timeslot;
	unsigned int seed; //We're in 16 bit, so int means short.

	time(&timeslot);	/* unix time function is seconds since 1970*/
	seed = (unsigned int) (timeslot & 0x0000FFFFL);
	rseed( seed );
	return seed;
}

unsigned short Randomshort(unsigned short n)
{
/* This returns a random integer from 0 to n - 1. */
/* Bail 0 if n <= 1 to avoid weirdness with modulo operator.*/
	if (n <= 0)
		return 0;
	return ( (unsigned short)(sixteenbits(0) % n) );
}

unsigned long Randomlong(unsigned long n)
{
	unsigned long pick;
/* This returns a random integer from 0 to n - 1. */
/* Bail 0 if n <= 1 to avoid weirdness with modulo operator.*/
	if (n <= 0)
		return 0;
	pick = ((unsigned long)sixteenbits(0) << 16) + sixteenbits(0);
	return ( pick % n );
}

unsigned int Random(unsigned int n)
{
	return Randomshort(n); //Assume 16 bit model
}

unsigned char Randombyte(void)
{
	return (unsigned char)( sixteenbits(0) & 0x00FF );
}

Real Randomreal()
{
/* This returns a random real between 0 and 1. */
	return ((Real)(sixteenbits(0))) / 0x10000L;
}

Real Randomsign(void)
{
	if (sixteenbits(0) & 1)
		return 1.0;
	else
		return -1.0;
}

#endif //__FLAT__
//-----------------------------------END 16 BIT VERSION------------

//---------------------------------BEGIN DERIVED FUNCTIONS----------

Real Randomsignreal(void)
{
/* This returns a random real between -1 and 1. */
	return ( -1.0f + 2.0f * Randomreal() );
}

Real Randomreal(Real lo, Real hi) //A real between lo and hi
{
	return ( lo + (hi-lo) * Randomreal() );
}

void Randomunitdiskpair(Real *x, Real *y)
{/* Randomizes the pair (x,y) within unit pythagorean distance of (0,0) */
	*x = Randomsignreal();
	*y = Randomsignreal();
	while ((*x)*(*x) + (*y)*(*y) > 1.0)
	{
		*x = Randomsignreal();
		*y = Randomsignreal();
	}
}

#define TOO_SMALL 0.0000001 //To avoid division by something close to zero
void Randomunitpair(Real *x, Real *y)
{
	Real dist;
	*x = Randomsignreal();
	*y = Randomsignreal();
	while ((dist = (*x)*(*x) + (*y)*(*y)) > 1.0 || dist < TOO_SMALL)
	{
		*x = Randomsignreal();
		*y = Randomsignreal();
	}
	dist = 1.0f/sqrt(dist); //reciprocal of length
	*x *= dist; //Now make (x,y) a unit vector.
	*y *= dist;
}

//Permute buffer stuff



#define MAX_PERMUTE 128

unsigned char permute_buffer[MAX_PERMUTE];

void jolt_permute_buffer(unsigned char n)
{
	// This is really a method of permute_buffer.
	// point is to set first n entries of permute_buffer to 
	// a permutation of 0,...,n-1.  If n is over the
	// original allocation for permute_buffer the program
	// crashes.

	unsigned char i, j;
	unsigned char available[MAX_PERMUTE];

	if (n>MAX_PERMUTE)
		return; //Should return an error signal.

	for (i=0; i<n; i++)
		available[i] = i;
	for (i=0; i<n; i++)
	{
		j = Random((unsigned short)(n-i));
		permute_buffer[i] = available[j];
		for (; j<n-i-2; j++)  //2017 had j<n-i-1, but then if i is 0, the j+1 could be n, too big.
			available[j] = available[j+1]; // slide down.
	}
}

