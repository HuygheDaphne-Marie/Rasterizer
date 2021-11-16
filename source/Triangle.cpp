#include "Triangle.h"

Triangle::Triangle(const FPoint3& position, const FPoint3& v0, const FPoint3& v1, const FPoint3& v2)
	: Geometry(position)
	, m_ModelVertex0(v0)
	, m_ModelVertex1(v1)
	, m_ModelVertex2(v2)
{


	RecalculateWorldVertices();
}

bool Triangle::Hit(const FPoint2& pixel) const
{
	FVector2 intersectToVertex{ pixel - m_ModelVertex0.xy };
	FVector3 edge{ m_ModelVertex1 - m_ModelVertex0 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeA
		return false;

	intersectToVertex = pixel - m_ModelVertex1.xy;
	edge = { m_ModelVertex2 - m_ModelVertex1 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeB
		return false;

	intersectToVertex = pixel - m_ModelVertex2.xy;
	edge = { m_ModelVertex0 - m_ModelVertex2 };
	if (Cross(edge.xy, intersectToVertex) > 0.f) // edgeC
		return false;

	return true;
}

void Triangle::RecalculateWorldVertices()
{
	FPoint4 vertex{ m_ModelVertex0, 1 };
	vertex = m_Transform * vertex;
	m_WorldVertex0 = vertex.xyz;

	vertex = m_ModelVertex1;
	vertex = m_Transform * vertex;
	m_WorldVertex1 = vertex.xyz;

	vertex = m_ModelVertex2;
	vertex = m_Transform * vertex;
	m_WorldVertex2 = vertex.xyz;
}

void Triangle::OnRecalculateTransform()
{
	RecalculateWorldVertices();
}


