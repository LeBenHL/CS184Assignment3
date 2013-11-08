#include "adaptive_point.h"

AdaptivePoint::AdaptivePoint(ThreeDVector* vector, long double _u, long double _v, ThreeDVector* _normal) {
	x = vector->x;
	y = vector->y;
	z = vector->z;
	u = _u;
	v = _v;
	normal = _normal;
}