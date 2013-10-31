#include "bez_curve.h"

BezCurve::BezCurve(ThreeDVector** curve) {
	this->control_points[0] = curve[0];
	this->control_points[1] = curve[1];
	this->control_points[2] = curve[2];
	this->control_points[3] = curve[3];
}