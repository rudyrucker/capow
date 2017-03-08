#include "ca.hpp" //Include this first, because it includes windows.
#include "carule.hpp" //Include this second
#include <math.h>
/* 2017  To build, open Carule.sln with Visual Studio's Visual C++ 2015.  Make sure you are
building the Release x96 version. Your target *.DLL file
will apear in the DLL subdirectory of the code directory. To change the name of the DLL
output, in Visual C++ 2015 with the Carule.sln open, go to View | Solution Explorer, right click on Carule, select
Properties, and do two things:
(1) Go to Configuration | General | Target Name and change it to, like, MyNewRule
(2) Go to Linker | General, and edit the name in the top Output File line to, like, MyNewRule.DLL
If you only do step (2) the file will still build, but you'll get some warnings.
If this is too much trouble, externally change the name in the output DLL directory after the build.  */

/* Usage

	Define a CA rule using the data names defined in carule.hpp.  It is
handy to keep all of your past rules in this file and simply comment in
exactly one of the #define statements to pick one rule.  A good way to
make a new rule is to look at a similar rule found in this sample code, block
copy that code, and alter it. 
	Sometimes if you keep changing the rules, you get compiler errors like this:
error C2084: function 'void __cdecl USERINITIALIZE(class CA *)' already has a body
You can make this error go away by using the "Rebuild All" option instead of
the "Build" option.  If the message still doesn't go away it means you have
two blocks of code with the same #ifdef label, so two version of your functions
are getting compiled.

	By default the CARule.sln project will build a new rule called
New User Rule.dll  It will be in the dll subdirectory of the source code
directory. When you have the rule working the way you like, you can right click on it
in Windows Explorer and change its name.
	Alternately, you can change the name of your output *.DLL by editing the 
Project Output File name by using the Solution Explorer. Right click on Carule, select
Properties to get the to Project Settings dialog. Change the target file name in two places
(1) Configuration Properties | General | Target.  Put the file name with no .dll extenstion
(2) Configureation Properties | Linker | Output File. Put file name with .dll extension.

	If you try and build a *.DLL while the a *.DLL of the same name has
been selected into a running session of CAPOW, you will get this error message
at link time.
"The DLL you are linking is in use by another application.  The link will be
 aborted."
	To avoid this you must terminate close down the CAPOW session OR use a
different name for the *.DLL you are building.
*/

//#define CARULE_WAVE				//Wave.DLL.  If I stretch the velocity, it's Wave Stretch Velocity.DLL
//#define CARULE_WAVE_SIMPLE		//Wave Unstable.DLL
//#define CARULE_WAVE_AVG_1			//Wave Half.DLL (Not included)
//#define CARULE_WAVE_AVG_2			//Wave Third.DLL (Not included)
//#define CARULE_WAVE_WRAP			//Wave Unstable Wrap.DLL (Not included)
//#define CARULE_OSCILLATOR_WAVE	//Oscillator Wave.DLL (Not included)
//#define CARULE_OSCILLATOR_CHAOTIC	//Oscillator Chaotic.DLL
//#define CARULE_OSCILLATOR_WAVE_CHAOTIC	//Oscillator Wave Chaotic.DLL
//#define CARULE_REACTION_DIFFUSION	//Reaction Diffusion.DLL (Not included)
//#define CARULE_REACTION_WAVE		//Reaction Wave.DLL (Not included)
//#define CARULE_WAVE_QUADRATIC		//Quadratic Wave.DLL (Not included)
//#define CARULE_WAVE_CUBIC			//General Cubic Wave.DLL
//#define CARULE_2D_WAVE_QUADRATIC	//2D Quadratic Wave.DLL
//#define CARULE_2D_WAVE_CUBIC		//2D Cubic Wave.DLL
//#define CARULE_2D_HEAT_9			//2D Heat 9-Neighbor.DLL
//#define CARULE_2D_HEAT_5			//2D Heat 5-Neighbor.DLL (Not included)
//#define CARULE_2D_LIFE				//2D Life.DLL
//#define CARULE_2D_HODGE			//2D Hodge.DLL
//#define CARULE_2D_HODGE_WAVE		//2D Hodge Wave.DLL (Not included)
//#define CARULE_2D_PAIR			//2D Pair.DLL (Not included)
//#define CARULE_2D_ACTIVATOR_INHIBITOR //2D Activator Inhibitor 9.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_MIN //2D Activator Inhibitor Min 9.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_5 //2D Activator Inhibitor 5.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION //2D Activator Inhibitor Saturation 9.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION_5//2D Activator Inhibitor Saturation 5.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_WAVE //2D Activator Inhibitor Wave.DLL 
#define CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_DIFFUSE //2D Activator Wave Inhibitor Diffuse.DLL 
//#define CARULE_2D_AIS_WAVE_DIFFUSE //2D AIS Wave Diffuse.DLL 
//#define CARULE_2D_ACTIVATOR_INHIBITOR_DIFFUSE_WAVE //2D Activator Diffuse Inhibitor Wave.DLL 
//#define CARULE_2D_OSCILLATOR_CHAOTIC    //2D Oscillator Chaotic.DLL
//#define CARULE_2D_OSCILLATOR_WAVE_CHAOTIC    //2D Oscillator Wave Chaotic.DLL
//#define CARULE_2D_OSCILLATOR    //2D Oscillator.DLL
//#define CARULE_2D_OSCILLATOR_WAVE    //2D Oscillator Wave.DLL
//#define CARULE_2D_BOILING_WAVE_5    //2D Boiling Wave 5.DLL 
//#define CARULE_2D_BOILING_WAVE_9    //2D Boiling Wave.DLL 
//#define CARULE_2D_BOILING_WAVE_9_SMOOTH    //2D Boiling Wave Smooth.DLL 
//#define CARULE_1D_INTERPOLATED  //1D Interpolated Continuous Rule.dll
//#define CARULE_ASYMMETRIC_HEAT //1D Asymmetric Heat.dll
//#define CARULE_TWOREGIME_HEAT //1D Two-Regime Heat.dll
//#define CARULE_2D_LOGISTIC//2D Logistic Diffusion ?.dll, where ? is 5 or 9
//#define CARULE__ABRAHAM//2D Logistic Diffusion Abraham ?.dll
	/* This is Ralph Abraham's rule in which he does diffusion based on
	current values.  Makes a nice Zhabo.  I  lost the
	code for the 2D Logistic Diffusion 9Nabe Kaneko.dll, which
	is probably similar. */
//#define CARULE_2D_DOUBLE_LOGISTIC //2D Logistic Double.dll
	/* Can get Zhabo here if you crank up logistic param to 5 instead of just 4.
	Cranking it up means, however, that you will be hitting the max and min. */
//#define CARULE_2D_DOUBLE_LOGISTIC_SMOOTH //2D Logistic Double Smooth.dll
	//This one uses a better averaging algoirthm and tries to avoid slamming
	//the max and min.
//#define CARULE_2D_WINFREE_LOGISTIC //2D Logistic Double Smooth.dll
//#define CARULE_2D_ACTIVATOR_INHIBITOR_BRAIN //2D Brain AI Zhabo.DLL 
//#define CARULE_2D_HODGE_BRAIN //2D Brain Hodge.dll
//#define CARULE_2D_SANDPILE_5 //2D Sandpile 5.DLL
//#define CARULE_2D_SANDPILE_9 //2D Sandpile 9.DLL
//#define CARULE_2D_WAVE_SANDPILE_5	//2D Sandpile Wave 5.DLL
//#define CARULE_2D_CITYFORMATION //2D City Formation 9.DLL
//#define CARULE_2D_CITYFORMATION_5 //2D City Formation 5.DLL
//#define CARULE_2D_CITYFORMATION_5_BADVERSION //2D City Formation Delay 5.DLL
//#define CARULE_2D_FORESTFIRE //2D Forest Fire ?.DLL
		//? can be 9 or 5
//#define CARULE_2D_WINFREE_ZHABO_NEW //2D Winfree Zhabo New.dll

//========================================================================


//========================================================================
#ifdef CARULE_WAVE
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
  	NEW_C_I0 = -PAST_C_I0 + 2.0*C_I0 + LAMBDA *(L_I0 - 2.0*C_I0 + R_I0);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
	NEW_C_V = 100.0*(NEW_C_I0 - C_I0) / DT; /* If you go to velocity view, normally it looks
		flat.  The velocity is too close to zero.  If I randomly multiply by 100.0, then the
		velocity looks okay.*/
	CLAMP(NEW_C_V, -MAX_VELOCITY, MAX_VELOCITY);
}
#endif //CARULE_WAVE
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_SIMPLE
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
}

/* This is a very simple wave rule which is right on the edge of
instability.  If you resize the simulation it is likely to split
into two curves.  We don't actually need to be computing the
NEW_C_V here, but only do this in case you want to display it. */
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 = -PAST_C_I0 + (L_I0 + R_I0);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
	NEW_C_V = (NEW_C_I0 - C_I0)/DT;
}
#endif //CARULE_WAVE_SIMPLE
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_AVG_1
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
}

/* The correct way to do the wave simulation is
  	NEW_C_I0 = -PAST_C_I0 + 2.0*C_I0 +
		((DT*DT)/(DX*DX))*(L_I0 - 2.0*C_I0 + R_I0);
Writing LAMBDA for (DT^2/DX^2), we get
NewC = -PastC + 2*(1 - LAMBDA)C + LAMBDA*(L + R)
Note that if PastC, L,C,R are all the same size S, then the expression is
-S + 2*(1-LAMBDA)*S + 2*LAMBDA*S, or -S + 2*S, or S.  The LAMBDA serves
as a weight.  We can think of it as (1-LAMBDA)(C+C) + LAMBDA(L+R).
If I take LAMBDA bigger than 1, then we get bogus values.  That's why
DT must be less than DX.  If I take, say, LAMBDA to be 1/2 I'd get
-PAST_C_I0 + C_I0 + 0.5*(L_I0 + R_I0);  [CARULE_WAVE_AVG_1]
If I were to take LAMBDA to be 2/3, I'd get
-PAST_C_I0 + (2.0/3.0)*C_I0 + (2.0/3.0)(L_I0 + R_I0); or
-PAST_C_I0 + (2.0/3.0)*(L_I0 + C_I0 + R_I0); [CARULE_WAVE_AVG_2]
We can compile these as CARULE_WAVE_AVG_1 and CARULE_WAVE_AVG_2.
It turns out that version 1 is quite stable, while version 2 is quite
unstable.
*/
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 = -PAST_C_I0 + C_I0 + 0.5*(L_I0 + R_I0);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
	NEW_C_V = (NEW_C_I0 - C_I0)/DT;
}
#endif //CARULE_WAVE_AVG_1
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_AVG_2
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
}

/* See the commment for CARULE_WAVE_AVG_1 */
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 = -PAST_C_I0 + (2.0/3.0)*(L_I0 + C_I0 + R_I0);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
	NEW_C_V = (NEW_C_I0 - C_I0)/DT;
}
#endif //CARULE_WAVE_AVG_2
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_WRAP
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
}

/* This is the same as CARULE_WAVE_UNSTABLE with WRAP instead of CLAMP.
It is cool with random seed.  Cool also if you put a wave
in it and then reduce max intensity with the Analog menu.*/
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 = -PAST_C_I0 + (L_I0 + R_I0);
	WRAP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
	NEW_C_V = (NEW_C_I0 - C_I0)/DT;
}
#endif //CARULE_WAVE_WRAP
//========================================================================

//========================================================================
#ifdef CARULE_OSCILLATOR_CHAOTIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_OSCILLATOR;
// These values from Analog and Electric Dialog seem to give chaotic oscillations.
	owner->_dt.SetVal(0.06);
	owner->_mass.SetVal(0.3);
	owner->_spring_multiplier.SetVal(1000.0);
}

/* One lack in our oscillator rule is that it never seems to give chaotic
oscillations.  This is because choas theory says you need a NONLINEARITY in the rule.
Putting in a sine function should work -- you can
check this with Point View */
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_V =  C_V + (DT/MASS)*(
		- FRICTION * C_V
		- SPRING * sin(C_I0) //Putting in sin here makes it nonlinear and chaotic.
		//-SPRING * C_I0 is what we use in the built-in Oscillator rule.
      + AMPLITUDE * cos(PHASE + FREQUENCY * TIME));
	NEW_C_I0 = C_I0 + DT * NEW_C_V;
	CLAMP(NEW_C_V, -MAX_VELOCITY, MAX_VELOCITY);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_OSCILLATOR_CHAOTIC
//========================================================================

//========================================================================
#ifdef CARULE_OSCILLATOR_WAVE_CHAOTIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_OSCILLATOR;
// These values from Analog and Electric Dialog seem to give chaotic oscillations.
	owner->_dt.SetVal(0.06);
	owner->_mass.SetVal(0.3);
	owner->_spring_multiplier.SetVal(1000.0);
}

/* This is the chatoic oscillator coupled with the wave.  We use a
VEL_LAMBDA which is dt/(dx^2).*/
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_V =  C_V + (DT/MASS)*(
		- FRICTION * C_V
		- SPRING * sin(C_I0) //Putting in sin here makes it nonlinear and chaotic.
		// -SPRING * C_I0 is what we use in the built-in Wave Oscillator rule.
      + AMPLITUDE * cos(PHASE + FREQUENCY * TIME))
	+ VEL_LAMBDA*(L_I0 - 2.0*C_I0 + R_I0); //Wave component
	NEW_C_I0 = C_I0 + DT * NEW_C_V;
	CLAMP(NEW_C_V, -MAX_VELOCITY, MAX_VELOCITY);
	CLAMP(NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_OSCILLATOR_WAVE_CHAOTIC
//========================================================================

//========================================================================
#ifdef CARULE_REACTION_DIFFUSION
/*
This is a one-dimensional reaction-diffusion rule.
C_I0 is the activator
C_I1 is the inhibitor
CELL_PARAM_0 is the cell_strength, making the rate of activator production
vary a little from cell to cell. */

#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val()
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()
#define MAX_ACTIVATOR				owner->userParamAdd[7]->Val()
#define MAX_INHIBITOR				owner->userParamAdd[8]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	double  editValue[] = { 1.0, 0.5, 2.0, 1.0, 0.1, 0.5, 100.0, 100.0 };
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay",
	"Max Activator", "Max Inhibitor"};
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_DIVERSE_OSCILLATOR;
	//use this style so the Variance button in Digital Dialog is active.

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.1, 10.0); 
	owner->userParamAdd[2]->SetRange(0.1, 10.0); 
	owner->userParamAdd[3]->SetRange(0.1, 10.0); 
	owner->userParamAdd[4]->SetRange(0.1, 10.0); 
	owner->userParamAdd[5]->SetRange(0.1, 10.0); 
	owner->userParamAdd[6]->SetRange(0.1, 10.0); 
	owner->userParamAdd[7]->SetRange(1.0, 500.0); 
	owner->userParamAdd[8]->SetRange(1.0, 500.0); 
}

//First kind of shell
DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 =
		(DIFFUSION_RATE_ACTIVATOR*L_I0+C_I0+DIFFUSION_RATE_ACTIVATOR*R_I0)/
			(1+2.0*DIFFUSION_RATE_ACTIVATOR) +
		DT * (CELL_PARAM_0 * ( ACTIVATOR_PRODUCTION)
			- C_I1 - C_I0*DECAY_RATE_ACTIVATOR);
	NEW_C_I1 = 
		(DIFFUSION_RATE_INHIBITOR*L_I2+C_I2+DIFFUSION_RATE_INHIBITOR*R_I2)/
			(1+2.0*DIFFUSION_RATE_INHIBITOR) +
		 DT * (INHIBITOR_PRODUCTION	- C_I0 - C_I1*DECAY_RATE_INHIBITOR);
	CLAMP(NEW_C_I0, 0.0, MAX_ACTIVATOR);
	CLAMP(NEW_C_I1, 0.0, MAX_INHIBITOR);

}
#endif //CARULE_REACTION_DIFFUSION
//========================================================================

//========================================================================
#ifdef CARULE_REACTION_WAVE
/*
This is a one-dimensional reaction-diffusion rule.
C_I0 is the activator
C_I1 is the inhibitor
CELL_PARAM_0 is the cell_strength, making the rate of activator production
vary a little from cell to cell. */

