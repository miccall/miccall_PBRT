#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_INTERACTION_H
#define PBRT_CORE_INTERACTION_H

// core/interaction.h*
#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "medium.h"
// #include "material.h"

namespace pbrt {
/**
 * SurfaceInteraction和MediumInteraction都继承自通用的Interaction类
 * 该类提供了一些常见的成员变量和方法
 */
struct Interaction {
	// Interaction Public Methods
	Interaction() : time(0) {}
	Interaction(const Point3f& p, 
		const Normal3f& n, 
		const Vector3f& pError,
		const Vector3f& wo, 
		Float time,
		const MediumInterface& mediumInterface)
		: p(p), 
		time(time), 
		pError(pError), 
		wo(wo), 
		n(n),
		mediumInterface(mediumInterface) { }
	bool IsSurfaceInteraction() const { return n != Normal3f(); }

	Interaction(const Point3f& p, 
		const Vector3f& wo, 
		Float time,
		const MediumInterface& mediumInterface)
		: p(p), 
		time(time), 
		wo(wo), 
		mediumInterface(mediumInterface) {}

	Interaction(const Point3f& p, 
		Float time,
		const MediumInterface& mediumInterface)
		: p(p), 
		time(time), 
		mediumInterface(mediumInterface) {}

	// Interaction Public Data
	Point3f p; // 相交点
	Float time; // 相交时间 
	Vector3f pError; // 浮点误差 
	Vector3f wo; // 出射方向的点
	Normal3f n; // 曲面法线
	MediumInterface mediumInterface; //Section 11.3.1
};


/**
 * SurfaceInteraction Declarations
 * 表示 2D 曲面上 某个点的局部信息 
 */
class SurfaceInteraction : public Interaction {
public:
	// SurfaceInteraction Public Methods
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f& p, const Vector3f& pError,
		const Point2f& uv, const Vector3f& wo,
		const Vector3f& dpdu, const Vector3f& dpdv,
		const Normal3f& dndu, const Normal3f& dndv, Float time,
		const Shape* sh,
		int faceIndex = 0);
	
	//计算阴影坐标系时进行更新
	void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
		const Normal3f& dndu, const Normal3f& dndv,
		bool orientationIsAuthoritative);

	// SurfaceInteraction Public Data
	Point2f uv;
	Vector3f dpdu, dpdv; //参数偏导数
	Normal3f dndu, dndv; //表面法线的偏导数
	const Shape* shape = nullptr;
	struct {
		Normal3f n;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
	} shading; // 记录发现扰动 
	const Primitive* primitive = nullptr;
	// BSDF* bsdf = nullptr;
	// BSSRDF* bssrdf = nullptr;

};

}  // namespace pbrt

#endif  // PBRT_CORE_INTERACTION_H