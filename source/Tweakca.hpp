#ifndef TWEAKCA_HPP
#define TWEAKCA_HPP
//==================================================================
//--------------TWEAKPARAM tweakParamType DEFINES-------------------
/* Mabye this should be made into an enum.  Then we wouldn't have to  know
these number names. To do this we would need a TweakParamArray _tweakparam
object in the CA class.  Each TweakParam would have an _id field, that
could be used in the CAlist::BumpTweakParam call like in analog.cpp.*/
// also used in CAPOW.RC
#define SPACE_STEP_TYPE 801				// analog param
#define TIME_STEP_TYPE 802             // analog param
#define MAX_INTENSITY_TYPE 803         // analog param
#define MAX_VELOCITY_TYPE 804          // analog param
#define HEAT_INC_TYPE	805            // analog param
#define CHUNK_TYPE 806						// analog param
#define NONLINEARITY1_TYPE 807			// analog param
#define NONLINEARITY2_TYPE 808			// analog param
#define SPRING_TYPE 809                // electric param
#define FRICTION_TYPE 810              // electric param
#define DRIVER_AMP_TYPE 811            // electric param
#define DRIVER_FREQ_TYPE 812           // electric param
#define MASS_TYPE 813                  // electric param
#define VARIANCE_TYPE 814              // digital param
#define MAX_VALPERCENT_TYPE 815		//analog param


//------------- TWEAKPARAM STRING DEFINES ---------
#define HEAT_INC_STR "Heat Inc"
#define MAX_INTENSITY_STR "Max Intensity"
#define MAX_VALPERCENT_STR "3D Height"
#define MAX_VELOCITY_STR "Max Velocity"
#define FRICTION_STR "Resistance"
#define MASS_STR "Inductance"
#define TIME_STEP_STR "Time Step"
#define SPACE_STEP_STR "Space Step"
#define NONLINEARITY_STR "Non-linearity" // for _nonlinearity1, _nonlinearity2
#define SPRING_STR "1/Capacitance"
#define DRIVER_AMP_STR "Amplitude"
#define DRIVER_FREQ_STR "Frequency"
#define CHUNK_STR "State Grain"
#define VARIANCE_STR "Mutation Strength"
//------------- TWEAKPARAM MAX,MIN DEFINES ---------

#define MIN_HEAT_INC  (Real)  0.0
#define MAX_HEAT_INC  (Real)256.0
#define MIN_MAXINTENSITY (Real)  1.0
#define MAX_MAXINTENSITY (Real) 10000.0
#define MIN_MAX_VALPERCENT (Real) 0.1
#define MAX_MAX_VALPERCENT (Real) 2.0
#define MIN_MAXVELOCITY (Real) 1.0 //2017
#define MAX_MAXVELOCITY (Real) 1000.0  //2017 Try MUCH bigger, as View|Velocity has been
//clamping out wiht all red screen for some rulse.  Was 1000.0. all red.  TOO big 1000000.0 gives all red.
//For some reason 100000.0 seems to make about half of the 1D wave CAs show decent patterns.
//But this seems stupidly high. Go back and set the velocity for these waves more carefully, and then 1000.0 is okay.
#define MIN_FRICTION    (Real) 0.0
#define MAX_FRICTION    (Real) 100.0
#define MIN_MASS  (Real) 0.1
#define MAX_MASS  (Real) 10000.0 // can't be too big because "dt/mass" would be close to zero
#define MIN_TIME_STEP    (Real) SMALL_REAL  // defined in types.h
#define MAX_TIME_STEP    (Real) 10.0
#define MIN_SPACE_STEP   (Real) SMALL_REAL  // defined in types.h
#define MAX_SPACE_STEP   (Real) BIG_REAL     // no upperbound???
#define MIN_SPRING   (Real) 0.0001
#define MAX_SPRING   (Real) BIG_REAL // no upperbound!!!
#define MIN_NONLINEARITY1   (Real) 0.0// no Clamp!!!
#define MAX_NONLINEARITY1   (Real) BIG_REAL
#define MIN_NONLINEARITY2   (Real) 0.0// no Clamp!!!
#define MAX_NONLINEARITY2   (Real) BIG_REAL
#define MIN_DRIVER_AMP (Real) 0.0
#define MAX_DRIVER_AMP (Real) 1000.0
#define MIN_DRIVER_FREQ (Real) 0.1
#define MAX_DRIVER_FREQ (Real) 100.0
#define MIN_CHUNK (Real) 0.0
#define MAX_CHUNK (Real) 10.0

