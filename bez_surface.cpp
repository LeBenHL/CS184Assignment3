#include "bez_surface.h"

BezSurface::BezSurface() {

}

void BezSurface::add_control_points(int curve_index, ThreeDVector** curve) {
	this->control_points[curve_index][0] = curve[0];
	this->control_points[curve_index][1] = curve[1];
	this->control_points[curve_index][2] = curve[2];
	this->control_points[curve_index][3] = curve[3];
}