/* This is header file for Rucker's randomize functions.  These
randomizing functions are based on a cellular automata rule discovered
by Stephen Wolfram and called Rule 30. */
#include "types.h" //For definition of Real as float or double.

extern void rseed(unsigned int); //Start the randomizer off in a specific state
extern unsigned int Randomize(void); //Seed with the time in seconds
extern unsigned int Random(unsigned int n); //Return an int betweeon 0 and n - 1
extern unsigned char Randombyte(void); //Return a byte between 0 and 255
extern unsigned short Randomshort(unsigned short n); // Short between 0 and n-1
extern unsigned long Randomlong(unsigned long n); //Long between 0 and n-1
extern Real Randomreal(void); //A real between 0.0 and 1.0
extern Real Randomsignreal(void); //A real between -1.0 and 1.0
extern Real Randomreal(Real lo, Real hi); //A real between lo and hi
extern Real Randomsign(void); //1.0 or -1.0
extern void Randomunitdiskpair(Real *x, Real *y);
	// Makes (x,y) a random point with distance <= 1 from (0,0)
extern void Randomunitpair(Real *x, Real *y);
	// Makes (x,y) a random point with distance 1 from (0,0)
extern unsigned char permute_buffer[];
extern void jolt_permute_buffer(unsigned char n);
