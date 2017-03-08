// ***************************************************************
// File: loadsav7.cpp
//
// This file contains code to support reading previous version
//	(1-7) of *ca and *.cas files.
// ***************************************************************

#include "ca.hpp"
#include "resource.h"
#include "tweakca.hpp"
#include <math.h>
#include <iomanip> //2017 no .h on this anymore
// 2017 replaced ios::in|ios::nocreate by ios::in, as the nocreate flag no longer exists 
#include <stdlib.h> //for itoa                       
#include <ctype.h>  //for isdigit
#include <string.h>

#define LIMIT_PRECISION
/* If you want to try not controlling the precision at all, then
comment LIMIT_PRECISION out. At present, the loading of the
real numbers doesn't work anyway.*/
#define LOADSAVE_PRECISION 3
/* This is the amount of precision used in saving the intensity
and parameter values.*/

BOOL compress_flag = FALSE;
// BOOL compress_flag = TRUE;
/*If this is TRUE, we use binary file io, if FALSE we use ASCII file io.  
This does not seem to appreciably affect the file size, and the "binary"
file still seem to have ASCII code in them. Is there a way to force a pure
binary write? */

extern HWND masterhwnd;	// The master window
extern CAlist *calife_list;

extern BOOL not_seeded_yet_flag;
extern BOOL load_save_cells_flag;
/*This determines whether to save and load the individual cell values.
When you are doing a save, this flag will be set from capow.cpp by
which File Save option you select.  When you are doing a load, this
flag will be set from either the file version number (FALSE for
versions < 4, TRUE for version 4), or from a flag in the file
for versions > 4)*/
/* These are the contents, with comments:
ifstream& operator>>(ifstream& ifs, CA* target)
	//Reads version, does switch to call a Version_?_Load_Individual.
	// 4,5 distinction is used to save load_save_cells_flag.
BOOL Version_7_Load_Individual(ifstream& ifs, CA* target)
ofstream& operator<<(ofstream& ofs, CA* target)
	//writes the current version 7 format.
void CAlist::Saveindividual(char *filename, CA *target)
	//uses filename to find ofs, then calls ofs << target.
void CAlist::Saveall(char *filename)
	//uses filename to find ofs, calls ofs << list[i].
void CAlist::Load_Individual(char* filename, CA *target)
	//uses filename to find ifs, then calls ifs >> target.
void CAlist::Loadall(char* filename)
	//uses filename to find ifs, then calls ifs >> list[i]. Then
	//SHOULD use version number to call Version_???_Loadall to load the
	//CAList parameters, but presently just does the current Load all
	//for the current version of the CAlist parameters.
void Version_123_Load_Individual(ifstream& ifs, CA* target, int version)
	//Reads the old versions.
void Version_45_Load_Individual(ifstream& ifs, CA* target)
	//These reads version4 and version5
	// 4,5 distinction is used to save load_save_cells_flag.
void Version_6_Load_Individual(ifstream& ifs, CA* target)
*/
//==========CA Load and Save ===================================


//--------------------------------------------------------------------------
ifstream& oldGetFrom(ifstream& ifs, CA* target)
{
	char dummybuf[80];
	int version;

	//      scan "\n1D CA file format %d." or  "\nCAPOW program file format %d."
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> version;

	switch(version)
	 {
		case 1:
		case 2:
		case 3:
			Version_123_Load_Individual(ifs, target, version);
			break;
		case 4:
			load_save_cells_flag = TRUE;
			Version_45_Load_Individual(ifs, target);
			break;
		case 5:
			load_save_cells_flag = FALSE;
			Version_45_Load_Individual(ifs, target);
			break;
		case 6:
			load_save_cells_flag = FALSE;
			Version_6_Load_Individual(ifs, target);
			break;
		case 7:
			load_save_cells_flag = FALSE;
			Version_7_Load_Individual(ifs, target);
			break;
	}
	return ifs;
}

//--------------------------------------------------------------------

