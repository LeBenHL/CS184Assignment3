#include "adaptive_triangle.h"

AdaptiveTriangle::AdaptiveTriangle(AdaptivePoint* _a, AdaptivePoint* _b, AdaptivePoint* _c) {
	a = _a;
	b = _b;
	c = _c;
}

//Return True if the triangle was split, False otherwise. 
//When Triangle is split, store resulting split triangles into the back of the queue
bool AdaptiveTriangle::split(queue<AdaptiveTriangle*>* queue, long double subdivision_parameter, BezSurface* surface) {

	//Edge 1 A->B
	ThreeDVector* mid_a_b = this->a->midpoint(this->b);
	pair<ThreeDVector*, ThreeDVector*> interpolated_mid_a_b = surface->interpolate((this->a->u + this->b->u)/2, (this->a->v + this->b->v)/2);
	bool split_a_b = mid_a_b->distance(interpolated_mid_a_b.first) > subdivision_parameter;

	//Edge 2 B->C
	ThreeDVector* mid_b_c = this->b->midpoint(this->c);
	pair<ThreeDVector*, ThreeDVector*> interpolated_mid_b_c = surface->interpolate((this->b->u + this->c->u)/2, (this->b->v + this->c->v)/2);
	bool split_b_c = mid_b_c->distance(interpolated_mid_b_c.first) > subdivision_parameter;

	//Edge 3 C->A
	ThreeDVector* mid_c_a = this->c->midpoint(this->a);
	pair<ThreeDVector*, ThreeDVector*> interpolated_mid_c_a = surface->interpolate((this->c->u + this->a->u)/2, (this->c->v + this->a->v)/2);
	bool split_c_a = mid_c_a->distance(interpolated_mid_c_a.first) > subdivision_parameter;

	if (!split_a_b && !split_b_c && !split_c_a) {
		//No Split
		return false
	} else if (split_a_b && !split_b_c && !split_c_a) {
		//Split Edge 1
		return true;
	} else if (!split_a_b && split_b_c && !split_c_a) {
		//Split Edge 2
		return true;
	} else if (!split_a_b && !split_b_c && split_c_a) {
		//Split Edge 3
		return true;
	} else if (split_a_b && split_b_c && !split_c_a) {
		//Split Edge 1 and 2
		return true;
	} else if (!split_a_b && split_b_c && split_c_a) {
		//Split Edge 2 and 3
		return true;
	} else if (split_a_b && !split_b_c && split_c_a) {
		//Split Edge 1 and 3
		return true;
	} else if (split_a_b && split_b_c && split_c_a) {
		//Split Edge 1 2 and 3
		return true;
	} else {
		cout << "WTF HAPPENED" << endl;
	}
}

AdaptiveTriangle::~AdaptiveTriangle(){
	delete a;
	delete b;
	delete c;
}