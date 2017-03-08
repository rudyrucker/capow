/*******************************************************************************
	FILE:				camore.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains additional CA class 
	                    function defintions.

	UPDATE LOG:			9-26-97
*******************************************************************************/
//====================INCLUDES===============

#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include "status.hpp"
#include "random.h"
#include "capowgl.hpp"
#include <math.h>
#include "userpara.hpp"
#include <lzexpand.h> // for compressing/decompressing files

//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============

//====================EXTERNAL DATA===============
extern HWND hDlgCycle;
extern HWND masterhwnd;
extern HWND hwndStatusBar;
extern char CA_STYLE_NAME[]; // temp buffer for userrule name, lives in CAPOW.CPP	
extern CapowGL *capowgl;

//================================================================

void CA::Changeradiusandstates(int iradius, int istates)
{
	/* Waves always have 256 states with radius of 1.
	We also give them 256 nabeoptions, and just view the state as
	the nabecode.  (Be careful never to try and allocate a Lookuptable for
	256 states radius 1.)*/
	if (type_ca != CA_STANDARD && type_ca != CA_REVERSIBLE)
	{
/*		if (states != 256 && //You are changing out of STANDARD or REVERSIBLE
			states != 0) //And this is not your first call to this method
		{
			oldradius = radius;
			oldstates = states;
		}
*/		radius = 1;
		states = 256;
		nabeoptions = 256; //Do this so I can do Entropy on Waves.
		nabesize = 0;
		return;
	}
	//At this point you must be STANDARD or REVERSIBLE.
	//If you are changing out of a WAVE type, put good args in.
 	if (states == 256)
	{
		iradius = 1;
		istates = 16;
	}

	//If an arg is -1, this means you're not changing this one.
	if (iradius == -1)
		iradius = radius;
	if (istates == -1)
		istates = states;
/* if you just want to change just radius or states, put a -1 in the
	field you don't want to change */

	if (radius == iradius && states == istates)
		return;

	if ( (istates > 16) || (iradius==2 && istates>4) ||
		 (iradius==3 && istates>4) ||
		(iradius>=4 && istates > 2) || (iradius > 6))
		return;  //No can do!

	radius = iradius;
	states = istates;
	statebits = log(states)/log(2.0); //log base two is number of bits.
	nabesize = 1 + 2*radius;
	nabeoptions = pow(states, nabesize);
	mask = (unsigned short)(nabeoptions - 1);
	Resetfreq();
	Lambdalookup(); //Randomize the lookup table
	Setcolortable();
	Seed();
	score = 0.0;    // Reset the score
}

/*  I use all combos where lookup is less than or equal to 16K

States  2               4                       8                       16                      32
Radius
1                       8               64                      512             4 K             32 K
2                       32              1 K             32 K            1 Meg           32 Meg
3                       128             16 K            2 Meg           256 Meg 32 Gig
4                       512     256 K           16 Meg  4 Gig           32 Ter
5                       2 K     4 Meg           8 Gig           16 Ter  32 Pet
6                       8 K     64 Meg  512 Gig 4 Pet           32 Ex
7                       32 K    1 Gig           32 Ter  1 Ex            32 K Ex


*/

/* This checks if the CAs are compatible with eachother.
	Since Standard and Reversible both use lookuptable, they can
	be exchange lookuptable information.
	Waves and Heatwaves can exchange velocity and intensity information */
unsigned char Compatible(int type1, int type2)
{
	if ((type1 == CA_STANDARD || type1 == CA_REVERSIBLE) &&
		(type2 == CA_STANDARD || type2 == CA_REVERSIBLE))
		return 1;
	if ((type1 != CA_STANDARD && type1 != CA_REVERSIBLE) &&
		(type2 != CA_STANDARD && type2 != CA_REVERSIBLE))
		return 1;
	return 0;
}

/* This copies everything! I replaced Copytable with this version.*/
void CA::CopyCA(CA *goodCA)
{
	int i;

	if (!Compatible(type_ca,goodCA->type_ca))
		return;

	Changeradiusandstates(goodCA->radius, goodCA->states);
	for(i=0; i<nabeoptions; i++)
		lookup[i] = goodCA->lookup[i];

	for (i = 0; i < _max_horz_count; i++)
	{
		source_row[i] = goodCA->source_row[i];
		past_row[i] = goodCA->past_row[i];
		wave_source_row[i].intensity = goodCA->wave_source_row[i].intensity;
		wave_source_row[i].velocity = goodCA->wave_source_row[i].velocity;
		wave_past_row[i].intensity = goodCA->wave_past_row[i].intensity;
		wave_past_row[i].velocity = goodCA->wave_past_row[i].velocity;
	}

	for(i = 0; i < (int)CX_2D * CY_2D; i++) //copy the WaveCell info with 
	{				
		wave_source_plane[i] = goodCA->wave_source_plane[i];
		wave_past_plane[i] = goodCA->wave_past_plane[i];
		wave_target_plane[i] = goodCA->wave_target_plane[i]; //2017 to be safe about full image copy. Maybe unnecessary?
	}

	lambda = goodCA->lambda;
	entropy= goodCA->entropy;
	target_entropy = goodCA->target_entropy;

	_dt = goodCA->_dt;
	_dx = goodCA->_dx;
	band_count = goodCA->band_count;
	_max_intensity = goodCA->_max_intensity;
	_max_velocity = goodCA->_max_velocity;
	_friction_multiplier = goodCA->_friction_multiplier;
	_spring_multiplier = goodCA->_spring_multiplier;
	_mass = goodCA->_mass;
	_driver_multiplier = goodCA->_driver_multiplier;
	_frequency_multiplier = goodCA->_frequency_multiplier;
	Adjust_acceleration_multiplier();

	for (i = 0; i < MAX_COLOR; i++)
		colortable[i]= goodCA->colortable[i];
	for (i = 0; i < MAX_COLOR; i++)
		colortable[i]= goodCA->colortable[i];
	Settype(goodCA->type_ca); //2017
	if (goodCA->type_ca == CA_USER)
	{

		// This way is hard but will always success.
		_lpfnUSERRULE_1 = goodCA->_lpfnUSERRULE_1;
		_lpfnUSERRULE_3 = goodCA->_lpfnUSERRULE_3;
		_lpfnUSERRULE_5 = goodCA->_lpfnUSERRULE_5;
		_lpfnUSERRULE_9 = goodCA->_lpfnUSERRULE_9;
		removeUserParam(this, TRUE); //Removes all the TweakParam fields, TRUE means remove the variance field too.
		Real v;
		Real min;
		Real max;
		char *c;
		for (int count = 0; count < goodCA->userParamAdd.size(); count++)
		{
			if (count == 0) 	// 2017 Slam the target variance at slot 0. //Crude attempted fix of high variances from loaded files in 2017.
			{
				v = goodCA->userParamAdd[0]->Val();
				if (v > DEFAULT_VARIANCE) v = DEFAULT_VARIANCE;
				min = MIN_VARIANCE;
				max = MAX_VARIANCE;
				goodCA->userParamAdd[0]->SetVal(v); //I want to stamp out high variances.
				userParamAdd[0]->SetMin(min); //2017 in the 2007 build we weren't copying the range.
				userParamAdd[0]->SetMax(max);
			} //Now go on and copy the field whether count is 0 or not.
			v = goodCA->userParamAdd[count]->Val();
			min = goodCA->userParamAdd[count]->Min();
			max = goodCA->userParamAdd[count]->Max();
			char *c = goodCA->userParamAdd[count]->Label();
			//Now use the v, min, max, and c.
			(*pAddUserParam)(this, c, v); //(*pAddUserParam) is a weird way of calling the function AddUserParam
			userParamAdd[count]->SetMin(min); //2017 in the 2007 build we weren't copying the range.
			userParamAdd[count]->SetMax(max);
		}
		lstrcpy(_userrulename, goodCA->_userrulename); //So install the file name,
		type_ca = CA_USER;
	}
}