#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val()
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()
#define MAX_ACTIVATOR				owner->userParamAdd[7]->Val()
#define MAX_INHIBITOR				owner->userParamAdd[8]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	double  editValue[] = { 1.0, 0.5, 2.0, 1.0, 0.1, 0.5, 100.0, 100.0 };
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay",
	"Max Activator", "Max Inhibitor"};
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_DIVERSE_OSCILLATOR;
	//use this style so the Variance button in Digital Dialog is active.

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.1, 10.0); 
	owner->userParamAdd[2]->SetRange(0.1, 10.0); 
	owner->userParamAdd[3]->SetRange(0.1, 10.0); 
	owner->userParamAdd[4]->SetRange(0.1, 10.0); 
	owner->userParamAdd[5]->SetRange(0.1, 10.0); 
	owner->userParamAdd[6]->SetRange(0.1, 10.0); 
	owner->userParamAdd[7]->SetRange(1.0, 500.0); 
	owner->userParamAdd[8]->SetRange(1.0, 500.0); 
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	NEW_C_I0 = -PAST_C_I0 + 2.0*C_I0 + LAMBDA *(L_I0 - 2.0*C_I0 + R_I0) +
		DT * ( CELL_PARAM_0 * ( ACTIVATOR_PRODUCTION) - C_I1 -
		C_I0*DECAY_RATE_ACTIVATOR );
	NEW_C_I1 = -PAST_C_I1 + 2.0*C_I1 + LAMBDA *(L_I1 - 2.0*C_I1 + R_I1) +
		 DT * (INHIBITOR_PRODUCTION	- C_I0 - C_I1*DECAY_RATE_INHIBITOR);
	CLAMP(NEW_C_I0, -MAX_ACTIVATOR, MAX_ACTIVATOR);
	CLAMP(NEW_C_I1, -MAX_INHIBITOR, MAX_INHIBITOR);
}
#endif //CARULE_REACTION_WAVE
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_QUADRATIC

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_ULAM_WAVE;
	owner->_max_intensity.SetVal(3.0f);
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	Real rcdiff = R_I0 - C_I0;
	Real cldiff = C_I0 - L_I0;
	NEW_C_I0 = -PAST_C_I0 + 2.0 * C_I0 + LAMBDA*(
		+ rcdiff - cldiff // Same as L_I0 - 2* C_I0 + R_I0  from regular Wave
		+ (NONLINEARITY) *(rcdiff*rcdiff  - cldiff*cldiff));
	CLAMP(NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_WAVE_QUADRATIC
//========================================================================

//========================================================================
#ifdef CARULE_WAVE_CUBIC

#define QUADRATIC_NONLINEARITY	owner->userParamAdd[1]->Val()
#define CUBIC_NONLINEARITY		owner->userParamAdd[2]->Val()

/*Note that we don't use the Nonlinearity control on
the Analog dialog in this rule, instead we put nonlinearity controls in
user dialog*/
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE; 
	double  editValue[] = { 0.0, 5.0};
	char *label[] = { "Quadratic Nonlineariy", "Cubic Nonlinearity"};
	
	owner->_max_intensity.SetVal(1.0f);

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 100.0); //QUADRATIC_NONLINEARITY
	owner->userParamAdd[2]->SetRange(0.0, 100.0); //CUBIC_NONLINEARITY
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	Real rcdiff = R_I0 - C_I0;
	Real cldiff = C_I0 - L_I0;
	NEW_C_I0 = -PAST_C_I0 + 2.0 * C_I0 + LAMBDA*(
		+ rcdiff - cldiff // Same as L_I0 - 2* C_I0 + R_I0  from regular Wave
		+ QUADRATIC_NONLINEARITY*(rcdiff*rcdiff  - cldiff*cldiff)
		+ CUBIC_NONLINEARITY*(rcdiff*rcdiff*rcdiff  - cldiff*cldiff*cldiff));
	CLAMP(NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_WAVE_CUBIC
//========================================================================

//========================================================================
#ifdef CARULE_2D_WAVE_QUADRATIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
	owner->_max_intensity.SetVal(3.0f);
	owner->_nonlinearity1.SetVal(Randomreal(0.05, 0.5));
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	Real ecdiff = PLANE_E_I0 - PLANE_C_I0;
	Real ncdiff = PLANE_N_I0 - PLANE_C_I0;
	Real cwdiff = PLANE_C_I0 - PLANE_W_I0;
	Real csdiff = PLANE_C_I0 - PLANE_S_I0;
	PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
		LAMBDA*( PLANE_FOUR_SUM_I0 / 4.0 - PLANE_C_I0 +
			(NONLINEARITY) *
		(ecdiff*ecdiff  - cwdiff*cwdiff + ncdiff*ncdiff - csdiff*csdiff));
	CLAMP(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);
	//2017 For velocity view, put velocity in owner->wave_target_plane[c].variable[1]
	PLANE_NEW_C_I1 = 
		(PLANE_NEW_C_I0 - PLANE_PAST_C_I0) / DT;
}
#endif //CARULE_2D_WAVE_QUADRATIC
//========================================================================

//========================================================================
#ifdef CARULE_2D_WAVE_CUBIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
	owner->_max_intensity.SetVal(1.0f);
	owner->_nonlinearity1.SetVal(Randomreal(1.0, 15.0));
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	Real ecdiff = PLANE_E_I0 - PLANE_C_I0;
	Real ncdiff = PLANE_N_I0 - PLANE_C_I0;
	Real cwdiff = PLANE_C_I0 - PLANE_W_I0;
	Real csdiff = PLANE_C_I0 - PLANE_S_I0;
	PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
		LAMBDA*( PLANE_FOUR_SUM_I0 / 4.0 - PLANE_C_I0 +
			(NONLINEARITY) *
		(ecdiff*ecdiff*ecdiff - cwdiff*cwdiff*cwdiff +
			ncdiff*ncdiff*ncdiff  - csdiff*csdiff*csdiff));
	CLAMP(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);
	//2017 For velocity view, put velocity in owner->wave_target_plane[c].variable[1], don't bother clamping
	PLANE_NEW_C_I1 =
		(PLANE_NEW_C_I0 - PLANE_PAST_C_I0) / DT;
}
#endif //CARULE_2D_WAVE_CUBIC
//========================================================================

//========================================================================
#ifdef CARULE_2D_HEAT_9
#define INCREMENT owner->userParamAdd[1]->Val()
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 0.5 };
	char *label[] = { "Fixed Heat Increment"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 100.0); 
}

/* A nine neighbor heat rule. */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	PLANE_NEW_C_I0 = (1.0/9.0)*PLANE_NINE_SUM_I0 + INCREMENT;
	WRAP(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);	
}
#endif //CARULE_2D_HEAT_9
//========================================================================

//========================================================================
#ifdef CARULE_2D_HEAT_5
#define INCREMENT owner->userParamAdd[1]->Val()
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 2.0 };
	char *label[] = { "Heat Increment Per Unit Timestep"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 100.0); 
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{ //VEL_LAMBDA is dt/(dx^2)
	PLANE_NEW_C_I0 = (VEL_LAMBDA*(PLANE_FOUR_SUM_I0)+C_I0))/(4*VEL_LAMBDA + 1)
		+ DT*INCREMENT;
	WRAP(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);	
}
#endif //CARULE_2D_HEAT_5
//========================================================================


//========================================================================
#ifdef CARULE_2D_LIFE
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(1.0f);
	owner->_max_intensity.SetVal(1.0f);
	owner->Set_band_count(2);
	//owner-Set_monochromeflag(TRUE);
}

/* The Game of Life! To make this work, use the Analog dialog to cut the
maximum intensity down to 1.0.  Use the minimum number of color bands, 
go to mono, and you'll see white Life on a gray background!*/
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real EightSum = PLANE_EIGHT_SUM_I0;
	if (PLANE_C_I0 == 0.0)
	{
		if(EightSum == 3.0)
			PLANE_NEW_C_I0 = 1.0;
		else
			PLANE_NEW_C_I0 = 0.0;
	}
	else //Assume PLANE_C_I0 is 1.0
	{
		if(EightSum >= 2.0 && EightSum <= 3.0)
			PLANE_NEW_C_I0 = 1.0;
		else
			PLANE_NEW_C_I0 = 0.0;
	}
	CLAMP(PLANE_NEW_C_I0, 0.0, 1.0);	
}
#endif //CARULE_2D_LIFE
//========================================================================

//========================================================================
#ifdef CARULE_2D_HODGE

#define HODGE_TOP		owner->_max_intensity.Val()
#define HODGE_BOTTOM	owner->userParamAdd[1]->Val()
#define HODGE_STIM1		owner->userParamAdd[2]->Val()
#define HODGE_STIM2		owner->userParamAdd[3]->Val()
#define HODGE_INC		owner->userParamAdd[4]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	double  editValue[] = { 0.1, 5.0, 100.0, 5.0 };
	char *label[] = { "Hodge Bottom", "Stim1", "Stim2", "Inc" };
	
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(32.0f);

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.1, 10.0); //HODGE_BOTTOM
	owner->userParamAdd[2]->SetRange(0.1, 1000.0); //HODGE_STIM1
	owner->userParamAdd[3]->SetRange(0.1, 1000.0); //HODGE_STIM2
	owner->userParamAdd[4]->SetRange(0.01, 10.0); //HODGE_INC
	}

/* The hodgepodge rule. This works with a max intensity value of 32*/
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real EightSum = PLANE_EIGHT_SUM_I0;
	if (PLANE_C_I0 <= HODGE_BOTTOM)
	{
		if(EightSum <HODGE_STIM1)
			PLANE_NEW_C_I0 = 0.0;
		else if (EightSum < HODGE_STIM2)
			PLANE_NEW_C_I0 = 2.0;
		else //EightSum >= HODGE_STIM2
			PLANE_NEW_C_I0 = 3.0;
	}
	else if (PLANE_C_I0 < HODGE_TOP)
	{
		PLANE_NEW_C_I0 = EightSum/8.0 + HODGE_INC;
		CLAMP(PLANE_NEW_C_I0, 0, HODGE_TOP);
	}
	else //PLANE_C_I0 is HODGE_TOP
		PLANE_NEW_C_I0 = 0.0;
	//2017 For velocity view, put velocity in owner->wave_target_plane[c].variable[1], don't bother clamping
	PLANE_NEW_C_I1 =
		(PLANE_NEW_C_I0 - PLANE_PAST_C_I0) / DT;
}
#endif //CARULE_2D_HODGE
//========================================================================

//========================================================================
#ifdef CARULE_2D_HODGE_WAVE

#define HODGE_TOP		owner->_max_intensity.Val()
#define HODGE_BOTTOM	owner->userParamAdd[1]->Val()
#define HODGE_STIM1		owner->userParamAdd[2]->Val()
#define HODGE_STIM2		owner->userParamAdd[3]->Val()
#define HODGE_INC		owner->userParamAdd[4]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	double  editValue[] = { 0.1, 5.0, 100.0, 5.0 };
	char *label[] = { "Hodge Bottom", "Stim1", "Stim2", "Inc" };
	
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(32.0f);

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.1, 10.0); //HODGE_BOTTOM
	owner->userParamAdd[2]->SetRange(0.1, 1000.0); //HODGE_STIM1
	owner->userParamAdd[3]->SetRange(0.1, 1000.0); //HODGE_STIM2
	owner->userParamAdd[4]->SetRange(0.01, 10.0); //HODGE_INC
}

/* The hodgepodge rule. This works with a max intensity value of 32*/
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real EightSum = PLANE_EIGHT_SUM_I0;
	if (PLANE_C_I0 <= HODGE_BOTTOM)
	{
		if(EightSum <HODGE_STIM1)
			PLANE_NEW_C_I0 = 0.0;
		else if (EightSum < HODGE_STIM2)
			PLANE_NEW_C_I0 = 2.0;
		else //EightSum >= HODGE_STIM2
			PLANE_NEW_C_I0 = 3.0;
	}
	else if (PLANE_C_I0 < HODGE_TOP)
	{
		PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
			LAMBDA*( PLANE_FOUR_SUM_I0 / 4.0 - PLANE_C_I0) + //wave
			HODGE_INC; 
		CLAMP(PLANE_NEW_C_I0, 0, HODGE_TOP);
	}
	else //PLANE_C_I0 is HODGE_TOP
		PLANE_NEW_C_I0 = 0.0;
}
#endif //CARULE_2D_HODGE_WAVE
//========================================================================

//========================================================================
#ifdef CARULE_2D_PAIR

#define TOTAL	4
#define avg1	owner->userParamAdd[0]->Val() /* I think this is a bug,
	isn't 0 always variance?*/
#define past1	owner->userParamAdd[1]->Val()
#define avg2	owner->userParamAdd[2]->Val()
#define past2	owner->userParamAdd[3]->Val()


DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_HEAT_2D;

	double  editValue[] = { 10.0, 0.4, 20.0, 0.4 };
	char *label[] = { "avg1", "past1", "avg2", "past2" };

	owner->_max_intensity.SetVal(32.0);

	owner->userParamAdd.erase(owner->userParamAdd.begin(),
						      owner->userParamAdd.end());
	for(int i = 0; i < TOTAL; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	Real FourSum0 = PLANE_FOUR_SUM_I0;
	Real FourSum1 = PLANE_FOUR_SUM_I1;

	if (PLANE_C_I0 < avg1 )
			PLANE_NEW_C_I0 = PLANE_C_I0 + 
				(DT * LAMBDA *(FourSum0 + past1 * PLANE_C_I0));
	else
		PLANE_NEW_C_I0 = PLANE_C_I0 +
			(DT * LAMBDA *(FourSum0 - past1 * PLANE_C_I0));
	CLAMP(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);

	if (PLANE_C_I1 < avg2)
		PLANE_NEW_C_I1 = PLANE_C_I1 +
			(DT * LAMBDA * (FourSum1 + past2 * PLANE_C_I1));
	else
		PLANE_NEW_C_I1 = PLANE_C_I1 +
			(DT * LAMBDA * (FourSum1 - past2 * PLANE_C_I1));
	CLAMP(PLANE_NEW_C_I1,-MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_2D_PAIR
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR
/*	This is a two-dimensional activator-inhibitor rule.
C_I0 is the activator substance a and C_I1 is the inhibitor or antagonist b.
The reaction works like A -> 2A + B, autocatalyzed in a nonlinear fashion by
the presence of A, and inhibited by B.  The equations I base this in are 2.1
in Hans Meinhardt "The Algorithmic Beauty of Seashells" (Springer 1995), p. 23.
Meinhardt gives two equations based on the following parameters:
	a	The concentration of the activator substance.
	b	The conentration of the inhibitor substance.
	Da	Diffusion rate of the activator.
	Db	Diffusion rate of the inhibitor.
	ba	The basic spontaneous activator production rate.
	ba	The basic spontaneous activator production rate.
	ra	The activator removal rate.
	ra	The activator removal rate.
	s	The source density, akin to the reaction rate.
	Min_b The mininum value of b.
The equations Meinhard gives are:
	da/dt = s * (a*a/b + ba) - ra*a + Da*d2a/dx2		(Eqn. 2.1.a)
	db/dt = s*a*a + bb -rb*b + Db*d2b/dx2			(Eqn. 2.1.b)
	Looking in his SP.BAS program, which runs this rule for one-dimensional CAs,
	we see that he computes the new values as
	New_a = a - ra*a + Da*(TwoSum_a - 2*a) + s*(a*a/b + ba)
	New_b = b - ra*b + Db*(TwoSum_b - 2*b) + s*a*a + bb
	We write "TwoSum_a" to mean the sum of the a values in the left and right
	neighbors, and "TwoSum_b" has a similar meaning.
Here are some comments on the terms.
	Da, Db.  For stable pattern formation, Db should be at least 7 times as large
		as Da.  The inhibitor, in other words, should disperse rapidly so that
		a local maximum of the activator can be stable.  The inhibitor makes a
		a ring of inhibition around it.  When you have a very broad activated
		region, the inhibitor can't escape fast enough, and there will be a hole
		in the middle of the region caused by the inhibitor killing off the
		activator.  For oscillating Zhabotinsky style reactions, on the other
		hand, it seems good to have Da and Db about the same size, or maybe
		Da only four times as big as Db.  In any case, very low values on the
		order of 0.01 for Da and Db keep the rule from evolving too rapidly and
		jerkily.  More about how we use Da and Db is below in the remark on
		diffusion.
	ba, aa.  Meinhardt says ba is so the system can regenerate itself and insert
		new maxima in blank areas, and he says bb is useful for travelling waves.
	ra, rb.  Note that these terms are multiplied times, respectively, -a and -b.
		The idea here is that this is a proportional die-off rate.  If, eg.,
		you have a population of 100 people, you might get 1 death, but with 
		a population of 1000 people you'd expect 10 deaths.  The idea here is
		that we are thinking of the a and b values as populations of molecules
		within the cell.
	s.       Meinhardt suggests that for stable patterns we set s equal to ra,
		so that the expected value of a is about 1.  That is, if a and b are
		roughly equal and ba is negligible, Eqn. 2.1.a would reduce to
		da/dt = s*a - ra*a + diffusion, so if s = ra, then we get a da/dt 
		consisting only of a diffusion.  For oscillating rules, however,
		we prefer to have s be 1.0.
	Min_b     Since we divide by b in the activator equation, we want to avoid
		dividing by 0.  We do this here by not replacing the divisor term by
		Min_b whenever the inhibitor value b is less than Min_b. Note that if
		Min_b is small, then the reaction gets a big boost whenever the
		inhibitor drops down, as dividing by Min_b is a big multiplication. 
		Note that we do allow b to drop to 0.0, we just don't divide by 0.
Comments on diffusion term Da*d2a/dx2.
		We're going to follow Meinhardt and ignore dt and dx in this rule, 
	essentially assuming that they are unity.
		The most obvious thing to do is to set Da*d2a/dx2 to Da*(EightSum - 8*a),
	where EightSum is the sum of the a values in the eight neighbor cells.  This will
	be numerically unstable if Da is greater than 1/8 or 0.125, as then a positive-a
	cell surrounded by zero-a cells would become negative, so we could enforce Da
	(and Db) to be less than 0.125.
		Actually, it's a bit better to use Da*(FourSum + 0.75*CornerSum - 7*a). 
	The idea is to weight the corner cells a bit less.  I come up with 0.75 because
	it's convenient (because 0.75*4=3) and because the area of a quarter circle
	is PI/4 the area of the enclosing square, adn PI/4 is close to 0.75.  In this
	case we need to clamp Da and Db to be less than 1/7, or 0.14.
		In converting Meinhardt's 1-D rules to our 2-D rules, we can use all of his
	parameters the same except for his diffusion parameters.  Our raw 2D difference
	term is roughly 7 times the difference between two cells, while in 1D the
	raw difference is 2 times the difference.  So in translating from Meinhardt,
	we multiply his diffusion paramters by a factor of 2/7.  Normally the rule
	is not highly sensitive to the  diffusion value so I in fact simply convert
	Meinhardt's diffusion values by multiplying by a factor of 1/4.
*/

#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.0020, 0.1,	//activator and inhibitor diffusion rates.
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = { "Activator Diffusion/7", "Inhibitor Diffusion/7",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.14);	//Da (max is 1/7)
	owner->userParamAdd[2]->SetRange(0.0, 0.14);	//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.000001, 10.0);	//Min_b
}


DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	//We copy PLANE_C_I1 to a nonzero "inhibitor" so we can divide by it.
	double inhibitor = PLANE_C_I1;
	if (inhibitor < MIN_INHIBITOR)
		inhibitor = MIN_INHIBITOR;
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0 - 7.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibitor)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1+ 0.75*PLANE_CORNER_SUM_I1 - 7.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay

	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_MIN
/*	This is a variation on CARULE_2D_ACTIVATOR_INHIBITOR which clamps
the inhibitor to be larger than Min_b.  This seems physically incorrect, but
this rule happens to have given a very nice Zhabotinsky called
"2D AI Zhabo Lace.CA", so I keep it around. That rule doesn't work well with 
the "correct" CARULE_2D_ACTIVATOR_INHIBITOR schema.*/

#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4a, found on page 26 of his book and stored on
his accompanying disk in the file SP24a.PRM */
	double  editValue[] = {
	0.00025, 0.1,	//activator and inhibitor diffusion rates.
	0.05, 0.0,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = { "Activator Diffusion/7", "Inhibitor Diffusion/7",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_max_intensity.SetVal(32.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0001, 0.14);	//Da (max is 1/7)
	owner->userParamAdd[2]->SetRange(0.0001, 0.4);	//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.0001, 10.0);	//Min_b
}


DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	/* MIN_INHIBITOR can't be 0 because we are going to divide by it.  If we
		just seeded, there's a chance it is 0.0 here, so we fix that. */
	if (PLANE_C_I1 < MIN_INHIBITOR)
		PLANE_C_I1 = MIN_INHIBITOR;
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0 - 7.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / PLANE_C_I1)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1+ 0.75*PLANE_CORNER_SUM_I1 - 7.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay

	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, MIN_INHIBITOR, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_MIN
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_5
/*	This is a two-dimensional activator-inhibitor rule with 5 neighbors,
instead of with nine, like CARULE_2D_ACTIVATOR_INHIBITOR just above.
C_I0 is the activator substance a and C_I1 is the inhibitor or antagonist b.
The reaction works like A -> 2A + B, autocatalyzed in a nonlinear fashion by
the presence of A, and inhibited by B.  The equations I base this in are 2.1
in Hans Meinhardt "The Algorithmic Beauty of Seashells" (Springer 1995), p. 23.
Meinhardt gives two equations based on the following parameters:
	a	The concentration of the activator substance.
	b	The conentration of the inhibitor substance.
	Da	Diffusion rate of the activator.
	Db	Diffusion rate of the inhibitor.
	ba	The basic spontaneous activator production rate.
	ba	The basic spontaneous activator production rate.
	ra	The activator removal rate.
	ra	The activator removal rate.
	s	The source density, akin to the reaction rate.
	Min_b The mininum value of b.
The equations Meinhard gives are:
	da/dt = s * (a*a/b + ba) - ra*a + Da*d2a/dx2		(Eqn. 2.1.a)
	db/dt = s*a*a + bb -rb*b + Db*d2b/dx2			(Eqn. 2.1.b)
	Looking in his SP.BAS program, which runs this rule for one-dimensional CAs,
	we see that he computes the new values as
	New_a = a - ra*a + Da*(TwoSum_a - 2*a) + s*(a*a/b + ba)
	New_b = b - ra*b + Db*(TwoSum_b - 2*b) + s*a*a + bb
	We write "TwoSum_a" to mean the sum of the a values in the left and right
	neighbors, and "TwoSum_b" has a similar meaning.
Here are some comments on the terms.
	Da, Db.  For stable pattern formation, Db should be at least 7 times as large
		as Da.  The inhibitor, in other words, should disperse rapidly so that
		a local maximum of the activator can be stable.  The inhibitor makes a
		a ring of inhibition around it.  When you have a very broad activated
		region, the inhibitor can't escape fast enough, and there will be a hole
		in the middle of the region caused by the inhibitor killing off the
		activator.  For oscillating Zhabotinsky style reactions, on the other
		hand, it seems good to have Da and Db about the same size, or maybe
		Da only four times as big as Db.  In any case, very low values on the
		order of 0.01 for Da and Db keep the rule from evolving too rapidly and
		jerkily.  More about how we use Da and Db is below in the remark on
		diffusion.
	ba, aa.  Meinhardt says ba is so the system can regenerate itself and insert
		new maxima in blank areas, and he says bb is useful for travelling waves.
	ra, rb.  Note that these terms are multiplied times, respectively, -a and -b.
		The idea here is that this is a proportional die-off rate.  If, eg.,
		you have a population of 100 people, you might get 1 death, but with 
		a population of 1000 people you'd expect 10 deaths.  The idea here is
		that we are thinking of the a and b values as populations of molecules
		within the cell.
	s.       Meinhardt suggests that for stable patterns we set s equal to ra,
		so that the expected value of a is about 1.  That is, if a and b are
		roughly equal and ba is negligible, Eqn. 2.1.a would reduce to
		da/dt = s*a - ra*a + diffusion, so if s = ra, then we get a da/dt 
		consisting only of a diffusion.  For oscillating rules, however,
		we prefer to have s be 1.0.
	Min_b     Since we divide by b in the activator equation, we want to avoid
		dividing by 0.  We do this here by not replacing the divisor term by
		Min_b whenever the inhibitor value b is less than Min_b. Note that if
		Min_b is small, then the reaction gets a big boost whenever the
		inhibitor drops down, as dividing by Min_b is a big multiplication. 
		Note that we do allow b to drop to 0.0, we just don't divide by 0.
Comments on diffusion term Da*d2a/dx2.
		We're going to follow Meinhardt and ignore dt and dx in this rule, 
	essentially assuming that they are unity.
		The most obvious thing to do is to set Da*d2a/dx2 to Da*(FourSum - 4*a),
	where FourSum is the sum of the a values in the four neighbor cells.  This will
	be numerically unstable if Da is greater than 1/4 or 0.25, as then a positive-a
	cell surrounded by zero-a cells would become negative, so we could enforce Da
	(and Db) to be less than 0.25.
		In converting Meinhardt's 1-D rules to our 2-D rules, we can use all of his
	parameters the same except for his diffusion parameters.  Our raw 2D difference
	term is roughly 4 times the difference between two cells, while in 1D the
	raw difference is 2 times the difference.  So in translating from Meinhardt,
	we multiply his diffusion paramters by a factor of 1/2. 
*/

#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.0040, 0.2,	//activator and inhibitor diffusion rates.
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = { "Activator Diffusion/4", "Inhibitor Diffusion/4",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.25);	//Da (max is 1/4)
	owner->userParamAdd[2]->SetRange(0.0, 0.25);	//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.0001, 10.0);	//Min_b
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	//We copy PLANE_C_I1 to a nonzero "inhibitor" so we can divide by it.
	double inhibitor = PLANE_C_I1;
	if (inhibitor < MIN_INHIBITOR)
		inhibitor = MIN_INHIBITOR;
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 - 4.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibitor)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1 - 4.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay

	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_5
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION 
/*	This is a two-dimensional activator-inhibitor rule with saturation of
autocatalysis.  This is the same as CARULE_2D_ACTIVATOR_INHIBITOR above,
with a change based on Equation 2.3 in Hans Meinhardt 
"The Algorithmic Beauty of Seashells" (Springer 1995), p. 25.
The new parameter is sa, a saturation parameter.  Here is a complete list of
our parameters:
	a	The concentration of the activator substance.
	b	The conentration of the inhibitor substance.
	Da	Diffusion rate of the activator.
	Db	Diffusion rate of the inhibitor.
	ba	The basic spontaneous activator production rate.
	ba	The basic spontaneous activator production rate.
	ra	The activator removal rate.
	ra	The activator removal rate.
	s	The source density, akin to the reaction rate.
	sb	The Michaelis-Menten term to prevent division by 0 inhibitor.
     sa	The activator saturation parameter.
	
The equations Meinhardt gives are:
	da/dt = s * (a*a/((sb+b)*(1 + sa*a*a)) + ba) - ra*a + Da*d2a/dx2	
		(Eqn. 2.3 with Michaelis-Menten modification from p. 42))
	db/dt = s*a*a + bb -rb*b + Db*d2b/dx2			(Eqn. 2.1.b)
	Looking in his SP.BAS program, which runs this rule for one-dimensional CAs,
	we see that he computes the new values as
	New_a = a - ra*a + Da*(TwoSum_a - 2*a) +
		s * (a * a / (sb + b) / (1 + sa * a * a) + ba)
	New_b = b - ra*b + Db*(TwoSum_b - 2*b) + s*a*a + bb
	We write "TwoSum_a" to mean the sum of the a values in the left and right
	neighbors, and "TwoSum_b" has a similar meaning.
Here are some comments on the terms.
	Da, Db.  For stable pattern formation, Db should be at least 7 times as large
		as Da.  The inhibitor, in other words, should disperse rapidly so that
		a local maximum of the activator can be stable.  The inhibitor makes a
		a ring of inhibition around it.  When you have a very broad activated
		region, the inhibitor can't escape fast enough, and there will be a hole
		in the middle of the region caused by the inhibitor killing off the
		activator.  For oscillating Zhabotinsky style reactions, on the other
		hand, it seems good to have Da and Db about the same size, or maybe
		Da only four times as big as Db.  In any case, very low values on the
		order of 0.01 for Da and Db keep the rule from evolving too rapidly and
		jerkily.  More about how we use Da and Db is below in the remark on
		diffusion.
	ba, aa.  Meinhardt says ba is so the system can regenerate itself and insert
		new maxima in blank areas, and he says bb is useful for travelling waves.
	ra, rb.  Note that these terms are multiplied times, respectively, -a and -b.
		The idea here is that this is a proportional die-off rate.  If, eg.,
		you have a population of 100 people, you might get 1 death, but with 
		a population of 1000 people you'd expect 10 deaths.  The idea here is
		that we are thinking of the a and b values as populations of molecules
		within the cell.
	s.       Meinhardt suggests that for stable patterns we set s equal to ra,
		so that the expected value of a is about 1.  That is, if a and b are
		roughly equal and ba is negligible, Eqn. 2.1.a would reduce to
		da/dt = s*a - ra*a + diffusion, so if s = ra, then we get a da/dt 
		consisting only of a diffusion.  For oscillating rules, however,
		we prefer to have s be 1.0.
	sb		Since we divide by b, we don't want to divide by 0.  We'll
		avoid this by always adding sb to the b term in the denominator.
		This correction is called the "Michaelis-Menten term for finite
		activator production at low inhibitor concentration," and is 
		introduced in Meinhardt, p. 42.
	sa      The saturation of activator production parameter.  This is supposed to
		allows larger larger regions of activator to form because the activator
		can only get up to a certain value.  The rule is VERY sensitive to this,
		a value of 0.00001 is often plenty.
Comments on diffusion term Da*d2a/dx2.
		We're going to follow Meinhardt and ignore dt and dx in this rule, 
	essentially assuming that they are unity.
		The most obvious thing to do is to set Da*d2a/dx2 to Da*(EightSum - 8*a),
	where EightSum is the sum of the a values in the eight neighbor cells.  This will
	be numerically unstable if Da is greater than 1/8 or 0.125, as then a positive-a
	cell surrounded by zero-a cells would become negative, so we could enforce Da
	(and Db) to be less than 0.125.
		Actually, it's a bit better to use Da*(FourSum + 0.75*CornerSum - 7*a). 
	The idea is to weight the corner cells a bit less.  I come up with 0.75 because
	it's convenient (because 0.75*4=3) and because the area of a quarter circle
	is PI/4 the area of the enclosing square, adn PI/4 is close to 0.75.  In this
	case we need to clamp Da and Db to be less than 1/7, or 0.14.
		In converting Meinhardt's 1-D rules to our 2-D rules, we can use all of his
	parameters the same except for his diffusion parameters.  Our raw 2D difference
	term is roughly 7 times the difference between two cells, while in 1D the
	raw difference is 2 times the difference.  So in translating from Meinhardt,
	we multiply his diffusion paramters by a factor of 2/7.  Normally the rule
	is not highly sensitive to the  diffusion value so I in fact simply convert
	Meinhardt's diffusion values by multiplying by a factor of 1/4.
*/

#define USERPARAM_COUNT 9 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Minb
#define SATURATION				owner->userParamAdd[9]->Val() //sa

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {  //Params from Meinhardt Sp24a.PRM, book 2.4d, p. 26
	0.00025, 0.1,	//activator and inhibitor diffusion rates.
	0.05, 0.00,	//activator and inhibitor production rates.
	0.01, .015,		//activator and inhibitor decay rates.
	0.01,			//source density or reaction density rate.
	0.001,			//minimum inhibitor value
	0.00001};		//saturation parameter 
	char *label[] = { "Activator Diffusion/7", "Inhibitor Diffusion/7",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Low Inhibitor Term", "Activator Saturation"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.14286);		//Da (max is 1/7)
	owner->userParamAdd[2]->SetRange(0.0, 0.14286);		//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.0001, 10.0);	//sb
	owner->userParamAdd[9]->SetRange(0.0, 10.0);		//sa
}


DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0 - 7.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*							//Source Density times...
		  (	ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared /					//Autocatalysis over
				((MIN_INHIBITOR+PLANE_C_I1)*			//Inhibition times
				  (1 + SATURATION*Plane_C_I0_squared))	//Saturation
		  )		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1+ 0.75*PLANE_CORNER_SUM_I1 - 7.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay

	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION_5
/*	This is a two-dimensional activator-inhibitor rule with saturation of
autocatalysis.  This is the same as CARULE_2D_ACTIVATOR_INHIBITOR above,
with a change based on Equation 2.3 in Hans Meinhardt 
"The Algorithmic Beauty of Seashells" (Springer 1995), p. 25.
The new parameter is sa, a saturation parameter.  Here is a complete list of
our parameters:
	a	The concentration of the activator substance.
	b	The conentration of the inhibitor substance.
	Da	Diffusion rate of the activator.
	Db	Diffusion rate of the inhibitor.
	ba	The basic spontaneous activator production rate.
	ba	The basic spontaneous activator production rate.
	ra	The activator removal rate.
	ra	The activator removal rate.
	s	The source density, akin to the reaction rate.
	sb	The Michaelis-Menten term to prevent division by 0 inhibitor.
     sa	The activator saturation parameter.
	
The equations Meinhardt gives are:
	da/dt = s * (a*a/((sb+b)*(1 + sa*a*a)) + ba) - ra*a + Da*d2a/dx2	
		(Eqn. 2.3 with Michaelis-Menten modification from p. 42))
	db/dt = s*a*a + bb -rb*b + Db*d2b/dx2			(Eqn. 2.1.b)
	Looking in his SP.BAS program, which runs this rule for one-dimensional CAs,
	we see that he computes the new values as
	New_a = a - ra*a + Da*(TwoSum_a - 2*a) +
		s * (a * a / (sb + b) / (1 + sa * a * a) + ba)
	New_b = b - ra*b + Db*(TwoSum_b - 2*b) + s*a*a + bb
	We write "TwoSum_a" to mean the sum of the a values in the left and right
	neighbors, and "TwoSum_b" has a similar meaning.
Here are some comments on the terms.
	Da, Db.  For stable pattern formation, Db should be at least 7 times as large
		as Da.  The inhibitor, in other words, should disperse rapidly so that
		a local maximum of the activator can be stable.  The inhibitor makes a
		a ring of inhibition around it.  When you have a very broad activated
		region, the inhibitor can't escape fast enough, and there will be a hole
		in the middle of the region caused by the inhibitor killing off the
		activator.  For oscillating Zhabotinsky style reactions, on the other
		hand, it seems good to have Da and Db about the same size, or maybe
		Da only four times as big as Db.  In any case, very low values on the
		order of 0.01 for Da and Db keep the rule from evolving too rapidly and
		jerkily.  More about how we use Da and Db is below in the remark on
		diffusion.
	ba, aa.  Meinhardt says ba is so the system can regenerate itself and insert
		new maxima in blank areas, and he says bb is useful for travelling waves.
	ra, rb.  Note that these terms are multiplied times, respectively, -a and -b.
		The idea here is that this is a proportional die-off rate.  If, eg.,
		you have a population of 100 people, you might get 1 death, but with 
		a population of 1000 people you'd expect 10 deaths.  The idea here is
		that we are thinking of the a and b values as populations of molecules
		within the cell.
	s.       Meinhardt suggests that for stable patterns we set s equal to ra,
		so that the expected value of a is about 1.  That is, if a and b are
		roughly equal and ba is negligible, Eqn. 2.1.a would reduce to
		da/dt = s*a - ra*a + diffusion, so if s = ra, then we get a da/dt 
		consisting only of a diffusion.  For oscillating rules, however,
		we prefer to have s be 1.0.
	sb		Since we divide by b, we don't want to divide by 0.  We'll
		avoid this by always adding sb to the b term in the denominator.
		This correction is called the "Michaelis-Menten term for finite
		activator production at low inhibitor concentration," and is 
		introduced in Meinhardt, p. 42.
	sa      The saturation of activator production parameter.  This is supposed to
		allows larger larger regions of activator to form because the activator
		can only get up to a certain value.
Comments on diffusion term Da*d2a/dx2.
		We're going to follow Meinhardt and ignore dt and dx in this rule, 
		essentially assuming that they are unity.
		The most obvious thing to do is to set Da*d2a/dx2 to Da*(EightSum - 8*a),
	where EightSum is the sum of the a values in the eight neighbor cells.  This will
	be numerically unstable if Da is greater than 1/8 or 0.125, as then a positive-a
	cell surrounded by zero-a cells would become negative, so we could enforce Da
	(and Db) to be less than 0.125.
		Actually, it's a bit better to use Da*(FourSum + 0.75*CornerSum - 7*a). 
	The idea is to weight the corner cells a bit less.  I come up with 0.75 because
	it's convenient (because 0.75*4=3) and because the area of a quarter circle
	is PI/4 the area of the enclosing square, adn PI/4 is close to 0.75.  In this
	case we need to clamp Da and Db to be less than 1/7, or 0.14.
		In converting Meinhardt's 1-D rules to our 2-D rules, we can use all of his
	parameters the same except for his diffusion parameters.  Our raw 2D difference
	term is roughly 7 times the difference between two cells, while in 1D the
	raw difference is 2 times the difference.  So in translating from Meinhardt,
	we multiply his diffusion paramters by a factor of 2/7.  Normally the rule
	is not highly sensitive to the  diffusion value so I in fact simply convert
	Meinhardt's diffusion values by multiplying by a factor of 1/4.
*/

#define USERPARAM_COUNT 9 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Minb
#define SATURATION				owner->userParamAdd[9]->Val() //sa

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {  //Params from Meinhardt Sp24a.PRM, book 2.4d, p. 26
	0.0005, 0.2,	//activator and inhibitor diffusion rates.
	0.05, 0.00,	//activator and inhibitor production rates.
	0.01, .015,		//activator and inhibitor decay rates.
	0.01,			//source density or reaction density rate.
	0.001,			//minimum inhibitor value
	0.00001};		//saturation parameter
	char *label[] = { "Activator Diffusion/4", "Inhibitor Diffusion/4",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Low Inhibitor Term", "Activator Saturation"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.25);		//Da (max is 1/4)
	owner->userParamAdd[2]->SetRange(0.0, 0.25);		//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.0000001, 10.0);	//sb
	owner->userParamAdd[9]->SetRange(0.0, 10.0);		//sa
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 - 4.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*							//Source Density times...
		  (	ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared /					//Autocatalysis over
				((MIN_INHIBITOR+PLANE_C_I1)*			//Inhibition times
				  (1 + SATURATION*Plane_C_I0_squared))	//Saturation
		  )		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1 - 4.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay

	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_SATURATION_5
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_WAVE
/*	This is a two-dimensional activator-inhibitor rule where I use the wave
equation in place of diffusion for both activator and inhibitor, just to see what
happens.  I get selforganizing seething that looks a little like the dynamics 
of water vapor clouds.  See CARULE_2D_ACTIVATOR_INHIBITOR_5 rule for notes on
the parameters.
*/

#define USERPARAM_COUNT 6 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[1]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[2]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[3]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[4]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[5]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[6]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4a, found on page 26 of his book and stored on
his accompanying disk in the file SP24a.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = {
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", 
		"Source Density", "Minimum Inhibitor"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[2]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[6]->SetRange(0.0001, 10.0);	//Min_b
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	/* MIN_INHIBITOR can't be 0 because we are going to divide by it.  If we
		just seeded, there's a chance it is 0.0 here, so we fix that. */
//#define INHIBPLUS
#ifdef INHIBPLUS
	double inhibition = fabs(PLANE_C_I1);
	if (inhibition < MIN_INHIBITOR)
		inhibition = MIN_INHIBITOR;
#else //not INHIBPLUS
	double inhibition = PLANE_C_I1;
	if (-MIN_INHIBITOR < inhibition && inhibition < MIN_INHIBITOR)
	{
		if (inhibition < 0)
			inhibition = -MIN_INHIBITOR;
		else
			inhibition = MIN_INHIBITOR;
	}
#endif //INHIBPLUS
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibition)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0 +			//Decay
		LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0); //DT^2 * Wave Accel.
		
	PLANE_NEW_C_I1 = 2*PLANE_C_I1 - PLANE_PAST_C_I1 + //C + DT * Velocity
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1 +	//Decay
		LAMBDA * (PLANE_FOUR_SUM_I1/4.0 - PLANE_C_I1); //DT^2 * Wave Accel.

	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, -MAX_INTENSITY, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_WAVE
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_DIFFUSE
/* 
	This is a two-dimensional activator-inhibitor rule where I use the wave
equation in place of diffusion for the activator but use diffusion for the
inhibitor.  See CARULE_2D_ACTIVATOR_INHIBITOR_5 rule for notes on
the parameters.
*/
#define USERPARAM_COUNT 7 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[1]->Val() //Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[2]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[3]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[4]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[5]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[6]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[7]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.2,			// inhibitor diffusion rate.
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = { "Inhibitor Diffusion/4",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.25);	//Db
	owner->userParamAdd[2]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[7]->SetRange(0.0001, 10.0);	//Min_b
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	/* MIN_INHIBITOR can't be 0 because we are going to divide by it.  If we
		just seeded, there's a chance it is 0.0 here, so we fix that. */
//#define INHIBPLUS
#ifdef INHIBPLUS
	double inhibition = fabs(PLANE_C_I1);
	if (inhibition < MIN_INHIBITOR)
		inhibition = MIN_INHIBITOR;
#else //not INHIBPLUS /* Don't let inhibition be in the interval (-MIN_INHIBITOR, MIN_INHIBITOR)
	double inhibition = PLANE_C_I1;
	if (-MIN_INHIBITOR < inhibition && inhibition < MIN_INHIBITOR)
	{
		if (inhibition < 0)
			inhibition = -MIN_INHIBITOR;
		else
			inhibition = MIN_INHIBITOR;
	}
#endif //INHIBPLUS
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do wave or diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibition)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0 +			//Decay
		LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0); //DT^2 * Wave Accel.

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1 - 4.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay
	/* 2017.  I notice that in one CA example I like with this rule, called 2D AIS Slammer.ca, the
	activator and inhibitor are repeatedly 	slamming up against the MAX_INTENSITY value and getting 
	clamped.  So I tried tripling that number 	to see what happens. And the rule just shoots up
	and dies. Ditto for 1.5 time MAX_INTENSITY. So it's kind of a finely tuned rule.*/
	CLAMP(PLANE_NEW_C_I0, MAX_INTENSITY, MAX_INTENSITY);
	CLAMP(PLANE_NEW_C_I1, MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_WAVE_DIFFUSE
//========================================================================
//========================================================================
#ifdef CARULE_2D_AIS_WAVE_DIFFUSE
/* 
	This is a two-dimensional activator-inhibitor-saturation rule where I use the wave
equation in place of diffusion for the activator but use diffusion for the
inhibitor.  See CARULE_2D_ACTIVATOR_INHIBITOR_5 rule for notes on
the parameters.
*/
#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[1]->Val() //Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[2]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[3]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[4]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[5]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[6]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[7]->Val()	//Min_b
#define SATURATION			owner->userParamAdd[8]->Val()	//aSaturation, bSaturation

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.2,			// inhibitor diffusion rate.
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.015,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001,		//minimum inhibitor value
	0.04};		//saturation value
	char *label[] = { "Inhibitor Diffusion/4",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor", "Inhibitor Saturation"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.25);	//Db
	owner->userParamAdd[2]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[7]->SetRange(0.0001, 10.0);	//Min_b
	owner->userParamAdd[8]->SetRange(0.0, 10.0);	//Saturation a, b
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	/* MIN_INHIBITOR can't be 0 because we are going to divide by it.  If we
		just seeded, there's a chance it is 0.0 here, so we fix that. In addtion,
	we are going to scale as if the zero of the activation and inhibition is 
	really -MAX_INTENSITY, and we'll let the wave oscillate around what's essentially
	the halfway point. */

  double activation = PLANE_C_I0 + MAX_INTENSITY;
	double inhibition = PLANE_C_I1;
	if (inhibition < MIN_INHIBITOR)
		inhibition = MIN_INHIBITOR;
  double signed_activation_squared = PLANE_C_I0 * fabs(PLANE_C_I0);
  double abs_activation_squared = fabs(signed_activation_squared);
	
//Do wave or diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 //C + DT * Velocity
		+ LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0) //DT^2 * Wave Accel.
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  signed_activation_squared / (inhibition* 	//The Reaction
			 (1 + SATURATION*abs_activation_squared)))	//Saturation
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = 2*PLANE_C_I1 - PLANE_PAST_C_I1 //C + DT * Velocity
		+ LAMBDA * (PLANE_FOUR_SUM_I1/4.0 - PLANE_C_I1) //DT^2 * Wave Accel.
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(abs_activation_squared)		//The Reaction
			 /(1 + SATURATION*(PLANE_C_I1*PLANE_C_I1))	//Saturation
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;		//Decay

	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0, MAX_INTENSITY);	
}
#endif //CARULE_2D_AIS_WAVE_DIFFUSE
//========================================================================

//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_DIFFUSE_WAVE
/* 
	This is a two-dimensional activator-inhibitor rule where I use diffusion for
the activator and the wave equation in place of diffusion for the 
inhibitor.  See CARULE_2D_ACTIVATOR_INHIBITOR_5 rule for notes on
the parameters.
*/
#define USERPARAM_COUNT 7 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[2]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[3]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[4]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[5]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[6]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[7]->Val()	//Min_b

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.05,			// activator diffusion rate.
	0.01, 0.0055,	//activator and inhibitor production rates.
	0.01, 0.025,	//activator and inhibitor decay rates.
	0.01,		//source density or reaction density rate.
	0.001};		//minimum inhibitor value
	char *label[] = { "Activator Diffusion/4",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_max_intensity.SetVal(4.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.25);		//Da
	owner->userParamAdd[2]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[7]->SetRange(0.0001, 10.0);	//Min_b
}


DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	/* MIN_INHIBITOR can't be 0 because we are going to divide by it.  If we
		just seeded, there's a chance it is 0.0 here, so we fix that. */
#define INHIBPLUS
#ifdef INHIBPLUS
	double inhibition = fabs(PLANE_C_I1);
	if (inhibition < MIN_INHIBITOR)
		inhibition = MIN_INHIBITOR;
#else //not INHIBPLUS
	double inhibition = PLANE_C_I1;
	if (-MIN_INHIBITOR < inhibition && inhibition < MIN_INHIBITOR)
	{
		if (inhibition < 0)
			inhibition = -MIN_INHIBITOR;
		else
			inhibition = MIN_INHIBITOR;
	}
#endif //INHIBPLUS
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do wave or diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0						//The activator update
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse		
			(PLANE_FOUR_SUM_I0 - 4.0*PLANE_C_I0)
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibition)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = 2*PLANE_C_I1 - PLANE_PAST_C_I1 + //C + DT * Velocity
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay
		LAMBDA * (PLANE_FOUR_SUM_I1/4.0 - PLANE_C_I1); //DT^2 * Wave Accel.

	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, -MAX_INTENSITY, MAX_INTENSITY);	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_DIFFUSE_WAVE
//========================================================================

//========================================================================
#ifdef CARULE_2D_OSCILLATOR_CHAOTIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
// These values from Analog and Electric Dialog seem to give chaotic oscillations.
	owner->_dt.SetVal(0.06);
	owner->_mass.SetVal(0.3);
	owner->_spring_multiplier.SetVal(1000.0);
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{ 
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		ACCEL_LAMBDA*(
			- (FRICTION * (PLANE_C_I0 - PLANE_PAST_C_I0)/DT) //Friction * Velocity
			- SPRING * sin(PLANE_C_I0) //Use sin to make nonlinear and chaotic.
			//- SPRING * PLANE_C_I0 //Or have it linear and nonchaotic.
			+ AMPLITUDE * cos(PHASE + FREQUENCY * TIME));
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}

