// ***********************************************************************
// File:		loadsave.cpp													
// PROJECT:		CAMCOS CAPOW
// ENVIRONMENT:	Visual C++ 4.0	Win32
//
// The following codes handle load and save features using compression.
//
// Date: 4/25/97
//
// Edit:
//
// ***********************************************************************

#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include <math.h>
#include <iomanip> //2017
#include <stdlib.h> //for itoa                       
#include <ctype.h>  //for isdigit
#include <string.h>
#include <stdio.h>
#include <LZexpand.h>
#include <process.h>
#include <ERRNO.H>
#include "userpara.hpp"
/* 2017.  Lots of problems rebuilding with VC ver 15.  Removed all  ios::nocreate flags. Removed .h from iomanip. 
*/

// #define DEBUG					/* For debugging only					 */

BOOL BINARY = TRUE;				/* Binary output or text output			 */
#define LIMIT_PRECISION			/* control of precision - TEXT mode only */
#define LOADSAVE_PRECISION 3	/* # of precision		- TEXT mode only */

extern HWND masterhwnd;			// The master window					 
extern CAlist *calife_list;
extern char capowDirectory[];
extern int blt_flag;
extern BOOL zoomviewflag;
extern BOOL not_seeded_yet_flag;
extern BOOL load_save_cells_flag;	// Save cell or not					
// This determines whether to save and load the individual cell values.
// When you are doing a save, this flag will be set from capow.cpp by
// which File Save option you select.  When you are doing a load, this
// flag will be set from either the file version number (FALSE for
// versions < 4, TRUE for version 4), or from a flag in the file
// for versions > 4)


BOOL errOccurred = FALSE;		// Reset to FALSE before any output/input
BOOL compressFile = FALSE;		// Flag to compress on save only
								// Load will automatic determine whether 
								//		compress it or not

void compressTheFile(char *filename);

//--------------------------------------------------------------------------

ifstream& operator>>(ifstream& ifs, CA* target)
{
	if (!inBinary(ifs, target))
		errOccurred = TRUE;
	return ifs;
}

//--------------------------------------------------------------------

BOOL outBinary(ofstream& ofs, CA* target);

//---------------------------------------------------------------
ofstream& operator<<(ofstream& ofs, CA* target) 
{
	if (!outBinary(ofs, target))
		errOccurred = TRUE;
	return ofs;
}

// **************************************************************************
// Following outWrite functions are used for output CA variable
// Comment only a couple
// **************************************************************************


BOOL checkError(ios& s, char *msg)
{
	if (s.fail())
	{
		s.clear();
		if (strlen(msg) != 0)
			MessageBox(masterhwnd, msg, "FILE ERROR", MB_APPLMODAL | MB_OK | 
				   MB_ICONEXCLAMATION);
		errOccurred = TRUE;
		return TRUE;
	}
	else return FALSE;
}

void writeDebugInfo(ofstream& ofs, char *msg)
//
// For debugging: if "msg" != dMsg then out of sychnoize
//
{
	#ifdef DEBUG
		int dTotal = strlen(msg);
		ofs.write((char *) &dTotal, sizeof(dTotal));
		ofs.write((char *) msg, dTotal);	
	#endif
}

ofstream& outWrite(ofstream& ofs, char *msg, unsigned char *val, int total)
//
// Output "total" unsigned char.
//
{
	if (BINARY)
	{	
		writeDebugInfo(ofs, msg);

		ofs.write((char *)&total, sizeof(total));
		ofs.write((char *)val, sizeof(unsigned char) * total);
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << total << " ";
		for(int count=0; count<total; count++)
			ofs << (int)val[count] << " "; 
		ofs << endl;
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, unsigned char *val, unsigned short total)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		int tot = total;
		ofs.write((char *)&tot, sizeof(tot));
		ofs.write((char *)val, sizeof(unsigned char) * tot);
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << total << " ";
		for(int count=0; count<total; count++)
			ofs << (int)val[count] << " "; 
		ofs << endl;
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, char *val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		int total = strlen(val);
		ofs.write((char *)&total, sizeof(total));
		ofs.write((char *)val, sizeof(char) * total);
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << strlen(val) << " ";
		for(int count=0; count<strlen(val); count++)
			ofs << val[count] << " "; 
		ofs << endl;
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, int val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, unsigned int val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, long val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, float val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, double val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, Real *val, int total)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *)&total, sizeof(total));
		ofs.write((char *) val, sizeof(val)*total);
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << total << " ";
		for(int count=0; count<total; count++)
			ofs << val[count] << " ";
		ofs << endl;
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, AdditiveTweakParam& val)
{
//We should be writing and reading the Min() and Max() as well!!!!
	Real value = val.Val();
	return outWrite(ofs, msg, value);
}

ofstream& outWrite(ofstream& ofs, char *msg, MultiplicativeTweakParam& val)
{
	Real value = val.Val();
	return outWrite(ofs, msg, value);
}

ofstream& outWrite(ofstream& ofs, char *msg, COLORREF val)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *) &val, sizeof(val));
	}
	else ofs << strlen(msg) << " " << msg << val << endl;
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, Wavecell *val, int total)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *)&total, sizeof(total));
		for(int count=0; count<total; count++)
		{
			ofs.write((char *) &(val[count].state), sizeof(val[count].state));
			outWrite(ofs, "Variable", val[count].variable, VARIABLE_COUNT);
//			ofs.write((char *) &(val[count].intensity), sizeof(val[count].intensity));
			ofs.write((char *) &(val[count].velocity), sizeof(val[count].velocity));
			int totalSub = CELL_PARAM_COUNT;
			char cst[80]; 
			sprintf(cst, "%5d:", (int)cst); // 2017.  Had sprintf(cst, "%5d:", cst)
											   // 2017 Throws a warning. See line 929 also
											   //---format string '%5d' requires an argument of type 'int', but variadic argument 1  has type 'char *'---
											   //The "variadic argument 1" means that second occurance of cst.  The : in the format string is part of the
											   //desired string's text?  %5d means print a number in decimal in a five character field possibly padded 
											   //by blanks in front.
											   //One suggestion I found online it to put p (for pointer) instead of d (for integer). 
											   //Supposedly in old all 32 bit world, integer and pointer are the same, but if you have 64 bit world then
											   // pointer is 64 bit.  And you have to cast the cst to void * to make the p format happy.
											   //So you could try sprintf(cst, "%5p:", (void *)cst);
											 // or to make it simpler, I tried sprintf(cst, "%5d:", (int) cst); Seems to work.

			cst[5] = 0; // make sure it lenght is five
			outWrite(ofs, cst, &(val[count]._cell_param[0]), totalSub);
		}
	}
	else 
	{
	  ofs << strlen(msg) << " " << msg << " " << total << " ";
	  for(int count=0; count<total; count++)
	  {
		ofs << val[count].state << " " << val[count].intensity << " " 
			<< val[count].velocity << " ";
		ofs << CELL_PARAM_COUNT << " ";
		for(int count=0; count < CELL_PARAM_COUNT; count++)
			ofs << val[count]._cell_param[count] << " ";
		ofs << endl;
      }
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, Wavecell2 *val, int total)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		ofs.write((char *)&total, sizeof(total));
		for(int count=0; count<total; count++)
		{
			//variable[k] VARIABLE_COUNT
			outWrite(ofs, "Variable", val[count].variable, PLANE_VARIABLE_COUNT);
//			ofs.write((char *) &(val[count].intensity), sizeof(val[count].intensity));
//			ofs.write((char *) &(val[count].intensity2), sizeof(val[count].intensity2));

//			int totalSub = PLANE_CELL_PARAM_COUNT;
//			ofs.write((char *)&totalSub, sizeof(totalSub));
//			ofs.write((char *) &(val[count].param[0]), sizeof(Real) * totalSub);
//			char cst[80];
//			sprintf(cst, "%5d:", cst);
//			cst[5] = 0; // make sure it lenght is five
//			outWrite(ofs, cst, &(val[count].param[0]), totalSub);
		}
	}
	else 
	{
	  ofs << strlen(msg) << " " << msg << " " << total << " ";
	  for(int count=0; count<total; count++)
	  {
		outWrite(ofs, "Variable", val[count].variable, VARIABLE_COUNT);
		ofs << endl;
	  }
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, COLORREF *val, int total)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		int tot = total;
		ofs.write((char *)&tot, sizeof(tot));
		ofs.write((char *) val, sizeof(COLORREF)*tot);
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << total << " ";
		for(int count=0; count<total; count++)
			ofs << val[count] << " ";
		ofs << endl;
	}
	return ofs;
}