void CA::Changelambda(Real increment)
{
	int i;
	Real oldlambda, flip_prob;

	oldlambda = lambda;
	lambda += increment;
	if (lambda > MAX_LAMBDA)
		lambda = MAX_LAMBDA;
	if (lambda < MIN_LAMBDA)
		lambda = MIN_LAMBDA;

	if (increment > 0) //increase lambda
	{
		flip_prob = (lambda - oldlambda)/(1.0 - oldlambda);
/* Because lambda = ((1.0 - oldlambda)*flip_prob*nabeoptions+
oldlambda*nabeoptions) / nabeoptions; or
lambda = (1-oldlambda)*flip_prob + oldlambda.*/
		for (i=0; i<nabeoptions; i++)
			if (!lookup[i] && Randomreal()< flip_prob )
				lookup[i] = (unsigned char)
            	(1 + Random((unsigned short)(states - 1)));
	}
	else //decrease lambda
	{
		flip_prob = (oldlambda - lambda)/(oldlambda);
/* Because lambda = (oldlambda*nabeoptions -
oldlambda*nabeoptions*flip_prob) / nabeoptions; or
lambda = oldlambda - oldlamda*flip_prob.*/
		for (i=0; i<nabeoptions; i++)
			if (lookup[i] && Randomreal()< flip_prob )
				lookup[i] = 0;
	}
	Computeactual_lambda();
	Resetfreq();
}

void CA::Changeentropy(Real newentropy)
{
	target_entropy = newentropy;
}

void CA::Become_child_of(CA *mom, CA *dad)
{
	int i, breakpoint, momfirst;

	if (!Compatible(mom->type_ca, dad->type_ca))
		return;

	Settype(mom->type_ca);

	if (mom->type_ca == CA_STANDARD ||
		 mom->type_ca == CA_REVERSIBLE)
	{
		Changeradiusandstates(mom->radius, mom->states);
		if (dad->nabeoptions < nabeoptions) //Engulf dad.
		{
			for(i = 0; i<dad->nabeoptions; i++)
				lookup[i] = (unsigned char)(dad->lookup[i] % states);
			for (i = dad->nabeoptions; i<nabeoptions; i++)
				lookup[i] = mom->lookup[i];
		}
		else //crossover with dad
			{
				momfirst = Random(2);
				breakpoint =(int)Randomlong(nabeoptions);
				if (momfirst)
				{
					for (i=0; i<breakpoint; i++)
						lookup[i] = mom->lookup[i];
					for (i=breakpoint; i<nabeoptions; i++)
						lookup[i] = (unsigned char)(dad->lookup[i] % states);
				}
				else
				{
					for (i=0; i<breakpoint; i++)
						lookup[i] = (unsigned char)(dad->lookup[i] % states);
					for (i=breakpoint; i<nabeoptions; i++)
						lookup[i] = mom->lookup[i];
				}
			}
		for (i=0; i < states; i++)
		{
			if (Random(2))
				colortable[i] = mom->colortable[i];
			else
				colortable[i] = dad->colortable[i];
		}
		Computeactual_lambda();
	}
	else
	{
//---CA_wave
		//_dt.SetVal((mom->_dt.Val() + dad->_dt.Val())/2.0);
		SetTweakParam (&_dt, (mom->_dt.Val() + dad->_dt.Val())/2.0);
		SetTweakParam(&_dx, (mom->_dx.Val() + dad->_dx.Val()) / 2.0);
		Adjust_acceleration_multiplier();
	}
}

void CA::Entropy()
{
	int i;
	Real sum = 0.0;
	Real temp;

#ifdef FAST_ENTROPY
	Real logcellcount;
#else
	Real probability;
#endif //FAST_ENTROPY

	if (entropyflag)
	{
// This is based on the definition of entropy as the negtative sum over
// the cases of the probability of each case times the log base two
// of that probability.  Use negative because the log of a probability,
// which is less than 1, is less than 1.  See next comment for why
// we don't have to divide each term by LOG2.
		if (!cellcount) //just in case
		{
			entropy = 0.0;
			return;
		}
#ifdef FAST_ENTROPY
/*This is an idea for a faster computation based on some twiddling
with the formula.  Instead of 1/log(nabeoptions) * Sum 1 to nabeoptions of
- freqlookup[i]/cellcount* log(freqlookup[i]/cellcount); I have
1/cellcount*log(nabeoptions) * Sum 1 to nabeoptions of - freqlookup[i] * (
log(freqlookup[i]) - log(cellcount)); and I store log(cellcount) at the
start to cut down on the computations.*/
		logcellcount = log((Real)cellcount); //We know cellcount != 0.
		for (i=0; i<nabeoptions; i++)
		{
			// Stay away from log(0.0).
			if (freqlookup[i])
				sum -= (Real)freqlookup[i] * ( log(freqlookup[i]) -
					logcellcount );
		}

		if (nabeoptions <= 1) //just in case
			entropy = 0.0;
		else
			entropy = sum / ((Real)cellcount * log((Real)nabeoptions) );
#else //The tried and true method
		for (i=0; i<nabeoptions; i++)
		{
			probability = (Real)(freqlookup[i]) / (Real)cellcount;
			// Stay away from log(0.0).
			if (probability != 0.0)
				sum -= probability * (log(probability));
			freqlookup[i] = 0; //reset for the next time.
		}
		// Note that I give the colony's target entropy as a percent between
		// 0 and 1.  The max actual entropy per bug action is
		// log-base-two of the number of possible outcomes, so the
		// percent entropy is actual entropy value of the sum
		// over the max possible entropy per bug action.  The
		// divisions by LOG2 (to scale to base two logarithm) cancel
		// each other out.
		if (nabeoptions <= 1) //just in case
			entropy = 0.0;
		else
			entropy = sum / log((Real)nabeoptions);
#endif //FAST_ENTROPY

//              fitness = 1.0 - fabs(entropy - target_entropy);
//              calist_ptr->Sortfitness(); /*update CAlist's bestinlist and
//                      secondbestinlist indices*/
//              if (fitness < HIGH_FITNESS)
//                      calist_ptr->Evolve(*this);

	} //End of entropyflag case.
	Resetfreq();

	if (calist_ptr->breedflag)      // Only keep score if breed cycle is enabled
	{
		temp = target_entropy - entropy;
		if (temp < 0)
			temp *= -1.0;   /* I would use abs(target_entropy-entropy), but abs()
							uses integers only */
		score += entropy_bonus * (1.0 - (temp));        /* the closer you are to
			the target_entropy, the more bonus you get */
	}

	if (calist_ptr->FocusCA() == this)
		if (hDlgCycle)
			SendMessage(hDlgCycle, WM_COMMAND, SC_UPDATE, 0L);
}