#endif //CARULE_2D_OSCILLATOR_CHAOTIC
//========================================================================

//========================================================================
#ifdef CARULE_2D_OSCILLATOR_WAVE_CHAOTIC
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
// These values from Analog and Electric Dialog seem to give chaotic oscillations.
//	owner->_dt.SetVal(0.06);
//	owner->_mass.SetVal(0.3);
//	owner->_spring_multiplier.SetVal(1000.0);
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{ 
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		ACCEL_LAMBDA*(
			- (FRICTION * (PLANE_C_I0 - PLANE_PAST_C_I0)/DT) //Friction * Velocity
			- SPRING * sin(PLANE_C_I0) //Use sin to make nonlinear and chaotic.
			//- SPRING * PLANE_C_I0 //Or have it linear and nonchaotic.
			+ AMPLITUDE * cos(PHASE + FREQUENCY * TIME)) +
		LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0); //DT^2 * Wave Accel.
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_2D_OSCILLATOR_WAVE_CHAOTIC
//========================================================================



//========================================================================
#ifdef CARULE_2D_OSCILLATOR_WAVE

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
}

/* Instead of using velocity, we can avoid it.  The principle is this:
Normally a 2D Wave motion is
(1) NewC = -P + 2C + dt^2/dx^2 (FOURSUM/4 - C);
[And 1D is exactly the same with (L+R - C) as the last term.]
I had been handling a force F by donig an acceleration of F/mass.
(2) NewV = V + dt*F + dt/dx^2 (FOURSUM/4 - C);
(3) NewC = C + dt*NewV.
But in 2D I don't have a velocity field in the cell that I can count on as
being initialized to 0.  So I've been faking it using the variable[1] field.
Which sometimes has garbage.  But I don't really need V.  Instead I can replace
V by (C - P)/dt in (2), and then substitute the new V-less expression for NewV
in equation (3), getting
(4) NewC = 2*C - P + (dt^2/mass) F + dt^2/dx^2(FOURSUM/4 - C)
or
(4) PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + ACCEL_LAMBDA*F +
		LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0);

 */
DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{ 
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		ACCEL_LAMBDA*(
			- (FRICTION * (PLANE_C_I0 - PLANE_PAST_C_I0)/DT) //Friction * Velocity
			//- SPRING * sin(PLANE_C_I0) //Use sin to make nonlinear and chaotic.
			- SPRING * PLANE_C_I0 //Or have it linear and nonchaotic.
			+ AMPLITUDE * cos(PHASE + FREQUENCY * TIME)) +
		LAMBDA * (PLANE_FOUR_SUM_I0/4.0 - PLANE_C_I0); //DT^2 * Wave Accel.
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}
#endif //CARULE_2D_OSCILLATOR_WAVE
//========================================================================



//========================================================================
#ifdef CARULE_2D_OSCILLATOR
DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
// These values from Analog and Electric Dialog seem to give chaotic oscillations.
//	owner->_dt.SetVal(0.06);
//	owner->_mass.SetVal(0.3);
//	owner->_spring_multiplier.SetVal(5.0);
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{ 
	PLANE_NEW_C_I0 = 2*PLANE_C_I0 - PLANE_PAST_C_I0 + //C + DT * Velocity
		ACCEL_LAMBDA*(
			- (FRICTION * (PLANE_C_I0 - PLANE_PAST_C_I0)/DT) //Friction * Velocity
			//- SPRING * sin(PLANE_C_I0) //Use sin to make nonlinear and chaotic.
			- SPRING * PLANE_C_I0 //Or have it linear and nonchaotic.
			+ AMPLITUDE * cos(PHASE + FREQUENCY * TIME));
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}

#endif //CARULE_2D_OSCILLATOR
//========================================================================

//========================================================================
#ifdef CARULE_2D_BOILING_WAVE_5

#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define WAVE_PARAM					owner->userParamAdd[1]->Val()	
#define MAX_NONLINEARITY			owner->userParamAdd[2]->Val()	
#define NONLINEARITY_GROW_FACTOR	owner->userParamAdd[3]->Val() 
#define MIN_NONLINEARITY			owner->userParamAdd[4]->Val()	
#define WAVE_THRESHOLD				owner->userParamAdd[5]->Val()	

/* The idea is that we need a recovery after a boiling eruption, so we go to
diffusion rule until the nonlinearity factor gets pumped up again.  WAVE_THRESHOLD
should be MIN_NONLINEARITY * (NONLINEARITY_GROW_FACTOR)^N for the N steps of 
diffusion you want. These params should be in a user dialog.*/

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {  
	0.5, //WAVE_PARAM
	1000.0, //MAX_NONLINEARITY
	1.1,		//NONLINEARITY_GROW_FACTOR.
	0.001,	//MIN_NONLINEARITY.
	0.01};	//WAVE_THRESHOLD
/* The form of the equation in my Santa Fe paper has 2*Wave as the param multiplied
times the "averaging" term, so I use that as a label.*/
	char *label[] = { "2*Wave Param", "Max Nonlinearity",
		"Nonlinearity GrowFactor", "MinNonlinearity", "Wave Threshold"};

	owner->_max_intensity.SetVal(1.0f); //We scale both variables 0 to 1.
	owner->_nonlinearity1.SetVal(1000.0f); //NONLINEARITY param
		//which shows up on the Analog dialog, even though I'm really going to use
		//my own MAX_NONLINEARITY.

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);

	owner->userParamAdd[1]->SetRange(0.0, 4.0);	
	owner->userParamAdd[2]->SetRange(0.0, 10000.0);	
	owner->userParamAdd[3]->SetRange(1.0, 10.0);	
	owner->userParamAdd[4]->SetRange(0.00001, 10.0);	
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		
}
/* A boiling wave blended with a heat rule.   */
DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	Real ecdiff = PLANE_E_I0 - PLANE_C_I0;
	Real ncdiff = PLANE_N_I0 - PLANE_C_I0;
	Real cwdiff = PLANE_C_I0 - PLANE_W_I0;
	Real csdiff = PLANE_C_I0 - PLANE_S_I0;
	
	if (PLANE_C_I1 <= WAVE_THRESHOLD) //Use this to Smooth for awhile
		PLANE_NEW_C_I0 = PLANE_FIVE_SUM_I0 / 5.0;
	else
		PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
			WAVE_PARAM *( PLANE_FOUR_SUM_I0 /4.0 - PLANE_C_I0 +
					((PLANE_C_I1) * MAX_NONLINEARITY)* //The Local Nonlinearity 
						(ecdiff*ecdiff*ecdiff - cwdiff*cwdiff*cwdiff +
						ncdiff*ncdiff*ncdiff  - csdiff*csdiff*csdiff));

/* I use the second plane of variables as my local nonlinearity parameters.  If
a cell hits the max value I kill off all the nonlinearity at this cell for the
next generation.  I used to do this for the cell's neighors as well, but that 
maybe makes the cells update-order dependent and hurts the CA's parallelism?*/
	if (RealClampAndTell(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY))
		PLANE_NEW_C_I1 = MIN_NONLINEARITY;
/* If you're not bouncing off the ceiling, increase nonlinearity.  This keeps
things lively.  Also here we go ahead and copy to the next generation.  This
means that if you a calmdown step at the next gneration the new value will go
to the third member of the buffer. */
	else 
	{
		PLANE_C_I1 *= NONLINEARITY_GROW_FACTOR;
		CLAMP(PLANE_C_I1, MIN_NONLINEARITY, 1.0);
		PLANE_NEW_C_I1 = PLANE_C_I1;
	}
}
#endif //CARULE_2D_BOILING_WAVE_5
//========================================================================

//========================================================================
#ifdef CARULE_2D_BOILING_WAVE_9

#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define WAVE_PARAM					owner->userParamAdd[1]->Val()	
#define MAX_NONLINEARITY			owner->userParamAdd[2]->Val()	
#define NONLINEARITY_GROW_FACTOR	owner->userParamAdd[3]->Val() 
#define MIN_NONLINEARITY			owner->userParamAdd[4]->Val()	
#define WAVE_THRESHOLD				owner->userParamAdd[5]->Val()	

/* The idea is that we need a recovery after a boiling eruption, so we go to
diffusion rule until the nonlinearity factor gets pumped up again.  WAVE_THRESHOLD
should be MIN_NONLINEARITY * (NONLINEARITY_GROW_FACTOR)^N for the N steps of 
diffusion you want. These params should be in a user dialog.*/

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {  
	0.5, //WAVE_PARAM
	1000.0, //MAX_NONLINEARITY
	1.1,		//NONLINEARITY_GROW_FACTOR.
	0.001,	//MIN_NONLINEARITY.
	0.01};	//WAVE_THRESHOLD
/* The form of the equation in my Santa Fe paper has 2*Wave as the param multiplied
times the "averaging" term, so I use that as a label.*/
	char *label[] = { "2*Wave Param", "Max Nonlinearity",
		"Nonlinearity GrowFactor", "MinNonlinearity", "Wave Threshold"};

	owner->_max_intensity.SetVal(1.0f); //We scale both variables 0 to 1.
	owner->_nonlinearity1.SetVal(1000.0f); //NONLINEARITY param
		//which shows up on the Analog dialog, even though I'm really going to use
		//my own MAX_NONLINEARITY.

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);

	owner->userParamAdd[1]->SetRange(0.0, 4.0);	
	owner->userParamAdd[2]->SetRange(0.0, 10000.0);	
	owner->userParamAdd[3]->SetRange(1.0, 10.0);	
	owner->userParamAdd[4]->SetRange(0.00001, 10.0);	
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		
}
/* A boiling wave blended with a 9 neighbor heat rule.  wave only needs 5 nabes,
but if we do a 5 nabe heat for smoothing, we get checkerboards */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real ecdiff = PLANE_E_I0 - PLANE_C_I0;
	Real ncdiff = PLANE_N_I0 - PLANE_C_I0;
	Real cwdiff = PLANE_C_I0 - PLANE_W_I0;
	Real csdiff = PLANE_C_I0 - PLANE_S_I0;
	
	if (PLANE_C_I1 <= WAVE_THRESHOLD) //Use this to Smooth for awhile
		PLANE_NEW_C_I0 =
			(PLANE_C_I0 + PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0) / 8.0;
	else
		PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
			WAVE_PARAM *(
				(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0)/7.0 - PLANE_C_I0 +
					((PLANE_C_I1) * MAX_NONLINEARITY)* //The Local Nonlinearity 
						(ecdiff*ecdiff*ecdiff - cwdiff*cwdiff*cwdiff +
						ncdiff*ncdiff*ncdiff  - csdiff*csdiff*csdiff));

/* I use the second plane of variables as my local nonlinearity parameters.  If
a cell hits the max value I kill off all the nonlinearity at this cell for the
next generation.  I used to do this for the cell's neighors as well, but that 
maybe makes the cells update-order dependent and hurts the CA's parallelism?*/
	if (RealClampAndTell(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY))
		PLANE_NEW_C_I1 = MIN_NONLINEARITY;
/* If you're not bouncing off the ceiling, increase nonlinearity.  This keeps
things lively.  Also here we go ahead and copy to the next generation.  This
means that if you a calmdown step at the next gneration the new value will go
to the third member of the buffer. */
	else 
	{
		PLANE_C_I1 *= NONLINEARITY_GROW_FACTOR;
		CLAMP(PLANE_C_I1, MIN_NONLINEARITY, 1.0);
		PLANE_NEW_C_I1 = PLANE_C_I1;
	}
}
#endif //CARULE_2D_BOILING_WAVE_9
//========================================================================

//========================================================================
#ifdef CARULE_2D_BOILING_WAVE_9_SMOOTH

#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define WAVE_PARAM					owner->userParamAdd[1]->Val()	
#define MAX_NONLINEARITY			owner->userParamAdd[2]->Val()	
#define NONLINEARITY_GROW_FACTOR	owner->userParamAdd[3]->Val() 
#define MIN_NONLINEARITY			owner->userParamAdd[4]->Val()	
#define WAVE_THRESHOLD				owner->userParamAdd[5]->Val()	

/* The idea is that we need a recovery after a boiling eruption, so we go to
diffusion rule until the nonlinearity factor gets pumped up again.  WAVE_THRESHOLD
should be MIN_NONLINEARITY * (NONLINEARITY_GROW_FACTOR)^N for the N steps of 
diffusion you want. These params should be in a user dialog.*/

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {  
	0.5, //WAVE_PARAM
	1000.0, //MAX_NONLINEARITY
	1.1,		//NONLINEARITY_GROW_FACTOR.
	0.001,	//MIN_NONLINEARITY.
	0.01};	//WAVE_THRESHOLD
/* The form of the equation in my Santa Fe paper has 2*Wave as the param multiplied
times the "averaging" term, so I use that as a label.*/
	char *label[] = { "2*Wave Param", "Max Nonlinearity",
		"Nonlinearity GrowFactor", "MinNonlinearity", "Wave Threshold"};

	owner->_max_intensity.SetVal(1.0f); //We scale both variables 0 to 1.
	owner->_nonlinearity1.SetVal(1000.0f); //NONLINEARITY param
		//which shows up on the Analog dialog, even though I'm really going to use
		//my own MAX_NONLINEARITY.

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);

	owner->userParamAdd[1]->SetRange(0.0, 4.0);	
	owner->userParamAdd[2]->SetRange(0.0, 10000.0);	
	owner->userParamAdd[3]->SetRange(1.0, 10.0);	
	owner->userParamAdd[4]->SetRange(0.00001, 10.0);	
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		
}
/* A boiling wave blended with a 9 neighbor heat rule.  wave only needs 5 nabes,
but if we do a 5 nabe heat for smoothing, we get checkerboards */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real ecdiff = PLANE_E_I0 - PLANE_C_I0;
	Real ncdiff = PLANE_N_I0 - PLANE_C_I0;
	Real cwdiff = PLANE_C_I0 - PLANE_W_I0;
	Real csdiff = PLANE_C_I0 - PLANE_S_I0;
	
	if (PLANE_C_I1 <= WAVE_THRESHOLD) //Use this to Smooth for awhile
		PLANE_NEW_C_I0 =
			(PLANE_C_I0 + PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0) / 8.0;
	else
		PLANE_NEW_C_I0 = -PLANE_PAST_C_I0 + 2.0 * PLANE_C_I0 + 
			WAVE_PARAM *(
				(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0)/7.0 - PLANE_C_I0 +
					((PLANE_C_I1) * MAX_NONLINEARITY)* //The Local Nonlinearity 
						(ecdiff*ecdiff*ecdiff - cwdiff*cwdiff*cwdiff +
						ncdiff*ncdiff*ncdiff  - csdiff*csdiff*csdiff));
/* Average and increase nonlinearity.   */
	PLANE_NEW_C_I1 =
			(PLANE_C_I1 + PLANE_FOUR_SUM_I1 + 0.75*PLANE_CORNER_SUM_I1) / 8.0;
	PLANE_NEW_C_I1 *= NONLINEARITY_GROW_FACTOR;
	CLAMP(PLANE_C_I1, MIN_NONLINEARITY, 1.0);
/* I use the second plane of variables as my local nonlinearity parameters.  If
a cell hits the max value I kill off all the nonlinearity at this cell for the
next generation.  I used to do this for the cell's neighors as well, but that 
maybe makes the cells update-order dependent and hurts the CA's parallelism?*/
	if (RealClampAndTell(PLANE_NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY))
		PLANE_NEW_C_I1 = MIN_NONLINEARITY;
}
#endif //CARULE_2D_BOILING_WAVE_9_SMOOTH
//========================================================================

//========================================================================
#ifdef CARULE_1D_INTERPOLATED
/* This rule is based on the notion that we might try extending the standard 2-state
radius-1 rules to contiuous valued rules.  The idea is to view a continous valued update
neighborhood (L, C, R) as being a point in a cube with the canonical discrete neighborhoods
at the vertices.  We set the value at (L, C, R) to be an appropriately weighted average
of the update values for the digital corners.  Our weighting is the 3D analog of how you
set f(x) = (1.0 - x)*f(0.0) + x*f(1.0).  The weight of each vertex is the region "away" from
the vertex, you can think of a point in a cube as cutting the cube in eight pieces (if you
regard the point as the origin of an xyz-axis system).  And the weight of the vertex is the
volume of the diagonally opposite piece of the cube.
 Unfortunately these rule seem to  wash out quickly, there's too much averaging.*/  


