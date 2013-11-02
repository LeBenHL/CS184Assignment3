#ifndef BEZCURVE_H
#define BEZCURVE_H
#include "three_d_vector.h"
#include "bez_curve.h"

class BezCurve{
	public:
		BezCurve(ThreeDVector** curve);
		ThreeDVector* control_points[4];
		std::pair<ThreeDVector*, ThreeDVector*> interpolate(long double u);

};

#endif