//------------- MULTIPLICATIVETWEAKPARAM MULT_INC ---------
#define HEAT_MULT_INC      (Real) 1.5
#define FRICTION_MULT_INC  (Real) 1.5
#define MASS_MULT_INC      (Real) 1.5
#define TIME_STEP_MULT_INC        (Real) 1.1
#define SPACE_STEP_MULT_INC       (Real) 1.1
#define NONLINEARITY_MULT_INC (Real) 1.5
#define DRIVER_AMP_MULT_INC (Real) 1.5
#define DRIVER_FREQ_MULT_INC (Real) 1.5
#define CHUNK_MULT_INC (Real) 2.0
#define VARIANCE_MULT_INC (Real) 1.1

//------------- TWEAKPARAM MEAN, VARIANCE DEFINES -----------
#define HEAT_INC_MEAN       (Real) 1.0  // fixed
#define HEAT_INC_VARIANCE   (Real) 0.0
#define MAX_INTENSITY_MEAN  (Real) 251.0  // 2.0 to 500.0
#define MAX_INTENSITY_VARIANCE (Real) 249.0
#define MAX_VALPERCENT_MEAN (Real) 0.5
#define MAX_VALPERCENT_VARIANCE (Real) 0.25
#define MAX_VELOCITY_MEAN      (Real) ((Real)(MAX_MAXVELOCITY - MIN_MAXVELOCITY)/2.0) //2017
#define MAX_VELOCITY_VARIANCE  (Real) 0.2 //2017
#define FRICTION_MEAN       (Real) 0.25  // 0.0 to 0.5
#define FRICTION_VARIANCE   (Real) 0.25
#define MASS_MEAN     (Real) 2.6     // 0.1 to 5.1
#define MASS_VARIANCE (Real) 2.5
#define TIME_STEP_MEAN     (Real) 0.0501  // 0.0001 to 0.1001
#define TIME_STEP_VARIANCE (Real) 0.05
#define SPACE_STEP_MEAN     (Real) 0.2501 // 0.0001 to 0.5001
#define SPACE_STEP_VARIANCE (Real) 0.25
#define SPRING_MEAN      (Real) 2.501 // 0.001 to 5.001
#define SPRING_VARIANCE  (Real) 2.5
#define NONLINEARITY1_MEAN     (Real) 0.501 // 0.1 to 1.001
#define NONLINEARITY1_VARIANCE (Real) 0.5
#define NONLINEARITY2_MEAN     (Real) 101 // 1 to 201
#define NONLINEARITY2_VARIANCE (Real) 100
// DRIVER_AMP: the range is [_max_intensity/4, 3*_max_intensity/4]
#ifndef FIXED_FREQ
	#define DRIVER_FREQ_MEAN     (Real) 3.0  // 1 to 5
	#define DRIVER_FREQ_VARIANCE (Real) 2.0
#else // FIXED_FREQ
	#define DRIVER_FREQ_MEAN     (Real) 1.0  // 1.0
	#define DRIVER_FREQ_VARIANCE (Real) 0.0
#endif //FIXED_FREQ
#define CHUNK_MEAN     (Real) 0.0 // fixed at 0.0
#define CHUNK_VARIANCE (Real) 0.0
#define VARIANCE_MEAN     (Real) 0.0 // fixed at 0.0
#define VARIANCE_VARIANCE (Real) 0.0
#define MAX_VARIANCE (Real) 0.1 //2017, was 0.99, but big variances ruin params when we rancomize in userpara.capp or in camore.cpp
#define MIN_VARIANCE (Real) 0.0
#define DEFAULT_VARIANCE (Real) 0.02 //2 percent

//---------- (AdditiveTweakParam's) TWEAKRANGE DEFINES -------
#define MAX_INTENSITY_LO (Real) 1.0
#define MAX_INTENSITY_MED (Real) 10.0
#define MAX_INTENSITY_HI (Real) 50.0
#define MAX_INTENSITY_LO_TO_MED (Real) 4.9999  // < 5
#define MAX_INTENSITY_MED_TO_HI (Real) 99.9999 // < 100

#define MAX_VALPERCENT_LO (Real) 0.1
#define MAX_VALPERCENT_MED (Real) 10.0
#define MAX_VALPERCENT_HI (Real) 50.0
#define MAX_VALPERCENT_LO_TO_MED (Real) 4.9999  // < 5
#define MAX_VALPERCENT_MED_TO_HI (Real) 99.9999 // < 100

#define MAX_VELOCITY_LO (Real) 1.0
#define MAX_VELOCITY_MED (Real) 10.0
#define MAX_VELOCITY_HI (Real) 50.0
#define MAX_VELOCITY_LO_TO_MED (Real) 4.9999  // < 5
#define MAX_VELOCITY_MED_TO_HI (Real) 99.9999 // < 100
//===========================================================================
#endif //TWEAKCA_HPP