#define USERPARAM_COUNT 1 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_WAVE;
	owner->_max_intensity.SetVal(1.0f); //We scale variable 0 to 1.

	double  editValue[] = {110}; //We'll use this as rulecode in the USERRULE.
	char *label[] = { "Discrete Rule Code (1-255)"};
	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(1.0, 255.0);	
}

//-----------Names for use by Interpolated CAs.
#define pL L_I0
#define pC C_I0
#define pR R_I0
#define nL (1.0-L_I0)
#define nC (1.0-C_I0)
#define nR (1.0-R_I0)

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	int rulecode = (int)(owner->userParamAdd[1]->Val()); //Initially is 110.
/* Note that the sum of the multipliers is one cubed, assuming that MAX_INTENSITY is 1.0 */
  	NEW_C_I0 = 0.0;
	NEW_C_I0 += nL*nC*nR*((rulecode>>0)&1);
	NEW_C_I0 += nL*nC*pR*((rulecode>>1)&1);
	NEW_C_I0 += nL*pC*nR*((rulecode>>2)&1);
	NEW_C_I0 += nL*pC*pR*((rulecode>>3)&1);
	NEW_C_I0 += pL*nC*nR*((rulecode>>4)&1);
	NEW_C_I0 += pL*nC*pR*((rulecode>>5)&1);
	NEW_C_I0 += pL*pC*nR*((rulecode>>6)&1);
	NEW_C_I0 += pL*pC*pR*((rulecode>>7)&1);

	CLAMP(NEW_C_I0, 0.0, MAX_INTENSITY);
	NEW_C_V = (NEW_C_I0 - C_I0)/DT;
}
#endif //CARULE_1D_INTERPOLATED
//========================================================================
 

//========================================================================
#ifdef CARULE_ASYMMETRIC_HEAT
/* */  


#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define MULTIPLIER		owner->userParamAdd[1]->Val()
#define INCREMENT		owner->userParamAdd[2]->Val()
#define LEFT_WEIGHT		owner->userParamAdd[3]->Val()	
#define CENTER_WEIGHT	owner->userParamAdd[4]->Val() 
#define RIGHT_WEIGHT	owner->userParamAdd[5]->Val()	

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_HEATWAVE;
	owner->_max_intensity.SetVal(1.0f); //We scale variable 0 to 1.

	double  editValue[] = {1.0, 0.01, 0.33, 0.33, 0.33};
	char *label[] = { "Multiply the Weighted Average By:", "Increment the Product By:",
		"Weight of Left Cell", "Weight of Center Cell", "Weight of Right Cell"};
	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(1.0, 2.0);	//Multiplier
	owner->userParamAdd[2]->SetRange(0.0, 2.0); //Increment	
	owner->userParamAdd[3]->SetRange(0.0001, 1.0);	//L Weight
	owner->userParamAdd[4]->SetRange(0.0001, 1.0);	//C Weight
	owner->userParamAdd[5]->SetRange(0.0001, 1.0);	//R Weight
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	Real multiplier = MULTIPLIER /(LEFT_WEIGHT + CENTER_WEIGHT + RIGHT_WEIGHT);
	NEW_C_I0 = multiplier*(LEFT_WEIGHT*L_I0 + CENTER_WEIGHT*C_I0 + RIGHT_WEIGHT*R_I0) +
		INCREMENT;
	WRAP(NEW_C_I0,0.0, MAX_INTENSITY);	
	NEW_C_V = ((C_I0 - (L_I0 + R_I0)/2.0)); /* Use the velocity field to show the difference
		between a cell and its neighbors, this will suppress the horizontal stripes pattern.*/
	CLAMP(NEW_C_V, -MAX_VELOCITY, MAX_VELOCITY); //Can scale MAX_VELOCITY to show this better
}
#endif //CARULE_ASYMMETRIC_HEAT


//========================================================================
#ifdef CARULE_TWOREGIME_HEAT
/* */  


#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define LO_MULTIPLIER		owner->userParamAdd[1]->Val()
#define LO_INCREMENT		owner->userParamAdd[2]->Val()
#define LO_LEFT_WEIGHT		owner->userParamAdd[3]->Val()	
#define LO_RIGHT_WEIGHT		owner->userParamAdd[4]->Val()	
#define HI_MULTIPLIER		owner->userParamAdd[5]->Val()
#define HI_INCREMENT		owner->userParamAdd[6]->Val()
#define HI_LEFT_WEIGHT		owner->userParamAdd[7]->Val()	
#define HI_RIGHT_WEIGHT		owner->userParamAdd[8]->Val()	

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 3;
	owner->_usercastyle = CA_HEATWAVE;
	owner->_max_intensity.SetVal(10.0f); //We scale variable 0 to 10 for good visibility

	double  editValue[] = {
		1.02,  0.01, 2.0, 1.0,  //Lo Mult, Inc, LWt., RWt.
		1.02, -0.01, 1.0, 2.0};   //Hi Mult, Inc, LWt., RWt.
	char *label[] = {
		"(Lo) Multiplier", "(Lo) Increment", "(Lo) Left Weight", "(Lo) Right Weight", 
		"(Hi) Multiplier", "(Hi) Increment", "(Hi) Left Weight", "(Hi) Right Weight", 
		};
	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	/*Note that the 0th one you added is the 1th one in the array, as
		by default the variance is always in place 0. */
	owner->userParamAdd[1]->SetRange(0.9, 1.5);	//Lo Multiplier
	owner->userParamAdd[2]->SetRange(-1.0, 1.0); //Lo Increment	
	owner->userParamAdd[3]->SetRange(0, 2.0);	//Lo L Weight
	owner->userParamAdd[4]->SetRange(0, 2.0);	//Lo R Weight

	owner->userParamAdd[5]->SetRange(0.9, 1.5);	//Hi Multiplier
	owner->userParamAdd[6]->SetRange(-1.0, 1.0); //Hi Increment	
	owner->userParamAdd[7]->SetRange(0.0, 2.0);	//Hi L Weight
	owner->userParamAdd[8]->SetRange(0.0, 2.0);	//Hi R Weight
	for(i = 0; i < USERPARAM_COUNT; i++)
		(owner->userParamAdd[i+1]->SetVal(editValue[i]));
			 /* Need to do this again as the constructor might not have 
			accepted the edit value becuase the range wasn't set yet to
			include it.  Note also that the userParamAdd index is one 
			higher than the editValue index. */
	owner->userParamAdd[0]->SetVal(0.05);  //Fix Variance
}

DllExport void USERRULE_3(CA* owner, int l, int c, int r)
{
	Real weightedaverage;
	if (C_I0 <= 0.0)
	{
		weightedaverage = (LO_LEFT_WEIGHT * L_I0 + C_I0 + LO_RIGHT_WEIGHT * R_I0) /
			(LO_LEFT_WEIGHT + 1.0 + LO_RIGHT_WEIGHT);
		NEW_C_I0 = ( LO_MULTIPLIER * weightedaverage ) + LO_INCREMENT;
	}
	else
	{
		 weightedaverage = (HI_LEFT_WEIGHT * L_I0 + C_I0 + HI_RIGHT_WEIGHT * R_I0) /
			(HI_LEFT_WEIGHT + 1.0 + HI_RIGHT_WEIGHT);
		NEW_C_I0 = ( HI_MULTIPLIER * weightedaverage ) + HI_INCREMENT;
	}

	WRAP(NEW_C_I0,-MAX_INTENSITY, MAX_INTENSITY);	
	NEW_C_V = C_I0 - ((L_I0 + C_I0 + R_I0)/3.0); /* Use the velocity field to show the difference
		between a cell and its neighbors, this can suppress the horizontal stripes pattern.*/
	CLAMP(NEW_C_V, -MAX_VELOCITY, MAX_VELOCITY); //Can scale MAX_VELOCITY to show this better
}
#endif //CARULE_TWOREGIME_HEAT
//========================================================================

#ifdef CARULE_2D_LOGISTIC
#define USERPARAM_COUNT 3 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define LOGISTICFACTOR		owner->userParamAdd[1]->Val()
#define GROWTHRATE		owner->userParamAdd[2]->Val()
#define DIFFUSIONRATE		owner->userParamAdd[3]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(1.0f); 

	double  editValue[] = {3.4, 1.0, 0.5}; 
		//LOGISTICFACTOR, GROWTHRATE, DIFFUSIONRATE
	char *label[] = {"Logistic Factor", "Growth Timestep", "Diffusion Rate"};
	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	/*Note that the 0th one you added is the 1th one in the array, as
		by default the variance is always in place 0. */
	owner->userParamAdd[1]->SetRange(1.0, 4.0);	//Logistic Growth Factor
	owner->userParamAdd[2]->SetRange(0.0, 1.0); //Growth Rate
	owner->userParamAdd[3]->SetRange(0.0, 1.0); //Diffusion Rate
	for(i = 0; i < USERPARAM_COUNT; i++)
		(owner->userParamAdd[i+1]->SetVal(editValue[i]));
			 /* Need to do this again as the constructor might not have 
			accepted the edit value becuase the range wasn't set yet to
			include it.  Note also that the userParamAdd index is one 
			higher than the editValue index. */
	owner->userParamAdd[0]->SetVal(0.2);  //Fix Variance
}
/* A nine neighbor logistic diffusion rule. */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real average;
#define NABE5
#ifdef NABE5
		//Store the present difference from neighbors' average for viewing
	average = (1.0/4.0)*PLANE_FOUR_SUM_I0;
#else //9 nabe
	average = (1.0/8.0)*PLANE_EIGHT_SUM_I0;
#endif //NABE5
	PLANE_NEW_C_I1 = fabs(PLANE_C_I0 - average); //Save diff for viewing
		//Do diffusion 
	PLANE_NEW_C_I0 = (1.0 - DIFFUSIONRATE)*PLANE_C_I0 + DIFFUSIONRATE*average;
		//Do logistic growth, but moderate its speed to be GROWTHRATE
	Real targetvalue = LOGISTICFACTOR * PLANE_NEW_C_I0 * (1.0 - PLANE_NEW_C_I0);
	PLANE_NEW_C_I0 = (1.0 - GROWTHRATE)*PLANE_NEW_C_I0 + GROWTHRATE*targetvalue;
	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
}

#endif //CARULE_2D_LOGISTIC

//========================================================================

#ifdef CARULE_2D_LOGISTIC_ABRAHAM//2D Logistic Diffusion Abraham ?.dll
//? can be 5 0r 9 depneding on ABRAHAM5 define below.
//Note that I lost the code for the 2D Logistic Diffusion 9Nabe Kaneko.dll
#define USERPARAM_COUNT 2 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define LOGISTICFACTOR		owner->userParamAdd[1]->Val()
#define DIFFUSIONRATE		owner->userParamAdd[2]->Val()

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(1.0f); 

	double  editValue[] = {2.1, 0.8}; 
		//LOGISTICFACTOR,  DIFFUSIONRATE
	char *label[] = {"Logistic Factor (1.0 to 4.0)", 
		"Diffusion Rate (0.0 to 1.0)"};
	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	/*Note that the 0th one you added is the 1th one in the array, as
		by default the variance is always in place 0. */
	owner->userParamAdd[1]->SetRange(1.0, 4.0);	//Logistic Growth Factor
	owner->userParamAdd[2]->SetRange(0.0, 1.0); //Diffusion Rate
	for(i = 0; i < USERPARAM_COUNT; i++)
		(owner->userParamAdd[i+1]->SetVal(editValue[i]));
			 /* Need to do this again as the constructor might not have 
			accepted the edit value becuase the range wasn't set yet to
			include it.  Note also that the userParamAdd index is one 
			higher than the editValue index. */
	owner->userParamAdd[0]->SetVal(0.2);  //Fix Variance
}

/* A nine neighbor logistic diffusion rule. */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	/*This is the method that Abraham used.  Calculate diffusion
	on basis of current values, calculate logistic on basis of
	current values and THEN combine.*/

//Compute a logisticoutput term
	Real logisticoutput = LOGISTICFACTOR * PLANE_C_I0 * (1.0 - PLANE_C_I0);

//Compute a diffusion term
	Real average;
#define ABRAHAM5 //Comment this in if I want 5nabe style rule.
	//5 nabe seems to work better.
#ifndef ABRAHAM5 //Do a 9 neighbor rule, look at 8 surrounding ones.
	average = (1.0/8.0)*PLANE_EIGHT_SUM_I0;
#else //Do a 5 neighbor average, look at 4 surrounding ones.
	average = (1.0/4.0)*PLANE_FOUR_SUM_I0;
#endif //ABRAHAM5
	//Compute difference between current val and nabe avg
	Real diffusion = average - PLANE_C_I0;
	//While you're at it, save the differnence for viewing. 
	PLANE_NEW_C_I1 = fabs(diffusion); 
	//Scale the difference down by your diffusion factor
	diffusion *= DIFFUSIONRATE;

//Combine diffusion and logisticoutput.
	PLANE_NEW_C_I0 = logisticoutput + diffusion;
	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
}

#endif //CARULE_2D_LOGISTIC_ABRAHAM

//============================================
#ifdef CARULE_2D_DOUBLE_LOGISTIC
/*	This is a variation on CARULE_2D_LOGISTIC, but using two species.*/

#define USERPARAM_COUNT 4 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define ACTIVATOR_DIFFUSIONRATE	owner->userParamAdd[1]->Val() //Da
#define INHIBITOR_DIFFUSIONRATE	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_GROWTH		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_GROWTH		owner->userParamAdd[4]->Val()	//bb

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4a, found on page 26 of his book and stored on
his accompanying disk in the file SP24a.PRM */
	double  editValue[] = {
	0.5, 0.7,	//activator and inhibitor diffusion rates.
	5.0, 7.0};	//activator and inhibitor growth rates.
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		"Activator Growth", "Inhibitor Growth"};

	owner->_max_intensity.SetVal(1.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0);	
	owner->userParamAdd[2]->SetRange(0.0, 1.0);
	owner->userParamAdd[3]->SetRange(0.0, 15.0);
	owner->userParamAdd[4]->SetRange(0.0, 15.0);
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real activator_avg = (1.0/8.0)*PLANE_EIGHT_SUM_I0;
	Real inhibitor_avg = (1.0/8.0)*PLANE_EIGHT_SUM_I1;
		//Do diffusion on both
	activator_avg = (1.0 - ACTIVATOR_DIFFUSIONRATE)*PLANE_C_I0 +
			ACTIVATOR_DIFFUSIONRATE*activator_avg;
	inhibitor_avg = (1.0 - INHIBITOR_DIFFUSIONRATE)*PLANE_C_I1 +
			INHIBITOR_DIFFUSIONRATE*inhibitor_avg;
		//Do logistic growth on both, but have inhibitor inhibit the activator
		//and have the activator activate the inhibitor.
	PLANE_NEW_C_I0 = ACTIVATOR_GROWTH * (1.0 - inhibitor_avg) * //Inhibition
		activator_avg * (1.0 - activator_avg); //Logistic 
	PLANE_NEW_C_I1 = 0.1 + INHIBITOR_GROWTH * activator_avg * //Activation
		inhibitor_avg * (1.0 - inhibitor_avg); //Logistic 
	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}
#endif //CARULE_2D_DOUBLE_LOGISTIC
//========================================================================
//============================================
#ifdef CARULE_2D_DOUBLE_LOGISTIC_SMOOTH
/*	This is a variation on CARULE_2D_LOGISTIC, but using two species.*/

