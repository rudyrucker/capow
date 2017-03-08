#include "Tweak.hpp"
#include "ca.hpp"

/*----------- TweakRange constructors -------------*/

TweakRange::TweakRange()
{
	int i;
	for (i = 0; i < MAX_POINTS; i++)
	{
		_transition[i] = DEFAULT_RANGE;
		_increment[i] = 0;
	}
	_increment[MAX_RANGES-1] = 0;
}

TweakRange::TweakRange(const TweakRange& new_TweakRange)
{
	int i;
	for (i = 0; i < MAX_POINTS; i++)
	{
		_transition[i] = new_TweakRange.Transition(i);
		_increment[i] = new_TweakRange.Increment(i);
	}
	_increment[MAX_RANGES-1] = new_TweakRange.Increment(MAX_RANGES-1);
}

TweakRange::TweakRange(Real lo_value,  Real lo_to_med,
							  Real med_value, Real med_to_hi,
							  Real hi_value,  Real hi_to_super,
							  Real super_value)
{
	_transition[0] = lo_to_med;
	_transition[1] = med_to_hi;
	_transition[2] = hi_to_super;

	_increment[0] = lo_value;
	_increment[1] = med_value;
	_increment[2] = hi_value;
	_increment[3] = super_value;
}

/*---------- TweakRange accessors ------------------*/

Real TweakRange::Transition(const int i) const
{  // index for _transition[]: 0 to MAX_POINTS-1
	if (i >= MAX_POINTS)
		return _transition[MAX_POINTS-1];
	else if (i <= 0)
		return _transition[0];
	else
		return _transition[i];
}

Real TweakRange::Increment(const int i) const
{  // index for _increment[]: 0 to MAX_RANGES-1
	if (i >= MAX_RANGES)
		return _transition[MAX_RANGES-1];
	else if (i <= 0)
		return _increment[0];
	else
		return _increment[i];
}

Real TweakRange::RangeVal(const Real transitionPoint) const
{
	if (transitionPoint <= _transition[0])
		return _increment[0];
	if (transitionPoint <= _transition[1])
		return _increment[1];
	if (transitionPoint <= _transition[2])
		return _increment[2];
	else
		return _increment[3];
}

/*---------- TweakRange mutators ------------------*/

void TweakRange::InitTweakRange(const TweakRange& tr)
{
	int i;
	for (i = 0; i < MAX_POINTS; i++)
	{
		_transition[i] = tr.Transition(i);
		_increment[i] = tr.Increment(i);
	}
	_increment[MAX_RANGES-1] = tr.Increment(MAX_RANGES-1);
}

/*-----------TweakParam constructor ---------------*/
TweakParam::TweakParam()
: _val(0.0),
  _min(-DEFAULT_RANGE),
  _max(DEFAULT_RANGE),
		/* I'm going to put a kludge fix in here.  The problem is that the inWrite and outWrite
		in loadsave.cpp don't save and load the tweak param ranges.  So by default we'll
		give the tweak param a limited range. */
  _label(NULL),
  _adjustothers(NO_ADJUST)
{}

TweakParam::TweakParam(Real min, Real valMean, Real valVariance, Real max, LPSTR label,
							  int adjustothers)
: _val(valMean + Randomsignreal() * valVariance),
  _min(min),
  _max(max),
  _adjustothers(adjustothers)

{	//  FIXME later
	// _val = valVariance; 

	_label = new char[lstrlen(label)+1];
	lstrcpy (_label, label);
	Clamp();
}

/*------------- TweakParam destructor ---------------*/
TweakParam::~TweakParam()
{
	Free();
}

/*------------ TweakParam accessors ----------------*/

/*------------ TweakParam mutators -----------------*/

void TweakParam::SetLabel (LPSTR label)
{
	if (_label && label)
	{
		Free();
		_label = NULL;
	}

	_label = new char[lstrlen(label)+1];
	lstrcpy (_label, label);
}

void TweakParam::SetVal(Real newval, CA* thisCA)
{
	_val = newval;
	Clamp();
	switch(_adjustothers)
	{
		case NO_ADJUST:
			break;
		case ADJUST_ACCELERATION:
			if (thisCA)
				thisCA->Adjust_acceleration_multiplier();
			break;
		case ADJUST_VARIANCE:
			if (thisCA)
				thisCA->Smooth_variance();
			break;
	}
}

void TweakParam::InitTweakParam (Real min, Real valMean, Real valVariance, Real max,
								 LPSTR label, int adjustothers)
{
	_min = min;
	_max = max;
	SetVal (valMean + Randomsignreal() * valVariance);
	SetLabel(label);
	_adjustothers = adjustothers;
}

//---------------- TweakParams methods ---------------
void TweakParam::Clamp()
{
	 if (_val < _min)
		 _val = _min;
	 else if (_val > _max)
		 _val = _max;
}

/*---------- AdditiveTweakParam constructors ------*/

