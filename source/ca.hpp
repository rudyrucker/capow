/*******************************************************************************
	FILE:				ca.hpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains CA class definitions.

	UPDATE LOG:			9-26-97 
*******************************************************************************/

//=================COMPILER SWITCHES==================================
#ifndef CA_HPP
#define CA_HPP
using namespace std; //2017
#include "types.h" //To include windows.h in a standard way
#include "resource.h"
#include <vector> //Threw errors 2017, changed
/* This is the STL header.  Because Microsoft makes it hard to
use STL and no longer automatically installs the STL files, we supply the necessary
ones with our source code.  They are these *.H files (no *.C or *.CPP files):
algobase.h, bool.h, defalloc.h, function.h, iterator.h, pair.h, and 
vector.h.  More STL *.H files exist, but the ones listed are the only ones needed
to make vector.h work. I might mention here that Microsoft's motivation for
withdrawing support of STL is probably to push programmers to use the Microsft
CArray class, which entails using MFC.  According to some of my students using
BoundsChecker, the CArray class as a memory leak as implemented.  This would
be typical of Microsoft, to greedily undermine a stable industry standard to
push the use of their own in-house standard, and to then implment this new
force-fed standard in a buggy way. RR 5/19/98.  */

#define VCC6
/*When you build the DEBUG version with Visual C++ Version 6, it crashes in a call
to CheckExtension in CAPOW.CPP.  It was fine in Visual C++ Version 5.  Presumably
this is a bug in VCC 6 that, God willing, will be fixed by a coming service patch.*/

//#define FORCENARROW
/*The FORCENARROW switch is used in the WindowBitmap constructor in
bitmap.cpp to force the bitmap to have the x by y dimensions of
FORCEXSIZE by FORCEYSIZE, which I define immediately below.
I used the FORCENARROW switch on May 5, 2005, so as
to be able to make 128 by 1280 bitmaps for borders of my book,
The Lifebox, the Seashell and the Soul, see http://www.rudyrucker.com/lifebox
I also used it n 2006 and 2007 to make borders of my online webzine FLURB at http://www.flurb.net 
In both cases, I wanted a long narrow CA that I could use Edit|Copy to copy the clipboard
and then paste into Photoshop to use as the left edge of an otherwise blank page 
backgroudn bitmap.  In practice the bitmap you get on the clipboard may not actually be as high as
FORCEYSIZE, it will be constrained by the actual size of the active area on your screen. */
#define FORCEXSIZE 64
#define FORCEYSIZE 1500

//#define LITE
 /*Allow only 1 CA.  When this is on, we call the
build CAPOWLOW.EXE (instead of CAPOW.EXE).  There is another build option
in types.h, we can define DOUBLE or not, and build CAPOWDBL.EXE or
CAPOWDLO.EXE, if we wanted to combine LITE and DOUBLE.*/
#ifdef LITE
#define FIXED_640_480 //Control this being in or not with LITE.
/* Allow only a maximum bitmap size of 640 by 480.  This lowers the
memory requirements by (a) making the _max_horz_count lower in the CAs
and, more important by lowering the size of the CX and CY in your
WindowBitmap WBM.  Ordinarily I turn this on just when LITE is on.*/
#endif//LITE

//#define FIXED_FREQ //incorrect attempt to fix global freq at 1.0
//#define ADHOC_FREQ_FIX  //in CA::Adjust_acceleration_multiplier
#define FIXED_CELL_FREQ   //don't vary the freq in the cells

//#define EDGE_FLUTTER_2D
/*If this is off, then when I make wrapflag WF_FIXED for a 2D CA with
 I set all the edge cells past,source,and target
intensities to the same value.  If this is on, then I don't set these
values to the same value, which makes the edges "flutter", which makes
an interesting effect like an ocean edge.  See CA::SetWrapFlag.*/

#define SMOOTH_VARIANCE
/* This is used by CA::Smooth_variance.  The idea is to smooth out the
varying parameters between neighboring cells.*/

#define FAST_ENTROPY
/* This means to use a faster formulat in CA::Entropy*/

//#define ONE_AT_A_TIME
/*Used in CAlist::Update_and_Show.  This make a faster response to the
controls, by only doing one CA update on each pulse of CellMain.  It seems
to to slow the runspeed down by too great a factor*/

//#define TAXI_CAB_METRIC
/*Used in seed.cpp in CA::OneSeed for the dimension == 2 case.  If this
is in, I use the abs(x)+abs(y) metric for sloping up to the seed point.
If this is out, then I use the slightly slower but  superior
standard Euclidean metric to get a nice cone of seed */

#define FIT_STATUS_BAR
/*In Spring 97, an team member (A. Forster?) wrote an alternate Locate method
switching on FIT_STATUS_BAR, replacing the old one in CA.CPP.  The new way is
good because it makes the right room for the status bar. Really the code
for the two Locates should be merged, though, it is crazy to maintain two
blocks that do the same thing, so the "old" locate should have this code. 
RR 5/20/97*/

//#define SAVE_QUERY
/* Whether to ask user whether to save current experiement in WM_CLOSE */

//#define LOAD_ACTIVE_CAS
/*Switch Whether to try and load ACTIVE.CAS at startup.  (ACTIVE.CAS, has to live in same dir as the .EXE)
RR 2007, this doesn't seem to work anymore, that is, if I make an ACTIVE.CAS,
then the program hangs on startup.*/

//=================INCLUDES==================================
#include <fstream> //For iostream //changed to not have .h 2017
#include <iomanip> // added 2017
#include "bitmap.hpp" //For WindowBitmap class
#include "tweak.hpp"   //For TweakParam class
//============MACROS============================================
#define MIN(x,y) ((x)<=(y))?(x):(y)
/* We use CLAMP on Reals in the CA rules, also we use it on ints and
floats in the accessors to set various params*/
#define CLAMP(x,lo,hi) (x)=(((x)<(lo))?(lo):(((x)>(hi))?(hi):(x)))
/*When you clamp an unsigned short colorindex, you don't have to
worry about it being negative*/
#define POSITIVECLAMP(x,hi) (x)=(((x)>(hi))?(hi):(x))
/*In the Ulam rules, we want to do a CLAMP and know if it happened, so
we need this variation, which we write as inline instead of macro.*/
inline BOOL RealClampAndTell(Real &x, Real lo, Real hi)
{
	if (x < lo)
	{
		x = lo;
		return TRUE;
	}
	if (x > hi)
	{
		x = hi;
		return TRUE;
	}
	return FALSE;
}
/*This could be a macro, but is too long, so we make it a "macro" by
inline-ing it*/
inline void WRAP(Real &vx, Real vlo, Real vhi)
 //  {if (vx<vlo){vx = vlo;return;} if (vx>vhi)vx = vhi;return;}
{   //assyne vl0 = -vhi, and vhi > 0.
	if (vx<vlo)
	{
		if (vx < vlo + vlo)    //vlo is negative
			vx = vlo;
		vx = vhi - (vlo-vx);
		return;
	}
	if (vx>vhi)
	{
		if (vx > vhi + vhi)
			vx = vhi;
		vx = vlo + (vx - vhi);
	}
	return;
}
//=========================DEFINES=================================
//-------------------------GENERAL ADJUSTMENT DEFINES--------------------
#define TOOLBARHEIGHT 34 //Used for sizing the window to hold the toolbar
#define STATUSBARHEIGHT 25 //Used for sizing the window to hold the toolbar
#define MAXFILENAME 256 //Length of file names
#define MAXCASTYLENAMESIZE 256
#define BORDER 5 //Spacing for the lines between CAs.
#define FILETYPE	"CA"
#define FILEVERSION 8
#ifndef LITE
#define MAX_CAS 9 /*64 works, but use less for faster comeup.  With
	the advent of 2D CAs, we also are using less of these.  For building
	LITECPW for low-memory machines, set this to 1*/
#define START_CAS 9
#else //LITE
#define MAX_CAS 1 /*  For building
	LITECPW for low-memory machines, set this to 1*/
#define START_CAS 1
#endif //LITE
#define VARIABLE_COUNT 2
/* This is the number of independent variables in
	a 1D Wavecell structure.  It must be at least 1.  We treat "intensity" 
	as a synonym name for variable[0].  (This is going to cause trouble
	with the variable name max_intensity.) */
#define intensity variable[0]
#define PLANE_VARIABLE_COUNT 2
/* This is the number of independent variables in a 2D Wavecell2 cell structure.
	It must be at least 1 */
#define START_FOCUSFLAG FOCUS //Set to ALL or to FOCUS, to set the dialog radio buttons.
#define INITIAL_XSIZE 800 //2017 Size of the start-up window
#define INITIAL_YSIZE 600 //2017 Size of the start-up window
					  //-------------------------2D ADJUSTMENT DEFINES----------------------
//#define BIGGEST2D
#define BIGGER2D
//#define BIG2D
//#define SMALL
/* Defining  BIGGER2D of BIG2D makes you use a bigger 2D CA, which used to be a big deal, but in 2007
is no problem for the RAM. We leave BIG2D on normally now.  We could even increase the size,
of CX_2D and CY_2D, although this will make some of the laoded *.CA and *.CAS files look bad. */

#ifdef BIGGEST2D // 180,000 Modern wide screen is close to a 2x1 aspect	
	#define CX_2D 600
	#define CY_2D 300
#endif //BIGGEST2D
#ifdef BIGGER2D // 125,000 Modern wide screen is close to a 2x1 aspect
	#define CX_2D 500
	#define CY_2D 250
