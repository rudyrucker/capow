#include "ca.hpp"
//=============================ONE DIMENSIONAL==========================

//#define SMOOTHGROW 1.1 //Use this to keep SMOOTH from flatting things
	//out too much.
void CA::Smooth(int l, int c, int r)
{//Use this just to smooth the curve out when _smoothsteps > 0
//Need this after a Seed, OneSeed or Locate for awhile, it's
//called in WaveUpdate.
	wave_target_row[c].intensity = //SMOOTHGROW *
		(wave_source_row[l].intensity +
		wave_source_row[c].intensity +
		wave_source_row[r].intensity) / 3.0;
	wave_target_row[c].velocity = (wave_target_row[c].intensity -
		wave_source_row[c].intensity)/_dt.Val(); //Calcualte just for graphing.
}

void CA::SmoothAverageStretch1D()
{
   int i;
 //Enter this with _smoothsteps > 0
 //Do some heat rule to smooth things.  Need this when you use
	// One of the Alt???? rules which expects the CAs states to be
	//continuosly arranged along the space axis.
	//I put in an adhoc method to keep things like a wave from
	//getting crushed flat by the smoothing.

	_smoothsteps--;
	if (_smoothsteps == _startsmoothsteps -1) //Just starting
	{ //Initialize the values
		_stretch_lasttime = FALSE;
		if (horz_count == 0)
			_stretch_cell_weight = 1.0;
		else
			_stretch_cell_weight = 1.0/(double)(horz_count);
/*
//#define NEWSMOOTH //Seems more logical, but crushes things flat in 2 calls
#ifdef NEWSMOOTH
		//These will be calculated on the first smooth step.
		_stretch_oldmaxintensity = -max_intensity;
		_stretch_oldminintensity = max_intensity;
		_stretch_oldavgintensity = 0.0;
#else //not NEWSMOOTH
*/
		//Just pick a nice spread and force it.
		_stretch_oldmaxintensity =  0.85 * _max_intensity.Val();
		_stretch_oldminintensity = -0.85 * _max_intensity.Val();
		_stretch_oldavgintensity = 0.0;
//#endif //NEWSMOOTH
		//These figures will be calculated on the last smooth step.
		_stretch_newmaxintensity = -_max_intensity.Val();
		_stretch_newminintensity = _max_intensity.Val();
		_stretch_newavgintensity = 0.0;
	}
	if (!_smoothsteps)
		_stretch_lasttime = TRUE;
	for (i = 1; i < horz_count-1; i++)
		Smooth(i-1, i, i+1);
// Do the same thing at the ends if wrapflag
	if(wrapflag)
	{
	// left end update
		Smooth(horz_count-1, 0, 1);
	// right end update
		Smooth(horz_count-2, horz_count-1, 0);
	}
#ifdef NEWSMOOTH
	//Now set the _stretch_old??? values if just starting
	if (_smoothsteps == _startsmoothsteps -1) //Just starting
		for (i = 0; i < horz_count; i++)
		{
			if (wave_source_row[i].intensity >
				_stretch_oldmaxintensity)
				_stretch_oldmaxintensity =
					wave_source_row[i].intensity;
			if (wave_source_row[i].intensity <
				_stretch_oldminintensity)
				_stretch_oldminintensity =
					wave_source_row[i].intensity;
			_stretch_oldavgintensity +=
				(double)(wave_source_row[i].intensity) *
				_stretch_cell_weight;
		}
#endif //NEWSMOOTH
 	//Get the _stretch_new??? values if the last time.
 	if (_stretch_lasttime)
 		for (i = 0; i < horz_count; i++)
 		{
 			if (wave_target_row[i].intensity >
 				_stretch_newmaxintensity)
 				_stretch_newmaxintensity =
 					wave_target_row[i].intensity;
 			if (wave_target_row[i].intensity <
 				_stretch_newminintensity)
 				_stretch_newminintensity =
					wave_target_row[i].intensity;
 			_stretch_newavgintensity +=
 				(double)(wave_target_row[i].intensity) *
 				_stretch_cell_weight;
 		}
  //Now if last time, use the _stretch info unless divide by 0.
 	if (_stretch_lasttime &&
 		((_stretch_newmaxintensity - _stretch_newavgintensity) >
 		0.000001) &&
 		((_stretch_newavgintensity - _stretch_newminintensity) >
 		0.000001))
	{
 		Real maxstretch =
 			(_stretch_oldmaxintensity - _stretch_oldavgintensity)/
 			(_stretch_newmaxintensity - _stretch_newavgintensity);
 		Real minstretch =
 			(_stretch_oldminintensity - _stretch_oldavgintensity)/
 			(_stretch_newminintensity - _stretch_newavgintensity);
		int low, high;

		if (wrapflag)
		{
			low = 0; high = horz_count;
		}
		else //not wrapflag
		{
          	low = 1; high = horz_count-1;
		}
 		for (i = low; i < high; i++)
 		{ //Fix source & target and source, which will be the new
              //past & source after WaveUpdateStep rolls the buffers
 			Real normalized_intensity =  //Do source
 				wave_source_row[i].intensity -
 				_stretch_newavgintensity;
 			if (normalized_intensity > 0.0)
 				wave_source_row[i].intensity =
 					_stretch_oldavgintensity +
 					maxstretch * normalized_intensity;
 			else
 				wave_source_row[i].intensity =
 					_stretch_oldavgintensity +
 					minstretch * normalized_intensity;
 			normalized_intensity =    //Do target
 				wave_target_row[i].intensity -
 				_stretch_newavgintensity;
 			if (normalized_intensity > 0.0)
 				wave_target_row[i].intensity =
 					_stretch_oldavgintensity +
 					maxstretch * normalized_intensity;
 			else
 				wave_target_row[i].intensity =
 					_stretch_oldavgintensity +
 					minstretch * normalized_intensity;
 		}
 	}
}

