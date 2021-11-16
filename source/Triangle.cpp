#include "Triangle.h"

Triangle::Triangle(const FPoint3& position, const RGBColor& color, const FPoint3& v0, const FPoint3& v1, const FPoint3& v2)
	: Geometry(position, color)
	, m_Vertex0(v0)
	, m_Vertex1(v1)
	, m_Vertex2(v2)
{
}

bool Triangle::Hit(const FPoint2& pixel) const
{
	FVector2 intersectToVertex{ pixel - m_Vertex0.xy };
	FVector3 edge{ m_Vertex1 - m_Vertex0 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeA
		return false;

	intersectToVertex = pixel - m_Vertex1.xy;
	edge = { m_Vertex2 - m_Vertex1 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeB
		return false;

	intersectToVertex = pixel - m_Vertex2.xy;
	edge = { m_Vertex0 - m_Vertex2 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeC
		return false;

	return true;
}


