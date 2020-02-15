#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_MEDIUM_H
#define PBRT_CORE_MEDIUM_H

// core/medium.h*
#include "pbrt.h"
#include "geometry.h"
// #include "spectrum.h"
#include <memory>

namespace pbrt {
// MediumInterface Declarations
struct MediumInterface {
	MediumInterface() : inside(nullptr), outside(nullptr) {}
	// MediumInterface Public Methods
	MediumInterface(const Medium* medium) : inside(medium), outside(medium) {}
	MediumInterface(const Medium* inside, const Medium* outside)
		: inside(inside), outside(outside) {}
	bool IsMediumTransition() const { return inside != outside; }
	const Medium* inside, * outside;
};

}  // namespace pbrt

#endif  // PBRT_CORE_MEDIUM_H