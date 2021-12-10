#pragma once
#include "Geometry.h"
#include "Vertex.h"

// Deprecated class, do not use

class Triangle final : public Geometry
{
public:
	Triangle(const FPoint3& position, 
		const FPoint3& v0, const RGBColor& c0, 
		const FPoint3& v1, const RGBColor& c1, 
		const FPoint3& v2, const RGBColor& c2);

	// Old version, didn't implement new method here since only TriangleMeshes are made
	//void Hit(const RenderInfo& renderInfo) const override;

	static bool Hit(const FPoint2& pixel, std::vector<Vertex>& vertices);
	// could add static rasterize function

private:
	const FPoint3 m_ModelVertex0;
	const FPoint3 m_ModelVertex1;
	const FPoint3 m_ModelVertex2;
	const RGBColor m_Color0;
	const RGBColor m_Color1;
	const RGBColor m_Color2;

	FPoint3 m_WorldVertex0{};
	FPoint3 m_WorldVertex1{};
	FPoint3 m_WorldVertex2{};

	void RecalculateWorldVertices();
	void OnRecalculateTransform() override;
};

