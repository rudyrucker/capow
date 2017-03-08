#include "ca.hpp"
#include "random.h"
#include <math.h>

#define FOURIER_SEED_TERMS 5
#define FOURIER_SEED_MAX_FREQ 10
#define FOURIER_SEED_TERMS_X_2D 2
#define FOURIER_SEED_TERMS_Y_2D 3
#define FOURIER_SEED_MAX_FREQ_2D 8

extern HINSTANCE hInst;
extern HWND masterhwnd;

#define BLANK_HELPERS 
/*This switch is to turn on the option of making all the variables except the top
intensity variable be 0.0 whenever you seed.  This is useful for activator-inhibitor
situations. BLANK_HELPERS was off in the CAPOW 6B ship version, turned this on
November 4, 1998. */

#ifndef BLANK_HELPERS //Do it the old way when BLANK_HELPERS is turned off.
	//that is, seed all the cell parameters

void CA::RandomSeed()
{ //The most obvious kind of random seeding.
	if (dimension == 1)
	{
		// Reset all the rows to the top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (int i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = (unsigned char)Random((unsigned short)states);
			past_row[i] = (unsigned char)Random((unsigned short)states);
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					0.5 * Randomsignreal() * _max_intensity.Val();
		/* Need to put consistent colorstates pattern into source_row
		for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));

			}
		}
		Resetfreq();
	}
	else //2D case
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (int j = 0; j < CY_2D; j++)
			for (int i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
					wave_source_plane[c].variable[k] =
						0.5 * Randomsignreal() * _max_intensity.Val();
				c++;
			}
	}
	generator_ptr->Seed();
	Reset_smoothsteps(SMOOTHRANDSTEPSFACTOR);
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}


void CA::FourierSeed()
{ /*Seed the wave values with a Fourier sum of sine waves.  The idea
is to use FOURIER_SEED_TERMS many sine terms, each with a frequency
between 1 and FOURIER_SEED_MAX_FREQ.  Arrange it so that their
amplitudes sum to fourier_amplitude_sum times max_intensity, and do
this in such a way that one term is likely to get most of the weight.*/
	Real fourier_frequency[FOURIER_SEED_TERMS];
	Real fourier_amplitude[FOURIER_SEED_TERMS];
	Real fourier_frequency_X_2D[FOURIER_SEED_TERMS_X_2D]
		[FOURIER_SEED_TERMS_Y_2D];
   // next line: l.andrews 11/3/01 changed from FOURIER_SEED_TERMS to FOURIER_SEED_TERMS_Y_2D
   // and inverted the dimensions to agree with indexing in code --THIS MAY BE WRONG --
   // IT IS POSSIBLE THAT THEN INDICES SHOULD BE INVERTED IN THE CODE INSTEAD !
	Real fourier_frequency_Y_2D[FOURIER_SEED_TERMS_X_2D] 
		[FOURIER_SEED_TERMS_Y_2D];
	Real fourier_amplitude_2D[FOURIER_SEED_TERMS_X_2D]
		[FOURIER_SEED_TERMS_Y_2D];
	Real fourier_amplitude_sum = 1.0; //Try 0.85 if too high.
	int i,j, k, c;

	if (dimension == 1)
	{
		if (!horz_count) //avoid divide by zero.
			return;
//Start 1D Setup
		//reset all rows to top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
//End 1D Setup	
//Start k loop on variable[]
		for (int k = 0; k < VARIABLE_COUNT; k++)
		{
		     for (j=0; j<FOURIER_SEED_TERMS; j++)
			{
				fourier_frequency[j] = 1 + Random(FOURIER_SEED_MAX_FREQ);
				fourier_frequency[j] *=	(2.0 * PI)/(horz_count);
					/*Scale it for use in an expression of form
					sin(fourier_frequency[j] * i), with i between 0 and
					horz_count.  The idea is to do the unscaled
					fourier_frequency[j] many full sinewave up/down
					oscillations across the width of the visible CA*/
				//Before that, just get a piece of what amplitude's left.
				fourier_amplitude[j] = Randomreal() * fourier_amplitude_sum;
				fourier_amplitude_sum -= fourier_amplitude[j];
				//Subtract off what you took.
				fourier_amplitude[j] *= _max_intensity.Val();
				//Scale your amplitude to fit the CA.
			}
			//Give the last term all of the amplitude that's left over.
			fourier_amplitude[FOURIER_SEED_TERMS-1] += fourier_amplitude_sum;
			for (i = 0; i < _max_horz_count; i++)
			{
				//Sum the Fourier series
				wave_source_row[i].variable[k] = 0.0;
				for (j=0; j< FOURIER_SEED_TERMS; j++)
					wave_source_row[i].variable[k] +=
				       	fourier_amplitude[j] *
						sin(fourier_frequency[j] * i);
				wave_past_row[i].variable[k] = 	wave_source_row[i].variable[k];
			}
		}
//End k loop on variable[]
//Start 1D Cleanup
		for (i = 0; i < _max_horz_count; i++)
		{
			//Do the digital seed with pure randomness.
 			source_row[i] = (unsigned char)Random((unsigned short)states);
			past_row[i] = (unsigned char)Random((unsigned short)states);
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
	/* Need to put consistent colorstates pattern into source_row
	for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));
			}
		}
		Resetfreq();
//End 1D Cleanup
	}
	else //dimension 2
	{
		if (!horz_count_2D || !vert_count_2D) //avoid divide by zero.
			return;
// Start 2D Setup
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
//End 2D Setup
//Start k loop on 2D variable[]
		for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
		{
		    for (i=0; i<FOURIER_SEED_TERMS_X_2D; i++)
			{
				for (j=0; j<FOURIER_SEED_TERMS_Y_2D; j++)
				{
					fourier_frequency_X_2D[i][j] =
						1 + Random(FOURIER_SEED_MAX_FREQ_2D);
					fourier_frequency_Y_2D[i][j] =
						1 + Random(FOURIER_SEED_MAX_FREQ_2D);
					fourier_frequency_X_2D[i][j] *=
						(2.0 * PI)/(horz_count_2D);
					fourier_frequency_Y_2D[i][j] *=
						(2.0 * PI)/(vert_count_2D);
					/* Scale for use in an expression of form
					sin(fourier_frequency_X_2D[i][j] * x) *
					sin(fourier_frequency_Y_2D[i][j] * y), with
					x between 0 and horz_count_2D and
					y between 0 and horz_count_2D. See comments above
					in the dimension 1 case for the rest of this.*/
					fourier_amplitude_2D[i][j] = Randomreal() *
						fourier_amplitude_sum;
					fourier_amplitude_sum -= fourier_amplitude_2D[i][j];
					fourier_amplitude_2D[i][j] *= _max_intensity.Val();
				}
	 		}
		//Give the last term all of the amplitude that's left over.
    		fourier_amplitude_2D[FOURIER_SEED_TERMS_X_2D-1]
			[FOURIER_SEED_TERMS_Y_2D-1] += fourier_amplitude_sum;
			for (int y = 0; y < CY_2D; y++)
			{
				c = index(0, y);
				for (int x = 0; x < CX_2D; x++)
				{
				//Do the Fourier sum
					wave_source_plane[c].variable[k] = 0.0;
					for (i=0; i< FOURIER_SEED_TERMS_X_2D; i++)
						for (j=0; j<FOURIER_SEED_TERMS_Y_2D; j++)
     				       	wave_source_plane[c].variable[k] +=
								fourier_amplitude_2D[i][j] *
								sin(fourier_frequency_X_2D[i][j] * x) *
								sin(fourier_frequency_Y_2D[i][j] * y);
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k];
					c++;
				}
			}
		}
