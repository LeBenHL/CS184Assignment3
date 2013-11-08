#include "adaptive_triangle.h"

AdaptiveTriangle::AdaptiveTriangle(AdaptivePoint* _a, AdaptivePoint* _b, AdaptivePoint* _c) {
	a = _a;
	b = _b;
	c = _c;
}

//Return True if the triangle was split, False otherwise. 
//When Triangle is split, store resulting split triangles into the back of the queue
bool AdaptiveTriangle::split(std::queue<AdaptiveTriangle*>* queue) {
	return false;
}

AdaptiveTriangle::~AdaptiveTriangle(){
	delete a;
	delete b;
	delete c;
}