#define USERPARAM_COUNT 4 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define ACTIVATOR_DIFFUSIONRATE	owner->userParamAdd[1]->Val() //Da
#define INHIBITOR_DIFFUSIONRATE	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_GROWTH		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_GROWTH		owner->userParamAdd[4]->Val()	//bb

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4a, found on page 26 of his book and stored on
his accompanying disk in the file SP24a.PRM */
	double  editValue[] = {
	0.5, 0.7,	//activator and inhibitor diffusion rates.
	5.0, 7.0};	//activator and inhibitor growth rates.
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		"Activator Growth", "Inhibitor Growth"};

	owner->_max_intensity.SetVal(1.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0);	
	owner->userParamAdd[2]->SetRange(0.0, 1.0);
	owner->userParamAdd[3]->SetRange(0.0, 10.0);
	owner->userParamAdd[4]->SetRange(0.0, 10.0);
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real activator_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0);
	Real inhibitor_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I1 + 0.75*PLANE_CORNER_SUM_I1);
		//Do diffusion on both
	activator_avg = (1.0 - ACTIVATOR_DIFFUSIONRATE)*PLANE_C_I0 +
			ACTIVATOR_DIFFUSIONRATE*activator_avg;
	inhibitor_avg = (1.0 - INHIBITOR_DIFFUSIONRATE)*PLANE_C_I1 +
			INHIBITOR_DIFFUSIONRATE*inhibitor_avg;
		//Do logistic growth on both, but have inhibitor inhibit the activator
		//and have the activator activate the inhibitor.
	Real activator_saturation = activator_avg/MAX_INTENSITY;
	Real inhibitor_saturation = inhibitor_avg/MAX_INTENSITY;
	PLANE_NEW_C_I0 = ACTIVATOR_GROWTH * (1.0 - inhibitor_saturation) * //Inhibition
		activator_avg * (1.0 - activator_saturation); //Logistic 
	PLANE_NEW_C_I1 = 0.1 + INHIBITOR_GROWTH * activator_avg * //Activation
		inhibitor_avg * (1.0 - inhibitor_saturation); //Logistic 
	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_INTENSITY);	
}

#endif CARULE_2D_DOUBLE_LOGISTIC_SMOOTH
//========================================================================
//========================================================================
//========================================================================
#ifdef CARULE_2D_ACTIVATOR_INHIBITOR_BRAIN
/*	This is a two-dimensional rule the same as CARULE_2D_ACTIVATOR_INHIBITOR,
but with Brian's Brain running on top of it.
The idea is to have Brain use the activator states BRAIN_RESTING_ACTIVATION
and BRAIN_FIRING_ACTIVATION.
*/

#define USERPARAM_COUNT 8 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define DIFFUSION_RATE_ACTIVATOR	owner->userParamAdd[1]->Val() //Da
#define DIFFUSION_RATE_INHIBITOR	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_PRODUCTION		owner->userParamAdd[3]->Val()	//ba
#define INHIBITOR_PRODUCTION		owner->userParamAdd[4]->Val()	//bb
#define DECAY_RATE_ACTIVATOR		owner->userParamAdd[5]->Val()	//ra
#define DECAY_RATE_INHIBITOR		owner->userParamAdd[6]->Val()	//rb
#define SOURCE_DENSITY			owner->userParamAdd[7]->Val()	//s
#define MIN_INHIBITOR			owner->userParamAdd[8]->Val()	//Min_b
#define MAX_ACTIVATORVALUE 5.0
#define BRAIN_RESTVALUE (MAX_ACTIVATORVALUE)
#define BRAIN_FIREVALUE (8*BRAIN_RESTVALUE) 
	//Make BRAIN_FIREVALUE high, so summing restvalues doesn't spoof a firing.
#define MAX_INTENSITY	owner->_max_intensity.Val()

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4d, found on page 26 of his book and stored on
his accompanying disk in the file SP24d.PRM */
	double  editValue[] = { //Params from Meinhardt Sp24d.PRM
	0.00125, 0.00625,	//activator and inhibitor diffusion rates.
	0.256, 0.004,	//activator and inhibitor production rates.
	0.52, 0.3,	//activator and inhibitor decay rates.
	0.52,		//source density or reaction density rate.
	0.52};		//minimum inhibitor value
	char *label[] = { "Activator Diffusion/7", "Inhibitor Diffusion/7",
		"Activator Production", "Inhibitor Production",
		"Activator Decay", "Inhibitor Decay", "Source Density",
		"Minimum Inhibitor"};

	owner->_dt.SetVal(0.05);
	owner->_max_intensity.SetVal(BRAIN_FIREVALUE + MAX_ACTIVATORVALUE);
	owner->_max_velocity.SetVal(30.0);
	owner->_nonlinearity1.SetVal(0.46);	

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 0.14);	//Da (max is 1/7)
	owner->userParamAdd[2]->SetRange(0.0, 0.14);	//Db
	owner->userParamAdd[3]->SetRange(0.0, 10.0);		//ba
	owner->userParamAdd[4]->SetRange(0.0, 10.0);		//bb
	owner->userParamAdd[5]->SetRange(0.0, 10.0);		//ra
	owner->userParamAdd[6]->SetRange(0.0, 10.0);		//rb
	owner->userParamAdd[7]->SetRange(0.0, 10.0);		//s
	owner->userParamAdd[8]->SetRange(0.000001, 10.0);	//Min_b
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	//Do the Brain rule=============================
	if (PLANE_C_I0 >= BRAIN_FIREVALUE) //Firing state goes to resting
		PLANE_NEW_C_I0 = BRAIN_RESTVALUE;
	else if (PLANE_C_I0 >= BRAIN_RESTVALUE) //Resting goes to neutral
		PLANE_NEW_C_I0 = 0.0;
	else if (PLANE_EIGHT_SUM_I0 >= 1.8*BRAIN_FIREVALUE &&
		PLANE_EIGHT_SUM_I0 < 2.2*BRAIN_FIREVALUE)// Neutral can go to Firing
		PLANE_NEW_C_I0 = BRAIN_FIREVALUE;
	else
	{
	//Do the Activator inhibitor rule=================
//We copy PLANE_C_I1 to a nonzero "inhibitor" so we can divide by it.
	double inhibitor = PLANE_C_I1;
	if (inhibitor < MIN_INHIBITOR)
		inhibitor = MIN_INHIBITOR;
	//We need the following number twice, so let's just compute it once.
	double Plane_C_I0_squared = PLANE_C_I0 * PLANE_C_I0; 
	
//Do diffusion, production, reaction, and decay in one step for each variable.
	PLANE_NEW_C_I0 = PLANE_C_I0 +						//The activator update:
		+ DIFFUSION_RATE_ACTIVATOR *					//Diffuse
			(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0 - 7.0*PLANE_C_I0)									
		+ SOURCE_DENSITY*(							//Reaction rate times...
			ACTIVATOR_PRODUCTION  					//Spontaneous Production
			+  Plane_C_I0_squared / inhibitor)		//The Reaction
		- DECAY_RATE_ACTIVATOR * PLANE_C_I0;			//Decay

	PLANE_NEW_C_I1 = PLANE_C_I1						//The inhibitor update:		
		+ DIFFUSION_RATE_INHIBITOR *					//Diffuse		
			(PLANE_FOUR_SUM_I1+ 0.75*PLANE_CORNER_SUM_I1 - 7.0*PLANE_C_I1)
		+ INHIBITOR_PRODUCTION						//Spontaneous Production
		+ SOURCE_DENSITY*(Plane_C_I0_squared)		//The Reaction
		- DECAY_RATE_INHIBITOR * PLANE_C_I1;			//Decay
	
	CLAMP(PLANE_NEW_C_I0, 0.0, MAX_INTENSITY); //MAX_ACTIVATORVALUE);
	CLAMP(PLANE_NEW_C_I1, 0.0, MAX_VELOCITY);
	}	
}
#endif //CARULE_2D_ACTIVATOR_INHIBITOR_BRAIN
//========================================================================
#ifdef CARULE_2D_HODGE_BRAIN

#define HODGE_TOP		owner->_max_intensity.Val()
#define HODGE_BOTTOM	owner->userParamAdd[1]->Val()
#define HODGE_STIM1		owner->userParamAdd[2]->Val()
#define HODGE_STIM2		owner->userParamAdd[3]->Val()
#define HODGE_INC		owner->userParamAdd[4]->Val()
#define BRAIN_RESTVALUE (HODGE_TOP/8.0)
#define BRAIN_FIREVALUE HODGE_TOP 

DllExport void USERINITIALIZE(CA* owner )
{
	double  editValue[] = { 0.1, 5.0, 100.0, 5.0 };
	char *label[] = { "Hodge Bottom", "Stim1", "Stim2", "Inc" };
	
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;
	owner->_max_intensity.SetVal(32.0f);

	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.1, 10.0); //HODGE_BOTTOM
	owner->userParamAdd[2]->SetRange(0.1, 1000.0); //HODGE_STIM1
	owner->userParamAdd[3]->SetRange(0.1, 1000.0); //HODGE_STIM2
	owner->userParamAdd[4]->SetRange(0.01, 10.0); //HODGE_INC
}

/* The hodgepodge rule. This works with a max intensity value of 32*/
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{

//Do Brain in Plane 0.
	Real BrainEightSum = PLANE_EIGHT_SUM_I0;
	if (PLANE_C_I0 >= BRAIN_FIREVALUE) //Firing state goes to resting
		PLANE_NEW_C_I0 = BRAIN_RESTVALUE;
	else if (PLANE_C_I0 >= BRAIN_RESTVALUE) //Resting goes to neutral
		PLANE_NEW_C_I0 = 0.0;
	else if (BrainEightSum >= 1.5*BRAIN_FIREVALUE &&
		BrainEightSum < 2.5*BRAIN_FIREVALUE)// Neutral can go to Firing
		PLANE_NEW_C_I0 = BRAIN_FIREVALUE;
	else
		PLANE_NEW_C_I0 = 0.0;
//Do Hodge in Plane 1
	Real HodgeEightSum = PLANE_EIGHT_SUM_I1;
	if (PLANE_C_I1 <= HODGE_BOTTOM)
	{
		if(HodgeEightSum <HODGE_STIM1)
			PLANE_NEW_C_I1 = 0.0;
		else if (HodgeEightSum < HODGE_STIM2)
			PLANE_NEW_C_I1 = 2.0;
		else //EightSum >= HODGE_STIM2
			PLANE_NEW_C_I1 = 3.0;
	}
	else if (PLANE_C_I1 < HODGE_TOP)
	{
		PLANE_NEW_C_I1 = HodgeEightSum/8.0 + HODGE_INC;
		CLAMP(PLANE_NEW_C_I1, 0, HODGE_TOP);
	}
	else //PLANE_C_I1 is HODGE_TOP
		PLANE_NEW_C_I1 = 0.0;

//Seed across from Brain Plane 0 to Hodge Plane 1;
	if (BrainEightSum > 4*BRAIN_FIREVALUE)
		PLANE_NEW_C_I1 = 1;
//	CLAMP(PLANE_NEW_C_I0, 0.0, 2.0);
//	PLANE_NEW_C_I0 += PLANE_NEW_C_I1;
}
#endif //CARULE_2D_HODGE_BRAIN
//========================================================================

#ifdef CARULE_2D_SANDPILE_5 //2D Sandpile 5.DLL

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(7.0f);
}

/* Bak-Tang-Weisenfeld Sandpile rule. */
DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	PLANE_NEW_C_I0 = int(PLANE_C_I0);
	if (PLANE_NEW_C_I0 >= 4.0)
		PLANE_NEW_C_I0 -= 4.0;
	if (PLANE_E_I0 >= 4.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_N_I0 >= 4.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_W_I0 >= 4.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_S_I0 >= 4.0)
		PLANE_NEW_C_I0 += 1.0;
	CLAMP(PLANE_NEW_C_I0, 0, 7.0);
}
#endif //CARULE_2D_SANDPILE_5
//========================================================================

#ifdef CARULE_2D_SANDPILE_9 //2D Sandpile 9.DLL

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	owner->_max_intensity.SetVal(15.0f);
}

/* Bak-Tang-Weisenfeld Sandpile rule with eight neighbors. */
DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	PLANE_NEW_C_I0 = int(PLANE_C_I0);
	if (PLANE_NEW_C_I0 >= 8.0)
		PLANE_NEW_C_I0 -= 8.0;
	if (PLANE_E_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_NE_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_N_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_NW_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_W_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_SW_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_S_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	if (PLANE_SE_I0 >= 8.0)
		PLANE_NEW_C_I0 += 1.0;
	CLAMP(PLANE_NEW_C_I0, 0, 15.0);
}
#endif //CARULE_2D_SANDPILE_9
//========================================================================
#ifdef CARULE_2D_WAVE_SANDPILE_5	//2D Sandpile Wave 5.DLL

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_WAVE_2D;
	owner->_max_intensity.SetVal(7.0f);
}

/* Bak-Tang-Weisenfeld Sandpile rule with wave motion. Looks like seething dog barf.  */
DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	Real C_topple = PLANE_C_I0;
	//Topple from High down towards zero
	if (C_topple >= 4.0)
		C_topple -= 4.0;
	if (PLANE_E_I0 >= 4.0)
		C_topple += 1.0;
	if (PLANE_N_I0 >= 4.0)
		C_topple += 1.0;
	if (PLANE_W_I0 >= 4.0)
		C_topple += 1.0;
	if (PLANE_S_I0 >= 4.0)
		C_topple += 1.0;
	//Topple from Low up towards zero.
	if (C_topple <= -4.0)
		C_topple += 4.0;
	if (PLANE_E_I0 <= -4.0)
		C_topple -= 1.0;
	if (PLANE_N_I0 <= -4.0)
		C_topple -= 1.0;
	if (PLANE_W_I0 <= -4.0)
		C_topple -= 1.0;
	if (PLANE_S_I0 <= -4.0)
		C_topple -= 1.0; 
	PLANE_NEW_C_I0 = C_topple ;//+ 0.01(-PLANE_PAST_C_I0 + PLANE_FOUR_SUM_I0/4.0); //Wave
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);
}

#endif // CARULE_2D_WAVE_SANDPILE_5

//========================================================================
#ifdef CARULE_2D_CITYFORMATION //2D City Formation 9.DLL
/* This rule is the Yakov Zeldovich model as presented by Damian Zanette and
 Susanna Manrubia in Phycial Review Leters 79, pp. 523-526, 1997.  Online
 at http://linkage.rockefeller.edu/wli/zipf/zanette97.pdf
 Basic Zeldovich idea is that half the cells double their values and half
 go to zero, and that the cells average with each other.  Result is 
 an inverse powerlaw distribution of the cell values:
 frequencyofagivensize ~ 1/size^2. The paper describes this with three
 parameters alpha, p, and q, which it tests for (0.25, 0.5, 0.0),
 (0.1, 0.75, 0.0), and (0.25, 0.5, 0.02), which seemingly are meant to
 produce 1/size^2 curves similar to those of the city
 population distributions in , respectively, the World, the USA, and
 Switzerland. */

#define DIFFUSION_RATE	owner->userParamAdd[1]->Val() //alpha
#define SUCCESS_PROBABILITY owner->userParamAdd[2]->Val() //p
#define SUCCESS_PROBABILITY_TWEAK owner->userParamAdd[3]->Val() //q

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 0.25, 0.5, 0.0 };
	char *label[] = { "Diffusion Rate", "Success Probability", "Probability Tweak"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0); 
	owner->userParamAdd[2]->SetRange(0.0, 1.0); 
	owner->userParamAdd[3]->SetRange(0.0, 0.5); 
	owner->_max_intensity.SetVal(10.0f);
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	bool growflag = (Randomreal() <= SUCCESS_PROBABILITY);
	//First diffuse whatever values got into the cells in the last update
	PLANE_NEW_C_I0 = PLANE_C_I0 + DIFFUSION_RATE * 
		((PLANE_EIGHT_SUM_I0/8.0) - PLANE_C_I0);	
	//And then grow some values and shrink others
	if (growflag)  //Get bigger
		PLANE_NEW_C_I0 =
			PLANE_NEW_C_I0*(1-SUCCESS_PROBABILITY_TWEAK)/SUCCESS_PROBABILITY;
			//In the simplest case this is just 2*C
	else	//Collapse
		PLANE_NEW_C_I0 =
			PLANE_NEW_C_I0*(SUCCESS_PROBABILITY_TWEAK)/(1-SUCCESS_PROBABILITY);
	CLAMP(PLANE_NEW_C_I0, 0, MAX_INTENSITY);
}
#endif CARULE_2D_CITYFORMATION //2D City Formation 9.DLL
//========================================================================
//========================================================================
#ifdef CARULE_2D_CITYFORMATION_5 //2D City Formation 5.DLL
/* This rule is the Yakov Zeldovich model as presented by Damian Zanette and
 Susanna Manrubia in Phycial Review Leters 79, pp. 523-526, 1997.  Online
 at http://linkage.rockefeller.edu/wli/zipf/zanette97.pdf
 Basic Zeldovich idea is that half the cells double their values and half
 go to zero, and that the cells average with each other.  Result is 
 an inverse powerlaw distribution of the cell values:
 frequencyofagivensize ~ 1/size^2. The paper describes this with three
 parameters alpha, p, and q, which it tests for (0.25, 0.5, 0.0),
 (0.1, 0.75, 0.0), and (0.25, 0.5, 0.02), which seemingly are meant to
 produce 1/size^2 curves similar to those of the city
 population distributions in , respectively, the World, the USA, and
 Switzerland. */