void CA::Avoidstripes()
{
	unsigned char stripe = 1;
	int i, j, k, lightcone, rowi;

/*Added this first check on 10/6/97, no longer check stripes for analog CAs. */
	if (type_ca != CA_STANDARD)
		return;
	//First check if you even care if you fail the stripe test or not.
	if (!( (calist_ptr->breedflag && fail_stripe) ||
		calist_ptr->stripekillflag ||
		calist_ptr->stripeseedflag ) )
			return;

	//First check if the row is all the same color
	for (i=radius; i<horz_count-radius; i++)
		if (source_row[i] != source_row[radius])
		{
			stripe = 0; //Not all the same in row
			break; //Bail from this i loop.
		}
	if (stripe) //Change the CA if it fails the test
	{
		if (type_ca != CA_REVERSIBLE && calist_ptr->stripeseedflag)
			Seed();

		if (calist_ptr->stripekillflag)
			Lambdalookup();
		else
		{
			if (calist_ptr->breedflag)      // Only keep score if breed cycle is enabled
				score += fail_stripe;   // penalty for fail stripe
		}
		if (calist_ptr->FocusCA() == this)
			update_flag = 1;                                        // update combo boxes
		return;
	}
	/* Now check for each of the rows in the rowbuffer if
		that row is a displacement of the sourcerow.  The possible
		displacements range up to radius * steps between rows. */
	for (j = sourcerowindex - 1; j>=0; j--)
	{ //Note that sourcerowindex = MEMORY - 1 when you call Avoidstripes.
		lightcone = radius * (sourcerowindex - j);
		for (k = -lightcone; k<= lightcone; k++)
		{
			stripe = 1; //Assume that it is a copy
	/* Now we want to compare all the cells in the row, with
	the rowbuffer row displaced by k cells.  Use rowi to
	index the displaced cells. The comments below trace through
	what would happen in the special case UPOS k=1 and UNEG k=-1
	just to make sure*/
			if (k<0)
			{
				rowi = horz_count+k; //UNEG rowi=horz_count-1
				if (rowi<0)
					continue; /* For a very small window, it might happen
				that horz_count is smaller than lightcone, which is
				MEMORY-1 * radius at the most.  Allowing a negative rowi
				would cause a crash when used as index. Continue skips
				to the next stage of the k loop.*/
			}
			else // k>= 0
				rowi = k;//UPOS rowi=1
			for (i=0; rowi<horz_count; i++)
			{
				if (source_row[i] != rowbuffer[j][rowi])
				{
					stripe = 0; //Not a copy
					break; //Bail from this i loop
				}
				rowi++;
			} //UNEG i=1 now, UPOS i=horz_count-1 now.
			if (stripe) //If didn't turn stripe off yet, keep checking.
			{
				rowi = 0; //UNEG i=1 is right, UPOS
				for (; i<horz_count; i++)
				{
					if (source_row[i] != rowbuffer[j][rowi])
					{
						stripe = 0; //Not a copy
						break; //Bail from this i loop
					}
					rowi++;
				}
			}
			if (stripe) //The whole row matched; change the CA & bail.
			{
				if (type_ca != CA_REVERSIBLE && calist_ptr->stripeseedflag)
					Seed();
				if (calist_ptr->stripekillflag)
					Lambdalookup();
				else
				{
					if (calist_ptr->breedflag)      // Only keep score if breed cycle is enabled
						score += fail_stripe;   // penalty for fail stripe
				}
				if (calist_ptr->FocusCA() == this)
					update_flag = 1;                                        // update combo boxes
				return;
			}
		}
	}
}
/* This function takes a Realing point and puts the decimal
part into decimal and the fractional part into fraction */
void Breakpoint(Real number, int &decimal, int &fraction)
{
	decimal = number;       // Decimal
	number -= decimal;      // chop off decimal part
	number *= 100;  // bump fractional part of number over the decimal place
	fraction = number;      // Fractional
	number -= fraction;
	if (number >= .5)
		++fraction;     // Round up fraction
	if (fraction > 99)
	{
		fraction -= 100; ++fraction;    // Round up > .9, add to decimal
	}
	if (fraction < 0)
		fraction *= -1; // Dont show the decimal part as negative!
}

/* This function takes two ints decimal and fraction and returns a
Realing point number that is decimal.fraction */
Real Makepoint(int decimal, int fraction)
{
	Real number;
	number = decimal;
	number += fraction * 0.01;
	return (number);
}

void CA::Computeactual_lambda()
{
	int nonzero = 0;

	if (Gettype() != CA_STANDARD && Gettype() != CA_REVERSIBLE)
		return;

	for (int i = 0; i < nabeoptions; ++i)
		if (lookup[i])
			nonzero++;
	if (!nabeoptions)
		return;
	actual_lambda = (Real)nonzero/(Real)nabeoptions;
}

void CA::Symmetrize()
{
	int statesmask, sourcei, cell, mirrori;

	if (Gettype() != CA_STANDARD && Gettype() != CA_REVERSIBLE)
		return;

	statesmask = states-1;
	for (int i=0; i<nabeoptions; i++)
	{
		sourcei = i;
		mirrori = 0;
		for (int j=0; j<nabesize; j++)
		{
			mirrori <<= statebits;
			cell = statesmask & sourcei;
			sourcei >>= statebits;
			mirrori |= cell;
		}
//              assert(mirrori < nabeoptions);
		if (i < mirrori)
			lookup[mirrori] = lookup[i];
	}
}

void CA::Reverse(void)
{
#define NEWREV
#ifndef NEWREV
	int i;
	unsigned char temp;
//	Wavecell wavetemp;
//	Wavecell2 wave2temp;

	if (type_ca == CA_REVERSIBLE)
		for (i = 0; i<horz_count; i++)
		{
			temp = past_row[i];
			past_row[i] = source_row[i];
			source_row[i] = temp;
		}
	}
#else //NEWREV
	unsigned char *temp;
	Wavecell *wavetemp;
	Wavecell2 *wave2temp;
	if (type_ca == CA_REVERSIBLE)
	{
		temp = past_row;
		past_row = rowbuffer[pastrowindex] = source_row;
		source_row = rowbuffer[sourcerowindex] = temp;
	}
	if (_castyle == CA_WAVE || _castyle == CA_ULAM_WAVE ||
			_castyle == CA_CUBIC_ULAM_WAVE )
	{
		wavetemp = wave_past_row;
		wave_past_row = waverowbuffer[wavepastindex] = wave_source_row;
		wave_source_row = waverowbuffer[wavesourceindex] = wavetemp;
	}
	if (_castyle == CA_WAVE_2D)
	{
		wave2temp = wave_past_plane;
		wave_past_plane = waveplanebuffer[wavepastindex] = wave_source_plane;
		wave_source_plane = waveplanebuffer[wavesourceindex] = wave2temp;
	}
#endif //NEWREV
}

void CA::Setviewmode(int newmode)
{
	viewmode = newmode;
	switch (viewmode)
	{
		case IDC_DOWN_VIEW :
			row_number = miny;
			break;
		case IDC_SCROLL_VIEW :
			WBM->ClearSection(minx, miny, maxx, maxy);
			row_number = maxy - (calist_ptr->_blt_lines) + 1;
			break;
		case IDC_SPLIT_VIEW :
			//SCROLL and GRAPH both do this
			WBM->ClearSection(minx, miny,maxx, maxy);
			//SCROLL does this
			row_number = splity - (calist_ptr->_blt_lines) + 1;
			break;
		case IDC_GRAPH_VIEW:
			Reset_tp_all();
			WBM->ClearSection(minx, miny, maxx, maxy);
			break;
		case IDC_POINT_GRAPH:
			Reset_tp_all();
			WBM->ClearSection(minx, miny, maxx, maxy);
			break;
		default :
          	break;
	}
}

