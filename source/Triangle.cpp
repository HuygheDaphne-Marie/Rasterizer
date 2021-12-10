#include "Triangle.h"

#include <SDL_pixels.h>

#include "MathHelper.h"
#include "SceneManager.h"

// Class instances are no longer in use, deprecated
// static method kept here since it's a Triangle that's checks if it's hit so it fits conceptually

Triangle::Triangle(const FPoint3& position, 
	const FPoint3& v0, const RGBColor& c0,
	const FPoint3& v1, const RGBColor& c1,
	const FPoint3& v2, const RGBColor& c2)
	: Geometry(position)
	, m_ModelVertex0(v0)
	, m_ModelVertex1(v1)
	, m_ModelVertex2(v2)
	, m_Color0(c0)
	, m_Color1(c1)
	, m_Color2(c2)
{
	RecalculateWorldVertices();
}

bool Triangle::Hit(const FPoint2& pixel, std::vector<Vertex>& vertices)
{
	// Hit check
	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel - vertices[0].position.xy };
	FVector3 edge{ vertices[1].position.xyz - vertices[0].position.xyz };
	const float crossA = Cross(edge.xy, pixelToVertex);
	if (crossA > 0.f) // edgeA
		return false;

	pixelToVertex = pixel - vertices[1].position.xy;
	edge = vertices[2].position.xyz - vertices[1].position.xyz;
	const float crossB = Cross(edge.xy, pixelToVertex);
	if (crossB > 0.f) // edgeB
		return false;

	pixelToVertex = pixel - vertices[2].position.xy;
	edge = vertices[0].position.xyz - vertices[2].position.xyz;
	const float crossC = Cross(edge.xy, pixelToVertex);
	if (crossC > 0.f) // edgeC
		return false;
	// Triangle has been hit at this point

	// Calculate weights
	const float area{ Cross(FVector2{vertices[0].position.xy - vertices[1].position.xy}, FVector2{vertices[0].position.xy - vertices[2].position.xy}) };
	vertices[0].weight = crossB / area;
	vertices[1].weight = crossC / area;
	vertices[2].weight = crossA / area;

	return true;
}

void Triangle::RecalculateWorldVertices()
{
	FPoint4 vertex{ m_ModelVertex0, 1 };
	vertex = GetTransform() * vertex;
	m_WorldVertex0 = vertex.xyz;

	vertex = m_ModelVertex1;
	vertex = GetTransform() * vertex;
	m_WorldVertex1 = vertex.xyz;

	vertex = m_ModelVertex2;
	vertex = GetTransform() * vertex;
	m_WorldVertex2 = vertex.xyz;
}
void Triangle::OnRecalculateTransform()
{
	RecalculateWorldVertices();
}
