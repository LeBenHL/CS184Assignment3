#ifndef ADAPTIVEPOINT_H
#define ADAPTIVEPOINT_H
#include "three_d_vector.h"

class AdaptivePoint: public ThreeDVector {
	public:
		//Parametric Values associated with this point
		AdaptivePoint(ThreeDVector* vector, long double u, long double v, ThreeDVector* normal);
		long double u;
		long double v;
		ThreeDVector* normal;
};

#endif