void CA::Settype(int newtype)
{
	BOOL reseed = FALSE;
	int old_catype = type_ca;
	int old_castyle = _castyle;
	int old_dimension = dimension;
		
	//if (type_ca == newtype && type_ca != CA_USER)
	//	return;
	type_ca = newtype;
	/*The weird usage in the switch cases is for doing member function pointers, 
	and was explained to rucker by Prof. Horstmann */
	ResetGenerationCount();
	switch(type_ca)
	{
		case CA_STANDARD:
			UpdateFunction = &CA::StandardUpdate;
			_castyle = CA_STANDARD;
			SetCAStyleName ( "Standard" );
			break;
		case CA_REVERSIBLE:
			UpdateFunction = &CA::ReversibleUpdate;
			_castyle = CA_REVERSIBLE;
			SetCAStyleName ( "Reversible" );
			break;
		case CA_HEATWAVE:
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::HeatInt1;
			_castyle = CA_HEATWAVE;
			SetCAStyleName ( "Driven Heat" );
			break;
		case CA_HEATWAVE2:
			UpdateFunction = &CA::WaveUpdate_5;
			UpdateCell_5 = &CA::HeatInt2;
			_castyle = CA_HEATWAVE;
			SetCAStyleName ( "Big Nabe Heat" );
			break;
		case CA_WAVE:
		case ALT_CA_WAVE2:
		case ALT_CA_WAVE:
			type_ca = ALT_CA_WAVE;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::AltWaveVelInt1;
			_castyle = CA_WAVE;
			SetCAStyleName ( "Wave Equation" );
			break;
		case CA_WAVE2:
			UpdateFunction = &CA::WaveUpdate_5;
			UpdateCell_5 = &CA::WaveVelInt2;
			_castyle = CA_WAVE;
			SetCAStyleName ( "Big Nabe Wave" );
			break;
		case CA_ULAM_WAVE:
		case ALT_CA_ULAM_WAVE:
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::AltUlamWave;
			_castyle = CA_ULAM_WAVE;
			SetCAStyleName ( "Quadratic Wave" );
			break;
		case CA_AUTO_ULAM_WAVE:
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::StableUlamWave;
			_castyle = CA_AUTO_ULAM_WAVE;
			SetCAStyleName ( "Boiling Wave" );
			break;
		case CA_CUBIC_ULAM_WAVE: //Cubic Ulam Wave.
			type_ca = CA_CUBIC_ULAM_WAVE;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::CubicUlamWave;
			_castyle = CA_CUBIC_ULAM_WAVE;
			SetCAStyleName ( "Cubic Wave" );
			break;
		case ALT_CA_OSCILLATOR:
		case CA_OSCILLATOR:
			type_ca = CA_OSCILLATOR;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::Oscillator;
			_castyle = CA_OSCILLATOR;
			SetCAStyleName ( "Oscillators" );			  
			break;
		case ALT_CA_DIVERSE_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
			type_ca = CA_DIVERSE_OSCILLATOR;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::DiverseOscillator;
			_castyle = CA_DIVERSE_OSCILLATOR;
			SetCAStyleName ( "Diverse Osc" );
			break;
//		case CA_OSCILLATOR_WAVE: Now use this number for CA_USER
		case ALT_CA_OSCILLATOR_WAVE:
			type_ca = ALT_CA_OSCILLATOR_WAVE;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::AltOscillatorWave;
			_castyle = CA_OSCILLATOR;
			SetCAStyleName ( "Wave Osc" );
			break;
		case CA_DIVERSE_OSCILLATOR_WAVE:
		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
			type_ca = ALT_CA_DIVERSE_OSCILLATOR_WAVE;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::AltDiverseOscillatorWave;
			_castyle = CA_DIVERSE_OSCILLATOR;
			SetCAStyleName ( "Diverse Wave Osc" );
			break;
		case CA_WAVE_2D:
			UpdateFunction = &CA::WaveUpdate2D;
			UpdateCell_5 = &CA::Wave2D;
			dimension = 2;
			_castyle = CA_WAVE_2D;
			SetCAStyleName ( "2D Wave" );
			break;
		case CA_HEAT_2D:
			UpdateFunction = &CA::WaveUpdate2D;
			UpdateCell_5 = &CA::Heat2D;
			dimension = 2;
			_castyle = CA_HEAT_2D;
			SetCAStyleName ( "2D Heat" );
			break;
		case CA_USER:
			/* the _user_nabesize and _user_catype should be set when you
			load the *.DLL.  Default is _user_nabesize 3, _user_catype CA_WAVE_2D */
			_castyle = _usercastyle;
			switch (_usernabesize)
			{
				case 1:
					UpdateFunction = &CA::NetworkUpdate;
					UpdateCell_1 = &CA::DLLRule_1;
					dimension = 2;
					break;
				case 5:
					switch (_castyle)
					{
						case CA_WAVE:
							UpdateFunction = &CA::WaveUpdate_5;
							dimension = 1;
							break;
						case CA_WAVE_2D:
						case CA_HEAT_2D:
						default:
							UpdateFunction = &CA::WaveUpdate2D;
							dimension = 2;
							break;
					}
					UpdateCell_5 = &CA::DLLRule_5;
					break;
				case 9:
					UpdateFunction = &CA::WaveUpdate2D_9;
					UpdateCell_9 = &CA::DLLRule_9;
					dimension = 2;
					break;
				case 3:
				default:
					UpdateFunction = &CA::WaveUpdate;
					UpdateCell_3 = &CA::DLLRule_3;
					dimension = 1;
					break;
			}
			GetUserRuleName ( CA_STYLE_NAME );
			SetCAStyleName ( CA_STYLE_NAME );

			break;
		default:
			reseed = 1;
			type_ca = ALT_CA_WAVE;
			UpdateFunction = &CA::WaveUpdate;
			UpdateCell_3 = &CA::AltWaveVelInt1;
			_castyle = CA_WAVE;
			SetCAStyleName ( "Wave Equation" );
			break;
	}
	if (_castyle == CA_WAVE_2D || _castyle == CA_HEAT_2D || 
		_castyle == CA_CUSTOM_2D || _castyle == CA_NETWORK)
		dimension = 2;
	else
		dimension = 1;
	Changeradiusandstates();
	Adjust_acceleration_multiplier();
	Basic_adjust_params_for_type(); //Sets _startsmoothsteps
	//Now set the reseed flag.
	/* Reseed in case this is the first initialization.  Note that type_ca is
	always intilized to 0 by the CA constructor. */
	if (!type_ca) 
		reseed = TRUE;
	//Reeed if either the new or old style is STANDARD or REVERSIBLE
	if ( _castyle == CA_STANDARD || _castyle == CA_REVERSIBLE ||
			old_castyle == CA_STANDARD || old_castyle == CA_REVERSIBLE )
			reseed = TRUE; 
	if (dimension == 2 && old_dimension == 1)
	{
		Setviewmode(IDC_2D_VIEW);
		generatorlist.Clear();
		reseed = TRUE;
	}
	if	(dimension == 1 && old_dimension == 2)
	{
		Setviewmode(IDC_SPLIT_VIEW);
		generatorlist.Clear();
		reseed = TRUE;
	}
	if (Getdimension() == 2)
		capowgl->AdjustHeightFactor(this);
	if (reseed)
	{
		Seed(); // Need to seed with correct values & reposition row pointers!
	    Setcolortable();
	}
	GetCAStyleName ( CA_STYLE_NAME ); //Andrew
   // l.andrews 11/2/01 added test for null handle to next line
	if ( hwndStatusBar != 0 ) Status_SetText(hwndStatusBar, 1, 0, CA_STYLE_NAME ); //Andrew



}

