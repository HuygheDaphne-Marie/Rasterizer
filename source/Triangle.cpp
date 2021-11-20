#include "Triangle.h"

#include "MathHelper.h"
#include "SceneManager.h"
#include "Vertex.h"

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

bool Triangle::Hit(const FPoint2& pixel, RGBColor& finalColor) const
{
	SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	const FMatrix4 worldToView{ activeScene.GetCamera()->GetWorldToView() };

	// should be member?
	Vertex vertex0{FPoint3{worldToView * FPoint4(m_WorldVertex0, 1)}, m_Color0};
	Vertex vertex1{FPoint3{worldToView * FPoint4(m_WorldVertex1, 1)}, m_Color1};
	Vertex vertex2{FPoint3{worldToView * FPoint4(m_WorldVertex2, 1)}, m_Color2};

	// camera correction
	const float fov = activeScene.GetCamera()->GetFov();
	const float aspectRatio = activeScene.GetCamera()->GetAspectRatio();

	vertex0.position.x /= aspectRatio * fov;
	vertex0.position.y /= fov;

	vertex1.position.x /= aspectRatio * fov;
	vertex1.position.y /= fov;

	vertex2.position.x /= aspectRatio * fov;
	vertex2.position.y /= fov;

	// Perspective divide
	vertex0.position.x = vertex0.position.x / -vertex0.position.z;
	vertex0.position.y = vertex0.position.y / -vertex0.position.z;

	vertex1.position.x = vertex1.position.x / -vertex1.position.z;
	vertex1.position.y = vertex1.position.y / -vertex1.position.z;

	vertex2.position.x = vertex2.position.x / -vertex2.position.z;
	vertex2.position.y = vertex2.position.y / -vertex2.position.z;

	vertex0.position.z *= -1;
	vertex1.position.z *= -1;
	vertex2.position.z *= -1;

	// to screen space
	const int width{ activeScene.GetCamera()->GetScreenWidth() };
	const int height{ activeScene.GetCamera()->GetScreenHeight() };
	vertex0.position.xy = FPoint2{ CalculateScreenSpaceX(vertex0.position.x, width), CalculateScreenSpaceY(vertex0.position.y, height) };
	vertex1.position.xy = FPoint2{ CalculateScreenSpaceX(vertex1.position.x, width), CalculateScreenSpaceY(vertex1.position.y, height) };
	vertex2.position.xy = FPoint2{ CalculateScreenSpaceX(vertex2.position.x, width), CalculateScreenSpaceY(vertex2.position.y, height) };

	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel - vertex0.position.xy };
	FVector3 edge{ vertex1.position - vertex0.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeA
		return false;

	pixelToVertex = pixel - vertex1.position.xy;
	edge = { vertex2.position - vertex1.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeB
		return false;

	pixelToVertex = pixel - vertex2.position.xy;
	edge = { vertex0.position - vertex2.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeC
		return false;

	// barycentric coordinates
	float area{ Cross(FVector2{vertex0.position.xy - vertex1.position.xy}, FVector2{vertex0.position.xy - vertex2.position.xy}) }; // member, recalculated
		// swapped compared to slides so sign is positive
	vertex0.weight = Cross(FVector2{ vertex2.position.xy - vertex1.position.xy }, FVector2{ pixel - vertex1.position.xy }) / area;
	vertex1.weight = Cross(FVector2{ vertex0.position.xy - vertex2.position.xy }, FVector2{ pixel - vertex2.position.xy }) / area;
	vertex2.weight = Cross(FVector2{ vertex1.position.xy - vertex0.position.xy }, FVector2{ pixel - vertex0.position.xy }) / area;
	
	// Set color
	finalColor = vertex0.color * vertex0.weight + vertex1.color * vertex1.weight + vertex2.color * vertex2.weight;
	
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