ofstream& outWrite(ofstream& ofs, char *msg, vector<class TweakParam *> *userParam)
{
	if (BINARY)
	{
		writeDebugInfo(ofs, msg);

		int total = userParam->size();
		ofs.write((char *)&total, sizeof(total));
		for(int count = 0; count < total; count++)
		{	
			Real value = (*userParam)[count]->Val();
			ofs.write((char *) &value, sizeof(value));
		}
	}
	else 
	{
		ofs << strlen(msg) << " " << msg << " " << userParam->size() << " ";
		for(int count = 0; count < userParam->size(); count++)
		{	
			Real value = (*userParam)[count]->Val();
			ofs << value << " ";
		}
		ofs << endl;
	}
	return ofs;
}

BOOL outBinary(ofstream& ofs, CA* target) 
//
// Version 8 - binary/Text
//
{	
	// Version and Save cell flag
	outWrite(ofs, "Version: ", (int)FILEVERSION);
	outWrite(ofs, "Load Save Cell Flag: ", load_save_cells_flag);

	// CA Type
	outWrite(ofs, "CA Type: ", target->type_ca);
	
	// Save user rule first
	if (target->type_ca == CA_USER)
	{
		outWrite(ofs, "User Rule Name: ", target->_userrulename);
//		outWrite(ofs, "User CA Style: ", target->_usercastyle);
//		outWrite(ofs, "User CA Nabe Size: ", target->_usernabesize);
		outWrite(ofs, "User Parameter: ", &target->userParamAdd);
	}

	outWrite(ofs, "Radius: ", target->radius);
	outWrite(ofs, "States: ", target->states);
	if (target->type_ca < CA_WAVE)
	{
//		outWrite(ofs, "Nabe Size: ", target->nabeoptions);
		outWrite(ofs, "Nabe Options (LooKUp Size): ", target->lookup, target->nabeoptions);
	}

	outWrite(ofs, "Show Mode: ", target->showmode);
	outWrite(ofs, "Generator Flag: ", target->generatorflag);
	outWrite(ofs, "WrapFlag: ", target->wrapflag);
	outWrite(ofs, "Lambda: ", target->lambda);
	outWrite(ofs, "Target Entropy: ", target->target_entropy);
	outWrite(ofs, "Bonus Entropy: ", target->entropy_bonus);
	outWrite(ofs, "Fail Stripe: ", target->fail_stripe);
	
	// Standard CA Base
	//Don't really need save horz_count as you don't use it when reading in because
	//you don't resize the CA.  But we fix it in Locate
	outWrite(ofs, "Horizontal Count: ", target->horz_count);
	outWrite(ofs, "Blank Edge Flag: ", target->blankedgeflag);
	outWrite(ofs, "Entropy Flag: ", target->entropyflag);
	outWrite(ofs, "View Mode: ", target->viewmode);
	outWrite(ofs, "Anchor Color: ", target->_anchor_color, MAX_BAND_COUNT);

	if (target->type_ca >= CA_WAVE)
	{
		outWrite(ofs, "DX: ", target->_dx);
		outWrite(ofs, "DT: ", target->_dt);
		outWrite(ofs, "Band Count: ", target->band_count);
		outWrite(ofs, "Heat Inc: ", target->_heat_inc);
		outWrite(ofs, "Max Intensity: ", target->_max_intensity);
		outWrite(ofs, "Max Velocity: ", target->_max_velocity);
		outWrite(ofs, "Friction Multiplier: ", target->_friction_multiplier);
		outWrite(ofs, "Spring: ", target->_spring_multiplier);
		outWrite(ofs, "Driver Multiplier: ", target->_driver_multiplier);
		outWrite(ofs, "Freq Multiplier: ", target->_frequency_multiplier);
		outWrite(ofs, "Mass: ", target->_mass);
		outWrite(ofs, "Nabe Size: ", target->nabesize);
		outWrite(ofs, "Chunk: ", target->_chunk);
		outWrite(ofs, "DX Lock: ", target->_dx_lock);
		outWrite(ofs, "Show Velocity: ", target->showvelocity);
		outWrite(ofs, "Time: ", target->time);
		outWrite(ofs, "Non Linearity 1: ", target->_nonlinearity1);
		outWrite(ofs, "Non Linearity 2: ", target->_nonlinearity2);

		outWrite(ofs, "Variance: ", target->_variance);
		outWrite(ofs, "Phase: ", target->_phase);
		outWrite(ofs, "Monochrome Flag: ", target->monochromeflag);
		outWrite(ofs, "Horz Count 2D: ", target->horz_count_2D);
		outWrite(ofs, "Vert Count 2D: ", target->vert_count_2D);
		outWrite(ofs, "Max X 2D: ", target->maxx_2D);
		outWrite(ofs, "Max Y 2D: ", target->maxy_2D);
	
	}	
		
	if (!load_save_cells_flag)
		return TRUE;

	switch(target->type_ca)
	{
		case CA_STANDARD:
		case CA_REVERSIBLE:
			outWrite(ofs, "Source Row Index (size): ", target->source_row, target->_max_horz_count);
			outWrite(ofs, "Past Row Index (Size): ", target->past_row, target->_max_horz_count);
			break;
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
		case CA_WAVE:
		case CA_WAVE2:
		case CA_OSCILLATOR:
 //		case CA_OSCILLATOR_WAVE:
		case CA_DIVERSE_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR_WAVE:
		case CA_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case ALT_CA_WAVE:
		case ALT_CA_WAVE2:
		case ALT_CA_OSCILLATOR:
		case ALT_CA_OSCILLATOR_WAVE:
		case ALT_CA_DIVERSE_OSCILLATOR:
		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
		case ALT_CA_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
			outWrite(ofs, "Wave Source Row: ", target->wave_source_row, target->_max_horz_count);
			outWrite(ofs, "Wave Past Row: ", target->wave_past_row, target->_max_horz_count);
			break;
		case CA_WAVE_2D:
		case CA_HEAT_2D:
			outWrite(ofs, "Wave Source Plane: ", target->wave_source_plane, (int)CX_2D * CY_2D);
			outWrite(ofs, "Wave Past Plane: ", target->wave_past_plane, (int)CX_2D * CY_2D);
			break;
		case CA_USER:
			switch(target->_usercastyle)
			{
				case CA_STANDARD:
				case CA_REVERSIBLE:
					outWrite(ofs, "Source Row Index (size): ", target->source_row, target->_max_horz_count);
					outWrite(ofs, "Past Row Index (Size): ", target->past_row, target->_max_horz_count);
					break;
				case CA_HEATWAVE:
				case CA_HEATWAVE2:
				case CA_WAVE:
				case CA_WAVE2:
				case CA_OSCILLATOR:
 //				case CA_OSCILLATOR_WAVE:
				case CA_DIVERSE_OSCILLATOR:
				case CA_DIVERSE_OSCILLATOR_WAVE:
				case CA_ULAM_WAVE:
				case CA_AUTO_ULAM_WAVE:
				case ALT_CA_WAVE:
				case ALT_CA_WAVE2:
				case ALT_CA_OSCILLATOR:
				case ALT_CA_OSCILLATOR_WAVE:
				case ALT_CA_DIVERSE_OSCILLATOR:
				case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
				case ALT_CA_ULAM_WAVE:
				case CA_CUBIC_ULAM_WAVE:
					outWrite(ofs, "Wave Source Row: ", target->wave_source_row, target->_max_horz_count);
					outWrite(ofs, "Wave Past Row: ", target->wave_past_row, target->_max_horz_count);
					break;
				case CA_WAVE_2D:
				case CA_HEAT_2D:
					outWrite(ofs, "Wave Source Plane: ", target->wave_source_plane, (int)CX_2D * CY_2D);
					outWrite(ofs, "Wave Past Plane: ", target->wave_past_plane, (int)CX_2D * CY_2D);
					break;
			}
			break;
	}

	// Not save because no one use it yet
//	outWrite(ofs, "Phylum: ", target->_phylum);

//	outWrite(ofs, "Test Point: ", target->test_point);
//	outWrite(ofs, "Temp Test Point: ", target->temp_test_point);
//	outWrite(ofs, "Rel Test Point: ", target->rel_test_point);
	
//	outWrite(ofs, "Fourier Array: ", target->tp_real_array, 4*target->_max_horz_count);
//	outWrite(ofs, "Fourier A: ", target->fourier_a, MAXTERM);
//	outWrite(ofs, "Fourier B: ", target->fourier_b, MAXTERM);
//	outWrite(ofs, "# of term: ", target->numofterm);
//	outWrite(ofs, "Fourier Approx: ", target->fourier_approx, 2*target->_max_horz_count);
//	outWrite(ofs, "TP Start Pos: ", target->tp_startpos);
//	outWrite(ofs, "TP End Pos: ", target->tp_endpos);
//	outWrite(ofs, "Fourier Flag: ", target->fourierflag);
//	outWrite(ofs, "TP View Mode: ", target->tp_viewmode);
//	outWrite(ofs, "TP View Type: ", target->tp_viewtype);
//	outWrite(ofs, "TP Graph Flag: ", target->tp_graphflag);
//	outWrite(ofs, "TP Approx Flag: ", target->tp_approxflag);
//	outWrite(ofs, "TP Cosine Flag: ", target->tp_cosineflag);
//	outWrite(ofs, "TP Sine Flag: ", target->tp_sineflag);
//	outWrite(ofs, "TP Spectrum Flag: ", target->tp_spectrumflag);
//	outWrite(ofs, "Max Val Percent: ", target->_maxvalpercent);
//	outWrite(ofs, "Past Row Index (Size): ", target->past_row, target->_max_horz_count);
//	outWrite(ofs, "Smooth Flag: ", target->_smoothflag);

	errOccurred |= checkError(ofs, "Unable to write to output file");
	
	return TRUE;
}