void CA::Basic_adjust_params_for_type()
{
/* Here we try and put in params to keep things reasonable when you
have just changed the type_ca*/
	blankedgeflag = TRUE;
  	switch(type_ca)
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR_WAVE:
			_startsmoothsteps = 0;
			break;
		case ALT_CA_OSCILLATOR_WAVE:
 		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
		case CA_WAVE2:
		case ALT_CA_WAVE:
		case CA_ULAM_WAVE:
		case ALT_CA_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE: 
		case CA_AUTO_ULAM_WAVE:
		case CA_WAVE_2D:
		case CA_USER:
			_startsmoothsteps = ALTSMOOTHSTEPS;
			break;
	}
}

void CA::Full_adjust_params_for_type()
{
/* Here we try and put in params to keep things reasonable when you
have just changed the type_ca*/
	Basic_adjust_params_for_type();
	switch(type_ca)
	{
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
			SetTweakParam(&_max_intensity,(20.0));
			break;
		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR_WAVE:
		case ALT_CA_OSCILLATOR_WAVE:
 		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
			SetTweakParam(&_mass,1.0);
			_1_over_mass = 1.0;
			SetTweakParam(&_friction_multiplier, 0.0);
			SetTweakParam(&_max_intensity, 100.0);
			SetTweakParam(&_max_velocity, 1000.0);
			break;
		case CA_ULAM_WAVE:
		case ALT_CA_ULAM_WAVE:
			SetTweakParam(&_max_intensity,(12.0));
			SetTweakParam(&_nonlinearity1, Randomreal()*0.1);
			break;
		case CA_AUTO_ULAM_WAVE:
			SetTweakParam(&_max_intensity,(12.0));
			SetTweakParam(&_nonlinearity2, 1.0+Randomreal()*10.0);
			break;
		case CA_CUBIC_ULAM_WAVE: 
			SetTweakParam(&_max_intensity,(1.0));
			SetTweakParam(&_nonlinearity2, Randomreal()*0.05);
			break;
	}
}