#endif //BIGGER2D
#ifdef BIG2D //60,000 cells with a photograph style 3x2 aspect.
	#define CX_2D 300
	#define CY_2D 200
#endif //BIG2D
#ifdef SMALL //10,800 cells  as you are using a Dark Ages tiny RAM computer with 4x3 aspect.
	#define CX_2D 120
	#define CY_2D 90
#endif //SMALL


//------BUILD SWITCH ID---------------------------
#define BUILD_EXE 0
#define BUILD_SCR 1
/* These are used as the value of buildtype in, respectively CAPOW.CPP and
CASCREEN.CPP so that in CONFIGURE.CPP I can tell exactly how to make the dialog
code work. */

//--------------------TWEAKPARAM DEFINES--------------------
	/* These have been moved to tweakca.hpp */
//-------------------------WAVE ADJUSTMENT DEFINES--------------------
#define ALTSMOOTHSTEPS 3
//Number of steps of heat rule to run after a discontinuity is
//introduced by seeding or by resizing or by toggling wrapflag.
#define MINSMOOTHSTEPS 3 //Don't use 1 or 2 or bad things may happen.
#define SMOOTHRANDSTEPSFACTOR 3 //Smooth 3 time as much for RandomSeed.
#define SMOOTHEDGESTEPS 1000 //this is used as a flag, you only do ONE step.
#define SMOOTHEDGERADIUS 20 /*How many pixels to smooth on either side of a
	changed edge*/
#define WAVE_PERIOD 200
#define GENERATOR_PERIOD 200
#define OSCILLATOR_BASE_PERIOD 200 //This  many timesteps
#define START_BLT_LINES 1 /* 3 is faster, but I don't like the way it looks
	as much.  For "Fastest" mode we do use 3.  But we start in "fast" mode.*/
#define AMPLIFY_VEL_COLOR (Real)80.0 /* 2017 to make the velocity values show up better in the View | Velocity 
	mode, see ca.cpp CA:WaveUpdateStep */
#define AMPLIFY_VEL_COLOR_2D (Real)0.5 /* 2017 to adjust the variable[1] values which are sometimes
	the same as a velocity, show up better in the View | Velocity mode, see ca.cpp CA:WaveUpdateStep2D */
//--------------------DIGITAL ADJUSTMENT DEFINES---------------
#define STATERADIUSCOUNT 10
#define MAXNABEOPTIONS 0x4000U //16K
#define MEMORY 16 /*This must be >= 2.  You need to be careful if it
	ever exceeds horz_count or vert_count.  This tracks how many
     past rows you remember for StripeAvoid.  Applies mainly to digital
	  CAs.*/
#define MAX_STATES 32
#define MAX_RADIUS 7
#define HIGH_FITNESS (Real)0.9
#define MAX_LAMBDA (Real)0.90
#define MIN_LAMBDA (Real)0.10
//-------------------- MIN MULTIPLIERS -----------
#define GENERAL_MIN_MULTIPLIER (Real)0.0003
#define NONLINEARITY_MIN_MULTIPLIER (Real)0.01
#define DRIVER_MIN_MULTIPLIER (Real)0.1
#define MIN_POS_CHUNK 0.00001
//--------------------EXPERIMENT DIALOG DEFINES-----------
#define RAMP_X_MIN (Real)0.1
#define RAMP_X_MAX (Real)0.3
#define RAMP_Y_MIN (Real)0.1
#define RAMP_Y_MAX (Real)0.3
	// used for experiment dialog
//-------------------ENUMS DEFINES FOR CONTROLS-------------
#define BOTH_SHOW 0
#define ODD_SHOW 1
#define EVEN_SHOW 2
#define WF_ZERO 0 //wrapflag
#define WF_FIXED 1
#define WF_WRAP 2
#define WF_FREE 3
#define WF_ABSORB 4
#define ZAP 0	// breed methods
#define CLONE 1
#define SEX 2
#define NONE 3
#define MAX_EVOLVEFLAG 4

/* code moved to Resource.h
#define CA_STANDARD 10
#define CA_REVERSIBLE 11
#define CA_WAVE 12
#define CA_WAVE2 13
#define CA_HEATWAVE 14
#define CA_OSCILLATOR 15
//#define CA_OSCILLATOR_WAVE 16
#define CA_USER 16
#define CA_DIVERSE_OSCILLATOR 17
#define CA_DIVERSE_OSCILLATOR_WAVE 18
#define CA_ULAM_WAVE 19
#define CA_AUTO_ULAM_WAVE 20
#define ALT_CA_WAVE 21
#define ALT_CA_WAVE2 22
#define ALT_CA_OSCILLATOR 23
#define ALT_CA_OSCILLATOR_WAVE 24
#define ALT_CA_DIVERSE_OSCILLATOR 25
#define ALT_CA_DIVERSE_OSCILLATOR_WAVE 26
#define ALT_CA_ULAM_WAVE 27
#define CA_CUBIC_ULAM_WAVE 28
#define CA_HEATWAVE2 29
#define CA_WAVE_2D 30
#define CA_HEAT_2D 31
*/
#define MAX_TYPE_CA 23 //counts number of types in use.
#define CA_CUSTOM 100
#define CA_CUSTOM_2D 101
#define CA_NETWORK 102
//---------TIMER CONSTANTS--------------------
#define UPDATE_TIMER_ID 1
#define RANDOMIZE_TIMER_ID 2
#define RANDOMIZE_TIMER_CYCLE_START 60000//60 seconds.
/* The ?_TIMER integers are passed to SetTimer in AUTORAND.CPP, and they are
returend as the wParam with WM_TIMER messages so you know which timer sent the 
signal. The ?_time_handle integers are returned by SetTimer (they are 0 if the
timer isn't active and equal to the corresponding ?_TIMER_ID if the timer is
active), we use the _handle to tell if we need to do the call to KillTimer to 
get rid of a timer */
//----------------RANDOMIZATION BIT FLAGS---------------
#define RF_ALLVW		0x00000001
#define RF_1D			0x00000002
#define RF_2D			0x00000004
#define RF_BOTHD		0x00000008
#define RF_DIGITALVAL	0x00000010
#define RF_ANALOGVAL	0x00000020
#define RF_BOTHVAL		0x00000040
#define RF_SPLITVW		0x00000080
#define RF_SCROLLVW		0x00000100
#define RF_BOTHVW		0x00000200
#define RF_DING		0x00000400
#define RF_COLOR		0x00000800
#define RF_COUNT1		0x00001000
#define RF_COUNT4		0x00002000
#define RF_COUNT9		0x00004000
#define RF_3D			0x00008000
#define RF_3DFLAT		0x00010000
#define RF_3DBOTH		0x00020000
#define RF_FILE		0x00040000
#define RF_START (RF_BOTHD | RF_BOTHVAL | RF_BOTHVW | RF_DING | RF_COUNT1 | RF_COLOR | RF_3DBOTH) //2017
	//Determines default method to randomize in IDM_RANDOMIZE,and the configure.cpp and sets fRandFlags
	//in AUTORAND.CPP
//---------------------START VALUE DEFINES------------
//Start with 1D Wave
#define START_TYPE_CA ALT_CA_WAVE
#define START_VIEWMODE IDC_SCROLL_VIEW //Viewmode params are defined in resource.h
//Or start with 2D Wave
//#define START_TYPE_CA CA_WAVE_2D
//#define START_VIEWMODE IDC_2D_VIEW 
//Or start with a nonlinearity-driven 1D cubic wave, a "Ulam wave"
//#define START_TYPE_CA CA_AUTO_ULAM_WAVE //It's a great wave, but if you start from random pixels it looks bad
//#define START_VIEWMODE IDC_SPLIT_VIEW //

#define START_SHOWMODE BOTH_SHOW
#define START_SHOWVELOCITY FALSE
#define START_WRAPFLAG  WF_WRAP
#define START_ENTROPYFLAG 1
#define START_EVOLVEFLAG ZAP
#define START_MUTATE_FLAG 0
#define START_TARGET_ENTROPY (Real)0.5
#define START_FAILSTRIPE_SCORE -100
#define START_ENTROPY_BONUS 200
#define START_BREED_FLAG 0
#define START_SEED_FLAG	0
#define START_BREED_CYCLE	600
#define START_SEED_CYCLE	400
#define START_SEED_METHOD	1
#define START_LAMBDA (Real)0.3
#define START_MUTATION_STRENGTH (Real)0.10
#define MAX_GENERATOR_INTENSITY (Real)256.0
#define START_BAND_COUNT 16
#define START_MIN_NONLINEARITY3 (Real)1.0
#define START_MAX_NONLINEARITY3 (Real)200.0
#define START_RADIUS 1
#define START_STATES 8
#define START_SCROLL_FLAG 	0 //0 is off, 1 is on
#define START_STRIPEKILL_FLAG 0
#define START_STRIPESEED_FLAG 1
#define START_GENERATOR_FLAG 0
#define START_COLORRAMP_FLAG 0
#define START_GENERATOR_LOCATION 0
#define START_WIRE_FLAG 0
#define START_GRAPH_FLAG 0
//-----------------------COLOR DEFINES--------------------------
// number of anchor colors in the logical palette
#define MAX_COLOR_LOGIC 13
#define MAX_COLOR 1024U //colorindex ranges over this
#define MAX_BAND_COUNT 512 // I had 128 before, but sometimes I need more
#define DEFAULT_BAND_COUNT 13
#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)
#define START_COLORRAMP_FLAG 0
#define SPLIT_SCROLL_PROPORTION 0.7
//------------------FOURIER DEFINES-------------------- Bang-Nguyen
#define MAXTERM      100  	// Maximum # of terms in the Fourier series
#define TPMAXARRAY  4000   // Maxximum size of the test-point array
//------------------COMPRESS DEFINES----------------------
#define  table_size   70
#define  s_size       150
#define  htable_size   16
#define  SIGNATURE    "capow_codes"
//--------------------CELL PARAM DEFINES------------------
#define CELL_PARAM_COUNT 3 //Number of extra cell params.
//Right now I actually only need three extra for friction,
//spring and mass a.k.a resistance, capacitance, induction.
#define friction_tweak _cell_param[0]
#define spring_tweak _cell_param[1]
#define mass_tweak _cell_param[2]

