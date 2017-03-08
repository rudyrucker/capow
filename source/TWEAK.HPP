/* written by Otto Leung, Oct 1, 96
	encapsulation for some CA parameters
*/

#ifndef TWEAK_HPP
#define TWEAK_HPP

#include "types.h"
#include "random.h"

#define MAX_POINTS 3   // Whenever you change this, you also need to update
							  // the constructors of class TweakRange,
							  // and class AdditiveTweakParam
							  //
							  // Since MAX_POINTS (96 Sept 27: 3) is very small,
							  // there is no need to use pointer (memovy allocation
							  // method) to save memory
#define MAX_RANGES MAX_POINTS+1
#define DEFAULT_RANGE 2.0  //As it happens most of my rules have ranges from 0 to 2



/*             TweakRange notes


	<      lo      }{    med   }{   hi  }{   super  >
	<--------------|-----------|--------|----------->
					  p0          p1       p2

the range (MINREAL, p0] has value lo
the range (p0, p1]      has value med
the range (p1, p2]      has value hi
the range (p2, MAXREAL) has value super

*/

class TweakRange
{
	private:
		Real _transition[MAX_POINTS];
		Real _increment[MAX_RANGES];
		friend class TweakParam;
	public:
		// constructors, destructor
		TweakRange();
		TweakRange(const TweakRange& new_TweakRange);  // copy constructor
		TweakRange(Real lo_value,        Real lo_to_med   = BIG_REAL,
					  Real med_value   = 0, Real med_to_hi   = BIG_REAL,
					  Real hi_value    = 0, Real hi_to_super = BIG_REAL,
					  Real super_value = 0);  // guarantee that unused transition points == BIG_REAL for safty
		~TweakRange() {}

		// accessors
		Real Transition(const int i) const;
		Real Increment(const int i) const;
		Real RangeVal(const Real transitionPoint) const; // return increment value based on the transitionPoint

		//  mutators
		void InitTweakRange(const TweakRange& tr);

		// methods
		TweakRange& operator= (const TweakRange& src)
		{  InitTweakRange(src);
			return *this;
		}
};

//#define _inc_lo _increment[0]
//#define _inc_med _increment[1]
//#define _inc_hi _increment[2]
//#define _inc_super _increment[3]
//#define _lo_to_med _transition[0]
//#define _med_to_hi _transition[1]
//#define _hi_to_super _transition[2]
#define NO_ADJUST 0
#define ADJUST_ACCELERATION 1
#define ADJUST_VARIANCE 2

class CA;
class TweakParam
{
	private:
//		typedef void (CA::*CAFunPtr)();

	protected:
		Real _val;  // [mean-variance, mean+variance]
		Real _min, _max;
		LPSTR _label;
		int _adjustothers;//void (CA::*_AdjustOthers)();
	public:
		// constructors, destructor
		TweakParam();
		TweakParam(Real min, Real valMean, Real valVariance, Real max, LPSTR label, BOOL adjustothers);
		~TweakParam();

		// accessors
		Real Min() const { return _min; }
		Real Val() const { return _val; }
		Real Max() const { return _max; }
	//	CAFunPtr TweakParam::AdjustOthers() { return _AdjustOthers; }
		// without typedef, use void (CA::*(TweakParam::AdjustOthers()))() {...}
		LPSTR Label() const { return _label; }

		// mutators
		void Free() { delete[] _label; _label = NULL;}
		void SetMin (Real min) { _min = min; Clamp();}
		void SetMax (Real max) { _max = max; Clamp();}
		void SetRange(Real min, Real max){_min = min; _max = max; Clamp();}
		void SetVal (Real newval, CA *thisCA = NULL);
		void SetLabel (LPSTR);
		void InitTweakParam(Real min, Real valMean, Real valVariance, Real max,
				LPSTR label, BOOL adjustothers);

		//methods
		void Clamp();
		virtual void Bump(Real, CA*) = 0;
};

class AdditiveTweakParam : public TweakParam
{
	protected:
		TweakRange _tRange;
		friend class TweakRange;

	public:
		// contructors, destructor
		AdditiveTweakParam();
		AdditiveTweakParam(const AdditiveTweakParam& source);
		AdditiveTweakParam(Real min , Real valMean, Real valVariance, Real max, LPSTR label,
								 BOOL adjustothers, const TweakRange& tweakRange);
		~AdditiveTweakParam() {}

		// accessors
		const TweakRange* Get_tRange () const { return (&_tRange); }

		// mutators
		void InitTweakParam(Real min, Real valMean, Real valVariance, Real max, LPSTR label,
								 int adjustothers, const TweakRange& tweakRange);

		// methods
		void Bump(Real updown, CA* thisCA);
		AdditiveTweakParam& operator= (const AdditiveTweakParam& src);
};

class MultiplicativeTweakParam : public TweakParam
{
	protected:
		Real _multInc; // cannot be 0; no error checking here!
		Real _1_over_multInc;
		BOOL _minPositiveValueFlag;
		Real _minPositiveValue;

	public:
		// contructors, destructor
		MultiplicativeTweakParam();
		MultiplicativeTweakParam(Real min, Real valMean, Real valVariance, Real max, LPSTR label,
										 int adjustothers,
										 Real multInc,
										 BOOL minPosFlag,
										 Real minPos=0.01);
		~MultiplicativeTweakParam() {}

		// mutators
		void SetMultInc (Real newMultInc);
		void InitTweakParam (Real min, Real valMean, Real valVariance, Real max,
			LPSTR label, int adjustothers, Real multInc, BOOL minPosFlag,
								  Real minPos=0.01);

		// methods
		void Bump(Real updown, CA* thisCA);
		MultiplicativeTweakParam& operator= (const MultiplicativeTweakParam& src);
};

/*CA
{
		  ChangeTweakParam(TweakParam* tp, Real updown)
*/
#endif