//End k loop on 2D variable[]
	}
//Common cleanup
	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function exists, yet //mike
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}

void CA::Sineseed()
{
	Real phase[VARIABLE_COUNT];
	Real phase2D[PLANE_VARIABLE_COUNT];
	if (dimension == 1)
	{
		// Reset all the rows to the top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (int k=0; k< VARIABLE_COUNT; k++)
			phase[k] = k * ((2.0*PI) / VARIABLE_COUNT);
		for (int i = 0; i < _max_horz_count; i++)
		{
/* Let's set it for two complete sinewave cycles to fit into the
size of the CA window at seed time.  So I want to cover 4*PI in
the horz_count steps)*/
			source_row[i] = (unsigned char)(((Real)states/2.0) +
				sin((i * 4.0 * PI)/(horz_count)) * ((Real)states/2.0));
			if (source_row[i]>states-1)
				source_row[i] = (unsigned char)(states-1);
			past_row[i] = (unsigned char)(((Real)states/2.0) +
				sin((i * 4.0 * PI)/(horz_count)) * ((Real)states/2.0));
			if (past_row[i]>states-1)
				past_row[i] = (unsigned char)(states-1);
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
//Seed the variables out of phase with each other.
			for (k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] = wave_source_row[i].variable[k] =
					0.5*sin(phase[k] + (i * 4.0 * PI)/(horz_count)) *
					_max_intensity.Val();
		/* Need to put consistent colorstates pattern into source_row
		for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));
			}
		}
		Resetfreq();
		generator_ptr->Seed();
		//generatorlist.Seed(); //seed function not defined, yet //mike
	}
	else //dimension 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		for (int k=0; k< PLANE_VARIABLE_COUNT; k++)
			phase2D[k] = k * ((2.0*PI) / VARIABLE_COUNT);
		int c = 0;
		for (int j = 0; j < CY_2D; j++)
			for (int i = 0; i < CX_2D; i++)
			{
				for (k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
					wave_source_plane[c].variable[k] =
						0.5*sin(phase2D[k] + (i * 4.0 * PI)/(horz_count_2D)) *
						sin(phase2D[k] + (j * 4.0 * PI)/(vert_count_2D)) *
						_max_intensity.Val();
				c++;
			}
	 //	Resetfreq();
  //		generator_ptr->Seed();
 	}
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}

void CA::Oneseed()
{

	int  oneseedspike = 20; //Default half width of spike
	int i, j;
	Real distance;
	int c;

	if (dimension == 1)
	{
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = 0;
			past_row[i] = 0;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] = wave_source_row[i].variable[k] = 0.0;
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));

		}
		source_row[horz_count/2] = 1;
		if (oneseedspike > horz_count/2)
			oneseedspike = horz_count /4;
		for (i=(horz_count/2)-oneseedspike; i< (horz_count/2) + oneseedspike;
			i++)
		{
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					_max_intensity.Val() * (1.0 -
					fabs(i - (horz_count/2.0)) / (Real)(oneseedspike));
		 }
	/* Need to put consistent colorstates pattern into source_row
	for meaningful Avoidstripes check in wave cases */
		if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			source_row[horz_count/2] = (unsigned char) (((MAX_COLOR-1) *
				(wave_source_row[horz_count/2].variable[0] +
				_max_intensity.Val())) / (2.0 * _max_intensity.Val()));

		Resetfreq();
	}
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;

		for (j = 0; j < CY_2D; j++)
		{
			c = index(0,j);
			for (i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] = 0.0;
				c++;
			}
		}
		oneseedspike = 20;
		if (oneseedspike > horz_count_2D/2)
			oneseedspike = horz_count_2D /4;
		if (oneseedspike > vert_count_2D/2)
			oneseedspike = vert_count_2D /4;
		for (j=(vert_count_2D/2)-oneseedspike;
			j< (vert_count_2D/2) + oneseedspike; j++)
			for (i=(horz_count_2D/2)-oneseedspike;
			i< (horz_count_2D/2) + oneseedspike; i++)
			{
				c = index(i,j);
				for (int k=0; k< PLANE_VARIABLE_COUNT; k++)
				{
#ifdef TAXI_CAB_METRIC
					wave_past_plane[c].variable[k] =
					wave_source_plane[c].variable[k] =
						max_intensity *
						(fabs(i - (horz_count_2D/2.0)) +
						fabs(j - (vert_count_2D/2.0))) /
						(2.0 *oneseedspike);
#else  //if not TAXI_CAB_METRIC  use Euclidean metric.
					distance = sqrt(
                    	(i-(horz_count_2D/2.0))*(i-(horz_count_2D/2.0))+
						(j-(vert_count_2D/2.0))*(j-(vert_count_2D/2.0)));
					distance /= oneseedspike;
					if (distance > 1.0)
						wave_past_plane[c].variable[k] =
							wave_source_plane[c].variable[k] = 0.0;
					else
						wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] =
							_max_intensity.Val() * (1.0 - distance);
				}
#endif //TAXI_CAB_METRIC
			 }
	}
 	generator_ptr->Seed();
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target
}