void readDebugInfo(ifstream& ifs, char *msg)
//
// For debugging: if "msg" != dMsg then out of sychnoize
//
{
	#ifdef DEBUG
		int dTotal;
		char dMsg[256];
		ifs.read((char *) &dTotal, sizeof(dTotal));
		ifs.read((char *) dMsg, dTotal);
		dMsg[dTotal] = 0;
	#endif
}

// ****************************************************************************
// Following inWrite functions are used to write CA variable
//
// ****************************************************************************

ifstream& inWrite(ifstream& ifs, char *msg)
//
// Only used for debug only - msg must be large enough
//
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
	}	
	else ifs.get();   // Only char so ignore it
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, unsigned char *val, int& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
	
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		if (left <= 0)
			ifs.read((char *)val, sizeof(unsigned char) * readTotal);
		else {
			ifs.read((char *)val, sizeof(unsigned char) * total);
			// Skip the rest
			ifs.seekg(left * sizeof(unsigned char), ios::cur);
  		}
	}
	else 
	{
		char inMsg[256];
		int len;
		int readTotal;
		ifs >> len; 
		ifs.getline(inMsg, len+1);
 		ifs >> readTotal;
		for(int count=0; count<readTotal; count++)
		{
			int temp;
			ifs >> temp;
			if (count < total)
				val[count] = (unsigned char)temp;
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, unsigned char *val, unsigned short& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
	
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		if (left <= 0)
			ifs.read((char *)val, sizeof(unsigned char) * readTotal);
		else {
			ifs.read((char *)val, sizeof(unsigned char) * total);
			// Skip the rest
			ifs.seekg(left * sizeof(unsigned char), ios::cur);
		}
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
 		int readTotal;
		ifs >> readTotal;
		for(int count=0; count<readTotal; count++)
		{
			int temp;
			ifs >> temp;
			if (count < total)
				val[count] = (unsigned char) temp; 
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, char *val, int total)
//
// Include Null terminator char.
//
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
	
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		if (left <= 0)
		{
			ifs.read((char *)val, sizeof(char) * readTotal);
			if (left == 0)
				val[readTotal-1] = 0;
			else val[readTotal] = 0;
		}
		else {
			ifs.read((char *)val, sizeof(char) * total);
			val[total-1] = 0;
			// Skip the rest
			ifs.seekg(left * sizeof(char), ios::cur);
		}
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int total;
		ifs >> total;
 		for(int count=0; count<total; count++)
			ifs >> val[count]; 
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, int& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 
	{
		int len;
		char inMsg[256];
		char ch;
		ifs >> len; 
		ifs.getline(inMsg, len+1);
	 	ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, unsigned int& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
	 	ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, long& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
	 	ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, float& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
	 	ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, double& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 	
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
	 	ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, Real *val, int& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		
		if (left <= 0)
			ifs.read((char *)val, sizeof(Real) * readTotal);
		else {
			ifs.read((char *)val, sizeof(Real) * total);
			// Skip the rest
			ifs.seekg(left * sizeof(Real), ios::cur);
		}
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int readTotal;
		ifs >> readTotal;
		for(int count=0; count<readTotal; count++)
		{
			Real temp;
			ifs >> temp;
			if (count < total)
				val[count] = temp;
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, AdditiveTweakParam& val, CA *ca)
{
	Real value;
	inWrite(ifs, msg, value);
//We should be writing and reading the Min() and Max() as well!!!!
	val.SetVal(value);
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, MultiplicativeTweakParam& val, CA *ca)
{
	Real value;
	inWrite(ifs, msg, value);
	val.SetVal(value, ca);
	return ifs;
}


ifstream& inWrite(ifstream& ifs, char *msg, COLORREF& val)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		ifs.read((char *) &val, sizeof(val));
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		ifs >> val;
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, Wavecell *val, int& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		
		if (left <= 0)
			total = readTotal;
		else ;
		for(int count=0; count<total; count++)
		{
			int temp;
			ifs.read((char *) &(val[count].state), sizeof(val[count].state));
			temp = VARIABLE_COUNT;
			inWrite(ifs, "Variable", val[count].variable, temp);
//			ifs.read((char *) &(val[count].intensity), sizeof(val[count].intensity));
			ifs.read((char *) &(val[count].velocity), sizeof(val[count].velocity));

			int totalSub = CELL_PARAM_COUNT;
			char cst[80];
			sprintf(cst, "%5d:", (int)cst); // 2017.  See line 929 also. Had sprintf(cst, "%5d:", cst)
											// 2017 Throws a warning. 
											//---format string '%5d' requires an argument of type 'int', but variadic argument 1  has type 'char *'---
											//The "variadic argument 1" means that second occurance of cst.  The : in the format string is part of the
											//desired string's text?  %5d means print a number in decimal in a five character field possibly padded 
											//by blanks in front.
											//One suggestion I found online is to put p (for pointer) instead of d (for integer). 
											//Supposedly in old all 32 bit world, integer and pointer are the same, but if you have 64 bit world then
											// pointer is 64 bit.  And you have to cast the cst to void * to make the p format happy.
											//So you could try sprintf(cst, "%5p:", (void *)cst);
											// or to make it simpler, I tried sprintf(cst, "%5d:", (int) cst); Seems to work.


			
			inWrite(ifs, cst, &(val[count]._cell_param[0]), totalSub);
		}
		
		int size = sizeof(Wavecell) + 5;	// plus five for XXXXX (count value)
		// Skip the rest
		ifs.seekg(left * size, ios::cur);
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int readTotal;
		ifs >> readTotal;
		for(int count=0; count<readTotal; count++)
		{
			Real rState, rIntensity, rVelocity;
			ifs >> rState >> rIntensity >> rVelocity;
			if (count < total)
			{
				val[count].state = rState;
				val[count].intensity = rIntensity;
				val[count].velocity = rVelocity;
			}
			int readTotal2;
			ifs >> readTotal2;
			for(int count2 = 0; count2 < readTotal2; count2++)
			{
				Real temp;
				ifs >> temp;
				if (count < total)
					val[count]._cell_param[count2] = temp;
			}
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, Wavecell2 *val, int& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		
		if (left <= 0)
			total = readTotal;
		else ;
		for(int count=0; count<total; count++)
		{
			int temp = PLANE_VARIABLE_COUNT;
			inWrite(ifs, "Variable", val[count].variable, temp);
		}
		int size = sizeof(Wavecell2) + 5;	// plus five for XXXXX (count value)
		// Skip the rest
		ifs.seekg(left * size, ios::cur);
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int readTotal;
		ifs >> readTotal;
		for(int count=0; count<readTotal; count++)
		{
			if (count < total)
			{
				int temp = PLANE_VARIABLE_COUNT;
				inWrite(ifs, "Variable", val[count].variable, temp);
			}
			else {
				int temp = PLANE_VARIABLE_COUNT;
				Real tempVal[PLANE_VARIABLE_COUNT];
				inWrite(ifs, "Variable", tempVal, temp);
			}
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, COLORREF *val, int& total)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		int readTotal;
		ifs.read((char *) &readTotal, sizeof(readTotal));
		int left = readTotal - total;
		if (left <= 0)
			ifs.read((char *)val, sizeof(COLORREF) * readTotal);
		else {
			ifs.read((char *)val, sizeof(COLORREF) * total);
			// Skip the rest
			ifs.seekg(left * sizeof(COLORREF), ios::cur);
		}
	}
	else 
	{
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int readTotal;
		ifs >> readTotal;
		for(int count=0; count < readTotal; count++)
		{
			// Real temp;
			COLORREF temp;
			ifs >> temp;
			if (count < total)
				val[count] = temp;
		}
		ifs.get();
	}
	return ifs;
}

ifstream& inWrite(ifstream& ifs, char *msg, vector<class TweakParam *> *userParam)
{
	if (BINARY)
	{
		readDebugInfo(ifs, msg);
		int readTotal;
		int total = userParam->size();
		ifs.read((char *) &readTotal, sizeof(readTotal));
		for(int count = 0; count < readTotal; count++)
		{	
			Real value;
			ifs.read((char *) &value, sizeof(value));
			if (count < total)
				(*userParam)[count]->SetVal(value);
		}
	}
	else 
	{
		Real value;
		int len;
		char inMsg[256];
		ifs >> len; 
		ifs.getline(inMsg, len+1);
		int readTotal;
		int total = userParam->size();
		ifs >> readTotal;
		for(int count = 0; count < readTotal; count++)
		{	
			ifs >> value;
			if (count < total)
				(*userParam)[count]->SetVal(value);
		}
		ifs.get();
	}
	return ifs;
}

BOOL findUserDLL(char *pathFileName)
{
	char fileName[256];
	char path[256];
	ifstream ifs;

	// Try in the default location
	ifs.open(pathFileName, ios::in);
	if(!ifs.fail())
	{	// Find in default location
		ifs.close();
		return TRUE;
	}
	ifs.clear();
	// Extract path and file name
// 2017 The code didn't complie with the following two lines, which seem to be an incorrectly written for loop and non-shareable post variable.
	
	/* for(int post = strlen(pathFileName); 
		(pathFileName[post] != '\\') && post >= 0; post--); */
		
	//New code 2017.
	int post;
	for (post = strlen(pathFileName); pathFileName[post] != '\\' && post >= 0; post--);

	//2017 The rest of the code is the same.

	if (post < 0 || pathFileName[post] != '\\')
	{
		strcpy(fileName, pathFileName);
		strcpy(path, "");
	}
	else
	{
		strcpy(path, pathFileName);
		path[post] = 0;
		strcpy(fileName, pathFileName+post+1);
	}
	// try in the current directory
	char curDirectory[256];
	char testFileName[256];
	GetCurrentDirectory(256, curDirectory);
	strcpy(testFileName, curDirectory);
	strcat(testFileName, "\\");
	strcat(testFileName, fileName);
	ifs.open(testFileName, ios::in);
	if(!ifs.fail())
	{	// Find in current directory
		ifs.close();
		strcpy(pathFileName, testFileName);
		return TRUE;
	}
	ifs.clear();
	// Try experiment directory
	strcpy(testFileName, capowDirectory);
	strcat(testFileName, "\\Files To Open\\");
	strcat(testFileName, fileName);
	ifs.open(testFileName, ios::in);
	if(!ifs.fail())
	{	// Find in experiment directory
		ifs.close();
		strcpy(pathFileName, testFileName);
		return TRUE;
	}
	ifs.clear();
	// Try User Rules directory
	strcpy(testFileName, capowDirectory);
	strcat(testFileName, "\\User Rules\\");
	strcat(testFileName, fileName);
	ifs.open(testFileName, ios::in);
	if(!ifs.fail())
	{	// Find in User Rules dirctory
		ifs.close();
		strcpy(pathFileName, testFileName);
		return TRUE;
	}
	ifs.clear();
	// Try capow directory
	strcpy(testFileName, capowDirectory);
	strcat(testFileName, "\\");
	strcat(testFileName, fileName);
	ifs.open(testFileName, ios::in);
	if(!ifs.fail())
	{	// Find in CAPOW dirctory
		ifs.close();
		strcpy(pathFileName, testFileName);
		return TRUE;
	}
	ifs.close();
	// Let user select
	char szFileTitle[MAXFILENAME];
	OPENFILENAME ofn;
	char szFilterSpecDLL [128] = "DLL Files (*.DLL)\0";
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner		  = masterhwnd;
	ofn.lpstrFilter		  = szFilterSpecDLL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	  = 0;
	ofn.nFilterIndex	  = 1;
	ofn.lpstrFile         = fileName;
	ofn.nMaxFile		  = MAXFILENAME;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrFileTitle    = szFileTitle;
	ofn.nMaxFileTitle     = MAXFILENAME;
	char title[256] = "Please select ";
	strcat(title, fileName);
	ofn.lpstrTitle        = title;
	ofn.lpstrDefExt       = "DLL";
	ofn.Flags             = OFN_PATHMUSTEXIST;
	if(GetOpenFileName((LPOPENFILENAME)&ofn) )
	{
		strcpy(pathFileName, ofn.lpstrFile);					
		return TRUE;
	}
	else return FALSE;
}

BOOL inBinary(ifstream& ifs, CA* target) 
//
// Version 8 - binary/Text
//
{	// Version and load Cell flag
	int version;
	int temp;
	char msg[256];

	inWrite(ifs, "Version: ", version);
	inWrite(ifs, "Load Save Cell Flag: ", load_save_cells_flag);

	// CA Type
	int loadTypeCA;
	int loadRadius;
	int loadStates;

	inWrite(ifs, "CA Type: ", loadTypeCA);
	
	if (loadTypeCA == CA_USER)
	{
		char userRuleName[MAXFILENAME+80];
		inWrite(ifs, "User Rule Name: ", userRuleName, MAXFILENAME);
		if (!findUserDLL(userRuleName))
			return FALSE;
		// Must Reload user rule (DLL)
		if(!target->LoadUserRule(masterhwnd, userRuleName))
			return FALSE;
		inWrite(ifs, "User Parameter: ", &target->userParamAdd);
		if (target == target->calist_ptr->FocusCA())
		{
			recreateUserDialog();
		}
	}
	
	inWrite(ifs, "Radius: ", loadRadius);
	inWrite(ifs, "States: ", loadStates);
	
	target->Settype(loadTypeCA);
	target->Changeradiusandstates(loadRadius, loadStates);

	if (target->type_ca < CA_WAVE)
	{
		temp = MAXNABEOPTIONS;	// Max storage
		inWrite(ifs, "Nabe Options (LooKUp Size): ", target->lookup, temp);
	}

	inWrite(ifs, "Show Mode: ", target->showmode);
	inWrite(ifs, "Generator Flag: ", target->generatorflag);
	inWrite(ifs, "WrapFlag: ", target->wrapflag);
	inWrite(ifs, "Lambda: ", target->lambda);
	inWrite(ifs, "Target Entropy: ", target->target_entropy);
	inWrite(ifs, "Bonus Entropy: ", target->entropy_bonus);
	inWrite(ifs, "Fail Stripe: ", target->fail_stripe);
	

	// Standard CA base
//Don't need read horz_count in as you aren't changing the size of the CA
//so this is bogus, but we fix it with Locate
	inWrite(ifs, "Horizontal Count: ", target->horz_count);

	inWrite(ifs, "Blank Edge Flag: ", target->blankedgeflag);
	inWrite(ifs, "Entropy Flag: ", target->entropyflag);
	inWrite(ifs, "View Mode: ", target->viewmode);
	temp = MAX_BAND_COUNT;
	inWrite(ifs, "Anchor Color: ", target->_anchor_color, temp);
	if (target->type_ca >= CA_WAVE)
	{
		inWrite(ifs, "DX: ", target->_dx, target);
		inWrite(ifs, "DT: ", target->_dt, target);
		inWrite(ifs, "Band Count: ", target->band_count);
		inWrite(ifs, "Heat Inc: ", target->_heat_inc, target);
		inWrite(ifs, "Max Intensity: ", target->_max_intensity, target);
		inWrite(ifs, "Max Velocity: ", target->_max_velocity, target);
		inWrite(ifs, "Friction Multiplier: ", target->_friction_multiplier, target);
		inWrite(ifs, "Spring: ", target->_spring_multiplier, target);
		inWrite(ifs, "Driver Multiplier: ", target->_driver_multiplier, target);
		inWrite(ifs, "Freq Multiplier: ", target->_frequency_multiplier, target);
		inWrite(ifs, "Mass: ", target->_mass, target);
		inWrite(ifs, "Nabe Size: ", target->nabesize);
		inWrite(ifs, "Chunk: ", target->_chunk, target);
		inWrite(ifs, "DX Lock: ", target->_dx_lock);
		inWrite(ifs, "Show Velocity: ", target->showvelocity);
		inWrite(ifs, "Time: ", target->time);
		inWrite(ifs, "Non Linearity 1: ", target->_nonlinearity1, target);
		inWrite(ifs, "Non Linearity 2: ", target->_nonlinearity2, target);

		inWrite(ifs, "Variance: ", target->_variance, target);
		inWrite(ifs, "Phase: ", target->_phase);
		inWrite(ifs, "Monochrome Flag: ", target->monochromeflag);
		inWrite(ifs, "Horz Count 2D: ", target->horz_count_2D);
		inWrite(ifs, "Vert Count 2D: ", target->vert_count_2D);
		inWrite(ifs, "Max X 2D: ", target->maxx_2D);
		inWrite(ifs, "Max Y 2D: ", target->maxy_2D);
	}

	if (load_save_cells_flag)
	{
		// Set this flag so that the call to SetWrapflag inside of CA::Locate
		//	will not reset smoothsteps and change the new cell info . 
		target->_justloadedcells = TRUE;
		// reset all buffers so the loaded information goes into rowbuffer[0] 
		target->sourcerowindex = 0;
		target->targetrowindex = 1;
		target->pastrowindex = MEMORY - 1;
		target->source_row = target->rowbuffer[0];
		target->target_row = target->rowbuffer[1];
		target->past_row = target->rowbuffer[MEMORY - 1];
		target->wavesourceindex = 0;
		target->wavetargetindex = 1;
		target->wavepastindex = 2;
		target->wave_source_row = target->waverowbuffer[0];
		target->wave_target_row = target->waverowbuffer[1];
		target->wave_past_row = target->waverowbuffer[2];
		target->SyncRows(); //I think we need this
		switch(target->type_ca)
		{
			case CA_STANDARD:
			case CA_REVERSIBLE:
				temp = target->_max_horz_count; 	
				inWrite(ifs, "Source Row Index (size): ", target->source_row, temp);
				temp = target->_max_horz_count;
				inWrite(ifs, "Past Row Index (Size): ", target->past_row, temp);
				break;
			case CA_WAVE:
			case CA_WAVE2:
			case CA_HEATWAVE:
			case CA_HEATWAVE2:
			case CA_OSCILLATOR:
 //			case CA_OSCILLATOR_WAVE:
			case CA_DIVERSE_OSCILLATOR:
			case CA_DIVERSE_OSCILLATOR_WAVE:
			case CA_ULAM_WAVE:
			case CA_AUTO_ULAM_WAVE:
			case ALT_CA_WAVE:
			case ALT_CA_WAVE2:
			case ALT_CA_OSCILLATOR:
			case ALT_CA_OSCILLATOR_WAVE:
			case ALT_CA_DIVERSE_OSCILLATOR:
			case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
			case ALT_CA_ULAM_WAVE:
			case CA_CUBIC_ULAM_WAVE:
				temp = target->_max_horz_count;
				inWrite(ifs, "Wave Source Row: ", target->wave_source_row, temp);
				temp = target->_max_horz_count;
				inWrite(ifs, "Wave Past Row: ", target->wave_past_row, temp);
				break;
			case CA_WAVE_2D:
			case CA_HEAT_2D:
				temp = (int)CX_2D * CY_2D;
				inWrite(ifs, "Wave Source Plane: ", target->wave_source_plane, temp);
				temp = (int)CX_2D * CY_2D;
				inWrite(ifs, "Wave Past Plane: ", target->wave_past_plane, temp);
				break;
			case CA_USER:
				switch(target->_usercastyle)
				{
					case CA_STANDARD:
					case CA_REVERSIBLE:
						temp = target->_max_horz_count; 	
						inWrite(ifs, "Source Row Index (size): ", target->source_row, temp);
						temp = target->_max_horz_count;
						inWrite(ifs, "Past Row Index (Size): ", target->past_row, temp);
						break;
					case CA_WAVE:
					case CA_WAVE2:
					case CA_HEATWAVE:
					case CA_HEATWAVE2:
					case CA_OSCILLATOR:
 //					case CA_OSCILLATOR_WAVE:
					case CA_DIVERSE_OSCILLATOR:
					case CA_DIVERSE_OSCILLATOR_WAVE:
					case CA_ULAM_WAVE:
					case CA_AUTO_ULAM_WAVE:
					case ALT_CA_WAVE:
					case ALT_CA_WAVE2:
					case ALT_CA_OSCILLATOR:
					case ALT_CA_OSCILLATOR_WAVE:
					case ALT_CA_DIVERSE_OSCILLATOR:
					case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
					case ALT_CA_ULAM_WAVE:
					case CA_CUBIC_ULAM_WAVE:
						temp = target->_max_horz_count;
						inWrite(ifs, "Wave Source Row: ", target->wave_source_row, temp);
						temp = target->_max_horz_count;
						inWrite(ifs, "Wave Past Row: ", target->wave_past_row, temp);
						break;
					case CA_WAVE_2D:
					case CA_HEAT_2D:
						temp = (int)CX_2D * CY_2D;
						inWrite(ifs, "Wave Source Plane: ", target->wave_source_plane, temp);
						temp = (int)CX_2D * CY_2D;
						inWrite(ifs, "Wave Past Plane: ", target->wave_past_plane, temp);
						break;
				}
				break;
		}

	}
	target->Adjust_acceleration_multiplier();
	target->Computeactual_lambda();
	target->entropy = 0.0;
	target->score = 0;
	if (!load_save_cells_flag)
		target->Seed();
	target->Setcolortable();//install the anchor colors.
	target->Set_monochromeflag(target->monochromeflag); //Rudy 11/10/97
	target->Locate(target->tile_number, masterhwnd, target->calist_ptr->count_per_edge());

//	inWrite(ifs, "Phylum: ", target->_phylum);
//	inWrite(ifs, "Test Point: ", target->test_point);
//	inWrite(ifs, "Temp Test Point: ", target->temp_test_point);
//	inWrite(ifs, "Rel Test Point: ", target->rel_test_point);
//	int temp = 4*target->_max_horz_count;
//	inWrite(ifs, "Fourier Array: ", target->tp_real_array, temp); // should be 4*target->_max_horz_count
//	temp = MAXTERM;
//	inWrite(ifs, "Fourier A: ", target->fourier_a, temp); // Should be MAXTERM
//	temp = MAXTERM;
//	inWrite(ifs, "Fourier A: ", target->fourier_b, temp); // Should be MAXTERM
//	inWrite(ifs, "# of term: ", target->numofterm);
//	temp = 2*target->_max_horz_count;
//	inWrite(ifs, "Fourier Approx: ", target->fourier_approx, temp);
//	inWrite(ifs, "TP Start Pos: ", target->tp_startpos);
//	inWrite(ifs, "TP End Pos: ", target->tp_endpos);
//	inWrite(ifs, "Fourier Flag: ", target->fourierflag);
//	inWrite(ifs, "TP View Mode: ", target->tp_viewmode);
//	inWrite(ifs, "TP View Type: ", target->tp_viewtype);
//	inWrite(ifs, "TP Graph Flag: ", target->tp_graphflag);
//	inWrite(ifs, "TP Approx Flag: ", target->tp_approxflag);
//	inWrite(ifs, "TP Cosine Flag: ", target->tp_cosineflag);
//	inWrite(ifs, "TP Sine Flag: ", target->tp_sineflag);
//	inWrite(ifs, "TP Spectrum Flag: ", target->tp_spectrumflag);
//	inWrite(ifs, "Max Val Percent: ", target->_maxvalpercent, target);
//	temp = target->_max_horz_count;
//	inWrite(ifs, "Past Row Index (Size): ", target->past_row, temp);
//	inWrite(ifs, "Smooth Flag: ", target->_smoothflag);
	return TRUE;
}


//==========CAlist Load and Save ===================================

void CAlist::Saveindividual(char *filename, CA *target)
{
	ofstream ofs;
	
	errOccurred = FALSE;
	if (!BINARY)
 		ofs.open(filename, ios::out); 
	else ofs.open(filename, ios::out|ios::binary); 
	if (!checkError(ofs, "Unable to create file"))
	{
		#ifdef LIMIT_PRECISION
			ofs.setf(ios::fixed |ios::showpoint);
			ofs.precision(LOADSAVE_PRECISION);
		#endif
		ofs.write(FILETYPE, strlen(FILETYPE));
		outWrite(ofs, "Number of CA: ", (int) 1);  // Must cast to an int because "count" is int
		ofs << target;
	}
	ofs.close();

	if (compressFile && !errOccurred)
		compressTheFile(filename);
}

void CAlist::Saveall(char *filename, BOOL auto_overwrite)
{
	ofstream testofs, ofs,outfile;
	ifstream infile;

	errOccurred = FALSE;

	if (!BINARY)
	{
		ofs.open(filename,ios::out);
	}
	else ofs.open(filename,ios::out | ios::binary);
	if (checkError(ofs, "Unable to create file"))
	{
		ofs.close();
		return;	
	}
	if (!BINARY)
	{
		#ifdef LIMIT_PRECISION
			ofs.setf(ios::fixed |ios::showpoint);
			ofs.precision(LOADSAVE_PRECISION);
		#endif
	}
	ofs.write(FILETYPE, strlen(FILETYPE));
	outWrite(ofs, "Number of CA: ", count);
	for (int i = 0; i < count; i++)
	{
		ofs << list[i];
		if (errOccurred)
		{	
			errOccurred = FALSE;
			ofs.close();
			return;
		}
	}
	outWrite(ofs, "Zoom Flag: ", zoomflag);
	outWrite(ofs, "Mutation Strength: ", mutation_strength);
	outWrite(ofs, "CA Count Per Edge: ", CA_count_per_edge);
	outWrite(ofs, "Mutate Flag: ", mutateflag);
	outWrite(ofs, "Breed Flag: ", breedflag);
	outWrite(ofs, "Breed Cycle: ", breedcycle);
	outWrite(ofs, "Bread Cycle Count: ", breedcycle_count);
	outWrite(ofs, "Evolve Flag: ", evolveflag);
	outWrite(ofs, "View Mode: ", (int)IDC_SPLIT_VIEW);
	
	// Next line is a workaround, as showvelocity was moved from CAlist to CA.
//	outWrite(ofs, "Show velocity: ", Getshowvelocity());
	outWrite(ofs, "Graph flag: ", 0); //(unsigned int) graphflag;
	outWrite(ofs, "Wire flag: ", 0); //(unsigned int) wireflag;
	outWrite(ofs, "Scroll flag: ", 0); //(unsigned int) scrollflag
	outWrite(ofs, "Stripekill flag: ", stripekillflag);
	outWrite(ofs, "Stripe seed flag: ", stripeseedflag);
	RECT scr;
	GetWindowRect(hwnd, &scr);
	outWrite(ofs, "Window top: ", scr.top);
	outWrite(ofs, "Window bottom: ", scr.bottom);
	outWrite(ofs, "Window right: ", scr.right);
	outWrite(ofs, "Window left: ", scr.left);
	outWrite(ofs, "Focus index: ", get_focus_index());

	checkError(ofs, "Unable to write to file");

	ofs.close();

	if (compressFile && !errOccurred)
	{
		compressTheFile(filename);
	}
}

void compressTheFile(char *pathFileName)
{
	// Rename it to a temp name
	char fileName[256];
	char path[256];
	char tempFileName[256];
	char realFileName[256];
	char curDirectory[256];

	GetCurrentDirectory(256, curDirectory);
	// 2017 The code didn't complie with the following two lines, which seem to be an incorrectly written for loop and non-shareable post variable.

	/* for(int post = strlen(pathFileName);
	(pathFileName[post] != '\\') && post >= 0; post--); */

	//New code 2017.
	int post;
	for (post = strlen(pathFileName); pathFileName[post] != '\\' && post >= 0; post--);

	//2017 The rest of the code is the same.
	if (post < 0 || pathFileName[post] != '\\')
	{
		strcpy(fileName, pathFileName);
		strcpy(path, "");
	}
	else
	{
		strcpy(path, pathFileName);
		path[post] = 0;
		strcpy(fileName, pathFileName+post+1);
	}
	SetCurrentDirectory(path);		
	
	strcpy(tempFileName, fileName);
	tempFileName[strlen(tempFileName)-1] = '~';
	DeleteFile(tempFileName);	// Remove old temp file if exist
	if (!MoveFile(fileName, tempFileName))
	{
		MessageBox(masterhwnd, 
			(LPSTR) "Saved as non-compress mode",
			(LPSTR)"File Error", MB_OK);
		SetCurrentDirectory(curDirectory);		
		return;
	}

	// Compress it
	char compressProgName[256];
	strcpy(compressProgName, capowDirectory);
	strcat(compressProgName, "\\compress");
	
	int res = _spawnl(_P_WAIT, compressProgName, compressProgName, 
					   tempFileName, fileName, NULL);
	if (res == -1)
	{	// Fail to compress
		if (MoveFile(tempFileName, fileName))
		{
			if (errno == ENOENT)
				MessageBox(masterhwnd, 
					   (LPSTR) "Saved as non-compress mode\nCompress.exe not found",
					   (LPSTR)"File Error", MB_OK);
			else MessageBox(masterhwnd, 
					   (LPSTR) "Saved as non-compress mode",
					   (LPSTR)"File Error", MB_OK);
			SetCurrentDirectory(curDirectory);		
			return;
		}
		else 
		{ 
			if (errno == ENOENT)
				MessageBox(masterhwnd, 
					(LPSTR) "Compress.exe not found",
					(LPSTR)"File Error - Compress.exe not found", MB_OK);
			else MessageBox(masterhwnd, 
					(LPSTR) "Unable to rename file",
					(LPSTR)"File Error", MB_OK);

		}
	}
	else
	{	// Check if successful
		ifstream ifs;
		ifs.open(fileName, ios::in);
		if(ifs.fail())
		{
			ifs.clear();
			// Fail to compress
			if (MoveFile(tempFileName, fileName))
			{
				if (errno == ENOENT)
					MessageBox(masterhwnd, 
						   (LPSTR) "Saved as non-compress mode\nCompress.exe not found",
						   (LPSTR)"File Error", MB_OK);
				else MessageBox(masterhwnd, 
						   (LPSTR) "Saved as non-compress mode",
						   (LPSTR)"File Error", MB_OK);
				SetCurrentDirectory(curDirectory);		
				return;
			}
			else 
			{ 
				if (errno == ENOENT)
					MessageBox(masterhwnd, 
						(LPSTR) "Compress.exe not found",
						(LPSTR)"File Error - Compress.exe not found", MB_OK);
				else MessageBox(masterhwnd, 
						(LPSTR) "Unable to rename file",
						(LPSTR)"File Error", MB_OK);

			}
		}
		else ifs.close();
	}
	DeleteFile(tempFileName);
	SetCurrentDirectory(curDirectory);		
}

//----------------------------------------------------------------------
BOOL CAlist::Load_Individual(char* filename, CA *target)
{
	ifstream ifs;
	char	fileType[25];
	int		filecount;
	OFSTRUCT	fileStruct;
	errOccurred = FALSE;
	INT compressFileHandler = -1;
	BOOL removeCompress = FALSE;

	if (!BINARY)
		ifs.open(filename, ios::in);
	else ifs.open(filename, ios::in|ios::binary);
	if (checkError(ifs, "Unable to open file"))
	{
		ifs.close();
		return FALSE;	
	}	
	if (!BINARY)
	{
		#ifdef LIMIT_PRECISION
			ifs.setf(ios::fixed |ios::showpoint);
			ifs.precision(LOADSAVE_PRECISION);
		#endif
	}
	ifs.read(fileType, 2);
	fileType[2] = 0;
	if (strcmp(fileType, FILETYPE) != 0)
	{	
		ifs.close();
		
		// Check if it is a compressed file
		compressFileHandler = LZOpenFile(filename, &fileStruct, OF_READ);
		switch(compressFileHandler)
		{
			case LZERROR_GLOBALLOC:
			case LZERROR_BADINHANDLE:
				MessageBox(masterhwnd, (LPSTR) "Invalid file",
					       (LPSTR)"File Error", MB_OK);
				return FALSE;
		}	
		INT result = LZRead(compressFileHandler, fileType, 2);
		switch(result)
		{
			case LZERROR_BADINHANDLE:
			case LZERROR_BADOUTHANDLE:
			case LZERROR_BADVALUE:
			case LZERROR_GLOBALLOC:	
			case LZERROR_GLOBLOCK:
			case LZERROR_READ:
			case LZERROR_WRITE:
				LZClose(compressFileHandler);
				MessageBox(masterhwnd, (LPSTR) "Unable to read file",
					       (LPSTR)"File Error", MB_OK);
				return FALSE;
		}
		if (result != 2)
		{
			LZClose(compressFileHandler);
			MessageBox(masterhwnd, (LPSTR) "Unable to read file",
				       (LPSTR)"File Error", MB_OK);
			return FALSE;
		}
		fileType[2] = 0;
		if (strcmp(fileType, FILETYPE) != 0)
		{	LZClose(compressFileHandler);
			// May be previous version format, so prompt for trial
			if (MessageBox(masterhwnd, 
			           (LPSTR) "Invalid file type\nIt may be a previous release, do you want to try open it?",
				       (LPSTR)"File Error", MB_YESNOCANCEL | MB_ICONEXCLAMATION ) == IDYES)
			{
				Load_Individual7(filename, target);
				return TRUE;
			}
			else
				return FALSE;
		}
		// File is compressed, decompress it
//		strcat(filename, "~"); This doesn't always work!
		filename[strlen(filename)-1] = '~'; //Do it by hand
		INT newCompressFileHandler = LZOpenFile(filename, &fileStruct, OF_CREATE | OF_WRITE);
		result = LZCopy(compressFileHandler, newCompressFileHandler);
		LZClose(compressFileHandler);
		LZClose(newCompressFileHandler);
		switch(result)
		{
			case LZERROR_BADINHANDLE:
			case LZERROR_BADOUTHANDLE:
			case LZERROR_GLOBALLOC:
			case LZERROR_GLOBLOCK:
			case LZERROR_READ:
				MessageBox(masterhwnd, (LPSTR)"Unable to create temporary file",
					(LPSTR)"File Error", MB_OK);
				return FALSE;
		}
		removeCompress = TRUE;
		if (!BINARY)
			ifs.open(filename, ios::in);
		else ifs.open(filename, ios::in|ios::binary);
		if (checkError(ifs, "Unable to open file"))
		{
			ifs.close();
			return FALSE;	
		}	
		if (!BINARY)
		{
			#ifdef LIMIT_PRECISION
				ifs.setf(ios::fixed |ios::showpoint);
				ifs.precision(LOADSAVE_PRECISION);
			#endif
		}
		ifs.read(fileType, 2);
		fileType[2] = 0;
		if (strcmp(fileType, FILETYPE) != 0)
		{
			ifs.close();
			MessageBox(masterhwnd, (LPSTR) "Invalid file type",
					       (LPSTR)"File Error", MB_OK);
			return FALSE;
		}
	}

	inWrite(ifs, "Number of CA: ", filecount);
	if (filecount != 1)
	{
		MessageBox(masterhwnd, (LPSTR) "Invalid file format",
				   (LPSTR)"File Error", MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	ifs >> target;
	ifs.close();
	Locate(); //This will prevent having an ugly lip at the bottom of 
	//the scrolling CA.  It *will* make a ding line in the other scrolling
	//CAs, but this is acceptale.  The main thing it is doing is setting
	//blt_flag to 0 and making all of the line counters the same.
	if (!errOccurred) 
	{
		// If we load a new file, start breed cycle all over to
		// give the new CA a fighting chance 
		if (breedflag)
	//		if (MessageBox(hwnd,
	//					   (LPSTR)"Reset Breedcycle counter and all scores?",
	//					   (LPSTR)"", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)
			{
				Resetscores();
				breedcycle_count = 0;
			}
		if (removeCompress)
			LZOpenFile(filename, &fileStruct, OF_DELETE);
	}
	return TRUE;
}

BOOL CAlist::Loadall_Individual(char* filename)// Load all CA with same *.CA
{
	if (!Load_Individual(filename, focus))
		return FALSE;
	for (int i = 0; i < count; i++)
		if (list[i] != focus)
		{
			list[i]->_usercastyle = focus->_usercastyle; 
			list[i]->_usernabesize = focus->_usernabesize;

			list[i]->Settype(focus->Gettype());
			list[i]->CopyCA(focus);
//			list[i]->Mutate(mutation_strength);
		}
	return TRUE;
} 


BOOL CAlist::Loadall(char* filename, BOOL startup)
{
	int filecount = 0;
	char msg[350] = "Unable to open file:";
	ifstream ifs;
	char fileType[25];
	INT compressFileHandler = -1;
	OFSTRUCT	fileStruct;
	BOOL removeCompress = FALSE;

	errOccurred = FALSE;

	if (!BINARY)
		ifs.open(filename, ios::in);
	else ifs.open(filename, ios::in|ios::binary);
	if (checkError(ifs, ""))
	{
		strcat(msg, filename);
		if (!strcmp(filename, "ACTIVE.CAS"))
			MessageBox(masterhwnd, (LPSTR)"Using Default Parameters", 
			           (LPSTR)"No ACTIVE.CAS File", MB_OK | MB_ICONEXCLAMATION );
		else MessageBox(masterhwnd, (LPSTR) msg,
				(LPSTR)"File Error", MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
//BUG When I am trying to read callwave.cas I jump from 70 lines down below up to
//this position and exit without doing the file load...
		return FALSE; 
	}
	if (!BINARY)
	{
		#ifdef LIMIT_PRECISION
			ifs.setf(ios::fixed |ios::showpoint);
			ifs.precision(LOADSAVE_PRECISION);
		#endif
	}
	ifs.read(fileType, 2);
	fileType[2] = 0;
	if (strcmp(fileType, FILETYPE) != 0)
	{
		ifs.close();

		// Check if it is a compressed file
		compressFileHandler = LZOpenFile(filename, &fileStruct, OF_READ);
		switch(compressFileHandler)
		{
			case LZERROR_GLOBALLOC:
			case LZERROR_BADINHANDLE:
				MessageBox(masterhwnd, (LPSTR) "Invalid file",
					       (LPSTR)"File Error", MB_OK);
				return FALSE;
		}	
		INT result = LZRead(compressFileHandler, fileType, 2);
		switch(result)
		{
			case LZERROR_BADINHANDLE:
			case LZERROR_BADOUTHANDLE:
			case LZERROR_BADVALUE:
			case LZERROR_GLOBALLOC:	
			case LZERROR_GLOBLOCK:
			case LZERROR_READ:
			case LZERROR_WRITE:
				LZClose(compressFileHandler);
				MessageBox(masterhwnd, (LPSTR) "Unable to read file",
					       (LPSTR)"File Error", MB_OK);
				return FALSE;
		}
		if (result != 2)
		{
			LZClose(compressFileHandler);
			MessageBox(masterhwnd, (LPSTR) "Unable to read file",
				       (LPSTR)"File Error", MB_OK);
			return FALSE;
		}
		fileType[2] = 0;
		if (strcmp(fileType, FILETYPE) != 0)
		{	LZClose(compressFileHandler);
			// May be previous version format, so prompt for trial
			if (MessageBox(masterhwnd, 
			           (LPSTR) "Invalid file type\nIt may be a previous release, do you want to try open it?",
				       (LPSTR)"File Error", MB_YESNOCANCEL | MB_ICONEXCLAMATION ) == IDYES)
			{
				return Loadall7(filename, startup);
			}
			else return FALSE;
		}
		
		// File is compressed, decompress it
//		strcat(filename, "~"); Doesn't always work.
		filename[strlen(filename)-1] = '~'; //Do it by hand
		INT newCompressFileHandler = LZOpenFile(filename, &fileStruct, OF_CREATE | OF_WRITE);
		result = LZCopy(compressFileHandler, newCompressFileHandler);
		LZClose(compressFileHandler);
		LZClose(newCompressFileHandler);
		switch(result)
//BUG.  When I load a compressed callwave.cas file, after this switch
//I jump up to a position above here in the code...
		{
			case LZERROR_BADINHANDLE:
			case LZERROR_BADOUTHANDLE:
			case LZERROR_GLOBALLOC:
			case LZERROR_GLOBLOCK:
			case LZERROR_READ:
				MessageBox(masterhwnd, (LPSTR)"Unable to create temporary file",
					(LPSTR)"File Error", MB_OK);
				return FALSE;
		}
		removeCompress = TRUE;
		if (!BINARY)
			ifs.open(filename, ios::in);
		else ifs.open(filename, ios::in|ios::binary);
		if (checkError(ifs, "Unable to open file"))
		{
			ifs.close();
				return FALSE;
		}	
		if (!BINARY)
		{
			#ifdef LIMIT_PRECISION
				ifs.setf(ios::fixed |ios::showpoint);
				ifs.precision(LOADSAVE_PRECISION);
			#endif
		}
		ifs.read(fileType, 2);
		fileType[2] = 0;
		if (strcmp(fileType, FILETYPE) != 0)
		{
			ifs.close();
			MessageBox(masterhwnd, (LPSTR) "Invalid file type",
					       (LPSTR)"File Error", MB_OK);
			return FALSE;
		}
	}

	inWrite(ifs, "Number of CA: ", filecount);
	if (filecount == 0)
	{	// load an empty *.CAS file 
		MessageBox(masterhwnd, (LPSTR)"Invalid file format", (LPSTR)"File Error",
				   MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	else if (filecount > MAX_CAS)
	{	// More CA in file than program can handle
		MessageBox(masterhwnd, (LPSTR)"Invalid file format, possible wrong version",
				   (LPSTR)"File Error", MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	else count = filecount; 
	
	for (int i = 0; i < count; i++)
	{
		ifs >> list[i];
		if (errOccurred)
		{	
			errOccurred = FALSE;
			ifs.close();
			return FALSE;
		}
	}
	BOOL tempzoomflag;
	inWrite(ifs, "Zoom Flag: ", tempzoomflag);
	//Use tempzoomflag  later after everything else is loaded.
	inWrite(ifs, "Mutation Strength: ", mutation_strength);
	inWrite(ifs, "CA Count Per Edge: ", CA_count_per_edge);
	inWrite(ifs, "Mutate Flag: ", mutateflag);
	inWrite(ifs, "Breed Flag: ", breedflag);
	inWrite(ifs, "Breed Cycle: ", breedcycle);
	inWrite(ifs, "Bread Cycle Count: ", breedcycle_count);
	inWrite(ifs, "Evolve Flag: ", evolveflag);
	int temp;
	inWrite(ifs, "View Mode: ", temp);
	
//	inWrite(ifs, "Show velocity: ", temp);
	inWrite(ifs, "Graph flag: ", temp); //(unsigned int) graphflag;
	inWrite(ifs, "Wire flag: ", temp); //(unsigned int) wireflag;
	inWrite(ifs, "Scroll flag: ", temp); //(unsigned int) scrollflag
	inWrite(ifs, "Stripekill flag: ", stripekillflag);
	inWrite(ifs, "Stripe seed flag: ", stripeseedflag);

	RECT scr;
	GetWindowRect(hwnd, &scr);
	inWrite(ifs, "Window top: ", scr.top);
	inWrite(ifs, "Window bottom: ", scr.bottom);
	inWrite(ifs, "Window right: ", scr.right);
	inWrite(ifs, "Window left: ", scr.left);
	int focus_index;
	inWrite(ifs, "Focus index: ", focus_index);

	if (checkError(ifs, "Reading file error"))
	{
		ifs.close();
		return FALSE;
	}
	ifs.close();
		
	BOOL old_not_seeded_yet_flag = not_seeded_yet_flag;
	not_seeded_yet_flag = FALSE;
	if(!(SetWindowPos(hwnd,  HWND_NOTOPMOST, scr.left, scr.top,
	      (scr.right - scr.left),(scr.bottom - scr.top), SWP_NOMOVE)))
	{
		MessageBox(hwnd, (LPSTR)"Resizing window",
			(LPSTR)"Unable to resize windows", MB_OK | MB_ICONEXCLAMATION );
	}
	blt_flag = 0; //This prevents the ugly lip at bottom of scrolling CAs. 
	SendMessage(masterhwnd, WM_RBUTTONDOWN, 0, 0L); //This zooms you out so
	//you can load properly, we had a problem loading unzoomed over zoomed.
	HDC hdc = GetDC(masterhwnd);
	Setfocusindex(hdc, focus_index);
	DrawDivider(hdc);
	Boxfocus(hdc,RGB(255,255,255));
	ReleaseDC(hwnd, hdc);
	not_seeded_yet_flag = old_not_seeded_yet_flag;
	BOOL mustfixGL = FALSE;
	if (tempzoomflag)
		mustfixGL = TRUE; //See comment two lines down
	Zoom(tempzoomflag); //So you are zoomed or not zoomed just like experiment was.
		//Note that we have to do this AFTER we rest to the old focus or we'll zoom
		//in the wrong place.
	if (tempzoomflag)
		zoomviewflag = TRUE;
/*The next lines fix a bug relating to the OpenGL view of 2D Cas.  Often
when you zoom in on a 2D CA this view was coming up dead with no action and
would only wake up when the user resized the window.  So we fake a resize,
and this gets rid of the bug! Rudy 5/21/97.  We also do this in WM_LBUTTONDOWN*/
	RECT rect;
	if (mustfixGL)
	{
		GetClientRect(masterhwnd, &rect);
		SendMessage(masterhwnd, WM_SIZE, SIZE_RESTORED,
			MAKELONG(rect.right, rect.bottom));// to wake up OpenGL
	}

	// Start breed cycle all over to give the new ca a fighting chance 
	if (breedflag)
//		if (MessageBox( hwnd,
//					(LPSTR)"",
//					(LPSTR)"Reset Breedcycle counter and all scores?",
//					MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)  */
		{
			Resetscores();
			breedcycle_count = 0;
		}
	if (load_save_cells_flag)
	{
		// Before you resize, set the _justloadedcells flags to TRUE again
		// for the second call to Locate (a first one took place in ift >> list[i]) 
		for (int i = 0; i < count; i++)
			list[i]->Set_justloadedcells(TRUE); // These are the CA mutators.
		Set_justloadedcells(TRUE); // This is the CAlist mutator.
	}
	if (removeCompress)
	{
		LZOpenFile(filename, &fileStruct, OF_DELETE);
	}
	return TRUE;
}