//-----------------------------------GENERATOR LIST DEFINES----------------------------
#define MAX_GENERATORS 40 //per CA

//=======================GLOBAL FUNCTION DECLARATIONS=========
extern BOOL timer_flag;	// Take a time reading? 1 - yes
extern BOOL update_flag; // Update all the dialog boxes? 1 - yes
	/*These live in CA.HPP */

//=========================WAVECELL CLASS==========================

struct Wavecell  //a struct is an all-public class
{
	COLORREF state;
	Real variable[VARIABLE_COUNT]; //intensity if variable[0].
		// In 1d User rules with "RD" or reaction and diffusion, you might have activator and inhibitor in slots 0 and 1.
		// Or wave rules with boiling might use velocity[1] Commented in 2017
	Real velocity; //This is a dependent helper variable used by wave-like functions, which may also have RD aspects.
	Real _cell_param[CELL_PARAM_COUNT];
public:
	Wavecell();
};


 struct Wavecell2  //a struct is an all-public class
{
	Real variable[PLANE_VARIABLE_COUNT];
	//Here we drop the kludge of having velocity as a separate variable. (I think.  Comment in 2017)
public:
	Wavecell2()
	{
		for (int i = 0; i<PLANE_VARIABLE_COUNT; i++)
			variable[i] = 0.0f;
	}
};

//============================GENERATOR CLASS==============================
class Generator
{
	friend class CA;
	friend class CAlist;
	
	friend class GeneratorList;
private:
	CA *ca_ptr; /* know your owner */
	int location; /* know the x coordinate of your onecell location
		relative to the row buffer of your owner*/
	int location_x;//2D CA
	int location_y;//2D CA
	unsigned char state;
	Real g_intensity; /* for doing waves */
	Real velocity;
	Real time; /* ticks of time increment in timestep of myca.  At
		present (Rudy November 7, 1994) we are using velocity_
		mulitplier for the timestep. */
	float omega;
	float percent_max_intensity;
	float phase;
public:
	Generator(){};
	Generator(CA *myca); //constructor.  We use the default destructor.
	void Seed(){g_intensity = 0.0f; time = 0.0f; state = 0; omega = 1.0;percent_max_intensity = 0.8;phase = 0.0f;}
	void Reset(){time = 0.0;phase = 0.0f;}
	void Step();
	float Omega() {return omega;}
	void Omega(float newomega){omega = newomega;}
	void SmoothOmega(float newomega); //this function will try to smooth the generator when omega is changed
	float Amplitude() {return percent_max_intensity;}
	void Amplitude(float newamp){(newamp > 1.0 || newamp  < 0.0)?percent_max_intensity = 1.0:percent_max_intensity = newamp;}
	int Location(){return location;}
	void Location (int newlocation){location = newlocation;}
	int Location_x(){return location_x;}
	void Location_x (int newlocation){location_x = newlocation;}
	int Location_y(){return location_y;}
	void Location_y (int newlocation){location_y = newlocation;}
};

//===============GENERATOR LIST CLASS ==================================
class GeneratorList              //mike ling
{
private:
	Generator generator[MAX_GENERATORS];
	int count;
	CA *ca_ptr;
public:
	GeneratorList();
	GeneratorList(CA *myca);
	~GeneratorList();
	void Clear();
	void Add(int x);
	void Add(int x, int y);
	void Delete(int x);
	void Deletexy(int x);
	void Step();
	void Reset();
	void SetPtr(CA* ptr){ca_ptr = ptr;};
	float GetOmega(int i){return generator[i].Omega();}
	void SetOmega(int i, float newomega);
	void SetSmoothOmega(int i, float newomega);

	float OmegaLast();
	void OmegaLast(float newomega);
	float GetAmplitude(int i){return generator[i].Amplitude();}
	void SetAmplitude(int i, float newamp);
	int Count(){return count;}
	float Intensity(int i) {return generator[i].g_intensity;}
	int Location(int i) {return generator[i].location;}		// 1D CA
	int Location_x(int i) {return generator[i].location_x;} // 2D CA
	int Location_y(int i) {return generator[i].location_y;} // 2D CA
};




//==============DERIVPAIR CLASS==========================
#ifdef RUNGE_KUTTE
struct DerivPair
{
	Real _di;
	Real _dv;
	DerivPair(){}
	DerivPair(Real di, Real dv):_di(di),_dv(dv){}
};
#endif //RUNGE_KUTTE

//==================Declarations of CA Things Found In DLL files=========
extern "C" DllExport int USERNABESIZE(void);
extern "C" DllExport int USERCASTYLE(void); 
extern "C" DllExport void USERINITIALIZE(CA *);
extern "C" DllExport void USERRULE_1(CA*, int); //1 Cell, has pointer to nabes
extern "C" DllExport void USERRULE_3(CA*, int, int, int); //3 1D nabes
extern "C" DllExport void USERRULE_5(CA*, int, int, int, int, int); //5 1D or 2D nabes
extern "C" DllExport void USERRULE_9(CA*, int, int, int, int, int, int,
	int, int, int); //5 1D or 2D nabes
