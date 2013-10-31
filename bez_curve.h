#ifndef BEZCURVE_H
#define BEZCURVE_H
#include "three_d_vector.h"

class BezCurve{
	public:
		BezCurve(ThreeDVector** curve);
		ThreeDVector* control_points[4];

};

#endif