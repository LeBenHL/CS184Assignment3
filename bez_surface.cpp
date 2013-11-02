#include "bez_surface.h"

BezSurface::BezSurface() {

}

void BezSurface::add_control_points(int curve_index, ThreeDVector** curve) {
	this->control_points[curve_index][0] = curve[0];
	this->control_points[curve_index][1] = curve[1];
	this->control_points[curve_index][2] = curve[2];
	this->control_points[curve_index][3] = curve[3];
}

std::pair<ThreeDVector*, ThreeDVector*> BezSurface::interpolate(long double u, long double v) {
	//Build Control Points for a Bezier curve in v
	ThreeDVector* vcurve_points[4];

	BezCurve* curve0 = new BezCurve(this->control_points[0]); 
	std::pair<ThreeDVector*, ThreeDVector*> p0 = curve0->interpolate(u);
	vcurve_points[0] = p0.first;

	BezCurve* curve1 = new BezCurve(this->control_points[1]); 
	std::pair<ThreeDVector*, ThreeDVector*> p1 = curve1->interpolate(u);
	vcurve_points[1] = p1.first;

	BezCurve* curve2 = new BezCurve(this->control_points[2]); 
	std::pair<ThreeDVector*, ThreeDVector*> p2 = curve2->interpolate(u);
	vcurve_points[2] = p2.first;

	BezCurve* curve3 = new BezCurve(this->control_points[3]); 
	std::pair<ThreeDVector*, ThreeDVector*> p3 = curve3->interpolate(u);
	vcurve_points[3] = p3.first;

	//Build Control Points for a Bezer Curve in u
	ThreeDVector* ucurve_points[4];

	ThreeDVector* ucurve0_points[4];
	ucurve0_points[0] = this->control_points[0][0];
	ucurve0_points[1] = this->control_points[1][0];
	ucurve0_points[2] = this->control_points[2][0];
	ucurve0_points[3] = this->control_points[3][0];
	BezCurve* ucurve0 = new BezCurve(ucurve0_points); 
	std::pair<ThreeDVector*, ThreeDVector*> up0 = ucurve0->interpolate(v);
	ucurve_points[0] = up0.first;

	ThreeDVector* ucurve1_points[4];
	ucurve1_points[0] = this->control_points[0][1];
	ucurve1_points[1] = this->control_points[1][1];
	ucurve1_points[2] = this->control_points[2][1];
	ucurve1_points[3] = this->control_points[3][1];
	BezCurve* ucurve1 = new BezCurve(ucurve1_points); 
	std::pair<ThreeDVector*, ThreeDVector*> up1 = ucurve1->interpolate(v);
	ucurve_points[1] = up1.first;

	ThreeDVector* ucurve2_points[4];
	ucurve2_points[0] = this->control_points[0][2];
	ucurve2_points[1] = this->control_points[1][2];
	ucurve2_points[2] = this->control_points[2][2];
	ucurve2_points[3] = this->control_points[3][2];
	BezCurve* ucurve2 = new BezCurve(ucurve2_points); 
	std::pair<ThreeDVector*, ThreeDVector*> up2 = ucurve2->interpolate(v);
	ucurve_points[2] = up2.first;

	ThreeDVector* ucurve3_points[4];
	ucurve3_points[0] = this->control_points[0][3];
	ucurve3_points[1] = this->control_points[1][3];
	ucurve3_points[2] = this->control_points[2][3];
	ucurve3_points[3] = this->control_points[3][3];
	BezCurve* ucurve3 = new BezCurve(ucurve3_points); 
	std::pair<ThreeDVector*, ThreeDVector*> up3 = ucurve3->interpolate(v);
	ucurve_points[3] = up3.first;

	//Evaluate Surface point and Derivative
	BezCurve* ucurve = new BezCurve(ucurve_points);
	BezCurve* vcurve = new BezCurve(vcurve_points);

	std::pair<ThreeDVector*, ThreeDVector*> control_pointu = ucurve->interpolate(u);
	std::pair<ThreeDVector*, ThreeDVector*> control_pointv = vcurve->interpolate(v);

	ThreeDVector* normal = (control_pointu.second)->cross_product(control_pointv.second);
	normal->normalize_bang();

	delete curve0;
	delete curve1;
	delete curve2;
	delete curve3;
	delete ucurve0;
	delete ucurve1;
	delete ucurve2;
	delete ucurve3;
	delete ucurve;
	delete vcurve;

	delete control_pointv.first;
	delete control_pointu.second;
	delete control_pointv.second;
	delete p0.first;
	delete p0.second;
	delete p1.first;
	delete p1.second;
	delete p2.first;
	delete p2.second;
	delete p3.first;
	delete p3.second;
	delete up0.first;
	delete up0.second;
	delete up1.first;
	delete up1.second;
	delete up2.first;
	delete up2.second;
	delete up3.first;
	delete up3.second;

	return std::make_pair(control_pointu.first, normal);

}