void CA::Zeroseed()
{
	int i,j;

	if (dimension == 1)
  {
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = 0;
			past_row[i] = 0;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					0.0;
  	}
		Resetfreq();
  }
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (j = 0; j < CY_2D; j++)
			for (i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] =	0.0;
				c++;
			}
	}
  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
}

void CA::Halfmaxseed()
{
	int i,j;
	int int_halfmax = states / 2;
	Real halfmax = 0.5*MaxIntensity();

	if (dimension == 1)
  {
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = int_halfmax;
			past_row[i] = int_halfmax;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					halfmax;
  	}
		Resetfreq();
  }
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (j = 0; j < CY_2D; j++)
			for (i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] =	halfmax;
				c++;
			}
	}
  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
}

void CA::BitmapSeed() //only works for 2-D CAs
//mike 10-97
//Uses a resource bitmap as key to seeding the cells. If the pixel is black, the cell
//will be changed to a greater intensity.
{
	int i, j, k;
	bool hit;
	HBITMAP hBitmap, oldhBitmap;
	HDC hdcMem;
	if (dimension==2)
	{
		hBitmap = LoadBitmap(hInst, "SEEDBITMAP");
		if (hBitmap==NULL)
			MessageBox(masterhwnd, "SeedBitmap(): hBitmap==NULL", "error", MB_OK);
		else
		{
			hdcMem = CreateCompatibleDC(NULL);
			if (hdcMem==NULL)
				MessageBox(masterhwnd, "SeedBitmap(): hdcMem==NULL", "error", MB_OK);
			oldhBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

			Zeroseed();  //set cells to zero.
			int c = 0;
			for (j = 0; j < CY_2D; j++)
				for (i = 0; i < CX_2D; i++)
				{
					for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					{
						if (GetPixel(hdcMem, i, j) == 0) //black
						wave_past_plane[c].variable[k] =
							wave_source_plane[c].variable[k] =	0.5*MaxIntensity();
					}
					c++;
				}
		}
		SelectObject(hdcMem, oldhBitmap);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
	}

  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target
}