#define DIFFUSION_RATE	owner->userParamAdd[1]->Val() //alpha
#define SUCCESS_PROBABILITY owner->userParamAdd[2]->Val() //p
#define SUCCESS_PROBABILITY_TWEAK owner->userParamAdd[3]->Val() //q

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 0.25, 0.5, 0.0 };
	char *label[] = { "Diffusion Rate", "Success Probability", "Probability Tweak"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0); 
	owner->userParamAdd[2]->SetRange(0.0, 1.0); 
	owner->userParamAdd[3]->SetRange(0.0, 0.5); 
	owner->_max_intensity.SetVal(10.0f);
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	bool growflag = (Randomreal() <= SUCCESS_PROBABILITY);
	//First diffuse whatever values got into the cells in the last update
	PLANE_NEW_C_I0 = PLANE_C_I0 + DIFFUSION_RATE * 
		((PLANE_FOUR_SUM_I0/4.0) - PLANE_C_I0);	
	//And then grow some values and shrink others
	if (growflag)  //Get bigger
		PLANE_NEW_C_I0 =
			PLANE_NEW_C_I0*(1-SUCCESS_PROBABILITY_TWEAK)/SUCCESS_PROBABILITY;
			//In the simplest case this is just 2*C
	else	//Collapse
		PLANE_NEW_C_I0 =
			PLANE_NEW_C_I0*(SUCCESS_PROBABILITY_TWEAK)/(1-SUCCESS_PROBABILITY);
	CLAMP(PLANE_NEW_C_I0, 0, MAX_INTENSITY);
}
#endif CARULE_2D_CITYFORMATION_5 //2D City Formation 5.DLL
//========================================================================
#ifdef CARULE_2D_CITYFORMATION_5_BADVERSION //2D City Formation Delay 5.DLL
/* Maybe I should be doubling and halving BEFORE doing the diffusion?  No, this
doesn't give the effect.  This approach doesn't conserve the population as well,
I don't think.  Because in the non delay approach every old cell val gets averaged with
the old neighbors, which preserves people, and half the cells are doubled and half are 
 zeroed, which preverse poeple if yof do it ranomly.
 In this delay approach, I double or halve first, but then I average
 those values with UNDOUBLED OR ZEROED old values, so I can lose poeple or gain.
Also it looks wrong.
 */
#define DIFFUSION_RATE	owner->userParamAdd[1]->Val() //alpha
#define SUCCESS_PROBABILITY owner->userParamAdd[2]->Val() //p
#define SUCCESS_PROBABILITY_TWEAK owner->userParamAdd[3]->Val() //q

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 5;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 0.25, 0.5, 0.0 };
	char *label[] = { "Diffusion Rate", "Success Probability", "Probability Tweak"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0); 
	owner->userParamAdd[2]->SetRange(0.0, 1.0); 
	owner->userParamAdd[3]->SetRange(0.0, 0.5); 
	owner->_max_intensity.SetVal(10.0f);
}

DllExport void USERRULE_5(CA* owner, int c, int e, int n, int w, int s)
{
	bool growflag = (Randomreal() <= SUCCESS_PROBABILITY);
	//Grow some values and shrink others
	if (growflag)  //Get bigger
		PLANE_NEW_C_I0 =
			PLANE_C_I0*(1-SUCCESS_PROBABILITY_TWEAK)/SUCCESS_PROBABILITY;
			//In the simplest case this is just 2*C
	else	//Collapse
		PLANE_NEW_C_I0 =
			PLANE_C_I0*(SUCCESS_PROBABILITY_TWEAK)/(1-SUCCESS_PROBABILITY);
	PLANE_NEW_C_I0 = PLANE_NEW_C_I0 + DIFFUSION_RATE * 
		((PLANE_FOUR_SUM_I0/4.0) - PLANE_NEW_C_I0);	
	//Then diffuse whatever values got into the cells in the last update
	CLAMP(PLANE_NEW_C_I0, 0, MAX_INTENSITY);
}
#endif CARULE_2D_CITYFORMATION_5_BADVERSION //2D City Formation Delay 5.DLL
//========================================================================
#ifdef CARULE_2D_FORESTFIRE //2D Forestfire ?.DLL
//The ? depends on whether I comment in the next line or not.
//#define USEFIRECORNERS //If USEFIRECORNERS, ? is 9, else it's 5

/* This rule is in B. Malamud, G. Morein, and D. Turcotte,
 "Forest Fires: An Example of Self-Organized Critical Behavoir,"
 Science 291 (1998), pp. 1840-1842.  Doesn't seem to be
 available for free online, but there is a Powerpoint
 presentation at
 http://eclectic.ss.uci.edu/~drwhite/Anthro179a/J-Doyle.ppt
 and a discussion of the paper at
 http://www.ent-consulting.com/articles/automata.pdf.
  See also the Java applet illustrating this at 
http://schuelaw.whitman.edu/JavaApplets/ForestFireApplet/
I view the green, burning, and dead cases as, respectively,
the cell value 0, 1, and 2, akin to Brian's Brain states of
neutral, firing, and resting.  */

#define REBIRTH_PROBABILITY (owner->userParamAdd[1]->Val()/100.0) //p
#define LIGHTNING_PROBABILITY (owner->userParamAdd[2]->Val()/100.0) //q

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_HEAT_2D;
	double  editValue[] = { 5.0, 0.0006};
	char *label[] = { "100*Rebirth Probability", "100*Lightning Probability"};
	for(int i = 0; i < sizeof(editValue)/sizeof(double); i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 10.0); 
	owner->userParamAdd[2]->SetRange(0.0, 10.0); 
	owner->_max_intensity.SetVal(2.1f);
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	bool growflag = (Randomreal() <= REBIRTH_PROBABILITY);
	bool lightningflag = (Randomreal() <= LIGHTNING_PROBABILITY);
	Real currentc = int(PLANE_C_I0);
	Real newc = currentc;
//The ready case
	if (currentc == 1)
	{
		if (lightningflag)
			newc = 2;
		if (PLANE_E_I0 == 2)
			newc = 2;
		if (PLANE_N_I0 == 2)
			newc = 2;
		if (PLANE_W_I0 == 2)
			newc = 2;
		if (PLANE_S_I0 == 2)
			newc = 2; 
#ifdef USEFIRECORNERS
		if (PLANE_NE_I0 == 2)
			newc = 2;
		if (PLANE_NW_I0 == 2)
			newc = 2;
		if (PLANE_SW_I0 == 2)
			newc = 2;
		if (PLANE_SE_I0 == 2)
			newc = 2; 
#endif USEFIRECORNERS
	}
//The firing case
	if (currentc == 2.0)
		newc = 0.0; //Go to dead case
//The dead case
	if (currentc == 0.0)
	{
		if(growflag)
			newc = 1.0;  //Go to green case
		else
			newc = 0.0;
	}
//Extra case
	PLANE_NEW_C_I0 = newc;
	CLAMP(PLANE_NEW_C_I0, 0, MAX_INTENSITY);
}
#endif CARULE_2D_FORESTFIRE //2D Forestfire ?.DLL
//========================================================================
//============================================
#ifdef CARULE_2D_WINFREE_LOGISTIC
/*	This is a variation on CARULE_2D_LOGISTIC, but using two species.*/

#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define ACTIVATOR_DIFFUSIONRATE	owner->userParamAdd[1]->Val() //Da
#define INHIBITOR_DIFFUSIONRATE	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_THRESHOLD		owner->userParamAdd[3]->Val()	//ba
#define ACTIVATOR_GROWTH		owner->userParamAdd[3]->Val()	//bb
#define INHIBITOR_GROWTH		owner->userParamAdd[4]->Val()	//bb

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

/* We set the default values to match the values used in Meinhardt's 
one-dimensional simulation 2.4a, found on page 26 of his book and stored on
his accompanying disk in the file SP24a.PRM */
	double  editValue[] = {
	0.66, 0.66,	//activator and inhibitor diffusion rates.
	0.1, 1.0, 4.5};	//activator threshold and inhibitor growth rates.
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		 "Activator Threshold","Activator Growth","Inhibitor Growth"};

	owner->_max_intensity.SetVal(1.0);
	owner->_dt.SetVal(1.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0);	
	owner->userParamAdd[2]->SetRange(0.0, 1.0);
	owner->userParamAdd[3]->SetRange(0.0, 2.0);
	owner->userParamAdd[4]->SetRange(0.0, 20.0);
	owner->userParamAdd[5]->SetRange(0.0, 20.0);
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	Real activator_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0);
	Real inhibitor_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I1 + 0.75*PLANE_CORNER_SUM_I1);
		//Do diffusion on both
	activator_avg = (1.0 - ACTIVATOR_DIFFUSIONRATE)*PLANE_C_I0 +
			ACTIVATOR_DIFFUSIONRATE*activator_avg;
	inhibitor_avg = (1.0 - INHIBITOR_DIFFUSIONRATE)*PLANE_C_I1 +
			INHIBITOR_DIFFUSIONRATE*inhibitor_avg;
		//Do logistic growth on both, but have inhibitor inhibit the activator
		//and have the activator activate the inhibitor.
	if (activator_avg < ACTIVATOR_THRESHOLD)
		PLANE_NEW_C_I0 = activator_avg +
			ACTIVATOR_GROWTH * (-activator_avg - inhibitor_avg);
	else //activator_avg >= ACTIVATOR_THRESHOLD
		PLANE_NEW_C_I0 = activator_avg + 
			ACTIVATOR_GROWTH * (1.0 - activator_avg - inhibitor_avg);
	PLANE_NEW_C_I1 = inhibitor_avg + INHIBITOR_GROWTH * activator_avg;
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, -MAX_INTENSITY, MAX_INTENSITY);	
}

#endif CARULE_2D_WINFREE_LOGISTIC
//========================================================================
//============================================
#ifdef CARULE_2D_WINFREE_ZHABO_NEW
/*	This is based on Winfree's paper. Arthur T. Winfree, "Rotating 
Chemical Reactions", Scientific American, June 1974, pp. 82-95.
We think of our 0 plane as holding an activator A, and the 1 plane
as holding an inhibitor B.  Winfree's rule is to set an inhibitor
growth rate K, an activator growth rate L, and an activator threshold T.
We get these dynamics:
dB/dt = K*A
dA/dt = - L*(A+B) if A < T
cA/dt = L*(1-(A+B)) if A > T
We can discretize these rules as follows.  
newB = B + K*A
newA = A -L*(A+B) if A < T
newA = A +L*(1-(A+B) if A >= T
Now, we also need to include diffusion.  The scheme I like to use is
to first carry out the diffusion, then to run the update rules on 
the diffused values, and then to put the diffused values into the cells.
That is, we compute Aaverage and Baverage, and use these equations
newB = Baverage + K*Aaverage
newA = Aaverage -L*(Aaverage+Baverage) if Aaverage < T
newA = Aaverage +L*(1-(Aaverage+Baverage) if Aaverage >= T
*/

#define USERPARAM_COUNT 5 //This is how many user params I list right here.
//Recall that by default userParamAdd[0] is always the Variance.
#define ACTIVATOR_DIFFUSIONRATE	owner->userParamAdd[1]->Val() //Da
#define INHIBITOR_DIFFUSIONRATE	owner->userParamAdd[2]->Val()	//Db
#define ACTIVATOR_THRESHOLD		owner->userParamAdd[3]->Val()	//ba
#define ACTIVATOR_GROWTH		owner->userParamAdd[3]->Val()	//bb 
#define INHIBITOR_GROWTH		owner->userParamAdd[4]->Val()	//bb

/* 2017, note that ACTIVATOR_GROWTH = ACTIVATOR THRESHOLD.  Maybe it was a mistake to use the same index twice, I don't remember.  But 
if I try to "fix" this by changing the last two indexes to 4 and 5, which would be logical....then the *dll rule just dies. */

DllExport void USERINITIALIZE(CA* owner )
{
	owner->_usernabesize = 9;
	owner->_usercastyle = CA_WAVE_2D;

	double  editValue[] = {
	0.25, 0.65,	//activator and inhibitor diffusion rates.
	0.1, 1.0, 0.1};	//activator threshold and inhibitor growth rates. //2017 note that slot [5] isn't actually used.
	char *label[] = { "Activator Diffusion", "Inhibitor Diffusion",
		//  "Activator Threshold","Activator Growth","Inhibitor Growth"}; //Change to the below in 2017 to be more accurate.
	"Activator Threshold and Growth","Inhibitor Growth","Unused"};

	owner->_max_intensity.SetVal(1.0);
	owner->_dt.SetVal(1.0);

	for(int i = 0; i < USERPARAM_COUNT; i++)
		(*(owner->pAddUserParam))(owner, label[i], editValue[i]);
	owner->userParamAdd[1]->SetRange(0.0, 1.0);	
	owner->userParamAdd[2]->SetRange(0.0, 1.0);
	owner->userParamAdd[3]->SetRange(0.0, 2.0);
	owner->userParamAdd[4]->SetRange(0.0, 2.0);
	owner->userParamAdd[5]->SetRange(0.0, 2.0); //2017 note that slot [5] isn't actually used.
}

DllExport void USERRULE_9(CA* owner, int c, int e, int ne, int n, int nw,
int w, int sw, int s, int se)
{
	//Do diffusion on both, weighting the sides a bit more than the corners.
	//First compute the weighted average of the neighboring cells.
	Real activator_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I0 + 0.75*PLANE_CORNER_SUM_I0);
	Real inhibitor_avg = (1.0/7.0)*(PLANE_FOUR_SUM_I1 + 0.75*PLANE_CORNER_SUM_I1);
	//Now make a weighted average of the cell's value with the neighbors average.
	//Here we use the diffusionrate params to control how much the neighbors count.
	activator_avg = (1.0 - ACTIVATOR_DIFFUSIONRATE)*PLANE_C_I0 +
			ACTIVATOR_DIFFUSIONRATE*activator_avg;
	inhibitor_avg = (1.0 - INHIBITOR_DIFFUSIONRATE)*PLANE_C_I1 +
			INHIBITOR_DIFFUSIONRATE*inhibitor_avg;
	/* So now activator_avg is what I called Aaverage above, and
	inhibitor_avg is what I called Baverage.  Now use the first
	equation to specify the new inhibitor value.*/
	PLANE_NEW_C_I1 = inhibitor_avg + INHIBITOR_GROWTH * activator_avg;
	/* Now use one of two update rules for the activator */
	if (activator_avg < ACTIVATOR_THRESHOLD)
		PLANE_NEW_C_I0 = activator_avg +
			ACTIVATOR_GROWTH * (-activator_avg - inhibitor_avg);
	else //activator_avg >= ACTIVATOR_THRESHOLD
		PLANE_NEW_C_I0 = activator_avg + 
			ACTIVATOR_GROWTH * (1.0 - activator_avg - inhibitor_avg);
	/* The activator values will generally range between -1 and 1,
	but the inhibitor values will always be positive.  But just to
	match the colors we view them as lying in the same range from
	neg max to pos max. */
	CLAMP(PLANE_NEW_C_I0, -MAX_INTENSITY, MAX_INTENSITY);	
	CLAMP(PLANE_NEW_C_I1, -MAX_INTENSITY, MAX_INTENSITY);	
}

#endif CARULE_2D_WINFREE_ZHABO_NEW
//========================================================================