BOOL Version_7_Load_Individual(ifstream& ifs, CA* target)
{
	char dummybuf[80];
	char ch;
	int		i, j;
	unsigned int uiread; //,red,green,blue;
  	double tempReal;
	int	   istates, iradius, itype, iwidth, readwidth;
	int	   userCAStyle, userNabeSize;
	
	// Match the real number format used by the corresponding ofs.
	// See the start of  ofstream& operator<<
#ifdef LIMIT_PRECISION
	ifs.setf(ios::fixed |ios::showpoint);
	ifs.precision(LOADSAVE_PRECISION);
#endif //LIMIT_PRECISION

	ifs.get();    // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> dummybuf >>
		load_save_cells_flag >> ch;
	ifs.get();    // read off eoln
	ifs >> dummybuf >> dummybuf >> itype;
	ifs.get();    // read off eoln
	ifs >> dummybuf >> iradius >> dummybuf >> istates >> ch;
	ifs.get();      // read off eoln
	ifs.getline(dummybuf, sizeof(dummybuf));

	// just to eat it.  This should have been unsigned int,
	// because 32K shows up as negative.
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed before reading lookup table!",
		(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
	}
	target->Settype(itype); //Second arg means don't 
	target->Changeradiusandstates(iradius, istates);
	//This sets nabeoptions.
	if (!(target->type_ca >= CA_WAVE))
	{
		j = 0;
		for (i = 0; i < target->nabeoptions; i++)
		{       //        scan "%u,"
			ifs >> uiread >> ch;
			target->lookup[i] = (unsigned char)uiread;
			if (++j>26)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
	}
	//      scanf "\nShow mode is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->showmode;
	//      scanf "\nGenerator flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->generatorflag = (unsigned char) uiread;
	//      scanf "\nWrap flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->wrapflag = (unsigned char) uiread;
	//      scanf "\nLambda value of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf;
   ifs >> dummybuf;
   ifs >> dummybuf;
   uiread = ifs.precision();
   ifs >> tempReal;
   /*Crashes right here in 32 bit, the first time we try and read a
   real number with >> .  I'm finding that tempReal
   does not get a correct value, it seems to always get 0, even
   if in the *.CA file it says "Lambda value of 0.1" */
   CLAMP(tempReal, MIN_LAMBDA, MAX_LAMBDA);
   target->lambda = tempReal;
	//      scan "\nEntropy level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->target_entropy;
	//      scan "\nEntropy score of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropy_bonus;
	//      scan "\nFailstripe level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->fail_stripe;
	// NEW DDC 3-18-96
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	/*We used to read this into target->horz_count;  Maybe later again,
	we will, but if we so do, then we will need to resize the active window
	to just the right size so that a call to Locate will make the target
	CA have its Locate-generated horz_count match the size of this read-in
	horz_count.*/
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->blankedgeflag;
	//ugly fix needed because blankedgeflag got accidentally saved
	//as FALSE for a bunch of Standard CAs, May 13, 1996.
	if (target->type_ca == CA_STANDARD)
		target->blankedgeflag = TRUE;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropyflag;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->viewmode;
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at viewmode!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	ifs.get();  // read off eoln
	// NEW 2-27-96 DDC - loading the anchor colors
	j = 0;
	unsigned long val;
	ifs >> dummybuf >> dummybuf >> dummybuf; // read off string
	ifs.get(); // read off eoln
	for (i = 0; i < MAX_BAND_COUNT; i++)
	{
		ifs >> val >> ch;
		target->_anchor_color[i] = (COLORREF)val;
		if (++j>10)
		{
			ifs.get();      // read off eoln
			j = 0;
		}
	}
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at anchor colors!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	//       scan "\nTime Step is %u.%u"
	if (target->type_ca >= CA_WAVE)
	{ //Read the analog info
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(TIME_STEP_TYPE, tempReal);
		/* DEBUGGING - DDC 3/25/96 */
		if (ifs.fail())
		{
			MessageBox( target->hwnd,
			(LPSTR)"ifstream failed in reading at Time step ( _dt )!",
			(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
		}
		//       scan "\nSpace step is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPACE_STEP_TYPE, tempReal);
		//       scan "\nBand count is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->band_count;
		//       scan "\nHeat increment is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(HEAT_INC_TYPE, tempReal);
		//       scan "\nMax intensity is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_INTENSITY_TYPE, tempReal);
		//       scan "\nVelocity range is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_VELOCITY_TYPE, tempReal);
		//       scan "\nFriction multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(FRICTION_TYPE, tempReal);
		//       scan "\nSpring multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPRING_TYPE,tempReal);
		//       scan "\nDriver multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_AMP_TYPE, tempReal);
		//       scan "\nFrequency multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_FREQ_TYPE, tempReal);
		//       scan "\nMass is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MASS_TYPE, tempReal);
		// NEW DDC 3-18-96
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->nabesize;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(CHUNK_TYPE,tempReal);
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->_dx_lock;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->showvelocity;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->time;
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(NONLINEARITY1_TYPE, tempReal);
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(NONLINEARITY2_TYPE, tempReal);
		ifs.get();  // read off eoln

	}
	if (load_save_cells_flag)
	{
	/* Set this flag so that the call to SetWrapflag inside of CA::Locate
		will not reset smoothsteps and change the new cell info . */
	target->_justloadedcells = TRUE;
	/* reset all buffers so the loaded information goes into rowbuffer[0] */
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
	switch (target->type_ca)
     {
		case CA_STANDARD:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%u,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_REVERSIBLE:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//       scan "%d,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			// scan "\nThe past_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%d,"
				ifs >> uiread >> ch;
				target->past_row[i] = (unsigned char) uiread;
				if (++j>26 )
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_WAVE:
		case CA_USER:
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
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
			// scan "\nThe wave_source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> tempReal >> ch;
				target->wave_source_row[i].intensity = tempReal;
/* The read real is failing and we're getting garbage values in here
of 0 all the time */
				//      scan "%d.%d,"
				ifs >> target->wave_source_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at source row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			// NEW 3-19-96: DDC - restoring past row
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_past_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_past_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at past row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			break;
	}//End of switch on type
	}//End of if (load_save_cells_flag)
	target->Adjust_acceleration_multiplier();
	target->Computeactual_lambda();
	target->entropy = 0.0;
	target->score = 0;
	if (!load_save_cells_flag)
		target->Seed();
	target->Setcolortable(); //install the anchor colors.
	return TRUE;
}

//----------------------------------------------------------------------

void CAlist::Load_Individual7(char* filename, CA *target)
{
	ifstream ifs;

	if (!compress_flag)
		ifs.open(filename, ios::in);
	else
		ifs.open(filename, ios::in|ios::binary);
	if (ifs.fail())
	{
		MessageBox( hwnd,
			(LPSTR)filename,//"File Problems!",
			(LPSTR)"Can't open file!",
			MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return;
	}
	oldGetFrom(ifs, target);
	ifs.close();
	/* If we load a new file, we may want to start breed cycle all over to
	give the new ca a fighting chance */
	if (breedflag)
 /*             if (MessageBox( hwnd,
			(LPSTR)"Reset Breedcycle counter and all scores?",
			(LPSTR)"",
			MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)   */
		{
			Resetscores();
			breedcycle_count = 0;
		}
}

BOOL CAlist::Loadall7(char* filename, BOOL startup)
{
	char dummybuf[80];
	unsigned int uiread;
	int filecount = 0;
	ifstream ifs;

	if (!compress_flag)
		ifs.open(filename, ios::in);
	else //compress_flag
		ifs.open(filename, ios::in|ios::binary);
	if (ifs.fail())
	{
		if (!strcmp(filename, "ACTIVE.CAS"))
			MessageBox( hwnd,
				(LPSTR)"No ACTIVE.CAS File, Using Default Parameters.",
				(LPSTR)"No Problem!",//"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
		else
			MessageBox( hwnd,
				(LPSTR)filename,//"File Problems!",
				(LPSTR)"Can't open file!",
				MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	// "\n Number of CAs is %d"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> filecount;
	if (filecount == 0)
	{  /*You will hit this if you try and load like an empty  *.CAS file */
		 MessageBox( hwnd,
				(LPSTR)filename,//"File Problems!",
				(LPSTR)"Empty experiment file with 0 CAs in it!",
				MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	else if (filecount > MAX_CAS)
	{  /*You will hit this if you try and load like a 9 CA *.CAS file
	  with a low memory CAPOW version like CAPOWLOW */
		 MessageBox( hwnd,
				(LPSTR)filename,//"File Problems!",
				(LPSTR)"Too many CAs for this CAPOW version!",
				MB_OK | MB_ICONEXCLAMATION );
		ifs.close();
		return FALSE;
	}
	else
		count = filecount; //In case this was changed downward for this ver.
	/*Right here I need somehow to peek ahead and see the version number
	so I can switch the loadall after I >> the individuals.  One
	solution would be to get rid of >> and put the switch inside
	int CAlist::Load_Individual and return the version.*/

	for (int i = 0; i < count; i++)
	{
		oldGetFrom(ifs, list[i]); //load fp_error.cas, and trace the 9th time F7
	}

	// "\n Zoom flag is %"
	//      ifs.get();  // read off eoln
	//      ifs >> dummybuf >> dummybuf >> dummybuf >>  uiread;
	 //     zoomflag = (BOOL) uiread;
	// "\n Mutation strength is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> mutation_strength;
	// "\n CA count per edge is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> dummybuf
		 >> CA_count_per_edge;
	// "\n Mutate flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	mutateflag = (BOOL) uiread;
	// "\n Breed flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	breedflag = (BOOL) uiread;
	// "\n Breed cycle is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> breedcycle;
	// "\n Bread cycle count is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf
		 >> breedcycle_count;
	// "\n Evolve flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	evolveflag = (int) uiread;
	// "\n View mode is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread; //viewmode;
	// "\n Show velocity is %"
	ifs.get();  // read off eoln
	//a workaround here becuase showveloity was moved from CAlist to CA
//	int dummyshowvelocity;
 //	ifs >> dummybuf >> dummybuf >> dummybuf >> dummyshowvelocity;
//	Setshowvelocity(dummyshowvelocity, 0); //0 means set as if with
	//no focus flag, means set all.  This still part of the workaround.
	//showveloicty should really be handled in CA load and save.
	// "\n Graph flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
 //	graphflag = (BOOL) uiread;
	// "\n Wire flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
//	wireflag = (BOOL) uiread;
	// "\n Scroll flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
//	scrollflag = (BOOL) uiread;
	// "\n Stripekill flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	stripekillflag = (BOOL) uiread;
	// "\n Stripe seed flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> uiread;
	stripeseedflag = (BOOL) uiread;

	//******** Read window size at save time ****** DDC - 4/16/96
	RECT scr;

	// "\n Window top is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> scr.top;
	// "\n Window bottom is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> scr.bottom;
	// "\n Window right is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> scr.right;
	// "\n Window left is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> scr.left;
	/* The SetWindowPos triggers a WM_RESIZE and if I'm in startup, I don't want to
	seed the thing yet as I'm going to get another one after I fiinsh 
	WM_CREATE */
	BOOL old_not_seeded_yet_flag = not_seeded_yet_flag;
	not_seeded_yet_flag = FALSE;
	if(!(SetWindowPos(hwnd,  HWND_NOTOPMOST, scr.left, scr.top,
	      (scr.right - scr.left),(scr.bottom - scr.top), SWP_NOMOVE)))
	{
		MessageBox( hwnd,
			(LPSTR)"problems resizing window!",
			(LPSTR)"Umm, this should work....",
			MB_OK | MB_ICONEXCLAMATION );
	}
	not_seeded_yet_flag = old_not_seeded_yet_flag;
	ifs.close();

	/* If we load a new file, we may want to start breed cycle all over to
		give the new ca a fighting chance */
	if (breedflag)
	/*              if (MessageBox( hwnd,
			(LPSTR)"Reset Breedcycle counter and all scores?",
			(LPSTR)"",
			MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)  */
		{
			Resetscores();
			breedcycle_count = 0;
		}
   if (load_save_cells_flag)
	{
/* Before you resize, set the _justloadedcells flags to TRUE again
for the second call to Locate (a first one took place in ift >> list[i]) */
		for (int i = 0; i < count; i++)
			list[i]->Set_justloadedcells(TRUE); //These are the CA mutators.
		Set_justloadedcells(TRUE); //This is the CAlist mutator.
	}
	/* trigger a window resizing event to
	clear and redraw the display*/
//I don't think I need this next thing, because SetWindowPos above generates a
//WM_SIZE
//	if (!startup)
//	   SendMessage(hwnd, WM_SIZE, NULL, 0L);
	return TRUE;
}

//=======================Old Versions=========================

void Version_123_Load_Individual(ifstream& ifs, CA* target, int version)
{
	char dummybuf[80];
	char ch;
	int i, j;
	unsigned int uiread,red,green,blue;
	Real tempReal;
	int istates, iradius, itype, iwidth, readwidth;

	if (version > 2)
	{  //    scan "\nCA type %d"
		ifs.get();    // read off eoln
		ifs >> dummybuf >> dummybuf
			 >> itype;
	}
	else
		itype = CA_STANDARD; // FILEVERSION 1 & 2 only have standard

	if (version < 3)
	{  //    scan "\n%d states and radius %d."
		ifs.get();    // read off eoln
		ifs >> istates >> dummybuf >> dummybuf >> dummybuf >> iradius;
	}

	if (version == 3)       // Al's getting crazy, radius must come first for some reason
	{       //       scan "\nradius %d  states %d."
		ifs.get();    // read off eoln
		ifs >> dummybuf >> iradius >> dummybuf >> istates >> ch;
	}

	/* We don't write the lookup table to the file if we are dealing
		with a Wave-based CA */
	if (version == 3 && itype >= CA_WAVE)
	{       //       scan "\nLookup table is not used.\n"
		ifs.get();      // read off eoln
		ifs.getline(dummybuf, sizeof(dummybuf));
	}
	else
	{       //       scan "\nThe lookup table with %d entries follows:\n"
		ifs.get();      // read off eoln
		ifs.getline(dummybuf, sizeof(dummybuf));
	}
		// just to eat it.  This should have been unsigned int,
		// because 32K shows up as negative.

	target->Settype(itype);
	target->Changeradiusandstates(iradius, istates); //This sets nabeoptions.
	/* We don't write the lookup table to the file if we are dealing
		with a Wave-based CA */

	if (!(version == 3 && target->type_ca >= CA_WAVE))
	{
		j = 0;
		for (i = 0; i < target->nabeoptions; i++)
		{       //        scan "%u,"
			ifs >> uiread >> ch;
			target->lookup[i] = (unsigned char)uiread;
			if (++j>26)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
	}

	if (version == 1)
		target->Setoldcolor();          // FILEVERSTION 1 doesn't save the color table

	if (version == 2)
	{       //       scan "\nThe color table with Red,Green,Blue values follows:"
		ifs.get();  // read off eoln
		ifs.getline(dummybuf, sizeof(dummybuf));
		j = 0;
		for (i = 0; i < target->states; i++)
		{       //       scan "%u, %u, %u,"
			ifs >> red >> ch >> green >> ch >> blue >> ch;
			target->colortable[i] = RGB(red,green,blue);
			if (++j>6)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
	}
	if (version == 3)
	{       //       scan "\nThe color table with %d Red,Green,Blue values follows:\n"
		ifs.get();      // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> uiread
			 >> dummybuf >> dummybuf >> dummybuf;
		ifs.get();      // read off eoln

		if (uiread >= MAX_COLOR )
		{
			MessageBox( target->hwnd, //calist_ptr->hwnd,
				(LPSTR)"Wrong # of colors saved!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			ifs.close();
			return;
		}

		j = 0;
		for (i = 0; i < target->states; i++)
		{       //       scan "%u, %u, %u,"
			ifs >> red >> ch >> green >> ch >> blue >> ch;
			target->colortable[i] = RGB(red,green,blue);
			if (++j>6)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
		//      scanf "\nShow mode is %"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->showmode;
		//      scanf "\nGenerator flag is %"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
		target->generatorflag = (unsigned char) uiread;
		//      scanf "\nWrap flag is %"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
		target->wrapflag = (unsigned char) uiread;

		// require Reals to have decimal point
		ifs >> setiosflags(ios::showpoint);

		//      scanf "\nLambda value of %u.%u"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->lambda;
		//      scan "\nEntropy level of %u.%u"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->target_entropy;
		//      scan "\nEntropy score of %u.%u"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropy_bonus;
		//      scan "\nFailstripe level of %u.%u"
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->fail_stripe;

		/* reset all buffers so the loaded information goes into rowbuffer[0] */
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

		if (load_save_cells_flag)
			switch (target->type_ca)
		{
			case CA_STANDARD:
				// scan "\nThe source_row with %d values follows:\n"
				ifs.get();
				ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
				ifs.getline(dummybuf, sizeof(dummybuf));
				j = 0;
				readwidth =  MIN(iwidth,target->_max_horz_count);
				for (i = 0; i < readwidth; i++)
				{
					//      scan "%u,"
					ifs >> uiread >> ch;
					target->source_row[i] = (unsigned char) uiread;
					if (++j>26)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				for (i=readwidth; i<iwidth; i++)
				{
					ifs >> uiread >> ch;
					if (++j>26)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				break;

			case CA_REVERSIBLE:
				// scan "\nThe source_row with %d values follows:\n"
				ifs.get();
				ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
				ifs.getline(dummybuf, sizeof(dummybuf));
				j = 0;
				readwidth =  MIN(iwidth,target->_max_horz_count);
				for (i = 0; i < readwidth; i++)
				{
					//       scan "%d,"
					ifs >> uiread >> ch;
					target->source_row[i] = (unsigned char) uiread;
					if (++j>26)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				for (i=readwidth; i<iwidth; i++)
				{
					ifs >> uiread >> ch;
					if (++j>26)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}

				// scan "\nThe past_row with %d values follows:\n"
				ifs.get();
				ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
				ifs.getline(dummybuf, sizeof(dummybuf));
				j = 0;
				for (i = 0; i < readwidth; i++)
				{
					//      scan "%d,"
					ifs >> uiread >> ch;
					target->past_row[i] = (unsigned char) uiread;
					if (++j>26 )
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				for (i=readwidth; i<iwidth; i++)
				{
					ifs >> uiread >> ch;
					if (++j>26)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				break;

			case CA_WAVE:
			case CA_HEATWAVE:
			case CA_HEATWAVE2:
			case CA_WAVE2:
			case CA_OSCILLATOR:
			case CA_USER:
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
				// scan "\nThe wave_source_row with %d values follows:\n"
				ifs.get();
				ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
				ifs.getline(dummybuf, sizeof(dummybuf));
				j = 0;
				readwidth =  MIN(iwidth,target->_max_horz_count);
				for (i = 0; i < readwidth; i++)
				{
					/* velocity and intensity could be negative */
					//       scan "%d.%d,"
					ifs >> target->wave_source_row[i].intensity >> ch;
					//      scan "%d.%d,"
					ifs >> target->wave_source_row[i].velocity >> ch;
					if (++j>8)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}
				for (i=readwidth; i<iwidth; i++)
				{
					ifs >> tempReal >> ch >> tempReal >> ch;
					if (++j>8)
					{       ifs.get();   // read off eoln
						j = 0;
					}
				}

				break;
		} //end if(load_save_cells_flag)
		if (target->type_ca >= CA_WAVE)
		{ //analog params
			//       scan "\nTime Step is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(TIME_STEP_TYPE, tempReal);
			//       scan "\nSpace step is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(SPACE_STEP_TYPE, tempReal);
			//       scan "\nBand count is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> target->band_count;
			//       scan "\nHeat increment is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(HEAT_INC_TYPE, tempReal);
			//       scan "\nMax intensity is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
			target->SetTweakParam(MAX_INTENSITY_TYPE, tempReal);
			//       scan "\nVelocity range is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(MAX_VELOCITY_TYPE, tempReal);
			//       scan "\nFriction multiplier is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(FRICTION_TYPE, tempReal);
			//       scan "\nSpring multiplier is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(SPRING_TYPE, tempReal);
			//       scan "\nDriver multiplier is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
			target->SetTweakParam(DRIVER_AMP_TYPE, tempReal);
			//       scan "\nFrequency multiplier is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
			target->SetTweakParam(DRIVER_FREQ_TYPE, tempReal);
			//       scan "\nMass is %u.%u"
			ifs.get();   // read off eoln
			ifs >> dummybuf >> dummybuf >> tempReal;
			target->SetTweakParam(MASS_TYPE, tempReal);
			target->Adjust_acceleration_multiplier();
     	} //end analog params
	} //end Version 3
	target->Computeactual_lambda();
	target->entropy = 0.0;
	target->score = 0;
}

//-------------------------------

void Version_45_Load_Individual(ifstream& ifs, CA* target)
{
	char dummybuf[80];
	char ch;
	int i, j;
	unsigned int uiread,red,green,blue;
	Real tempReal;
	int istates, iradius, itype, iwidth, readwidth;

	ifs.get();    // read off eoln
	ifs >> dummybuf >> dummybuf >> itype;
	ifs.get();    // read off eoln
	ifs >> dummybuf >> iradius >> dummybuf >> istates >> ch;
	ifs.get();      // read off eoln
	ifs.getline(dummybuf, sizeof(dummybuf));
	// just to eat it.  This should have been unsigned int,
	// because 32K shows up as negative.
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed before reading lookup table!",
		(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
	}
	target->Settype(itype);
	target->Changeradiusandstates(iradius, istates);
	//This sets nabeoptions.
	if (!(target->type_ca >= CA_WAVE))
	{
		j = 0;
		for (i = 0; i < target->nabeoptions; i++)
		{       //        scan "%u,"
			ifs >> uiread >> ch;
			target->lookup[i] = (unsigned char)uiread;
			if (++j>26)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
	}
      //       scan "\nThe color table with %d Red,Green,Blue values follows:\n"
 	ifs.get();      // read off eoln
 	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> uiread
 		 >> dummybuf >> dummybuf >> dummybuf;
 	ifs.get();      // read off eoln
 	if (uiread >= MAX_COLOR )
 	{
 		MessageBox( target->hwnd, //calist_ptr->hwnd,
 			(LPSTR)"Wrong Number of colors saved!",
 			(LPSTR)"File Problems!",
 			MB_OK | MB_ICONEXCLAMATION );
 		ifs.close();
	}
     j = 0;
	for (i = 0; i < target->states; i++)
	{       //       scan "%u, %u, %u,"
		ifs >> red >> ch >> green >> ch >> blue >> ch;
		target->colortable[i] = RGB(red,green,blue);
		if (++j>6)
		{
			ifs.get();      // read off eoln
			j = 0;
		}
	}
	//      scanf "\nShow mode is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->showmode;
	//      scanf "\nGenerator flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->generatorflag = (unsigned char) uiread;
	//      scanf "\nWrap flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->wrapflag = (unsigned char) uiread;
	// require Reals to have decimal point
	ifs >> setiosflags(ios::showpoint);
	//      scanf "\nLambda value of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->lambda;
	//      scan "\nEntropy level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->target_entropy;
	//      scan "\nEntropy score of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropy_bonus;
	//      scan "\nFailstripe level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->fail_stripe;
	// NEW DDC 3-18-96
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	/*We used to read this into target->horz_count;  Maybe later again,
	we will, but if we so do, then we will need to resize the active window
	to just the right size so that a call to Locate will make the target
	CA have its Locate-generated horz_count match the size of this read-in
	horz_count.*/
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->blankedgeflag;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropyflag;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->viewmode;
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at viewmode!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	ifs.get();  // read off eoln
	// NEW 2-27-96 DDC - loading the anchor colors
	j = 0;
	unsigned long val;
	ifs >> dummybuf >> dummybuf >> dummybuf; // read off string
	ifs.get(); // read off eoln
	for (i = 0; i < MAX_BAND_COUNT; i++)
	{
		ifs >> val >> ch;
		target->_anchor_color[i] = (COLORREF)val;
		if (++j>10)
		{
			ifs.get();      // read off eoln
			j = 0;
		}
	}
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at anchor colors!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	/* reset all buffers so the loaded information goes into rowbuffer[0] */
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
	//We don't need target->SyncRows() here, do we?
	if (load_save_cells_flag)
	switch (target->type_ca)
     {
		case CA_STANDARD:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%u,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_REVERSIBLE:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//       scan "%d,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			// scan "\nThe past_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%d,"
				ifs >> uiread >> ch;
				target->past_row[i] = (unsigned char) uiread;
				if (++j>26 )
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_WAVE:
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
		case CA_WAVE2:
		case CA_OSCILLATOR:
		case CA_USER:
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
			// scan "\nThe wave_source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_source_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_source_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at source row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			// NEW 3-19-96: DDC - restoring past row
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_past_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_past_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at past row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			// NEW 3-19-96: DDC - restoring target row
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_target_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_target_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
					MessageBox( target->hwnd,
					(LPSTR)"ifstream failed reading at target row values!",
					(LPSTR)"File Problems!",
					MB_OK | MB_ICONEXCLAMATION );
				}
				break;
	}//End of switch on type
	//End of if (load_save_cells_flag)
	else //!load_save_cells_flag
		target->Seed();
	//       scan "\nTime Step is %u.%u"
	if ((unsigned int) target->type_ca >= CA_WAVE)
	{ //Read the analog info
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(TIME_STEP_TYPE, tempReal);
		/* DEBUGGING - DDC 3/25/96 */
		if (ifs.fail())
		{
			MessageBox( target->hwnd,
			(LPSTR)"ifstream failed in reading at Time step ( _dt )!",
			(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
		}
		//       scan "\nSpace step is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPACE_STEP_TYPE, tempReal);
		//       scan "\nBand count is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->band_count;
		//       scan "\nHeat increment is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(HEAT_INC_TYPE, tempReal);
		//       scan "\nMax intensity is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_INTENSITY_TYPE, tempReal);
		//       scan "\nVelocity range is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_VELOCITY_TYPE, tempReal);
		//       scan "\nFriction multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(FRICTION_TYPE, tempReal);
		//       scan "\nSpring multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPRING_TYPE, tempReal);
		//       scan "\nDriver multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_AMP_TYPE, tempReal);
		//       scan "\nFrequency multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_FREQ_TYPE, tempReal);
		//       scan "\nMass is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MASS_TYPE, tempReal);
		// NEW DDC 3-18-96
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->nabesize;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(CHUNK_TYPE,tempReal);
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->_dx_lock;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->showvelocity;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->time;
		ifs.get();  // read off eoln
	}
	target->Adjust_acceleration_multiplier();
	target->Computeactual_lambda();
	target->entropy = 0.0;
	target->score = 0;
}

void Version_6_Load_Individual(ifstream& ifs, CA* target)
{
	char dummybuf[80];
	char ch;
	int i, j;
	unsigned int uiread; //,red,green,blue;
  	Real tempReal;
	int istates, iradius, itype, iwidth, readwidth;

	// Match the real number format used by the corresponding ofs.
   // See the start of  ofstream& operator<<
	ifs.setf(ios::fixed |ios::showpoint);
   ifs.precision(2);  //We used precision of 2 in these files

	ifs.get();    // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> dummybuf >> dummybuf >>
		load_save_cells_flag >> ch;
	ifs.get();    // read off eoln
	ifs >> dummybuf >> dummybuf >> itype;
	ifs.get();    // read off eoln
	ifs >> dummybuf >> iradius >> dummybuf >> istates >> ch;
	ifs.get();      // read off eoln
	ifs.getline(dummybuf, sizeof(dummybuf));
	// just to eat it.  This should have been unsigned int,
	// because 32K shows up as negative.
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed before reading lookup table!",
		(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
	}
	target->Settype(itype);
	target->Changeradiusandstates(iradius, istates);
	//This sets nabeoptions.
	if (!(target->type_ca >= CA_WAVE))
	{
		j = 0;
		for (i = 0; i < target->nabeoptions; i++)
		{       //        scan "%u,"
			ifs >> uiread >> ch;
			target->lookup[i] = (unsigned char)uiread;
			if (++j>26)
			{       ifs.get();      // read off eoln
				j = 0;
			}
		}
	}
	//      scanf "\nShow mode is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->showmode;
	//      scanf "\nGenerator flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->generatorflag = (unsigned char) uiread;
	//      scanf "\nWrap flag is %"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	target->wrapflag = (unsigned char) uiread;
	//      scanf "\nLambda value of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf;
   ifs >> dummybuf;
   ifs >> dummybuf;
//   uiread = ifs.precision();
   ifs >> tempReal;
   /* Was Crashing right here in 32 bit, the first time we try and read a
   real number with >> .  I'm finding that tempReal
   does not get a correct value, it seems to always get 0, even
   if in the *.CA file it says "Lambda value of 0.1" */
   CLAMP(tempReal, MIN_LAMBDA, MAX_LAMBDA);
   target->lambda = tempReal;
	//      scan "\nEntropy level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->target_entropy;
	//      scan "\nEntropy score of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropy_bonus;
	//      scan "\nFailstripe level of %u.%u"
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->fail_stripe;
	// NEW DDC 3-18-96
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> uiread;
	/*We used to read this into target->horz_count;  Maybe later again,
	we will, but if we so do, then we will need to resize the active window
	to just the right size so that a call to Locate will make the target
	CA have its Locate-generated horz_count match the size of this read-in
	horz_count.*/
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->blankedgeflag;
	//ugly fix needed because blankedgeflag got accidentally saved
	//as FALSE for a bunch of Standard CAs, May 13, 1996.
	if (target->type_ca == CA_STANDARD)
		target->blankedgeflag = TRUE;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->entropyflag;
	ifs.get();  // read off eoln
	ifs >> dummybuf >> dummybuf >> dummybuf >> target->viewmode;
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at viewmode!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	ifs.get();  // read off eoln
	// NEW 2-27-96 DDC - loading the anchor colors
	j = 0;
	unsigned long val;
	ifs >> dummybuf >> dummybuf >> dummybuf; // read off string
	ifs.get(); // read off eoln
	for (i = 0; i < MAX_BAND_COUNT; i++)
	{
		ifs >> val >> ch;
		target->_anchor_color[i] = (COLORREF)val;
		if (++j>10)
		{
			ifs.get();      // read off eoln
			j = 0;
		}
	}
	/* DEBUGGING - DDC 3/25/96 */
	if (ifs.fail())
	{
		MessageBox( target->hwnd,
		(LPSTR)"ifstream failed in reading at anchor colors!",
		(LPSTR)"File Problems!",
		MB_OK | MB_ICONEXCLAMATION );
	}
	/* reset all buffers so the loaded information goes into rowbuffer[0] */
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
	if (load_save_cells_flag)
	switch (target->type_ca)
     {
		case CA_STANDARD:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%u,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_REVERSIBLE:
			// scan "\nThe source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				//       scan "%d,"
				ifs >> uiread >> ch;
				target->source_row[i] = (unsigned char) uiread;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			// scan "\nThe past_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			for (i = 0; i < readwidth; i++)
			{
				//      scan "%d,"
				ifs >> uiread >> ch;
				target->past_row[i] = (unsigned char) uiread;
				if (++j>26 )
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> uiread >> ch;
				if (++j>26)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			break;
		case CA_WAVE:
		case CA_USER:
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
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
			// scan "\nThe wave_source_row with %d values follows:\n"
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_source_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_source_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at source row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			// NEW 3-19-96: DDC - restoring past row
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_past_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_past_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
				MessageBox( target->hwnd,
				(LPSTR)"ifstream failed reading at past row values!",
				(LPSTR)"File Problems!",
				MB_OK | MB_ICONEXCLAMATION );
			}
			// NEW 3-19-96: DDC - restoring target row
			ifs.get();
			ifs >> dummybuf >> dummybuf >> dummybuf >> iwidth;
			ifs.getline(dummybuf, sizeof(dummybuf));
			j = 0;
			readwidth =  MIN(iwidth,target->_max_horz_count);
			for (i = 0; i < readwidth; i++)
			{
				/* velocity and intensity could be negative */
				//       scan "%d.%d,"
				ifs >> target->wave_target_row[i].intensity >> ch;
				//      scan "%d.%d,"
				ifs >> target->wave_target_row[i].velocity >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			for (i=readwidth; i<iwidth; i++)
			{
				ifs >> tempReal >> ch >> tempReal >> ch;
				if (++j>8)
				{       ifs.get();   // read off eoln
					j = 0;
				}
			}
			/* DEBUGGING - DDC 3/25/96 */
			if (ifs.fail())
			{
					MessageBox( target->hwnd,
					(LPSTR)"ifstream failed reading at target row values!",
					(LPSTR)"File Problems!",
					MB_OK | MB_ICONEXCLAMATION );
				}
				break;
	}//End of switch on type
	//End of if (load_save_cells_flag)
	//       scan "\nTime Step is %u.%u"
	if ((unsigned int) target->type_ca >= CA_WAVE)
	{ //Read the analog info
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(TIME_STEP_TYPE, tempReal);
		/* DEBUGGING - DDC 3/25/96 */
		if (ifs.fail())
		{
			MessageBox( target->hwnd,
			(LPSTR)"ifstream failed in reading at Time step ( _dt )!",
			(LPSTR)"File Problems!",
			MB_OK | MB_ICONEXCLAMATION );
		}
		//       scan "\nSpace step is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPACE_STEP_TYPE, tempReal);
		//       scan "\nBand count is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->band_count;
		//       scan "\nHeat increment is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(HEAT_INC_TYPE, tempReal);
		//       scan "\nMax intensity is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_INTENSITY_TYPE, tempReal);
		//       scan "\nVelocity range is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MAX_VELOCITY_TYPE, tempReal);
		//       scan "\nFriction multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(FRICTION_TYPE, tempReal);
		//       scan "\nSpring multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(SPRING_TYPE,tempReal);
		//       scan "\nDriver multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_AMP_TYPE, tempReal);
		//       scan "\nFrequency multiplier is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf>> dummybuf >> tempReal;
		target->SetTweakParam(DRIVER_FREQ_TYPE, tempReal);
		//       scan "\nMass is %u.%u"
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(MASS_TYPE, tempReal);
		// NEW DDC 3-18-96
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->nabesize;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(CHUNK_TYPE,tempReal);
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> target->_dx_lock;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->showvelocity;
		ifs.get();   // read off eoln
		ifs >> dummybuf >> dummybuf >> target->time;
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(NONLINEARITY1_TYPE, tempReal);
		ifs.get();  // read off eoln
		ifs >> dummybuf >> dummybuf >> dummybuf >> tempReal;
		target->SetTweakParam(NONLINEARITY2_TYPE, tempReal);
		ifs.get();  // read off eoln

	}
	target->Adjust_acceleration_multiplier();
	target->Computeactual_lambda();
	target->entropy = 0.0;
	target->score = 0;
	if (!load_save_cells_flag)
		target->Seed();
	target->Setcolortable(); //install the anchor colors.
}