#else //BLANK_HELPERS is turned on


void CA::RandomSeed()
{ //The most obvious kind of random seeding.
	if (dimension == 1)
	{
		// Reset all the rows to the top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (int i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = (unsigned char)Random((unsigned short)states);
			past_row[i] = (unsigned char)Random((unsigned short)states);
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			wave_past_row[i].variable[0] =
				wave_source_row[i].variable[0] =
				0.5 * Randomsignreal() * _max_intensity.Val();
			for (int k = 1; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					0.0;
		/* Need to put consistent colorstates pattern into source_row
		for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));

			}
		}
		Resetfreq();
	}
	else //2D case
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (int j = 0; j < CY_2D; j++)
			for (int i = 0; i < CX_2D; i++)
			{
				wave_past_plane[c].variable[0] =
				wave_source_plane[c].variable[0] =
					0.5 * Randomsignreal() * _max_intensity.Val();
				for (int k = 1; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
					wave_source_plane[c].variable[k] =
						0.0;
				c++;
			}
	}
	generator_ptr->Seed();
	Reset_smoothsteps(SMOOTHRANDSTEPSFACTOR);
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}


void CA::FourierSeed()
{ /*Seed the wave values with a Fourier sum of sine waves.  The idea
is to use FOURIER_SEED_TERMS many sine terms, each with a frequency
between 1 and FOURIER_SEED_MAX_FREQ.  Arrange it so that their
amplitudes sum to fourier_amplitude_sum times max_intensity, and do
this in such a way that one term is likely to get most of the weight.*/
	Real fourier_frequency[FOURIER_SEED_TERMS];
	Real fourier_amplitude[FOURIER_SEED_TERMS];
	Real fourier_frequency_X_2D[FOURIER_SEED_TERMS_X_2D]
		[FOURIER_SEED_TERMS_Y_2D];
   // next line: l.andrews 11/3/01 changed from FOURIER_SEED_TERMS to FOURIER_SEED_TERMS_Y_2D
   // and inverted the dimensions to agree with indexing in code --THIS MAY BE WRONG --
   // IT IS POSSIBLE THAT THEN INDICES SHOULD BE INVERTED IN THE CODE INSTEAD !
	// RR, May 17, 2007.  I think the change here is okay.  We're just making sure hte allocation
	//is big enough.  Hopefully the code is okay.
	Real fourier_frequency_Y_2D[FOURIER_SEED_TERMS_X_2D]
		[FOURIER_SEED_TERMS_Y_2D];
	Real fourier_amplitude_2D[FOURIER_SEED_TERMS_Y_2D]
		[FOURIER_SEED_TERMS_Y_2D];
	Real fourier_amplitude_sum = 1.0; //Try 0.85 if too high.
	int i,j, k, c;

	if (dimension == 1)
	{
		if (!horz_count) //avoid divide by zero.
			return;
//Start 1D Setup
		//reset all rows to top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
//End 1D Setup	
	     for (j=0; j<FOURIER_SEED_TERMS; j++)
		{
			fourier_frequency[j] = 1 + Random(FOURIER_SEED_MAX_FREQ);
			fourier_frequency[j] *=	(2.0 * PI)/(horz_count);
				/*Scale it for use in an expression of form
				sin(fourier_frequency[j] * i), with i between 0 and
				horz_count.  The idea is to do the unscaled
				fourier_frequency[j] many full sinewave up/down
				oscillations across the width of the visible CA*/
			//Before that, just get a piece of what amplitude's left.
			fourier_amplitude[j] = Randomreal() * fourier_amplitude_sum;
			fourier_amplitude_sum -= fourier_amplitude[j];
			//Subtract off what you took.
			fourier_amplitude[j] *= _max_intensity.Val();
			//Scale your amplitude to fit the CA.
		}
		//Give the last term all of the amplitude that's left over.
		fourier_amplitude[FOURIER_SEED_TERMS-1] += fourier_amplitude_sum;
		for (i = 0; i < _max_horz_count; i++)
		{
			//Sum the Fourier series
			wave_source_row[i].variable[0] = 0.0;
			for (j=0; j< FOURIER_SEED_TERMS; j++)
				wave_source_row[i].variable[0] +=
			       	fourier_amplitude[j] *
					sin(fourier_frequency[j] * i);
			wave_past_row[i].variable[0] = wave_source_row[i].variable[0];
			//Start k loop on variable[]
			for (int k = 1; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] = wave_source_row[i].variable[k] = 0.0;
			//End k loop on variable[]
		}
//Start 1D Cleanup
		for (i = 0; i < _max_horz_count; i++)
		{
			//Do the digital seed with pure randomness.
 			source_row[i] = (unsigned char)Random((unsigned short)states);
			past_row[i] = (unsigned char)Random((unsigned short)states);
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
	/* Need to put consistent colorstates pattern into source_row
	for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));
			}
		}
		Resetfreq();
//End 1D Cleanup
	}
	else //dimension 2
	{
		if (!horz_count_2D || !vert_count_2D) //avoid divide by zero.
			return;
// Start 2D Setup
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
//End 2D Setup
//Start k loop on 2D variable[]
	    for (i=0; i<FOURIER_SEED_TERMS_X_2D; i++)
		{
			for (j=0; j<FOURIER_SEED_TERMS_Y_2D; j++)
			{
				fourier_frequency_X_2D[i][j] =
					1 + Random(FOURIER_SEED_MAX_FREQ_2D);
				fourier_frequency_Y_2D[i][j] =
					1 + Random(FOURIER_SEED_MAX_FREQ_2D);
				fourier_frequency_X_2D[i][j] *=
					(2.0 * PI)/(horz_count_2D);
				fourier_frequency_Y_2D[i][j] *=
					(2.0 * PI)/(vert_count_2D);
				/* Scale for use in an expression of form
				sin(fourier_frequency_X_2D[i][j] * x) *
				sin(fourier_frequency_Y_2D[i][j] * y), with
				x between 0 and horz_count_2D and
				y between 0 and horz_count_2D. See comments above
				in the dimension 1 case for the rest of this.*/
				fourier_amplitude_2D[i][j] = Randomreal() *
					fourier_amplitude_sum;
				fourier_amplitude_sum -= fourier_amplitude_2D[i][j];
				fourier_amplitude_2D[i][j] *= _max_intensity.Val();
			}
		}
	//Give the last term all of the amplitude that's left over.
    	fourier_amplitude_2D[FOURIER_SEED_TERMS_X_2D-1]
		[FOURIER_SEED_TERMS_Y_2D-1] += fourier_amplitude_sum;
		for (int y = 0; y < CY_2D; y++)
		{
			c = index(0, y);
			for (int x = 0; x < CX_2D; x++)
			{
			//Do the Fourier sum
				wave_source_plane[c].variable[0] = 0.0;
				for (i=0; i< FOURIER_SEED_TERMS_X_2D; i++)
					for (j=0; j<FOURIER_SEED_TERMS_Y_2D; j++)
     			       	wave_source_plane[c].variable[0] +=
							fourier_amplitude_2D[i][j] *
							sin(fourier_frequency_X_2D[i][j] * x) *
							sin(fourier_frequency_Y_2D[i][j] * y);
				wave_past_plane[c].variable[0] =
					wave_source_plane[c].variable[0];
				for (int k = 1; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] = 0.0;
				c++;
			}
		}
	} //end dimension 2
//Common cleanup
	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function exists, yet //mike
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}

void CA::Sineseed()
{
	if (dimension == 1)
	{
		// Reset all the rows to the top
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (int i = 0; i < _max_horz_count; i++)
		{
/* Let's set it for two complete sinewave cycles to fit into the
size of the CA window at seed time.  So I want to cover 4*PI in
the horz_count steps)*/
			source_row[i] = (unsigned char)(((Real)states/2.0) +
				sin((i * 4.0 * PI)/(horz_count)) * ((Real)states/2.0));
			if (source_row[i]>states-1)
				source_row[i] = (unsigned char)(states-1);
			past_row[i] = (unsigned char)(((Real)states/2.0) +
				sin((i * 4.0 * PI)/(horz_count)) * ((Real)states/2.0));
			if (past_row[i]>states-1)
				past_row[i] = (unsigned char)(states-1);
			wave_past_row[i].variable[0] = wave_source_row[i].variable[0] =
					0.5*sin((i * 4.0 * PI)/(horz_count)) *
					_max_intensity.Val();
			for (int k = 1; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] = wave_source_row[i].variable[k] =
					0.0;
		/* Need to put consistent colorstates pattern into source_row
		for meaningful Avoidstripes check in wave cases */
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			{
				if (!(showvelocity))
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));
				else
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].velocity +
					_max_velocity.Val())) / (2.0 * _max_velocity.Val()));
			}
		}
		Resetfreq();
		generator_ptr->Seed();
		//generatorlist.Seed(); //seed function not defined, yet //mike
	}
	else //dimension 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (int j = 0; j < CY_2D; j++)
			for (int i = 0; i < CX_2D; i++)
			{
				wave_past_plane[c].variable[0] =
				wave_source_plane[c].variable[0] =
					0.5*sin((i * 4.0 * PI)/(horz_count_2D)) *
					sin((j * 4.0 * PI)/(vert_count_2D)) *
					_max_intensity.Val();
				for (int k = 1; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
					wave_source_plane[c].variable[k] = 0.0;
				c++;
			}
	 //	Resetfreq();
  //		generator_ptr->Seed();
 	}
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target

}

