#pragma once
#include "Geometry.h"
class Triangle final : public Geometry
{
public:
	Triangle(const FPoint3& position, const RGBColor& color, const FPoint3& v0, const FPoint3& v1, const FPoint3& v2);

	bool Hit(const FPoint2& pixel) const override;

private:
	// vertices stored in screen space
	FPoint3 m_Vertex0;
	FPoint3 m_Vertex1;
	FPoint3 m_Vertex2;
};

