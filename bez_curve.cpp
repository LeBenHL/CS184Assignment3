#include "bez_curve.h"

BezCurve::BezCurve(ThreeDVector** curve) {
	this->control_points[0] = curve[0];
	this->control_points[1] = curve[1];
	this->control_points[2] = curve[2];
	this->control_points[3] = curve[3];
}

std::pair<ThreeDVector*, ThreeDVector*> BezCurve::interpolate(long double u) {

	ThreeDVector* A1 = (this->control_points[0])->scalar_multiply(1.0 - u);
	ThreeDVector* A2 = (this->control_points[1])->scalar_multiply(u);
	ThreeDVector* B1 = (this->control_points[1])->scalar_multiply(1.0 - u);
	ThreeDVector* B2 = (this->control_points[2])->scalar_multiply(u);
	ThreeDVector* C1 = (this->control_points[2])->scalar_multiply(1.0 - u);
	ThreeDVector* C2 = (this->control_points[3])->scalar_multiply(u);

	ThreeDVector* A = A1->vector_add(A2);
	ThreeDVector* B = B1->vector_add(B2);
	ThreeDVector* C = C1->vector_add(C2);

	ThreeDVector* D1 = A->scalar_multiply(1.0 - u);
	ThreeDVector* D2 = B->scalar_multiply(u);
	ThreeDVector* E1 = B->scalar_multiply(1.0 - u);
	ThreeDVector* E2 = C->scalar_multiply(u);

	ThreeDVector* D = D1->vector_add(D2);
	ThreeDVector* E = E1->vector_add(E2);

	ThreeDVector* control_point1 = D->scalar_multiply(1.0 - u);
	ThreeDVector* control_point2 = E->scalar_multiply(u);

	ThreeDVector* control_point = control_point1->vector_add(control_point2);
	ThreeDVector* derivative = E->vector_subtract(D);
	derivative->scalar_multiply_bang(3);

	delete A;
	delete B;
	delete C;
	delete D;
	delete E;
	delete A1;
	delete A2; 
	delete B1;
	delete B2;
	delete C1;
	delete C2;
	delete D1;
	delete D2;
	delete E1;
	delete E2;
	delete control_point1;
	delete control_point2;

	return std::make_pair(control_point, derivative);

}