void CA::Oneseed()
{

	int  oneseedspike = 20; //Default half width of spike
	int i, j;
	Real distance;
	int c;

	if (dimension == 1)
	{
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = 0;
			past_row[i] = 0;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] = wave_source_row[i].variable[k] = 0.0;
			if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
				source_row[i] = (unsigned char) (((MAX_COLOR-1) *
					(wave_source_row[i].variable[0] +
					_max_intensity.Val())) / (2.0 * _max_intensity.Val()));

		}
		source_row[horz_count/2] = 1;
		if (oneseedspike > horz_count/2)
			oneseedspike = horz_count /4;
		for (i=(horz_count/2)-oneseedspike; i< (horz_count/2) + oneseedspike;
			i++)
		{
			wave_past_row[i].variable[0] =
			wave_source_row[i].variable[0] =
				_max_intensity.Val() * (1.0 -
				fabs(i - (horz_count/2.0)) / (Real)(oneseedspike));
			for (int k = 1; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] = 0.0;
		 }
	/* Need to put consistent colorstates pattern into source_row
	for meaningful Avoidstripes check in wave cases */
		if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
			source_row[horz_count/2] = (unsigned char) (((MAX_COLOR-1) *
				(wave_source_row[horz_count/2].variable[0] +
				_max_intensity.Val())) / (2.0 * _max_intensity.Val()));

		Resetfreq();
	}
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;

		for (j = 0; j < CY_2D; j++)
		{
			c = index(0,j);
			for (i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] = 0.0;
				c++;
			}
		}
		oneseedspike = 20;
		if (oneseedspike > horz_count_2D/2)
			oneseedspike = horz_count_2D /4;
		if (oneseedspike > vert_count_2D/2)
			oneseedspike = vert_count_2D /4;
		for (j=(vert_count_2D/2)-oneseedspike;
			j< (vert_count_2D/2) + oneseedspike; j++)
			for (i=(horz_count_2D/2)-oneseedspike;
			i< (horz_count_2D/2) + oneseedspike; i++)
			{
				c = index(i,j);
#ifdef TAXI_CAB_METRIC
					wave_past_plane[c].variable[0] =
					wave_source_plane[c].variable[0] =
						max_intensity *
						(fabs(i - (horz_count_2D/2.0)) +
						fabs(j - (vert_count_2D/2.0))) /
						(2.0 *oneseedspike);
#else  //if not TAXI_CAB_METRIC  use Euclidean metric.
					distance = sqrt(
                    	(i-(horz_count_2D/2.0))*(i-(horz_count_2D/2.0))+
						(j-(vert_count_2D/2.0))*(j-(vert_count_2D/2.0)));
					distance /= oneseedspike;
					if (distance > 1.0)
						wave_past_plane[c].variable[0] =
							wave_source_plane[c].variable[0] = 0.0;
					else
						wave_past_plane[c].variable[0] =
						wave_source_plane[c].variable[0] =
							_max_intensity.Val() * (1.0 - distance);
#endif //TAXI_CAB_METRIC
			 }
	}
 	generator_ptr->Seed();
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target
}

