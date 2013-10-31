#ifndef BEZSURFACE_H
#define BEZSURFACE_H
#include "three_d_vector.h"

class BezSurface{
	public:
		BezSurface();
		void add_control_points(int curve_index, ThreeDVector** curve);
		ThreeDVector* control_points[4][4];

};

#endif