#ifndef TYPES_H
#define TYPES_H

//============WINDOWS INCLUDE=======
/* We want to do this here so that the same "STRICT" version
of windows.h gets used throughout.  Don't allow any #include windows.h
anywhere else.  Always do #include types.h instead. */
#define STRICT
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#pragma warning (disable : 4068)

//=================VARIABLE TYPES=====
/* double gives you better accuracy, float gives you better speed.*/
//#define DOUBLE
#ifdef DOUBLE
	typedef double Real;
#else //not DOUBLE means float
	typedef float Real;
#endif //DOUBLE

//================CONSTANTS=====
#define PI (Real)3.141592653
/*If time grows without bound, you might get a floating point overflow
during a long run.  But you don't want to wrap the time at 2*PI in case
you frequency is like 0.1, becuase that needs 10*PI to go through a
full cycle*/
#define TIMEWRAP (Real)100000.0*PI
#define SMALL_REAL 0.0001 //for avoiding division by zero.
#define BIG_REAL 100000.0 //For default maximum of TweakParam

//=================ASSERT============
//#define NDEBUG //Comment this in to turn off assert checking.
#include <assert.h>
#include <COMMCTRL.H>
//================FUNCTION TYPES=====
class CA; //Forward declaration for the FPCELLUPDATE typedef.
//Microsoft Visual C++ case
	#define DllExport __declspec( dllexport )
	typedef DllExport int (*FPINT)(void); //For getting int values out of DLLs
	typedef DllExport void (*FPUSERINITIALIZE)(CA *);
	typedef DllExport void (*FPCELLUPDATE_1)(CA*, int);
	typedef DllExport void (*FPCELLUPDATE_3)(CA*, int, int, int);
	typedef DllExport void (*FPCELLUPDATE_5)(CA*, int, int, int, int, int);
	typedef DllExport void (*FPCELLUPDATE_9)(CA*, int, int, int, int, int, int,
		int, int, int);

#endif //TYPES_H