void CA::Zeroseed()
{
	int i,j;

	if (dimension == 1)
  {
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = 0;
			past_row[i] = 0;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			for (int k = 0; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					0.0;
  	}
		Resetfreq();
  }
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (j = 0; j < CY_2D; j++)
			for (i = 0; i < CX_2D; i++)
			{
				for (int k = 0; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] =	0.0;
				c++;
			}
	}
  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
}

void CA::Halfmaxseed()
{
	int i,j;
	int int_halfmax = states / 2;
	Real halfmax = 0.5*MaxIntensity();

	if (dimension == 1)
  {
		// Reset all the rows
		source_row = rowbuffer[0];
		sourcerowindex = 0;
		target_row = rowbuffer[1];
		targetrowindex = 1;
		past_row = rowbuffer[MEMORY - 1];
		pastrowindex = MEMORY - 1;
		wave_source_row = waverowbuffer[0];
		wavesourceindex = 0;
		wave_target_row = waverowbuffer[1];
		wavetargetindex = 1;
		wave_past_row = waverowbuffer[2];
		wavepastindex = 2;
		for (i = 0; i < _max_horz_count; i++)
		{
			source_row[i] = int_halfmax;
			past_row[i] = int_halfmax;
			wave_past_row[i].velocity = wave_source_row[i].velocity = 0.0;
			wave_past_row[i].variable[0] =
				wave_source_row[i].variable[0] =
					halfmax;
			for (int k = 1; k < VARIABLE_COUNT; k++)
				wave_past_row[i].variable[k] =
				wave_source_row[i].variable[k] =
					0.0;
  	}
		Resetfreq();
  }
	else //dimension == 2
	{
		// Reset all the rows to the top
		wave_source_plane = waveplanebuffer[0];
		wavesourceindex = 0;
		wave_target_plane = waveplanebuffer[1];
		wavetargetindex = 1;
		wave_past_plane = waveplanebuffer[2];
		wavepastindex = 2;
		int c = 0;
		for (j = 0; j < CY_2D; j++)
			for (i = 0; i < CX_2D; i++)
			{
				wave_past_plane[c].variable[0] =
					wave_source_plane[c].variable[0] =	halfmax;
				for (int k = 1; k < PLANE_VARIABLE_COUNT; k++)
					wave_past_plane[c].variable[k] =
						wave_source_plane[c].variable[k] =	0.0;
				c++;
			}
	}
  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
}