void CA::Mutate(Real mutation_strength)
{
	int i, bitstomutate;

	if (!mutation_strength)
	{
		Adjust_acceleration_multiplier();
		return;
	}
	bitstomutate = nabeoptions * mutation_strength;
	if (!bitstomutate)
		bitstomutate = 1;
	if (nabeoptions)        // check for wave
		for (i=0; i<bitstomutate; i++)
			lookup[Random(nabeoptions)] ^= 1;
	Resetfreq();
	Computeactual_lambda();
//---BEGIN CA_wave
	SetTweakParam(&_dx, 0.002 +
		_dx.Val() *(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_dt, 0.001 +
		_dt.Val() * (1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_max_intensity, _max_intensity.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_max_velocity, _max_velocity.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_friction_multiplier, _friction_multiplier.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_spring_multiplier, _spring_multiplier.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_mass, _mass.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
	SetTweakParam(&_driver_multiplier, _driver_multiplier.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
  #ifndef FIXED_FREQ
	SetTweakParam(&_frequency_multiplier, _frequency_multiplier.Val() *
		(1.0 + (0.9*Randomsignreal()) * mutation_strength));
  #else // FIXED_FREQ
	SetTweakParam(&_frequency_multiplier, 1.0);
  #endif //FIXED_FREQ
	Adjust_acceleration_multiplier();
	//Begin User
	if (type_ca == CA_USER)
	{
		userParamAdd[0]->SetVal(DEFAULT_VARIANCE); //Put in a low variance, which means tweakstrength.
			for (int index = 1; index < userParamAdd.size(); index++)
				//2017 STart loop at 1 so you don't randomize the variance which is in slot 0
				RandomizeTweakParamPercent(userParamAdd[index], userParamAdd[index]->Val(), DEFAULT_VARIANCE);  //Mutate just a little.
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
		if( CA_WAVE <= type_ca && type_ca <= CA_AUTO_ULAM_WAVE )
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

void CA::Change_phase(int updown)
{
	if (updown >0)
		_phase += 0.1;
	else
		_phase -= 0.1;
 	CLAMP(_phase, -2.0*PI, 2.0*PI);
}

void CA::Reset_smoothsteps(int factor)
{
	if (_smoothflag)
		_smoothsteps = factor * _startsmoothsteps;
}

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

void CA::Set_dx_lock(BOOL onoff)
{
	_dx_lock = onoff;
	if (_dx_lock)
		Adjust_acceleration_multiplier();
}


// tll

void CA::Setfailstripe(int updown)
{
	if (updown > 0)
		fail_stripe += 10;
	else
		fail_stripe -= 10;
}

void CA::Setentropybonus(int updown)
{
	if (updown > 0)
		entropy_bonus += 10;
	else
		entropy_bonus -= 10;
}

void CA::Setgeneratorflag(int onoff)
{
	generatorflag = onoff;
	if (generatorflag)
		Zeroseed();
}


void CA::Setwrapflag(int newwrapflag)
{
	int c, d,i;

	wrapflag = newwrapflag;
/* Don't smooth if you just loaded cell values.  If this is the case
then turn off the _justloadedflag so you only skip the smooth once. */
	if (!_justloadedcells)
		Reset_smoothsteps();
	else
		_justloadedcells = FALSE;
	if (wrapflag == WF_ZERO)
	{
		past_row[0] = target_row[0] = source_row[0] = 0;
		wave_past_row[0].intensity = wave_target_row[0].intensity =
			wave_source_row[0].intensity = 0.0;
		wave_past_row[0].velocity = wave_target_row[0].velocity =
			wave_source_row[0].velocity = 0.0;

		past_row[horz_count-1] = target_row[horz_count-1] =
			source_row[horz_count-1] = 0;
		wave_past_row[horz_count-1].intensity =
			wave_target_row[horz_count-1].intensity =
			wave_source_row[horz_count-1].intensity = 0.0;
		wave_past_row[horz_count-1].velocity =
			wave_target_row[horz_count-1].velocity =
			wave_source_row[horz_count-1].velocity = 0.0;

		c = index(0,0);
		d = index(0,vert_count_2D-1);
		for (i=0; i<horz_count_2D; i++)
          {
			wave_past_plane[c].intensity = wave_target_plane[c].intensity =
				wave_source_plane[c].intensity = 0.0;
			c++;
			wave_past_plane[d].intensity = wave_target_plane[d].intensity =
				wave_source_plane[d].intensity = 0.0;
			d++;
		}
		c = index(0,0);
		d = index(horz_count_2D-1,0);
		for (i=0; i<vert_count_2D; i++)
          {
			wave_past_plane[c].intensity = wave_target_plane[c].intensity =
				wave_source_plane[c].intensity = 0.0;
			c += CX_2D;
			wave_past_plane[d].intensity = wave_target_plane[d].intensity =
				wave_source_plane[d].intensity = 0.0;
			d += CX_2D;
		}
	}
	if (wrapflag == WF_FIXED)
	{  
		past_row[0] = target_row[0] = source_row[0];
		wave_past_row[0].intensity = wave_target_row[0].intensity =
			wave_source_row[0].intensity;
		wave_past_row[0].velocity = wave_target_row[0].velocity =
			wave_source_row[0].velocity;

		past_row[horz_count-1] = target_row[horz_count-1] =
			source_row[horz_count-1];
		wave_past_row[horz_count-1].intensity =
			wave_target_row[horz_count-1].intensity =
			wave_source_row[horz_count-1].intensity;
		wave_past_row[horz_count-1].velocity =
			wave_target_row[horz_count-1].velocity =
			wave_source_row[horz_count-1].velocity;
#ifndef EDGE_FLUTTER_2D
		c = index(0,0);
		d = index(0,vert_count_2D-1);
		for (int i=0; i<horz_count_2D; i++)
			 {
			wave_past_plane[c].intensity =
				wave_target_plane[c].intensity =
				wave_source_plane[c].intensity;
			c++;
			wave_past_plane[d].intensity =
				wave_target_plane[d].intensity =
				wave_source_plane[d].intensity;
			d++;
		}
		c = index(0,0);
		d = index(horz_count_2D-1,0);
		for (i=0; i<vert_count_2D; i++)
			 {
			wave_past_plane[c].intensity =
				wave_target_plane[c].intensity =
				wave_source_plane[c].intensity;
			c += CX_2D;
			wave_past_plane[d].intensity =
				wave_target_plane[d].intensity =
				wave_source_plane[d].intensity;
			d += CX_2D;
		}
#endif //EDGE_FLUTTER_2D
	}
}

//--------------------Generator things-----------------

Generator::Generator(CA *myca)
{
	ca_ptr = myca;
	g_intensity = 0.0;
	percent_max_intensity = 0.8;
	velocity = 0.0;
	location =0; //put it in the first spot
	state = 0;
	time = 0.0;
	omega = 1.0f;
	location_x = 0;// 2D x-y locations.  Used in gendlg.cpp to display
	location_y = 0;// the generator position in the list box.
	phase = 0.0f;
}


void Generator::Step()
{
/* This is called right after the target row is updated, but before
it is shown to the bitmap and swapped with the source row.  This way,
the generator value gets in as the visible target value, and is used
as the new source value.  We use the internal onoff flag to decide
whether to really do this, that way we can always call this Step.
	
	IF you switch from 2D to 1D, this will crash because location
	is big.*/
	g_intensity = sin(omega*time+phase);
	velocity = -cos(omega*time+phase);
	/* Set the wave value to generator intensity times one eighth of
		the max possible. */
	if (ca_ptr->Gettype() != CA_STANDARD &&
		ca_ptr->Gettype() != CA_REVERSIBLE)
	{
	if (ca_ptr->Getdimension() == 2)
			ca_ptr->wave_target_plane[location].intensity =
			g_intensity * percent_max_intensity *(ca_ptr->_max_intensity.Val());//MAX_GENERATOR_INTENSITY;
	  else //dimension == 1
	  {
		ca_ptr->wave_target_row[location].intensity =
			g_intensity * percent_max_intensity *(ca_ptr->_max_intensity.Val() / 2.0);//MAX_GENERATOR_INTENSITY;
		ca_ptr->wave_target_row[location].velocity = 0;
	  }/*You don't
			use this veloicty, and nobody else looks at it.*/
	}
	else
	{
	//Set the standard value in the range 0 to maximum value (states - 1)
	//Note that this osicallates with half the time of period, due to fabs.
		state = (ca_ptr->states) * fabs(g_intensity);
		if (state == ca_ptr->states)
			state--; //Just in case you hit an overflow via g_intensity 1.0
		ca_ptr->target_row[location] = state;
	}
//Now step the time.
	time += ca_ptr->_dt.Val();
	if (time > TIMEWRAP)
		time -= TIMEWRAP;
}





// tl 10.29.94

void CA::Install_Ramp(int paramcode, Real rampvalue)
{
	switch (paramcode)
	{
		case RAMP_ACCEL:
			SetTweakParam(&_dx, (rampvalue));
			break;

		case RAMP_VEL_MULT:
			SetTweakParam(&_dt, (rampvalue));
			break;

		case RAMP_MAX_VEL:
			SetTweakParam(&_max_velocity,(rampvalue));
			break;

		case RAMP_LAMBDA:
			Setlambda(rampvalue);
			break;

		case RAMP_BAND:
			Set_band_count(rampvalue);
			break;

		case RAMP_MAX_INTENSITY:
			SetTweakParam(&_max_intensity,(rampvalue));
			break;

		case RAMP_HEAT_COUNT:
			SetTweakParam(&_heat_inc, (rampvalue));
			break;

		case RAMP_FRICTION:
			SetTweakParam(&_friction_multiplier, (rampvalue));
			break;

		case RAMP_SPRING:
			if (type_ca == CA_ULAM_WAVE || type_ca == ALT_CA_ULAM_WAVE)
				SetTweakParam(&_nonlinearity1, (rampvalue));
			else if (type_ca == CA_AUTO_ULAM_WAVE || type_ca == CA_CUBIC_ULAM_WAVE)
				SetTweakParam(&_nonlinearity2, (rampvalue));
			else
				SetTweakParam(&_spring_multiplier, (rampvalue));
			break;

		case  RAMP_DRIVER_AMP:
			SetTweakParam(&_driver_multiplier,(rampvalue));
			break;

		case  RAMP_DRIVER_FREQ:
			SetTweakParam(&_frequency_multiplier,(rampvalue));
			break;

		case RAMP_INDUCTANCE:
			SetTweakParam(&_mass,rampvalue);
			_1_over_mass = 1 / _mass.Val();   // it was done in Set_mass()
			_dt_over_mass = _dt.Val() / _mass.Val(); // it was done in Set_mass()
			_dt_2_over_mass = _dt.Val()*_dt.Val() / _mass.Val(); // it was done in Set_mass()
			break;
	}
}


//  tl 10.29.94

void CAlist::Install_Ramp(int paramcode, Real ramptable[])
{
	for (int i = 0; i < count; i++)
		list[i]->Install_Ramp(paramcode, ramptable[i]);
}

// tl 10-31-94

void CAlist::CheckRange (int paramcode, Real minmax[])
{
	Real tmp;
	if (minmax[0] > minmax[1])
		{
			tmp = minmax[0];
			minmax[0] = minmax[1];
			minmax[1] = tmp;
		}
//#define OBSOLETE_CLAMPING
#ifdef OBSOLETE_CLAMPING //Comment this out because the SetTweakParam functions in Install_Ramp
	//take care of this.

	switch (paramcode)
	{
		case RAMP_ACCEL:
		case RAMP_VEL_MULT:
			if (minmax[0] < GENERAL_MIN_MULTIPLIER)
				minmax[0] = GENERAL_MIN_MULTIPLIER;
			if (minmax[1] < minmax[0])
				minmax[1] = minmax[0];
			break;

		case RAMP_LAMBDA:
			CLAMP (minmax[0], MIN_LAMBDA, MAX_LAMBDA);
			CLAMP (minmax[1], MIN_LAMBDA, MAX_LAMBDA);
			break;

		case RAMP_BAND:
			CLAMP (minmax[0], 2, (MAX_COLOR-1));
			CLAMP (minmax[1], 2, (MAX_COLOR-1));
			break;

		case RAMP_MAX_VEL:
			CLAMP(minmax[0], 1.0, 1000.0);
			CLAMP(minmax[1], 1.0, 1000.0);
			break;

		case RAMP_MAX_INTENSITY:
			CLAMP(minmax[0], 1, 10000);
			CLAMP(minmax[1], 1, 10000);
			break;

		case RAMP_HEAT_COUNT:
			CLAMP(minmax[0], 0, 256);
			CLAMP(minmax[1], 0, 256);
			break;

		case RAMP_FRICTION:
			if (minmax[0] < GENERAL_MIN_MULTIPLIER)
				minmax[0] = GENERAL_MIN_MULTIPLIER;
			if (minmax[1] < minmax[0])
				minmax[1] = minmax[0];
			break;

		case RAMP_SPRING:
			if (minmax[0] < SPRING_MEAN - SPRING_VARIANCE)
				minmax[0] = SPRING_MEAN - SPRING_VARIANCE;
			if (minmax[1] < minmax[0])
				minmax[1] = minmax[0];
			break;

		case  RAMP_DRIVER_AMP:
			CLAMP(minmax[0], 1.0, 10000 / 2.0);
			CLAMP(minmax[1], 1.0, 10000 / 2.0);
			break;

		case  RAMP_DRIVER_FREQ:
			CLAMP(minmax[0], 1.0, 1000);
			CLAMP(minmax[1], 1.0, 1000);
			break;

		case RAMP_INDUCTANCE:
			if (minmax[0] < (MASS_MEAN - MASS_VARIANCE))
				minmax[0] = MASS_MEAN - MASS_VARIANCE;
			if (minmax[1] < minmax[0])
				minmax[1] = minmax[0];
			break;
	}
#endif //OBSOLETE_CLAMPING
}


void CA::BumpTweakParam(int tweakParamType, Real updown)
{
	switch (tweakParamType)
	{
		case HEAT_INC_TYPE:
			_heat_inc.Bump(updown, this);
			break;

		case MAX_INTENSITY_TYPE:
			_max_intensity.Bump(updown, this);
			break;

		case MAX_VELOCITY_TYPE:
			_max_velocity.Bump(updown, this);
			break;

		case FRICTION_TYPE:
			_friction_multiplier.Bump(updown, this);
			break;

		case MASS_TYPE:
			_mass.Bump(updown, this);
			break;

		case TIME_STEP_TYPE:
			_dt.Bump(updown, this);
			break;

		case SPACE_STEP_TYPE:
			_dx.Bump(updown, this);
			break;

		case SPRING_TYPE:
			_spring_multiplier.Bump(updown, this);
			break;

		case NONLINEARITY1_TYPE:
			_nonlinearity1.Bump(updown, this);
			break;

		case NONLINEARITY2_TYPE:
			_nonlinearity2.Bump(updown, this);
			break;

		case DRIVER_AMP_TYPE:
			_driver_multiplier.Bump(updown, this);
			break;

		case DRIVER_FREQ_TYPE:
			_frequency_multiplier.Bump(updown, this);
			break;

		case CHUNK_TYPE:
			_chunk.Bump(updown, this);
			break;

		case VARIANCE_TYPE:
			_variance.Bump(updown, this);
			break;
	}
}

void CA::RandomizeTweakParam (TweakParam* t, Real old_val, Real tweak_strength)
{
	t->SetVal(old_val+Randomsignreal()*tweak_strength, this);
}

void CA::RandomizeTweakParamPercent (TweakParam* t, Real old_val, Real tweak_strength)
{
/*1998 The RandomizeTweakParam method we've been using is too coarse for using
in the Userpara.cpp module for randomizing the user params.  The problem is that
if old_val is something like 0.0001 and the variance is 0.1, then a straight
RandomizeTweakParam usually slams the t->Val() down to 0.  So I'm going to do
two things (i) I'll make it a percent change and (b) I'll bounce any new val back
off the max or min. Rudy R. November 8, 1998.
	2003. There is maybe a problem using this method if you are near zero in that the changes are very
small.  Put in a fix RR. March 21, 2003. 
2017 the method is too powerful to be useful in the User Param menu or in the Mutate method. I rewrote it
it from scratch.  All the old crap is commented out. Now think of it this way.  tweak_strength ranges
from 0 to 1 and (if multiplied by 100) would be the maximum percent that you want to change the thing by.
So tweak_strength of 0.01 means change it by at most 1%, and sometimes you'd want 0.001 for change by at
most a tenth of a percent.  Randomsignreal()*tweak_strength is a random number between -tweak_strength
and tweak_strength.
*/
//2017 Version
	Real actual_tweak_strength = Randomsignreal()*tweak_strength;
	t->SetVal(old_val + actual_tweak_strength*old_val, this);
//And you're done.

/* Commented out old crap
#define USERANGE //2003
	Real new_val;
#ifdef USERANGE
	Real range = fabs(t->Max() - t->Min());
	new_val = old_val + Randomsignreal()*tweak_strength*fabs(range); //Percent change
#else //not USERANGE
	BOOL allowsignflip = t->Min() < 0.0 && t->Max() > 0.0;
	if (allowsignflip)
		old_val -= t->Min(); //To get away from 0.0
	new_val = old_val + Randomsignreal()*tweak_strength*fabs(old_val); //Percent change
	if (allowsignflip)
		new_val += t->Min(); 
#endif //USERANGE
	if (new_val <= t->Min() || new_val >= t->Max()) //bounce back to center from the bottom or top.
		new_val = (t->Min() + t->Max()) / 2.0;
	t->SetVal(new_val, this);*/
}

void CA::SetTweakParam (TweakParam* t, Real val)
{
	 t->SetVal(val, this);
}

TweakParam* CA::GetTweakParam(int tweakParamType)
{
	switch (tweakParamType)
	{
		case HEAT_INC_TYPE:
			return &_heat_inc;

		case MAX_INTENSITY_TYPE:
			return &_max_intensity;

		case MAX_VELOCITY_TYPE:
			return &_max_velocity;

		case FRICTION_TYPE:
			return &_friction_multiplier;

		case MASS_TYPE:
			return &_mass;

		case TIME_STEP_TYPE:
			return &_dt;

		case SPACE_STEP_TYPE:
			return &_dx;

		case SPRING_TYPE:
			return &_spring_multiplier;

		case NONLINEARITY1_TYPE:
			return &_nonlinearity1;

		case NONLINEARITY2_TYPE:
			return &_nonlinearity2;

		case DRIVER_AMP_TYPE:
			return &_driver_multiplier;

		case DRIVER_FREQ_TYPE:
			return &_frequency_multiplier;

		case CHUNK_TYPE:
			return &_chunk;

		case VARIANCE_TYPE:
			return &_variance;
	}
	assert(0);
	return 0;  // should NEVER get executed
}



void CA::SetTweakParam(int tweakParamType, Real val)
{
	switch (tweakParamType)
	{
		case HEAT_INC_TYPE:
			_heat_inc.SetVal(val, this);
			break;

		case MAX_INTENSITY_TYPE:
			_max_intensity.SetVal(val, this);
			break;

		case MAX_VELOCITY_TYPE:
			_max_velocity.SetVal(val, this);
			break;

		case FRICTION_TYPE:
			_friction_multiplier.SetVal(val, this);
			break;

		case MASS_TYPE:
			_mass.SetVal(val, this);
			break;

		case TIME_STEP_TYPE:
			_dt.SetVal(val, this);
			break;

		case SPACE_STEP_TYPE:
			_dx.SetVal(val, this);
			break;

		case SPRING_TYPE:
			_spring_multiplier.SetVal(val, this);
			break;

		case NONLINEARITY1_TYPE:
			_nonlinearity1.SetVal(val, this);
			break;

		case NONLINEARITY2_TYPE:
			_nonlinearity2.SetVal(val, this);
			break;

		case DRIVER_AMP_TYPE:
			_driver_multiplier.SetVal(val, this);
			break;

		case DRIVER_FREQ_TYPE:
			_frequency_multiplier.SetVal(val, this);
			break;

		case CHUNK_TYPE:
			_chunk.SetVal(val, this);
			break;

		case VARIANCE_TYPE:
			_variance.SetVal(val, this);
		   break;
	}
}


void CA::Setlambda(Real newlambda)
{
	unsigned short i;
  Real increment;
	Real oldlambda, flip_prob;

	oldlambda = lambda;
	lambda = newlambda;
	if (lambda > MAX_LAMBDA)
		lambda = MAX_LAMBDA;
	if (lambda < MIN_LAMBDA)
		lambda = MIN_LAMBDA;
  increment = lambda - oldlambda;

	if (increment > 0) //increase lambda
	{
		flip_prob = (lambda - oldlambda)/(1.0 - oldlambda);
/* Because lambda = ((1.0 - oldlambda)*flip_prob*nabeoptions+
oldlambda*nabeoptions) / nabeoptions; or
lambda = (1-oldlambda)*flip_prob + oldlambda.*/
		for (i=0; i<nabeoptions; i++)
			if (!lookup[i] && Randomreal()< flip_prob )
				lookup[i] = (unsigned char)
            	(1 + Random((unsigned short)(states - 1)));
	}
	else //decrease lambda
	{
		flip_prob = (oldlambda - lambda)/(oldlambda);
/* Because lambda = (oldlambda*nabeoptions -
oldlambda*nabeoptions*flip_prob) / nabeoptions; or
lambda = oldlambda - oldlamda*flip_prob.*/
		for (i=0; i<nabeoptions; i++)
			if (lookup[i] && Randomreal()< flip_prob )
				lookup[i] = 0;
	}
	Computeactual_lambda();
	Resetfreq();
}

//------------GENERATORLIST DEFINITIONS-----------------by mike 4/97
/* had to add the default constructor to the Generator class
*/
GeneratorList::GeneratorList()
{
	count = 0;
}

GeneratorList::GeneratorList(CA *myca)
{
	count = 0;
	ca_ptr = myca;
}

GeneratorList::~GeneratorList()
{

}

void GeneratorList::Clear(void)
{
	count = 0;
}
void GeneratorList::Add(int x) // x represents the x location of the generator being added
{
	if (ca_ptr) //valid?
	{
		if (x>=0 && x < ca_ptr->horz_count && count <MAX_GENERATORS && ca_ptr->dimension==1)
		{	
			for(int i=0; i<count;i++)
				if(generator[i].location == x) // already have a generator at that location
					return;
			generator[count].Seed();
			generator[count].ca_ptr = ca_ptr;
			generator[count].location = x;		
			count++;
		}

	}
	
}


void GeneratorList::Delete(int x) // x is the index of the generator to delete from the generator list
{
	if (ca_ptr) //valid?
	{
		if (x>=0 && x < ca_ptr->horz_count &&  ca_ptr->dimension==1 && count > 0)
		{	
			for(int i=0; i<count;i++)
				if(i == x)
				{               //just slide everything down one
					for(int j = i; j < count - 1; j++)
					{
						generator[j].g_intensity = generator[j+1].g_intensity;
						generator[j].time = generator[j+1].time;
						generator[j].state = generator[j+1].state;
						generator[j].omega = generator[j+1].omega;
						generator[j].percent_max_intensity = generator[j+1].percent_max_intensity;
						generator[j].ca_ptr = generator[j+1].ca_ptr;
						generator[j].location = generator[j+1].location;
					}
				}
			count--;
		}
	}
}


void GeneratorList::Add(int x, int y) // x and y are the x and y locations of the generator being added to the 2D CA
{
	if(ca_ptr) //valid?
	{
		if(x>=0 && y>=0 && ca_ptr->dimension==2 && x < CX_2D && y<CY_2D && count <MAX_GENERATORS)
		{
			for(int i=0; i<count;i++)
				if(generator[i].location_x == x && 
					     generator[i].location_y == y) // already have a generator at that location
					return;
		
			generator[count].Seed();
			generator[count].ca_ptr = ca_ptr;
			generator[count].location =	ca_ptr->index(x,y);
			generator[count].location_x = x;
			generator[count].location_y = y;
			count++;
		}
	}
}



void GeneratorList::Deletexy(int x) // x is the index of the generator to delete from the generator list
{
	if (ca_ptr) //valid?
	{
		if (x>=0 && x < count && ca_ptr->dimension==2)
		{	
			for(int i=0; i<count;i++)
			{
				if(i == x)
				{               //just slide everything down one
					for(int j = i; j < count - 1; j++)
					{
						generator[j].g_intensity = generator[j+1].g_intensity;
						generator[j].time = generator[j+1].time;
						generator[j].state = generator[j+1].state;
						generator[j].omega = generator[j+1].omega;
						generator[j].percent_max_intensity = generator[j+1].percent_max_intensity;
						generator[j].ca_ptr = generator[j+1].ca_ptr;
						generator[j].location = generator[j+1].location;
						generator[j].location_x = generator[j+1].location_x;
						generator[j].location_y = generator[j+1].location_y;
					}
				}
			}
			count--;
		}
	}
}


void GeneratorList::Step(void)
{
	int i;
	for (i=0; i<count; i++)
	{
		generator[i].Step();
	}
}


void GeneratorList::Reset(void)
{
	int i;
	for (i=0; i<count; i++)
	{
		generator[i].Reset();
	}
}

float GeneratorList::OmegaLast()
{
	float result = 1.0f;
	if (count >0)
	{
		result = generator[count-1].Omega();
	}
	return result;
}

void GeneratorList::OmegaLast(float newomega)
{
	if (count >0)
	{
		if (newomega <0.0f)
			newomega = 0.0f;
		generator[count-1].Omega(newomega);

	}
}

void GeneratorList::SetOmega(int i, float newomega)
{
	if (i >= 0)
	{
		if (newomega <0.0f)
			newomega = 0.0f;
		generator[i].Omega(newomega);

	}
}


//mike 2/98  This is an attempt to let generators 
//transition smoothly between changes in omega
void GeneratorList::SetSmoothOmega(int i, float newomega)
{
	if (i >= 0)
	{
		if (newomega <0.0f)
			newomega = 0.0f;
		generator[i].SmoothOmega(newomega);  //see next function
	}
}

void Generator::SmoothOmega(float newomega)
{
	float oldphase = phase;
	phase = omega*time + phase - newomega*time;	
	omega = newomega;
}

void GeneratorList::SetAmplitude(int i, float newamp)
{
	if (i >= 0)
	{
		if (newamp <0.0f)
			newamp = 0.0f;
		generator[i].Amplitude(newamp);

	}
}