AdditiveTweakParam::AdditiveTweakParam()
: _tRange(),
  TweakParam(0,0,0,0,"",NULL)
{}

/*AdditiveTweakParam::AdditiveTweakParam(const AdditiveTweakParam& source)
{
	 TweakRange::InitTweakRange( *(source->Get_tRange()) ) ;
	 InitTweakParam(source.Min(), source.Val(), source.Max(), source.Label(), source.adjustothers());
} */

AdditiveTweakParam::AdditiveTweakParam(Real min, Real valMean, Real valVariance,
	Real max, LPSTR label, int adjustothers, const TweakRange& tweakRange)
: _tRange (tweakRange), TweakParam(min,valMean,valVariance, max,label, adjustothers)
{}

/*----------- AdditiveTweakParam mutators ----------*/
void AdditiveTweakParam::InitTweakParam(Real min, Real valMean, Real valVariance,
	Real max, LPSTR label,int adjustothers, const TweakRange& tweakRange)
{
	_tRange.InitTweakRange(tweakRange);
	TweakParam::InitTweakParam(min,valMean,valVariance,max,label, adjustothers);
}

/*----------- AdditiveTweakParam methods ----------*/
#pragma argsused
void AdditiveTweakParam::Bump(Real updown, CA* thisCA)
{
	Real add_Inc;
	add_Inc = _tRange.RangeVal(_val);

	if (updown > 0)
		SetVal(_val + add_Inc);
	else
		SetVal(_val - add_Inc);
}

AdditiveTweakParam& AdditiveTweakParam::operator= (const AdditiveTweakParam& src)
{
	if ( this != &src)
	{
		_tRange = src._tRange;
		_min = src._min;
		_val = src._val;
		_max = src._max;
		if (_label)
		{
			Free();
			_label = NULL;
		}
		_label = new char[lstrlen(src._label)+1];
		lstrcpy (_label, src._label);
		_adjustothers = src._adjustothers;
	}
	return *this;
}

/*----------- MultiplicativeTweakParam contructors ---------*/

MultiplicativeTweakParam::MultiplicativeTweakParam()
: _multInc(1),
  _1_over_multInc(1),
  _minPositiveValueFlag(FALSE),
  _minPositiveValue(1),
  TweakParam(0,0,0,0,"",FALSE)
{}

MultiplicativeTweakParam::MultiplicativeTweakParam(Real min, Real valMean,
	Real valVariance, Real max,	LPSTR label, int adjustothers,
	Real multInc, BOOL minPosFlag, Real minPos)
: _multInc(multInc),
  _1_over_multInc(1.0/multInc),
  _minPositiveValueFlag(minPosFlag),
  _minPositiveValue(minPos),
  TweakParam(min,valMean,valVariance,max,label,adjustothers)
{
	if (_minPositiveValueFlag)
		_min = 0.0;
		/*Because if you are using minPositiveValueFlag, this means that you
		want to be able to go down to 0.0, and you wouldn't want Clamp to
		ruin this*/
}

/*--------- MultiplicativeTweakParam mutators --------------*/

void MultiplicativeTweakParam::SetMultInc (Real newMultInc)
{
	if (newMultInc != 0.0)
	{
		_multInc = (Real)newMultInc;
		_1_over_multInc = 1.0/_multInc;
	}
}

void MultiplicativeTweakParam::InitTweakParam (Real min, Real valMean,
		Real valVariance, Real max, LPSTR label, int adjustothers,
															 Real multInc,
															 BOOL minPosFlag,
															 Real minPos)
{
	TweakParam::InitTweakParam(min,valMean, valVariance,max,label,adjustothers);
	SetMultInc(multInc);
	_minPositiveValueFlag = minPosFlag;
	_minPositiveValue = minPos;
}

/*--------- MultiplicativeTweakParam methods --------------*/

void MultiplicativeTweakParam::Bump(Real updown, CA* thisCA)
{
	if (updown > 0.0)
	{
		if (_minPositiveValueFlag && _val < _minPositiveValue)
			_val = _minPositiveValue;
		SetVal(_val * _multInc, thisCA);
	}
	else  // updown <= 0
	{
		_val *= (_1_over_multInc);
		if (_minPositiveValueFlag && _val < _minPositiveValue)
			_val = 0.0;
		SetVal(_val, thisCA);
	}
}

MultiplicativeTweakParam& MultiplicativeTweakParam::operator=
										(const MultiplicativeTweakParam& src)
{
	if ( this != &src)
	{
		_min = src._min;
		_val = src._val;
		_max = src._max;
		if (_label)
		{
			Free();
			_label = NULL;
		}
		_label = new char[lstrlen(src._label)+1];
		lstrcpy (_label, src._label);
		_adjustothers = src._adjustothers;
		_multInc = src._multInc;
		_1_over_multInc = src._1_over_multInc;
		_minPositiveValueFlag = src._minPositiveValueFlag;
		_minPositiveValue = src._minPositiveValue;
	}
	return *this;
}