void CA::BitmapSeed() //only works for 2-D CAs
//mike 10-97
//Uses a resource bitmap as key to seeding the cells. If the pixel is black, the cell
//will be changed to a greater intensity.
{
	int i, j, k;
	bool hit;
	HBITMAP hBitmap, oldhBitmap;
	HDC hdcMem;
	if (dimension==2)
	{
		hBitmap = LoadBitmap(hInst, "SEEDBITMAP");
		if (hBitmap==NULL)
			MessageBox(masterhwnd, "SeedBitmap(): hBitmap==NULL", "error", MB_OK);
		else
		{
			hdcMem = CreateCompatibleDC(NULL);
			if (hdcMem==NULL)
				MessageBox(masterhwnd, "SeedBitmap(): hdcMem==NULL", "error", MB_OK);
			oldhBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

			Zeroseed();  //set cells to zero.
			int c = 0;
			for (j = 0; j < CY_2D; j++)
				for (i = 0; i < CX_2D; i++)
				{
					if (GetPixel(hdcMem, i, j) == 0) //black
					wave_past_plane[c].variable[0] =
						wave_source_plane[c].variable[0] =
						0.5*MaxIntensity();
					c++;
				}
		}
		SelectObject(hdcMem, oldhBitmap);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
	}

  	generator_ptr->Seed();
	//generatorlist.Seed();  //no seed function defined, yet //mike
	generatorlist.Reset();
	Setwrapflag(wrapflag); //In case of WF_ZERO or WF_FIXED you need to match past/source/target
}

#endif //BLANK_HELPERS case