//============================CA CLASS==============================
/* In the comments on the fields of the CA class, we note which
params are AUTOSET by some method, and which are to be set by LOADSAVE
values which you can read and write to files.*/
class CA
{
//	friend class Graph3D;
	friend class CapowGL;
	friend class CAlist;
	friend class Generator;
	friend class GeneratorList;
	friend ofstream& operator<<(ofstream& ofs, CA* target);
	friend ifstream& operator>>(ifstream& ifs, CA* target);
	friend void Version_123_Load_Individual(ifstream& ifs, CA* target,
		int version); // DDC 4-25-96
	friend void Version_45_Load_Individual(ifstream& ifs, CA* target); // DDC 4-25-96
	friend void Version_6_Load_Individual(ifstream& ifs, CA* target); // DDC 4-25-96
	friend BOOL Version_7_Load_Individual(ifstream& ifs, CA* target); // DDC 4-25-96
	friend BOOL inBinary(ifstream& ifs, CA* target);
	friend BOOL outBinary(ofstream& ofs, CA* target);
private:
//==============================CA Private=========================
	WindowBitmap *WBM; //AUTOSET by CAlist::SetWindowBitmap
	HWND hwnd; //AUTOSET by CAlist::CAlist.Where I live.
 	class CAlist *calist_ptr; //AUTOSET by CAlist::CAlist.  The parent.
	int type_ca; //LOADSAVE set, used by CA::Settype.
	int _castyle; //Gives the CA_* value, or type_ca, of the most similar CA rule
		//AUTOSET.  Use this to reduce the cases in dialog boxes, and with CA_USER.
	char _castylename[MAXCASTYLENAMESIZE];
	long _generationcount;
	int _phylum; /* NEW LOADSAVE, used by CA::Settype when type_ca is
		CA_USER */
	int dimension; //AUTOSET by CA::Settype. 1D or 2D.
	int viewmode; /* LOADSAVE.  Currently IDC_DOWN_VIEW, IDC_SCROLL_VIEW,
		IDC_WIRE_VIEW IDC_GRAPH_VIEW, IDC_SPLIT_VIEW, IDC_2D_VIEW*/
	int row_number; /* AUTOSET. Initialized in CA::Locate and updated
     	in CA::?????Update.  A counter to keep track where you are.
		This is wrapped back to miny after it hits maxy. */
	long int cellcount; /*AUTOSET. Initialized in CA::Locate and updated
     	in CA::?????Update. How many cells have you updated so ?  Use
		this with entropy and freqlookup. */
	int wrapflag; /* LOADSAVE.  Set by CAlist::SetWrapFlag, not by
     	CA::SetWrapFlag, because you need to set the smooth param as
          well.  WF_ZERO, WF_FIXED, WF_WRAP, WF_FREE, WF_ABSORB.*/
	int blankedgeflag;/* This is obsolete and could be removed, but note that then
		all the *.DLL may need to be rebuilt as then a CA object will have a different
		size.  Note that it's also used in the loadsave code, so then all the
		*.CA and *.CAS would need to chagne.  So let's leave it in place, and
		next time we need a new int field of CA, call it blankedgeflag! */
//-------------------CA Color Private-----------------------------
	int band_count; //LOADSAVE used by all continuous CAs
	COLORREF  *colortable; /*AUTOSET by CA::Setcolortable.
		For digital CAs, the color of a cell will just be
          colortable[state].  For CA_wave guys, we'll do some map from
          intensity into the MAX_COLOR range to get colorindex_target_row,
          and use colortable on that.*/
//---------------------CA Window Private-----------------
	int _max_horz_count; //AUTOSET Either screensize or 640, see FIXED_640_480 switch.
   int horz_count; /*LOADSAVE.  This measures the active visible length
     	of the 1D CA.  TODO, when you load, use this number
     	to force the window to the desired size.*/
	int minx, miny, maxx, maxy, vert_count; /* AUTOSET
		by CAlist::Locate and CA::Locate according to tile_number and
          the size of the display */
	int splity, split_vert_count; /*AUTOSET by CAlist::Locate.
		For IDC_SPLIT view with SCROLL on top, GRAPH on bottom.
          splity is somewhere between miny and maxy; split_vert_count
          measures maxy - splity, the space for the graph.*/
	int tile_number; /*AUTOSET by CAlist::Locate.  Use this for
     	multiple CAs on one screen.*/
//---------------------CA Generator Private-----------------
	BOOL generatorflag; /*LOADSAVE.*/
	class Generator *generator_ptr; /*AUTOSET by CA::CA.  TODO, make
		this a list so you can have multiple genearators.*/
	public:
	GeneratorList generatorlist;   //mike
	private:
	//---------------------CA Evolution Private-----------------
	Real fitness; /* AUTOSET. lies between 0 and 1, is typically set to
		fabs(entropy - target_entropy). But if a rule fails the
		shift test then its fitness is 0. */
	BOOL entropyflag; /* LOADSAVE. 1 means compute entropy every 64K
		cells (and act on it) and 0 means don't */
	Real entropy; /* AUTOSET. normalized entropy lies between 0.0 and 1.0, it
		tells what percentage of maximum entropy the nabe patterns have.*/
	Real target_entropy; /* LOADSAVE. normalized entropy between 0.0 and 1.0,
		that you want to evolve towards */
	Real entropy_bonus;
		/* LOADSAVE. The score bonus for being close to targ4et_entropy */
	Real fail_stripe; /* LOADSAVE. score penalty for fail strip-ing */
	Real score; /* AUTOSET. A ca's current score */
	unsigned short  *freqlookup; //AUTOSET
		/* freqlookup keeps track of how often each nabeoption occurs.
		Whenever you check lookup[nabecode], you at the same time
		increment freqlookup[nabecode].  Note that you can run ths over
		64K really quickly, like in 100 lines of fullscreen.  But suppose
		we only use in screens of 4 or 9 CAs.  In 4, it takes 200, not
		quite a full frame.  Well, that'll do.  And I'll check entropy
		each time the worst case count hits 64K.  unsigned char would
		be out of the question, like one line even in 9 mode.  Would
		prefer long unsigned int, kind of, but that's overkill.  At
		the worst, counting lookup and freqtalbe, we are looking at
		9*3*32K  = 864K bytes, which won't work.  So there has to be
		a check on it.*/
//-------------------Fourier CA private-------------------
	int	test_point, temp_test_point;
	Real	rel_test_point;
	Real 	*tp_real_array;
	Real 	*fourier_a,     			// to contain the Fourier coefficients
			*fourier_b;             //
	Real 	*fourier_approx;			// approximating values by Fourier series
	int numofterm;        		// number of terms in the approximation
	int   tp_startpos, tp_endpos;
	BOOL  fourierflag;      		// fourier stuff being displayed or not
	int	tp_viewmode;				// time scale 1/2, 1, or 2
	int tp_viewtype;				// graph or scroll
	int tp_approxtype;				// sine only, cosine only, or both
	BOOL	tp_graphflag, tp_approxflag,
			tp_cosineflag, tp_sineflag, tp_spectrumflag;
//--------------CA--Tweak Params Private-------------------------
	//....................CA.Wave Tweak Params...............
	AdditiveTweakParam _max_intensity;  // LOADSAVE
	AdditiveTweakParam _max_velocity;  // LOADSAVE
	AdditiveTweakParam _maxvalpercent;
	MultiplicativeTweakParam _chunk; //LOADSAVE.
	MultiplicativeTweakParam _dx;  // LOADSAVE
	MultiplicativeTweakParam _dt;  // LOADSAVE
	//....................CA.Wave Tweak Params...............
	//----Oscillator--------
	MultiplicativeTweakParam _frequency_multiplier; //LOADSAVE
	MultiplicativeTweakParam _driver_multiplier; //LOADSAVE
	MultiplicativeTweakParam _spring_multiplier; //LOADSAVE
	MultiplicativeTweakParam _mass;  // LOADSAVE
	MultiplicativeTweakParam _friction_multiplier;
	//....................CA Diverse Tweak Params...............
	MultiplicativeTweakParam _variance;//LOADSAVE
	//....................CA Heat Tweak Params...............
	MultiplicativeTweakParam _heat_inc;
 		/* LOADSAVE.  This is the amount to increment the intensity*/
	//....................CA Nonlinear Tweak Params...............
	MultiplicativeTweakParam _nonlinearity1, _nonlinearity2; /*LOADSAVE.
		nonlinearity1 is used by fermi-pasta-ulam, and
		nonlinearity2 is used by the cubic ferm-pasta-ulam.*/
	//....................CA Not Yet Tweak Params...............
	Real  _phase; //LOADSAVE, Used by Oscillator types.
	Real lambda; /* LOADSAVE. Used by Standard and Reversible.  This
		measures the density of non-zero	entries desried in the lookup table.
		Start with START_LAMBDA.*/
//-------------------CA Userrule private--------------------------------
	HMODULE _DLLhandle; //Currently loaded DLL
	char _userrulename[MAXFILENAME];
	int _usernabesize; // Set from DLL with _lpfnUSERNABESIZE function.
		//Will be 1, 3, 5, or 9. 
	int _usercastyle;   // Set from DLL with _lpfnUSERCASTYLE function.
		//Is a CA_* code of the type_ca kind, corresponding to a similar CA.
	FPINT _lpfnUSERNABESIZE;
	FPINT _lpfnUSERCASTYLE;
	FPCELLUPDATE_1 _lpfnUSERRULE_1; // typedef of FPCELLUPDATE_1 is in types.h 
	FPCELLUPDATE_3 _lpfnUSERRULE_3; // typedef of FPCELLUPDATE_3 is in types.h 
	FPCELLUPDATE_5 _lpfnUSERRULE_5; // typedef of FPCELLUPDATE_5 is in types.h 
	FPCELLUPDATE_9 _lpfnUSERRULE_9; // typedef of FPCELLUPDATE_9 is in types.h
	//---------------CA Digital Private----------------------------------------
	int sourcerowindex; /* AUTOSET. always have source_row =
		rowbuffer[sourcerowindex] */
	int targetrowindex; /* AUTOSET. always have target_row =
		rowbuffer[targetrowindex].  Also keep targetrowindex =
		sourcerowindex + 1 MOD MEMORY.*/
	unsigned char  *source_row;//LOADSAVE.  This is TODO.
	unsigned char  *target_row;
		/* use these two buffers to update the CA; the present values are
		read from source_row and the new values are stored in targdt_row.
		Then you swap these two pointers, show the source_row, and do the
		update again.  These are allocated in Locate, after the actual
		size of the CA is known.*/
	unsigned char  *rowbuffer[MEMORY]; /*AUTOSET. Use for remembering
     	old rows and checking for repeats stripes.*/
	int radius; //LOADSAVE. radius of cells to examine, range 1 to 4.
	int states; //LOADSAVE. number of states per cell.
	int statebits; //AUTOSET. number of bits per state.
	int nabesize; /* LOADSAVE 1 + 2*radius, the number of cells in a
     	neighborhood.*/
	unsigned short nabeoptions; //AUTOSET. Keep this under 32K
		/* pow(2,nabesize), the number of possible lookup
		tables.  For radius 1,2,3,4, this is 8,32,128,512. */
	unsigned short mask; /* AUTOSET.  nabeoptions - 1. Use to mask the
     	nabecode in Update.*/
	Real actual_lambda; /* AUTOSET.  actuallambda measures the density
     	of non-zero entries that are actually in the lookup table.
          This needs to be kept separate from the *desired* value
          lambda because the value drifts in a brownian walk, and can
          end up stuck at 0.0 or 1.0 unless you keep normalizing it to
			 the desired value lambda.*/
	unsigned char  *lookup;/* LOADSAVE.  lookup holds the new cell
     	values, indexed by the nabe code numbers which are assembled in
          Update.  lookup is allocated in the constructor.  Lookups are
          all well under 32K*/
//------------------CA Reversible Digital private-------------------------
	int pastrowindex; //AUTOSET
	unsigned char  *past_row; //LOADSAVE.  This is TODO
//-----------------------CA Wave private----------------------------------
	int monochromeflag; //NEW_LOADSAVE
	BOOL _smoothflag; //Whether or not to do automatic smoothing.
	int _startsmoothsteps; /* default startsmoothsteps is SMOOTHSTEPS for
		the Alt??? type rules, 0 for the others */
	int _smoothsteps; /*AUTOSET. Use this as the number of remaining steps
		of heat rule to run in order to eliminate discontinuities,
		this gets set to startsmoothsteps after OneSeed, Seed, or Locate.
		Need this for the Alt???? type rules. */
	BOOL _justloadedcells; /*Set this to TRUE when you have just loaded a
		rule which loads individual cell values, this is to prevent the
		call to smooth which would wipe out the values */
	Real _stretch_oldmaxintensity;/*AUTOSET. This and the following
     	_stretch_??? params are all helper variables used in the smooth
			 process.  Search for "smoothsteps--" in WaveUpdate and WaveUpdate2D
          CA.CPP to see them in action.  Note that I don't need to initialize
          them in the CA::CA constructor; they get initialized in
          WaveUpdate and WaveUpdate2D*/
	Real _stretch_newmaxintensity;//AUTOSET.
	Real _stretch_oldminintensity;//AUTOSET.
	Real _stretch_newminintensity;//AUTOSET.
	double _stretch_oldavgintensity;//AUTOSET.
	double _stretch_newavgintensity;//AUTOSET.
	BOOL _stretch_lasttime;//AUTOSET.
	double _stretch_cell_weight;//AUTOSET.
	BOOL _dx_lock; /* LOADSAVE. Whether or not user can change _dx to value
     	 other than 2.0 * _dt.  TRRE means no other vals, FALSE means
           yes you can.*/
	int showmode;  /*LOADSAVE.  This determines whether you show
     	all the cells or just every other cell.*/
	int showvelocity; /*LOADSAVE.  0 means show intensity, 1 means
     	show velocity.*/
	Real time; /* LOADSAVE.  I don't think we use it really, but at some
     	point it might make a difference what the CA thinks the time
          is, particularly if this time gets used by the generators.*/
	  Real _wavespeed, _dt_2, _dx_2, _dt_over_2, _dt_over_6, _dt_over_dx_2,
		_wavespeed_2_times_dt_2_over_dx_2, _dt_over_12_times_dx_2,
		_wavespeed_2_times_dt_2_over_12_times_dx_2,_1_over_mass,
			 _dt_over_mass, _wavespeed_2_times_dt_over_dx_2, _dt_2_over_mass;
          /* AUTOSET. _wavespeed is always 1.0 at this point.  All the other
			 params get set by CA::Adjust_acceleration_multiplier() on
			 the basis of _dt, _dx, _dx_lock, and _wavespeed.*/
	Wavecell  *wave_past_row;  //LOADSAVE (TODO). Holds info for time - 1
	Wavecell  *wave_source_row; //LOADSAVE (TODO) Holds info for time
	Wavecell  *wave_target_row; //AUTOSET. Holds infor for time + 1
	COLORREF  *COLORREF_target_row; /* AUTOSET. Convert digital
         	target_row by doing lookup into colortable, or convert Wave
     	colorindex_target_row by	doing lookup.  The difference is that
     	target_row is for digital states which will only be one byte,
     	but Wave's colorindex_target_row indices are two bytes.*/
	int wavepastindex; /* AUTOSET. always have wave_old_row =
		waverowbuffer[waveoldindex] */
	int wavesourceindex; /* AUTOSET. always have wave_source_row =
		waverowbuffer[wavesourceindex] */
	int wavetargetindex; /* AUTOSET. always have wave_target_row =
		waverowbuffer[wavetargetindex] */
	Wavecell  *waverowbuffer[3];//AUTOSET.
	unsigned short  *colorindex_target_row; /* AUTOSET. Use this
     	instead of target_row to save the (possibly bigger than 256)
     	color index values for wave.*/
	COLORREF  *_anchor_color; //LOADSAVE.
//---------------CA Wave 2D private-----------------------------------
	int horz_count_2D, vert_count_2D; /*LOADSAVE.  TODO as with
		the 1D horz_count, these should be forced in by making the
          window the right size.  ACtually you don't need to change
			 the windwo size if you use stretchblt.*/
	int maxx_2D, maxy_2D; /* LOADSAVE.  These are set according to
     	available memory for the buffer.  At present we AUTOSET them,
          but eventually it would be better to allocate.  If in Locate
          we find that the horz_count_2D, say is larger than the maxx_2D,
			 then we use the smaller value and show with StretchBlt.*/
	Wavecell2  *wave_past_plane;  //LOADSAVE (Maybe TODO). time - 1
	Wavecell2  *wave_source_plane; //LOADSAVE (Maybe TODO). time
	Wavecell2  *wave_target_plane; //AUTOSET. Holds infor for time + 1
	Wavecell2  *waveplanebuffer[3];//AUITOSET
	 /* .  We always have
		wave_old_plane = waveplanebuffer[waveoldindex]  always have
			 wave_source_plane = waveplanebuffer[wavesourceindex]  always have
			 wave_target_plane = waveplanebuffer[wavetargetindex] */
//------------------CA Wave Oscillator private-----------------------------
	Real frequency_factor; /*AUTOSET.  This is
	  is a helping variable defined in CA::Adjust_acceleration as
		2*PI/(_dt*OSCILLATOR_BASE_PERIOD), so that BASE_PERIOD
		steps puts it thru a full oscillation.*/
public:

