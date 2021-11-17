#include "Triangle.h"

#include "MathHelper.h"
#include "SceneManager.h"

Triangle::Triangle(const FPoint3& position, const FPoint3& v0, const FPoint3& v1, const FPoint3& v2)
	: Geometry(position)
	, m_ModelVertex0(v0)
	, m_ModelVertex1(v1)
	, m_ModelVertex2(v2)
{


	RecalculateWorldVertices();
}

bool Triangle::Hit(const FPoint2& pixel, const FMatrix4& worldToView) const
{
	FPoint3 vertex0{ worldToView * FPoint4(m_WorldVertex0, 1) };
	FPoint3 vertex1{ worldToView * FPoint4(m_WorldVertex1, 1) };
	FPoint3 vertex2{ worldToView * FPoint4(m_WorldVertex2, 1) };

	// temp camera get
	const float fov = SceneManager::GetInstance().GetActiveScene().GetCamera()->GetFov();
	const int width = 640;
	const int height = 480;
	const float aspectRatio = static_cast<float>(height) / static_cast<float>(width);

	vertex0.x /= aspectRatio * fov;
	vertex0.y /= fov;

	vertex1.x /= aspectRatio * fov;
	vertex1.y /= fov;

	vertex2.x /= aspectRatio * fov;
	vertex2.y /= fov;

	// Perspective divide
	vertex0.x = vertex0.x / -vertex0.z;
	vertex0.y = vertex0.y / -vertex0.z;

	vertex1.x = vertex1.x / -vertex1.z;
	vertex1.y = vertex1.y / -vertex1.z;

	vertex2.x = vertex2.x / -vertex2.z;
	vertex2.y = vertex2.y / -vertex2.z;

	vertex0.z *= -1;
	vertex1.z *= -1;
	vertex2.z *= -1;

	// to screen space
	vertex0.xy = FPoint2{ CalculateScreenSpaceX(vertex0.x, width), CalculateScreenSpaceY(vertex0.y, height) };
	vertex1.xy = FPoint2{ CalculateScreenSpaceX(vertex1.x, width), CalculateScreenSpaceY(vertex1.y, height) };
	vertex2.xy = FPoint2{ CalculateScreenSpaceX(vertex2.x, width), CalculateScreenSpaceY(vertex2.y, height) };

	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel - vertex0.xy };
	FVector3 edge{ vertex1 - vertex0 };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeA
		return false;

	pixelToVertex = pixel - vertex1.xy;
	edge = { vertex2 - vertex1 };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeB
		return false;

	pixelToVertex = pixel - vertex2.xy;
	edge = { vertex0 - vertex2 };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeC
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


