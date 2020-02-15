#include "shape.h"
// #include "stats.h"
// #include "lowdiscrepancy.h"

namespace pbrt {
	
// Shape Method Definitions
// Shape::~Shape() {}


Shape::Shape(const Transform* ObjectToWorld, const Transform* WorldToObject,
	bool reverseOrientation)
	: ObjectToWorld(ObjectToWorld),
	WorldToObject(WorldToObject),
	reverseOrientation(reverseOrientation),
	transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {
	// ++nShapesCreated;
}

Bounds3f Shape::WorldBound() const {
	return (*ObjectToWorld)(ObjectBound());
}

}