	vector<class TweakParam *> userParamAdd;

//=========CA====CONSTRUCTORS===================================
	CA(CAlist *mylist);
		/* Sets Allocates. constructor is called when a CA object is
		 declared.  I have the constructor set all the possible fields.
		adopts the hwnd of mylist, adopts mylist as calist_ptr.*/
	~CA(); /* The destructor.  Frees the memory in source_row,
		target_row, lookup, etc.*/
	void Allocate();
		/*Allocates rowbuffer[i], lookup and freqlookup  handles*/
//==============ACCESSORS=====================================
	int Gettype(void) {return type_ca;}
	int Getcastyle(void) {return _castyle;}
	int Getshowmode(){return showmode;}
	int Getwrapflag(void) {return wrapflag;}
	int Getdimension(){return dimension;}
	long BumpAndReturnGenerationCount() {  return ++_generationcount; }
	long GetGenerationCount() {  return _generationcount; }
	void ResetGenerationCount() { _generationcount = 0 ;}
	int Maxx() { return maxx; }
	int Minx() { return minx; }
	int Get_TestPoint() { return test_point ; }
	Real MaxIntensity(){return _max_intensity.Val();}
//------------------CA-Color accessors-------------------------------
	COLORREF  *GetColortable() { return colortable;}
	COLORREF  *GetAnchors() { return _anchor_color;}
	COLORREF Color_look(int numb);
		// Returns colortable[lookup[numb]]. Used by Lookup Dialog Box???
	int Get_monochromeflag(){return monochromeflag;}
	int Getband(void) {return band_count;}
//---------------------CA-TweakParam Accessors---------------------------
	TweakParam* GetTweakParam(int tweakParamType);
//---------CA-Digital Accessors--------------------------------------
	int Getradius(void) {return radius;}
	int Getstates(void) {return states;}
	unsigned short Getnabe(void) {return nabeoptions;}
		// Returns the number of nabeoptions. Used by Lookup Dialog Box
	Real Getlambda(void) {return lambda;}
	Real Getactuallambda(void) {return actual_lambda;}
//---------------------CA-Wave accessors-------------------------------
	int Get_smoothflag(){return _smoothflag;}
	Real Get_phase(void) {return _phase;}
	BOOL Get_dx_lock(){return _dx_lock;}
	int Getshowvelocity(){return showvelocity;}
//----------------CA-Evolution accessors--------
	int Getentropyflag(void) {return entropyflag;}
	Real Gettargetentropy(void) {return target_entropy;}
		// desired entropy
	Real Getentropy(void) {return entropy;}	// actual entropy
	BOOL Getgeneratorflag(void) {return generatorflag;}
	Real Getfailstripe(void) {return fail_stripe;}
	Real Getscore(){return score;}
	void Setscore(Real val) { score = val; }
	Real Getentropybonus(void) {return entropy_bonus;}
//----------------CA-Fourier Accessors-----------------------
	int Get_tpviewmode(void) {return tp_viewmode;}
	int Get_tpviewtype(void) {return tp_viewtype;}
	int Get_tpapproxtype(void) {return tp_approxtype;}
	BOOL Get_fourierflag(void) {return fourierflag;}
	int Get_numofterm(void) {return numofterm;}
	BOOL	Get_graphflag(){return tp_graphflag;}
	BOOL	Get_approxflag(){return tp_approxflag;}
	BOOL	Get_cosineflag(){return tp_cosineflag;}
	BOOL	Get_sineflag(){return tp_sineflag;}
	BOOL	Get_spectrumflag(){return tp_spectrumflag;}
//==============MUTATORS=====================================
	void Settype(int newtype);
	void Setviewmode(int newmode);
	void Setshowmode(int newmode){showmode = newmode;}
	void Set_dx_lock(BOOL l);
	void Setshowvelocity(int newshowvelocity){showvelocity = newshowvelocity;}
	void Setgeneratorflag(BOOL onoff);
//-------------CA-Seed Mutators-----------------
	void Seed(){FourierSeed();}; //Choose default Seed behaviour here.
	void RandomSeed();
		/* this seeds the source_row with random numbers
		between 0 and states - 1.  It is called in Locate at startup,
		after source_row is allocated.  Let each Wavecell.intensity and
		Wavecell.velocity range from -MAX_INTENSITY to +MAX_INTENSITY,
		and let the velocity be 0.0*/
	void FourierSeed();
		/* this seeds the source_row with fourier sums.*/
	void Sineseed();
		/* This seeds the source row with a periodic number pattern.
		Used especially for waves, where it seeds intentsity as a sine
		wave and velocity as the corresponding derivative cosine wave.*/
	void Oneseed();
		/* this seeds the source_row with all zeroes except for a
		single one in the center.  Set all the Wavecell's intensity
		and velocity to 0.0, except the central one, which gets a
		random intensity between -MAX_INTENSITY to +MAX_INTENSITY*/
	void Zeroseed();
		/* seeds with all zeros to calm down so you can see the
		generator*/
	void Halfmaxseed();
		/* seeds with all half of max value to "pluck" to get oscillators
		excited */
	void BitmapSeed();
		/* use the pixels in a resource bitmap to seed the CA */
//---------CA-Color mutators---------------------
	void Setcolortable();
		/* Depending on the number of states, we set color values.*/
	void Setmonochromecolortable();
	void Setoldcolor();
		/* Try to get the good old 16 values of Rudy's color table */
	void SetSamecolortable(COLORREF *color, int bc);
	void SetColors(); // 2-24-96 DDC
	void Change_band_count(int updown);
	void Set_monochromeflag(int onoff);
	void Set_band_count(Real band);
//----------CA-Tweak Param mutators-----------------------
	void BumpTweakParam(int tweakParamType, Real updown);
	void SetTweakParam(TweakParam* t, Real val);
	void SetTweakParam(int tweakParamType, Real updown);
	void RandomizeTweakParam(TweakParam* t, Real valMean, Real valVariance);
	void RandomizeTweakParamPercent(TweakParam* t, Real valMean, Real valVariance);
	void Set_phase(Real p);
//----------CA-Digital Mutators----------------
	void Lambdalookup();
		// Sets a new random lookup with lambda percent non-zero values.
	void Changeradiusandstates(int newradius = 1, int newstates = -1);
	// if newradius or newstates == -1, then that part is unchanged
		/* These reset radius, states, nabesize, nabeoptions, and
		mask, frees the old lookup, and allocates a new one and
		fills it.  We need the combined form for copying CAs so that
		a legit copy isn't rejected because e.g. the radius looks too
		big before you notice the new state makes it ok. */
	void Symmetrize(); /* Make the lookup table summetric */
	void Setlambda(Real newlambda);
	void Changelambda(Real newlambda);
		/* Increment or decrement lambda if updown is, respectively
		1 or -1.  In the range 0.1 to 0.9, change by adding or
		subtracting 0.1.  Out of that range chagne by halving the
		distance to, respectively, 1.0 or 0.0.  Then change the current
		lookup table's density of nonzero entries to the new lambda.*/
//--------------------CA-Digital Reversible Mutator---------------
	void Reverse(); /* swap the past_row entries with the
		source_row entries.  This makes a TYPE_CA_REVERSIBLE
		rule run backwards.  It works like this.  In a rule
		like this,
		Target = f(Source) - Past, so also
		Past = f(Source) - Target.  At the end of a call to
		ReversibleUpdate the buffers get rolled forward.  So
		if I start with Past0 and Source0 and compute Target0,
		then Past1 is Source0 and Source1 is Target0.  If I
		exchange the entries in these two to get Source2 =
		Past1 and Past2 = Source1, then when I update
		Target1 = f(Source2) - Past2, I am really doing
		Target1 = f(Past1) - Source1, which is
		Target1 = f(Source0) - Target0, which  is Past0.
			 TODO: make this work for the alt waves!*/
//---------------------CA-Wave mutators-------------------------------
	void Set_smoothflag(BOOL onoff){_smoothflag = onoff;}
	/*Needed to split Get_spring next one into multiple rocker use.
		TODO it would be better to add separate methods*/
	void Reset_smoothsteps(int factor = 1);
	void Set_justloadedcells(BOOL onoff){_justloadedcells = onoff;}
	void One_smoothsteps(){_smoothsteps = MINSMOOTHSTEPS;}
	void Change_phase(int updown);
//---------------CA-Diverse Cell Mutators---------------
	void Smooth_variance();
//------------CA-Evolution Mutators---------------------
	void Setfailstripe(int updown);
		// Penalty for failing the stripe test
	void Setwrapflag(int newrapflag);
	void Setentropyflag(BOOL onoff) {entropyflag = onoff;
		 if(entropyflag)Resetfreq();}
	void Setentropybonus(int updown);
		// Bonus for getting an entropy close to target_entropy
	void CopyCA(CA *goodCA);
		/* Copy everything from goodCA. Wave stuff, source_row, ... */
	void Mutate(Real mutation_strength);
		/* mutation_strength between 0.0 and 1.0.  Mutate that
		proportion of the table's entries*/
	void Install_Ramp(int paramcode, Real rampvalue);
//----------------CA-Fourier Mutators-----------------------
	void Set_tpviewmode(int new_viewmode) {tp_viewmode = new_viewmode;}
	void Set_tpviewtype(int new_viewtype) {tp_viewtype = new_viewtype;}
	void Set_tpapproxtype(int new_atype){tp_approxtype = new_atype;}
	void Set_fourierflag(BOOL num) {fourierflag = num;}
	void Set_TP(void);
	void Set_TempTP(void) {temp_test_point = test_point;}
	void Change_TempTP(int);
	void Set_numofterm(int nt) {numofterm = nt;}
	void Change_numofterm(int);
	void Reset_tp_array(void);
	void  Reset_fourier_array(void);
	void	Reset_tp_all(void);
	void	Change_approxflag(){tp_approxflag = 1 - tp_approxflag;}
	void	Change_cosineflag(){tp_cosineflag = 1 - tp_cosineflag;}
	void	Change_sineflag(){tp_sineflag = 1 - tp_sineflag;}
	void	Change_spectrumflag(){tp_spectrumflag = 1 - tp_spectrumflag;}
	void	Change_graphflag(){tp_graphflag = 1 - tp_graphflag;}
//=========================METHODS=================
	void Full_adjust_params_for_type(); /* Fix various things to be stable.
		This gets called after SetType in user RandomizeSettype.  This
		 makes problems in loading files.  I'll use this when user
		hand-changes a type also.*/
	void Basic_adjust_params_for_type(); /* Fix the minimum.  OK
		for loading files.*/
	void CA::Locate(int tile, HWND hwnd, int CA_count_per_edge);

