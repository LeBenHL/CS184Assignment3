#ifndef ADAPTIVETRIANGLE_H
#define ADAPTIVETRIANGLE_H

#include "adaptive_point.h"
#include "bez_surface.h"
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

class AdaptiveTriangle {
	public:
		//vertices
		AdaptivePoint* a;
		AdaptivePoint* b;
		AdaptivePoint* c;

		AdaptiveTriangle(AdaptivePoint* _a, AdaptivePoint* _b, AdaptivePoint* _c);
		bool split(std::queue<AdaptiveTriangle*>* queue, long double subdivision_parameter, BezSurface* surface);
		~AdaptiveTriangle();

};

#endif
