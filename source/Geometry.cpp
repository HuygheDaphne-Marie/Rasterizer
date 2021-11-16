#include "Geometry.h"

Geometry::Geometry(const FPoint3& position, const RGBColor& color)
	: m_Transform()
	, m_Color(color)
{
	m_Transform[3] = FVector4{ position.x, position.y, position.z, 1.f };
}