	void Locate(int itile_number = 0,int dmaxx = 0, int dmaxy = 0,
		int CA_count_per_edge = 1);
		/*tile_count tells how many CAs you want on screen, and
		itile_number tells which position this CA is in.  itile_number
		lies between 0 and tile_count-1.  Locate sets minx,maxx,miny,
		maxy, and intializes row_number to miny.Locate sets horz_count to
		maxx + 1 - minx, which is the and allocates source_row and
		target_row.*/
	void SyncRows();
		/* Copy source_row[0 to horz_count] into rowbuffer[0],
		copy past_row[0 to horz_count] into rowbuffer[MEMORY - 1],
		copy wave_source_row[0 to horz_count] back to start_wave_row
		this only does something if source and target are flipped
		from their original positions.
		This is because when we seed in the beginning, we only seed
		rowbuffer[0] with the 'whole' line.
		We need to do this when we resize, because we need the extra
		values stored in rowbuffer[0]. File save also uses this because
		we want to save the whole line */
	int Getviewmode(){return viewmode;}
  	void Touch_CA(int cx, int cy, int sender); /* Impliments users ability to
   	touch a CA at location cx,cy. sender is WM_LBUTTONDOWN or WM_RBUTTONDOWN.*/
	void RandomTouch_CA( ); //touch at a random spot.
//----------CA Digital helpers---------------------
	void Computeactual_lambda();	// Find a CAs actual lambda level
//----------------------CA Wave helpers-------------------------------
	void SmoothAverageStretch1D();
	void SmoothAverageStretch2D();
	void SmoothEdge1D();
	void SmoothEdge2D();
	void Adjust_acceleration_multiplier(); //Adjusts a bunch of params
//--------------CA-Oscillator Helper--------------------------------------
#ifdef RUNGE_KUTTE
	/* RUNGE-KUTTE helper for Oscillator , and Oscillator, no longer used. */
	DerivPair OscDerivs(Real intensity, Real velocity, Real dt);
	DerivPair DiverseOscDerivs(Real intensity, Real velocity, Real dt,
		Real massparam, Real frictionparam, Real springparam);
#endif //RUNGE_KUTTE
//------------------CA 1D Update functions public------------------------
	void (CA::*UpdateFunction)(HDC hdc);  /* This is a member function
		pointer as decribed to me by Horstmann, with a reference to
		 "Coplein's book on advanced C++ idioms".*/
	void Update(HDC hdc){(this->*UpdateFunction)(hdc);} //CRASH here once,
	//suggests that sometimes UpdateFunction isn't getting initialized anymore.
	//Define in ca.cpp as a switch on ca_type.
	void (CA::*UpdateCell_1)(int);
	void (CA::*UpdateCell_3)(int, int, int);
		/* This is a member function pointer used by Wave functions
		to update the velocity and intinsity*/
	void (CA::*UpdateCell_5)(int, int, int, int, int);  //2nd order or 2D
		/* This is a member function pointer used by Wave functions
		to update the velocity and intinsity. Can be 1D or 2D.*/
	void (CA::*UpdateCell_9)(int, int, int, int, int, int, int, int, int);
		/*2D member function pointer used by Wave functions
		to update the velocity and intinsity. Can be 1D or 2D.*/
	void Show(HDC hdc); /* This holds the code common to the STandard
		Updates and the Wave Updates, the show & stripe stuff, also
		the stuff dealing with wire mode. */
	void StandardUpdate(HDC hdc);
		/* this is the heart of the algorithm.  You update the values
		of target_row by successively converting each cell's neighborhood
		into a bit-assembled nabe code number.  When states is 2, you use
		repeated calls of nabe <<= 1, nabe&= mask, and
		nabe |= source_row[i], and then compute target_row[i] as
		lookup[nabe].  When target_row is ready, you swap its pointer
		with source_row.  And then you increment row_number.*/
	void ReversibleUpdate(HDC hdc);
		/* This is the same as the CA_standard Update except that
		while before, you have NewC = lookup(C_nabe), you now let
		NewC = ( f(C_nabe) - OldC ) MOD (states).*/
	void WaveUpdate(HDC hdc);
		/* The wave update method is to compute acceleration as
		(2* C_intensity) - (L_intensity + R_intensity),
		to then do velocity += _dt * acceleration,
		and then do intensity += _dt * velocity.*/
	void WaveUpdate_5(HDC hdc);
		/* Two neighbors either side.*/
	void WaveUpdateStep(HDC hdc);
		/* This holds the code common to WaveUpdate and WaveUpdate_5, all
		the stepping stuff.*/
	//first order time derivative
	void Smooth(int, int, int);
	void HeatInt1(int, int, int);
	void HeatInt2(int, int, int, int, int);
	//wrong wave
	void WaveVelInt1(int, int, int);
	void WaveVelInt2(int, int, int, int, int); // five nabe
	//Right wave
	void AltWaveVelInt1(int, int, int);
	void Oscillator(int, int, int);
	void DiverseOscillator(int, int, int);
	void AltOscillatorWave(int, int, int);
	void AltDiverseOscillatorWave(int, int, int);
	//---------------CA-Nonlinear wave UPdate functions-------
	//alternate algorithms for second order time derivatives
	void UlamWave(int, int, int);
	void StableUlamWave(int, int, int);
	void AltUlamWave(int, int, int);
	void CubicUlamWave(int, int, int);
//--------------CA 2D update functions ----------------------
	int index(int x, int y)
		{return (int)y*CX_2D + (int)x;}
	void WaveUpdate2D(HDC hdc);
	void WaveUpdate2D_9(HDC hdc);
	void Wave2D(int, int, int,
		int, int);
	void Heat2D(int, int, int,
		int, int);
	void Smooth2D(int, int, int,
		int, int);
	void WaveUpdateStep2D(HDC hdc);
//--------------CA Network update functions ----------------------
	void NetworkUpdate(HDC hdc);
//-------------------CA DLL Update Methods -----------------------------------
	BOOL LoadUserRule(HWND);
	BOOL LoadUserRule(HWND, char *);
	BOOL GetUserRulePtr(HWND, char *);

