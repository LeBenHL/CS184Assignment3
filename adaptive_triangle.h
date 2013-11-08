#ifndef ADAPTIVETRIANGLE_H
#define ADAPTIVETRIANGLE_H

#include "adaptive_point.h"
#include <queue>
#include <vector>

class AdaptiveTriangle {
	public:
		//vertices
		AdaptivePoint* a;
		AdaptivePoint* b;
		AdaptivePoint* c;

		AdaptiveTriangle(AdaptivePoint* _a, AdaptivePoint* _b, AdaptivePoint* _c);
		bool split(std::queue<AdaptiveTriangle*>* queue);
		~AdaptiveTriangle();

};

#endif