void CA::SmoothEdge1D()
{ /*Enter this with _startsmoothsteps == SMOOTHEDGE.  You
	smooth the source row intensity and velocity and copy the source
	to the target*/

	int leftindex, rightindex;
	Real leftval, rightval;
	Real slope;
   int i;

	_smoothsteps = 0; //You're done with smoothing after this one step.
	//First smooth the right end which is damaged for sure.
	leftindex = horz_count-1-SMOOTHEDGERADIUS;
	rightindex = horz_count-1;
	//First fix source row
	leftval = wave_source_row[leftindex].intensity;
	rightval = wave_source_row[rightindex].intensity;
	slope = (rightval - leftval) / SMOOTHEDGERADIUS;
	for (i=leftindex; i<rightindex; i++)
		wave_source_row[i].intensity = leftval + slope * (i-leftindex);

	leftval = wave_source_row[leftindex].velocity;
	rightval = wave_source_row[rightindex].velocity;
	slope = (rightval - leftval) / SMOOTHEDGERADIUS;
	for (i=leftindex; i<rightindex; i++)
		wave_source_row[i].velocity = leftval + slope * (i-leftindex);
	//Then fix past row
	leftval = wave_past_row[leftindex].intensity;
	rightval = wave_past_row[rightindex].intensity;
	slope = (rightval - leftval) / SMOOTHEDGERADIUS;
	for (i=leftindex; i<rightindex; i++)
		wave_past_row[i].intensity = leftval + slope * (i-leftindex);

	leftval = wave_past_row[leftindex].velocity;
	rightval = wave_past_row[rightindex].velocity;
	slope = (rightval - leftval) / SMOOTHEDGERADIUS;
	for (i=leftindex; i<rightindex; i++)
		wave_past_row[i].velocity = leftval + slope * (i-leftindex);


	if (wrapflag) //Fix the left end as well.
	{
		//first fix source row
		rightindex = SMOOTHEDGERADIUS-1;
		leftval = wave_source_row[horz_count-1].intensity;
		rightval = wave_source_row[rightindex].intensity;
		slope = (rightval - leftval) / SMOOTHEDGERADIUS;
		for (i=0; i<rightindex; i++)
			wave_source_row[i].intensity = leftval +
				slope * (i + 1); /*The effective leftindex 0 is
			step past leftval's evaluation location, i.e. horz-1*/
		leftval = wave_source_row[horz_count-1].velocity;
		rightval = wave_source_row[rightindex].velocity;
		slope = (rightval - leftval) / SMOOTHEDGERADIUS;
		for (i=0; i<rightindex; i++)
			wave_source_row[i].velocity = leftval +
				slope * (i + 1); /*The effective leftindex 0 is
			step past leftval's evaluation location, i.e. horz-1*/

		//then fix past row
		rightindex = SMOOTHEDGERADIUS-1;
		leftval = wave_past_row[horz_count-1].intensity;
		rightval = wave_past_row[rightindex].intensity;
		slope = (rightval - leftval) / SMOOTHEDGERADIUS;
		for (i=0; i<rightindex; i++)
			wave_past_row[i].intensity = leftval +
				slope * (i + 1); /*The effective leftindex 0 is
			step past leftval's evaluation location, i.e. horz-1*/
		leftval = wave_past_row[horz_count-1].velocity;
		rightval = wave_past_row[rightindex].velocity;
		slope = (rightval - leftval) / SMOOTHEDGERADIUS;
		for (i=0; i<rightindex; i++)
			wave_past_row[i].velocity = leftval +
				slope * (i + 1); /*The effective leftindex 0 is
			step past leftval's evaluation location, i.e. horz-1*/
	}
}