	void GetUserRuleName ( char UserRuleName[] );  // Gets User Rule Name
	void SetCAStyleName ( char CA_Style_Name[] );  // Sets CA Style Name
	void GetCAStyleName ( char CA_Style_Name[] );  // Gets CA Style Name

	//DLLRule is a rule like UlamWave, etc., that you use for CA::*UpdateCell
	void DLLRule_1(int); //calls (*pbfnUSERRULE_3)(this,c);
	void DLLRule_3(int, int, int); //calls (*pbfnUSERRULE_3)(this,l,c,r);
	void DLLRule_5(int, int, int, int, int);
	//calls (*pbfnUSERRULE_5)(this,c,e,n,w,s) or (*pbfnUSERRULE_5(this,ll,l,c,r,rr)
	void DLLRule_9(int, int, int, int, int, int, int, int, int); 
	//calls (*pbfnUSERRULE_9)(this,c,e,ne,n,nw,w,sw,s,se);
	//Need a "friend" declaration so the USERRULE module can see CA privates.
	friend DllExport void USERRULE_1(CA*, int);
	friend DllExport void USERRULE_3(CA*, int, int, int);
	friend DllExport void USERRULE_5(CA*, int, int, int, int, int);
	friend DllExport void USERRULE_9(CA*, int, int, int, int, int, int,
		int, int, int);
	friend DllExport void USERINITIALIZE(CA*);
	friend void createUserDialog();

//-------------CA-Evolution Methods-------------------------------------
	void Resetscore() {score = 0.0f;}	// CA's score
	void Prepareforentropy();
		/* This gets freqlookup ready according as to whether
		entropyflag is 0 for off or 1 for on */
	void Changeentropy(Real newentropy);
		/* increment or decrement the target_entropy */
	void Become_child_of(CA *mom, CA *dad);
		/* Adopt mom's states and radius.  Make your lookup be
		a combination of mom's and dad's.  Use the part of mom's
		and part of dad's.  If dad's is shorter, copy his whole into
		the start of mom's lookup.*/
	void Entropy();
		/* Use freqlookup to compute the entropy if the entropyflag is
		on.  This is called everytime that cellcount passes 64K.  The
		fitness is set according to the entropy compared to
		the target_entropy.*/
	void Resetfreq();
		/* set cellcount to zero and zero out the freqlookup table
		if entropyflag is on*/
	void Avoidstripes(); // If the CA has a reoccuring pattern, Seed it.
//----------------CA-Fourier Methods-----------------------
	void Fourier_init(void);
	void SFT(void);						// slow fourier transform
	void FFT(void);						// fast fourier transform
	void SineSFT(int, int, int);
	void CosineSFT(int, int, int);
	void BothSFT(int, int, int);
	void Showpointgraph(HDC);			// show the graph
	int  Screen_yvalue(Real, Real, int, int);// translate to screen y_coord

	void (*pAddUserParam)(CA *, LPSTR, Real); /* For some screwy reason we
		make this a function pointer, but in fact it is always (so far) the
		same as the global AddUserParam method defined in ca.cpp. */

};

//===================================CALIST CLASS====================
class DLLlist;

