#ifndef BEZSURFACE_H
#define BEZSURFACE_H
#include "three_d_vector.h"
#include "bez_curve.h"

class BezSurface{
	public:
		BezSurface();
		void add_control_points(int curve_index, ThreeDVector** curve);
		std::pair<ThreeDVector*, ThreeDVector*> interpolate(long double u, long double v);
		ThreeDVector* control_points[4][4];

};

#endif