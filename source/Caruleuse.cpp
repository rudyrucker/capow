//
//	User CA rule need this file to compile. These are the only CA method
//  that it will be call. DO NOT INCLUDE CA.CPP!!!
//

#include "ca.hpp"
#include <math.h>
#include "random.h"
#include "resource.h"
#include "tweakca.hpp"
//#include <vector.h> already in ca.hpp


void CA::Smooth_variance()
{
	int xcount = _max_horz_count;
   int i;
		for (int j=0; j < CELL_PARAM_COUNT; j++)
	{
#ifndef SMOOTH_VARIANCE
		for (i=0; i<xcount; i++)
			wave_past_row[i]._cell_param[j] =
			wave_source_row[i]._cell_param[j] =
			wave_target_row[i]._cell_param[j] =
				1.0 + Randomsignreal()*_variance.Val();
#else   //Use the three buffers for three smoothing steps.
		for (i=0; i<xcount; i++)
			wave_source_row[i]._cell_param[j] =
				1.0 + Randomsignreal()*_variance.Val();
		for (i=1; i<xcount-1; i++)
			wave_target_row[i]._cell_param[j] =
			(wave_source_row[i-1]._cell_param[j] +
			 wave_source_row[i]._cell_param[j] +
			 wave_source_row[i+1]._cell_param[j]) /3.0;
		for (i=1; i<xcount-1; i++)
			wave_past_row[i]._cell_param[j] =
			(wave_target_row[i-1]._cell_param[j] +
			 wave_target_row[i]._cell_param[j] +
			 wave_target_row[i+1]._cell_param[j]) /3.0;
		for (i=1; i<xcount-1; i++)
			wave_source_row[i]._cell_param[j] =
			(wave_past_row[i-1]._cell_param[j] +
			 wave_past_row[i]._cell_param[j] +
			 wave_past_row[i+1]._cell_param[j]) /3.0;
		for (i=0; i<xcount; i++)
			wave_past_row[i]._cell_param[j] =
				wave_target_row[i]._cell_param[j] =
				wave_source_row[i]._cell_param[j];
#endif //SMOOTH_VARIANCE
	}
}

void CA::Adjust_acceleration_multiplier()
{
/* Be careful here not to call SetTeakParam(&_dx, ???), becuase
this will in turn call _dx.SetVal(???, this), which will in
turn call this->Adjust_acceleration_multiplier, which will
put you in an endless loop. Instead do _dx.SetVal(???), or
do SetTweakParam(&_dx, ???, NULL). */
	if (_dt.Val() < SMALL_REAL) //Do this to protect yourself as later you'll
		//divide by _dt in like AltWave to figure out velocity.
		_dt.SetVal(SMALL_REAL);
	if ( _dx_lock)  /* Try to automatically obey Courant condition.
		These numbers are kind of ad hoc, just what seems to work.
		Can I do this in terms of _castyle.*/
	  {
		if( CA_WAVE <= type_ca && type_ca <= CA_AUTO_ULAM_WAVE)
			_dx.SetVal(_dt.Val() * 2.0);
		else
			_dx.SetVal(_dt.Val() * 1.2);
		if (type_ca == CA_WAVE2)
			_dx.SetVal(_dt.Val() * 6.0);
	  }
	_dt_over_2 = _dt.Val()/2.0;
	_dt_over_6 = _dt.Val()/6.0;
	_dt_2 = _dt.Val() * _dt.Val();
	_dx_2 = _dx.Val() * _dx.Val();
	_dt_over_dx_2 = _dt.Val() / (_dx.Val() * _dx.Val());
	_1_over_mass = 1 / _mass.Val();    // it was done in Set_mass() before
	_dt_over_mass = _dt.Val() / _mass.Val(); // it was done in Set_mass() before
	_dt_2_over_mass = _dt.Val() * _dt.Val() / _mass.Val(); // it was done in Set_mass() before
	_wavespeed_2_times_dt_2_over_dx_2 =
		(_wavespeed * _wavespeed * _dt.Val() * _dt.Val()) /
			(_dx.Val() * _dx.Val());
	_wavespeed_2_times_dt_over_dx_2 =
		(_wavespeed * _wavespeed * _dt.Val()) /
			(_dx.Val() * _dx.Val());
	_dt_over_12_times_dx_2 = _dt.Val() / (12.0 * _dx.Val() * _dx.Val());
	_wavespeed_2_times_dt_2_over_12_times_dx_2 =
		(_wavespeed * _wavespeed * _dt.Val() * _dt.Val()) /
			(_dx.Val() * _dx.Val());
//Because second diff is (-LL + 16L -30 C + 16R - RR) / (12*dx*dx)
//I figured this out in the Mathematica notebook wavenabe.ma by using
//Lagrangian interpolation to get a cubic through (-2,LL)(-1,L)(0,C)
//(1,R) and (2,RR), then taking the second derivative & evaluating at 0.

#ifdef ADHOC_FREQ_FIX //NOt currently in use, May 6, 1996
	frequency_factor = (2.0*PI*_frequency_multiplier.Val())/
	(_dt*OSCILLATOR_BASE_PERIOD);
	/* If frequency_multiplier is 1, this means that the oscillator
	has frequency_factor go from 0 to 2*pi as time goes from 0 to
	BASE_PERIOD*_dt.  If frequncy multiplier is 2 its twice as
	fast, if 0.5 half as fast, and so on.  I use a visually good
	BASE_PERIOD, like 200*/
#else
	frequency_factor = _frequency_multiplier.Val();
#endif

}