class CAlist
{
	friend class CA;
private:
//-----------------CAlistBase private--------------------------------
	HWND hwnd; //AUTOSET.  Where I live
	int count; // LOADSAVE. Number of CA objects in the list.
	CA *list[MAX_CAS]; //AUTOSET. Pointer list to the CAs
	CA *focus; //AUTOSET. This is the CA with the box around it.
	WindowBitmap *WBM; /*AUTOSET WindowBitmap. Put pixels here,
     	 then bitblt all to screen*/
	BOOL zoomflag; //LOADSAVE. If this is on, we only show the focus CA.
	int CA_count_per_edge; //AUTOSET. length & height of displayed cells
	int sleep; /* AUTOSET. Used when the window is minimized. If sleep = 1,
		then update and show will not work. */
	int gl_sleep; /* Have this so that I have the optoin of waking up
		the rendering process even when the CA updates are turned off.  Default
		is that this is set to 0 whenever sleep is set to 0, you can
		wake it up from the 3D dialog */
	BOOL _justloadedcells; /*Set this to TRUE when you have just loaded a
		*.CAS file which loads individual cell values, this is to prevent the
		call to smooth which would wipe out the values */
	int _blt_lines;
//--------CAlist Color private----------------------------
	int _numcolor; //AUTOSET
	HPALETTE _hpal; //AUTOSET
	LOGPALETTE *_plgpl; //AUTOSET
//--------CAlist Evolution private----------------------------
	Real mutation_strength; //LOADSAVE. Amount to mutate 0 to 1.
	int best_list[MAX_CAS]; /* AUTOSET.
     	This has all the CA position numbers sorted
		out by the best score. Set by Sortfittness() */
	BOOL mutateflag; // LOADSAVE. Mutate all CAs at breed cycle time?
	BOOL breedflag; 	// LOADSAVE. Enable breed cycle?
	int breedcycle; // LOADSAVE. We breed after this many cycles
	int breedcycle_count; // LOADSAVE. breed cycle counter
	int evolveflag; /* LOADSAVE. Methods to breed. 0, 1, 2 are
		Zap, Clone, and Sex. */
	BOOL stripekillflag; /* LOADSAVE. If 1, then we do a lamndalookup
     	right away on somebody who fails the stripe test, otherwise
			 we add the penalty to his score */
	BOOL stripeseedflag; //LOADSAVE. seed if stripe fail
	// palette stuff added by JC 12/95
//--------CAlist Fourier private----------------------------
	int tp_dowhich; //fourier
public:
//===============CAlist Constructors================
	CAlist(HWND myhwnd, int icount); //constructor
	~CAlist(); //destructor
//===============CAlist Accessors================
	int Count(){return count;} //listlength
	CA *FocusCA(){return focus;}	// This is a pointer to the focus CA
	int get_focus_index();
	CA *GetCA(int i) {return list[i];}  // returns ith CA
	int Getdimension(){return list[0]->Getdimension();}
	int Getshowvelocity(){return FocusCA()->showvelocity;}
	BOOL Getzoomflag() { return zoomflag; }
	int GetSleep(){return sleep;} // Use with IDM_PAUSE in capow.cpp
	int GetGlSleep(){return gl_sleep;} // Use with IDC_OPENGL_GLSLEEP
	BOOL Get_justloadedcells(){return _justloadedcells;}
	int  WidthCADisplayWindow() { return ( focus->Maxx()-focus->Minx() ); }
	int count_per_edge(){return CA_count_per_edge;}
	void GetFocusRect(RECT *rect);
	int blt_lines(){return _blt_lines;}
//---------------CAlist Color Accessors------------------
	int numcolor() { return _numcolor; }
	HPALETTE hpal() { return _hpal; }
	LOGPALETTE * logpal(){return _plgpl;}
//---------------CAlist Evolution Accessors------------------
	int Getbreedingsteps() {return breedcycle;} // Breed after this many cycles
	BOOL Getbreedflag() {return breedflag;}
		// Breed cycle enabled?
	BOOL Getstripekillflag(void) { return stripekillflag; }
	BOOL Getstripeseedflag(void) { return stripeseedflag; }
	Real Getmutation() {return mutation_strength;}
		/* Mutation strength is a percentage 0.0 to 1.0 of how much
		of the lookup you wanted mutated */
	BOOL Getmutateflag() {return mutateflag;}
		// Mutate at Breed cycle?
	int Getevolve(void) {return evolveflag;}
		/* Sets and Gets the ca's evolve flag for use by Cycle
		0 - Zap, 1 - Clone, 2 - Breed */
//===============CAlist Mutators================
	int Changecount(int icount); /* Used by expand and shrink. Changes the
		number of CAs shown on the screen to icount */
	int Bumpcount(int updown); /* Adds or takes away a whole CA_count_per_edge of
		CAs. e.g. 3x3 to 4x4 or vise versa. Updown is 1 or -1 */
	void SetCAType(CA *target, int newtype, BOOL fixflag);
		/* Changes the type of the CA.  If fixflag is TRUE, you
		also call the CA::Full_adjust_params_for_type to make the
		params be nice looking.*/
	/* The first _blt_lines mutator just changes the value, this is to use
	in WM_CREATE only.  The second one assumes you are in mid-career of a
	program run and does some extra work, such as a call to Locate() */
	void initizlize_blt_lines(int linecount){_blt_lines = linecount;}
	void set_blt_lines(int linecount);
	void SetAllType(int newtype, BOOL fixflag);
		/* Changes the type of all the cas, fixflag as with SetCAType*/
	void Setviewmode(int newmode); //Do all in list.
	void Setwrapflag(int newwrapflag);	
	void Set_smoothflag(BOOL onoff);
	void Setshowmode(int newmode);
	void Setgeneratorflag(BOOL onoff);	/* 0 for off, 1 for on */
	void Set_dx_lock(BOOL onoff);
	void Reset_smoothsteps();
	void One_smoothsteps();
	void Setshowvelocity(int newshowvelocity, short focusflag);//new way
	int Setfocus(HDC hdc, CA *newfocus);	/* Change focus CA */
	int Setfocusindex(HDC hdc, int focus_index);	/* Change focus CA */
	void SetWindowBitmap(WindowBitmap *myWBM);	/* Initializes the Bitmap
		pointer for use by calist and each CA gets a copy also*/
	void SetSleep(BOOL onoff){ sleep = onoff;} //Overriding concern is to PAUSE
	void ToggleSleep(){sleep ^= 1;} //Ditto
	void ToggleGlSleep(){gl_sleep ^= 1; /*if(!sleep)gl_sleep = 0;*/}
	/* I used to insist on resetting gl_sleep to 0 whenever I set sleep to 0,
	but this doesn't seem necessary as the program will fully pause in any case
	whenever it loses focus.  The purpose of the sleep vs. gl_sleep distinction
	is that sleep controls the updating of the cells in the CA while gl_sleep
	controls the updating of the VR OpenGL view of a 2D CA as a surface.  If you
	want to freeze some surface configuration and look at it from various angles,
	you want sleep TRUE and gl_sleep FALSE. */
	void Set_justloadedcells(BOOL onoff){_justloadedcells = onoff;}
	void SyncRows(); //Calls this on the members.
	void UpdateGenerationCount();
	void ResetAllGenerationCount();
//---------------CAlist Color Mutators------------------
	void Set_monochromeflag(int onoff);
	void Set_band_count(Real band);
	void Setnumcolor(int newnumcolor);
	void SetDefaultLogPalette();
	void SetRandomLogPalette(COLORREF *_anchor_color, int band_count);
	void SetMonochromeLogPalette();
   void SetColors(); //NEW 3-1-96 DDC
	void Setcolortable();
//------------------ CAlist TweakParam mutator------------------------
	void BumpCATweakParam (int tweakParamType, int updown);
   void SetTweakParam (int tweakParamType, Real val);
//--------------------CAlist Digital mutator-------------------------
	void Changelambda(Real newlambda); // Change all CA's lambda
   void Setlambda (Real newlambda);  // Set newlambda to all CAs
//------------------CAlist Wave mutator-------------------------------
	void Change_band_count(int updown);
	void Change_phase(int updown);
//----------------------CAlist Fourier mutators---------------------
	void Reset_tpgraph();
	int Get_tpdowhich(void) {return tp_dowhich;}
	void  Set_tpdowhich(int new_dowhich) {tp_dowhich = new_dowhich;}
//---------------CAlist Evolution Mutators------------------
	void Setentropy(BOOL onoff); /* 0 for off, 1 for on */
	void Setstripekillflag(BOOL onoff) { stripekillflag = onoff; }
	void Setstripeseedflag(BOOL onoff) { stripeseedflag = onoff; }
	void Setmutation(int updown);
	void Setmutateflag(BOOL imutate) {mutateflag = imutate;}
	void Setbreedflag(BOOL ibreed) {breedflag = ibreed;}
	void Setfailstripe(int updown); // fail stripe penalty
	void Setentropybonus(int updown); // bonus for a close entropy
	void Setbreedcycle(int updown); // When do we Breed? 1 or -1
	void SetEvolve(BOOL newevolve) {evolveflag = newevolve;}
//===============CAlist Methods================
	void Show(HDC hdc, const RECT &ps);
	void Update_and_Show(HDC hdc);  /*Update all active CAs and show the
		updates in a rapid way.*/
	void Locate(int dmaxx, int dmaxy); /*This gnarly method uses count and

		dp to figure out where to tell the list[i] CAs to put their
		minx, maxx, miny, maxy.*/
	void Locate();   //uses the global masterhwnd   //mike 4/97	
	/* The default randomize randomizes everything.  We can also set the randflag
to various thigns, using the RF_? flags.  This is used for autorandomizing and
for the screensaver */
	void Randomize(UINT randflag = RF_START); //Randomizes type_ca, state, radius
	void RandomizeCount(UINT randflag); //Helper function called by Randomize.
	void Touch_CA(int cx, int cy, int sender); /* Impliments users ability to
	touch a CA at location cx,cy*/
	void RandomTouch_CA();
	int Zoom(int inout); /* 1 for in (1 CA), -1 for out (the active CAs).
		When you do Zoom(1), you move focus to the CA you get to see.
		Return a 0 if you're repeating a zoom.*/
 	CA *Getfocus(int cursorx, int cursory);	/* return pointer to the CA
		that is positioned on screen at cursorx,cursory */
	CA *Focus(){return focus;}
	void Boxfocus(HDC hdc, COLORREF color); //Draw a box around focus CA.
	void DrawDivider(HDC hdc);
	void LocateNewGenerator(int cx, int cy, int sender);
//------------CAlist Seed Methods----------------------
	void Seed(); //Default seed with CA::Seed
	void RandomSeed(); //Seed all active CAs.
	void FourierSeed(); //Seed all active CAs.
	void Oneseed();	// Call each CA's oneseed
	void Zeroseed(); //Call each CA's zeroseed
	void Halfmaxseed(); //Call each CA's halfmaxseed
	void Sineseed();	// Call each CA's sineseed
	void BitmapSeed(); //Call each CA's BitmapSeed
//------------CAlist Evolution Methods----------------------
	void Evolve();/* After each breed cycle this is called
		depending what evolveflag is set to this can happen:
		0-zap - mutates everybody
		1-clone - clones the best ones and mutates all
		2-breed - breeds the best ones and mutates all */
	void Sortfitness(); /* This uses the best_list[] array to sort
		all the CAs by their score.  The best_list[] array holds
		CA numbers */
	Real Mutation_strength(){return mutation_strength;} /* How much to
		mutate 0 to 1.0 */
	void Changeentropy(Real newentropy);	// Changes the target_entropy
	void Mutate();	// Mutate each CA by mutation_strength
	void Breed();	// Makes all the CAs a child of the focus and top left
	void Copymutate();	// Copys a mutation of the focus to each CA
	void Resetscores();	// Sets all CA scores to 0
	void CopyCA(CA *ca1, CA *ca2);
		/* Copies ca1 to ca2 */
//------------CAlist Compress Methods----------------------
	int  compress(ifstream &infile,ofstream &outfile);
   int  uncompress(ifstream &infile,ofstream &outfile);
//------------CAlist Experiment Methods----------------------
	void Install_Ramp(int paramcode, Real *rampvalue);
	void CheckRange (int paramcode, Real minmax[]);
//--------CAlist File Methods-------------------------------
	void Saveindividual(char *filename, CA *target); // Save focus CA
	void Saveall(char *filename, BOOL auto_overwrite = FALSE); // Save entire experiment
     BOOL Load_Individual(char* filename, CA *target); // Load on focus CA
     BOOL Loadall_Individual(char* filename); // Load all CA with same *.CA
	BOOL Loadall(char* filename, BOOL startup=FALSE); // Load entire experiment
	// ********** FOR OLD Version of CAPOW **********	
	void Load_Individual7(char* filename, CA *target); // Load on focus CA
	BOOL Loadall7(char* filename, BOOL startup=FALSE); // Load entire experiment

	//If startup then you don't call a WM_SIZE from this.
	//--------------------CAlist Digital methods-------------------------
	void Changeradiusandstates(int newradius = -1, int newstates = -1);
	// if newradius or newstates == -1, then that part is unchanged
	void Lambdalookup(); // Call each CA's lambda lookup.
	void Symmetrize(); // Call each CA's symmetrize
//------------------CAlist Reversible public-------------------------
	void Reverse(); // Call each CA's Reverse
//----------------------CAlist DLL methods-----------------------
	BOOL LoadUserRule(HWND, short& focusflag);
	BOOL LoadUserRule(HWND, char* filename, short& focusflag);
};

#endif CA_HPP
