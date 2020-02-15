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
 * SurfaceInteraction��MediumInteraction���̳���ͨ�õ�Interaction��
 * �����ṩ��һЩ�����ĳ�Ա�����ͷ���
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
	Point3f p; // �ཻ��
	Float time; // �ཻʱ�� 
	Vector3f pError; // ������� 
	Vector3f wo; // ���䷽��ĵ�
	Normal3f n; // ���淨��
	MediumInterface mediumInterface; //Section 11.3.1
};


/**
 * SurfaceInteraction Declarations
 * ��ʾ 2D ������ ĳ����ľֲ���Ϣ 
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
	
	//������Ӱ����ϵʱ���и���
	void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
		const Normal3f& dndu, const Normal3f& dndv,
		bool orientationIsAuthoritative);

	// SurfaceInteraction Public Data
	Point2f uv;
	Vector3f dpdu, dpdv; //����ƫ����
	Normal3f dndu, dndv; //���淨�ߵ�ƫ����
	const Shape* shape = nullptr;
	struct {
		Normal3f n;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
	} shading; // ��¼�����Ŷ� 
	const Primitive* primitive = nullptr;
	// BSDF* bsdf = nullptr;
	// BSSRDF* bssrdf = nullptr;

};

}  // namespace pbrt

#endif  // PBRT_CORE_INTERACTION_H