//==========================TWO DIMENSIONAL========================

void CA::Smooth2D(int c, int e, int n, int w, int s)
{//Use this just to smooth the curve out when _smoothsteps > 0
//Need this after a Seed, OneSeed or Locate for awhile, it's
//called in WaveUpdate2D.
	wave_target_plane[c].intensity = //SMOOTHGROW *
		(wave_source_plane[c].intensity +
		wave_source_plane[e].intensity +
		wave_source_plane[n].intensity +
		wave_source_plane[w].intensity +
		wave_source_plane[s].intensity) / 5.0;
}

void CA::SmoothAverageStretch2D()
{
	int y, x;
	int c,e,n,w,s;
  //Do some heat rule to smooth things.  Need this when you use
		// One of the Alt???? rules which expects the CAs states to be
		//continuosly arranged along the space axis.
		//I put in an adhoc method to keep things like a wave from
		//getting crushed flat by the smoothing.
		_smoothsteps--;
		if (_smoothsteps == _startsmoothsteps -1) //Just starting
		{ //Initialize the values
			_stretch_lasttime = FALSE;
			if (!horz_count_2D || !vert_count_2D)
				_stretch_cell_weight = 1.0;
			else
				_stretch_cell_weight =
				 1.0/(((double)horz_count_2D)*(vert_count_2D));
			//Put in some comfortable values.
			_stretch_oldmaxintensity =  0.85 * _max_intensity.Val();
			_stretch_oldminintensity = -0.85 * _max_intensity.Val();
			_stretch_oldavgintensity = 0.0;
			//These figures will be calculated on the last smooth step.
			_stretch_newmaxintensity = -_max_intensity.Val();
			_stretch_newminintensity = _max_intensity.Val();
			_stretch_newavgintensity = 0.0;
		}
		if (!_smoothsteps)
			_stretch_lasttime = TRUE;

		for (y = 1; y < vert_count_2D-1; y++)
		{  //Smooth the cells
			c = index(1,y); e = index(2,y);
			n = index(1,y-1); w = index(0,y); s=index(1,y+1);
			//And accumulate stretching info to use below to keep
			//there from being a flattening out effect.
			for (x = 1; x< horz_count_2D-1; x++)
			{
				Smooth2D(c, e, n, w, s);
				c++; e++; n++; w++; s++;
			}
		 }
		if(wrapflag)   //Smooth the edges
		{
		// upper left corner update
			Smooth2D(
				index(0, 0),
				index(1, 0),
				index(0, vert_count_2D - 1),
				index(horz_count_2D - 1, 0),
				index(0, 1));
		// upper right corner update
			Smooth2D(
				index(horz_count_2D - 1, 0),
				index(0, 0),
				index(horz_count_2D - 1, vert_count_2D - 1),
				index(horz_count_2D - 2, 0),
				index(horz_count_2D - 1, 1));
		// bottom left corner update
			Smooth2D(
				index(0, vert_count_2D - 1),   								//c
				index(1, vert_count_2D - 1),                          //e
				index(0, vert_count_2D - 2),                          //n
				index(horz_count_2D - 1, vert_count_2D - 1) ,         //w
				index(0, 0));                                         //s
		// bottom right corner update
			Smooth2D(
				index(horz_count_2D - 1, vert_count_2D - 1),
				index(0, vert_count_2D - 1),                           //e
				index(horz_count_2D - 1, vert_count_2D - 2),           //n
				index(horz_count_2D - 2, vert_count_2D - 1) ,          //w
				index(horz_count_2D - 1, 0));                          //s
		//top edge
			c = index(1,0);
			e = index(2,0);
			n = index(1, vert_count_2D - 1);
			w = index(0,0);
			s = index(1,1);
			for (x = 1; x< horz_count_2D-1; x++)
			{
				Smooth2D(c, e, n, w, s);
				c++; e++; n++; w++; s++;
			}
		//bottom edge
			c = index(1,vert_count_2D-1);
			e = index(2,vert_count_2D-1);
			n = index(1, vert_count_2D - 2);
			w = index(0,vert_count_2D-1);
			s = index(1,0);
			for (x = 1; x< horz_count_2D-1; x++)
			{
				Smooth2D(c, e, n, w, s);
				c++; e++; n++; w++; s++;
			}
		//left edge
			c = index(0, 1);
			e = index(1, 1);
			n = index(0, 0);
			w = index(horz_count_2D - 1, 1);
			s = index(0, 2);
			for (y = 1; y< vert_count_2D-1; y++)
			{
				Smooth2D(c, e, n, w, s);
				c += CX_2D;     //Step down one row in maximun square.
				e += CX_2D;     //Look in definition of index() in ca.hpp
				n += CX_2D;     //to see that incrementing y adds CX_2D.
				w += CX_2D;
				s += CX_2D;
			}

		//right edge
			c = index(horz_count_2D - 1, 1);
			e = index(0, 1);
			n = index(horz_count_2D - 1, 0);
			w = index(horz_count_2D - 2, 1);
			s = index(horz_count_2D - 1, 2);
			for (y = 1; y< vert_count_2D-1; y++)
			{
				Smooth2D(c, e, n, w, s);
				c += CX_2D;
				e += CX_2D;
				n += CX_2D;
				w += CX_2D;
				s += CX_2D;
			}
		}
/*		else   //non wrap flag case   //mike 9/17/97
		{
		// upper left corner update
			Smooth2D(
				index(0, 0),
				index(1, 0),
				index(0, 0),  //mike
				index(0, 0),  //mike
				index(0, 1));
		// upper right corner update
			Smooth2D(
				index(horz_count_2D - 1, 0),
				index(horz_count_2D-1,0), //mike
				index(horz_count_2D - 1, 0), //mike
				index(horz_count_2D - 2, 0),
				index(horz_count_2D - 1, 1));
		// bottom left corner update
			Smooth2D(
				index(0, vert_count_2D - 1),   								//c
				index(1, vert_count_2D - 1),                          //e
				index(0, vert_count_2D - 2),                          //n
				index(0, vert_count_2D - 1) ,         //w   mike
				index(0, vert_count_2D-1)); //mike                 //s
		// bottom right corner update
			Smooth2D(
				index(horz_count_2D - 1, vert_count_2D - 1),
				index(horz_count_2D - 1, vert_count_2D - 1),  //mike  //e
				index(horz_count_2D - 1, vert_count_2D - 2),           //n
				index(horz_count_2D - 2, vert_count_2D - 1) ,          //w
				index(horz_count_2D - 1, vert_count_2D -1)); //mike                         //s
		//top edge
			c = index(1,0);
			e = index(2,0);
			n = index(1, 0);  //mike
			w = index(0,0);
			s = index(1,1);
			for (x = 1; x< horz_count_2D-1; x++)
			{
				Smooth2D(c, e, n, w, s);
				c++; e++; n++; w++; s++;
			}
		//bottom edge
			c = index(1,vert_count_2D-1);
			e = index(2,vert_count_2D-1);
			n = index(1, vert_count_2D - 2);
			w = index(0,vert_count_2D-1);
			s = index(1,vert_count_2D-1);  //mike
			for (x = 1; x< horz_count_2D-1; x++)
			{
				Smooth2D(c, e, n, w, s);
				c++; e++; n++; w++; s++;
			}
		//left edge
			c = index(0, 1);
			e = index(1, 1);
			n = index(0, 0);
			w = index(0 , 1);  //mike
			s = index(0, 2);
			for (y = 1; y< vert_count_2D-1; y++)
			{
				Smooth2D(c, e, n, w, s);
				c += CX_2D;     //Step down one row in maximun square.
				e += CX_2D;     //Look in definition of index() in ca.hpp
				n += CX_2D;     //to see that incrementing y adds CX_2D.
				w += CX_2D;
				s += CX_2D;
			}

		//right edge
			c = index(horz_count_2D - 1, 1);
			e = index(horz_count_2D - 1, 1);  //mike
			n = index(horz_count_2D - 1, 0);
			w = index(horz_count_2D - 2, 1);
			s = index(horz_count_2D - 1, 2);
			for (y = 1; y< vert_count_2D-1; y++)
			{
				Smooth2D(c, e, n, w, s);
				c += CX_2D;
				e += CX_2D;
				n += CX_2D;
				w += CX_2D;
				s += CX_2D;
			}
		}
*/
	//If the last runthrough, set the _stretch_new info.
 		if (_stretch_lasttime)
			for (y = 0; y < vert_count_2D; y++)
			{
				c = index(0,y);
				for (x = 0; x< horz_count_2D; x++)
 				{
 					if (wave_target_plane[c].intensity >
 						_stretch_newmaxintensity)
 						_stretch_newmaxintensity =
 							 wave_target_plane[c].intensity;
	 				if (wave_target_plane[c].intensity <
 						_stretch_newminintensity)
 						_stretch_newminintensity =
 							wave_target_plane[c].intensity;
	 				_stretch_newavgintensity +=
 						(double)(wave_target_plane[c].intensity) *
 						_stretch_cell_weight;
					c++;
	 			}
			}
		 //Now stretch if this is the last runthrough and not divide by 0.
		if (_stretch_lasttime &&
			((_stretch_newmaxintensity - _stretch_newavgintensity) >
				0.000001) &&
			((_stretch_newavgintensity - _stretch_newminintensity) >
				0.000001))
		{
			Real maxstretch =
				(_stretch_oldmaxintensity - _stretch_oldavgintensity)/
				(_stretch_newmaxintensity - _stretch_newavgintensity);
			Real minstretch =
				(_stretch_oldminintensity - _stretch_oldavgintensity)/
				(_stretch_newminintensity - _stretch_newavgintensity);
			for (int y = 0; y < vert_count_2D; y++)
			{
				c = index(0,y);
				for (int x = 0; x< horz_count_2D; x++)
				{ //Fix target and source, which will be new source and
				  //past after WaveUpdateStep rolls the buffers
					if (wave_target_plane[c].intensity >
						_stretch_newavgintensity)
						wave_target_plane[c].intensity =
							_stretch_oldavgintensity + maxstretch *
							(wave_target_plane[c].intensity -
							_stretch_newavgintensity);
					else
						wave_target_plane[c].intensity =
							_stretch_oldavgintensity + minstretch *
							(wave_target_plane[c].intensity -
							_stretch_newavgintensity);
					if (wave_source_plane[c].intensity >
						_stretch_newavgintensity)
						wave_source_plane[c].intensity =
							_stretch_oldavgintensity + maxstretch *
							(wave_source_plane[c].intensity -
							_stretch_newavgintensity);
					else
						wave_source_plane[c].intensity =
							_stretch_oldavgintensity + minstretch *
							(wave_source_plane[c].intensity -
							_stretch_newavgintensity);
					c++;
				}
			}
		} //end of undo flattening block.
}

