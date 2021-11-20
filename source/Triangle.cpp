#include "Triangle.h"

#include <SDL_pixels.h>

#include "MathHelper.h"
#include "SceneManager.h"

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

void Triangle::Hit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels) const
{
	Vertex vertex0, vertex1, vertex2;
	GetTransformedVertices(vertex0, vertex1, vertex2);

	FPoint2 topLeft{};
	FPoint2 bottomRight{};
	GetBoundingPoints(topLeft, bottomRight, vertex0, vertex1, vertex2);

	Camera* pCamera{ SceneManager::GetInstance().GetActiveScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	//Loop over all the pixels in triangle
	FPoint3 pixel{};
	for (auto row = static_cast<uint32_t>(std::ceilf(topLeft.y)); row < static_cast<uint32_t>(std::ceilf(bottomRight.y)); ++row)
	{
		pixel.y = static_cast<float>(row);

		for (auto col = static_cast<uint32_t>(std::ceilf(topLeft.x)); col < static_cast<uint32_t>(std::ceilf(bottomRight.x)); ++col)
		{
			pixel.x = static_cast<float>(col);

			RGBColor finalColor{};
			if (PixelHit(pixel, finalColor, vertex0, vertex1, vertex2))
			{
				if (pixel.z < depthBuffer[PixelToBufferIndex(col, row, width)])
				{
					depthBuffer[PixelToBufferIndex(col, row, width)] = pixel.z;

					pBackBufferPixels[PixelToBufferIndex(col, row, width)] = SDL_MapRGB(pBackBuffer->format,
						static_cast<Uint8>(finalColor.r * 255.f),
						static_cast<Uint8>(finalColor.g * 255.f),
						static_cast<Uint8>(finalColor.b * 255.f));
				}
			}
		}
	}
}

bool Triangle::PixelHit(FPoint3& pixel, RGBColor& finalColor, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const
{
	// Hit check
	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel.xy - vertex0.position.xy };
	FVector3 edge{ vertex1.position - vertex0.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeA
		return false;

	pixelToVertex = pixel.xy - vertex1.position.xy;
	edge = { vertex2.position - vertex1.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeB
		return false;

	pixelToVertex = pixel.xy - vertex2.position.xy;
	edge = { vertex0.position - vertex2.position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeC
		return false;

	CalculateBarycentricWeights(pixel.xy, vertex0, vertex1, vertex2);

	// Depth test
	float pixelDepth{ vertex0.position.z * vertex0.weight + vertex1.position.z * vertex1.weight + vertex2.position.z * vertex2.weight };
	pixel.z = pixelDepth;
	
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

void Triangle::ApplyCameraCorrection(float fov, float aspectRatio, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const
{
	vertex0.position.x /= aspectRatio * fov;
	vertex0.position.y /= fov;

	vertex1.position.x /= aspectRatio * fov;
	vertex1.position.y /= fov;

	vertex2.position.x /= aspectRatio * fov;
	vertex2.position.y /= fov;
}
void Triangle::ApplyPerspectiveDivide(Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const
{
	vertex0.position.x = vertex0.position.x / -vertex0.position.z;
	vertex0.position.y = vertex0.position.y / -vertex0.position.z;

	vertex1.position.x = vertex1.position.x / -vertex1.position.z;
	vertex1.position.y = vertex1.position.y / -vertex1.position.z;

	vertex2.position.x = vertex2.position.x / -vertex2.position.z;
	vertex2.position.y = vertex2.position.y / -vertex2.position.z;

	vertex0.position.z *= -1;
	vertex1.position.z *= -1;
	vertex2.position.z *= -1;
}
void Triangle::CalculateBarycentricWeights(const FPoint2& pixel, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const
{
	const float area{ Cross(FVector2{vertex0.position.xy - vertex1.position.xy}, FVector2{vertex0.position.xy - vertex2.position.xy}) };

	// swapped compared to slides so sign is positive
	vertex0.weight = Cross(FVector2{ vertex2.position.xy - vertex1.position.xy }, FVector2{ pixel - vertex1.position.xy }) / area;
	vertex1.weight = Cross(FVector2{ vertex0.position.xy - vertex2.position.xy }, FVector2{ pixel - vertex2.position.xy }) / area;
	vertex2.weight = Cross(FVector2{ vertex1.position.xy - vertex0.position.xy }, FVector2{ pixel - vertex0.position.xy }) / area;
}

void Triangle::GetBoundingPoints(FPoint2& topLeft, FPoint2& bottomRight, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2) const
{
	topLeft = FPoint2{
		std::min(vertex0.position.x, std::min(vertex1.position.x, vertex2.position.x)),
		std::min(vertex0.position.y, std::min(vertex1.position.y, vertex2.position.y))
	};
	bottomRight = FPoint2{
		std::max(vertex0.position.x, std::max(vertex1.position.x, vertex2.position.x)),
		std::max(vertex0.position.y, std::max(vertex1.position.y, vertex2.position.y))
	};

	const float width{ static_cast<float>(SceneManager::GetInstance().GetActiveScene().GetCamera()->GetScreenWidth()) };
	const float height{ static_cast<float>(SceneManager::GetInstance().GetActiveScene().GetCamera()->GetScreenHeight()) };

	LimitPointToScreenBoundaries(topLeft, width, height);
	LimitPointToScreenBoundaries(bottomRight, width, height);
}

void Triangle::GetTransformedVertices(Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const
{
	SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	Camera* pCamera{ activeScene.GetCamera() };
	const FMatrix4& worldToView{ pCamera->GetWorldToView() };

	// Transform to viewSpace and make vertices out of points
	vertex0 = { FPoint3{worldToView * FPoint4(m_WorldVertex0, 1)}, m_Color0 };
	vertex1 = { FPoint3{worldToView * FPoint4(m_WorldVertex1, 1)}, m_Color1 };
	vertex2 = { FPoint3{worldToView * FPoint4(m_WorldVertex2, 1)}, m_Color2 };

	// bring vertices to projection space
	ApplyCameraCorrection(pCamera->GetFov(), pCamera->GetAspectRatio(), vertex0, vertex1, vertex2);
	ApplyPerspectiveDivide(vertex0, vertex1, vertex2);

	// to screen space
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };
	vertex0.position.xy = FPoint2{ CalculateScreenSpaceX(vertex0.position.x, width), CalculateScreenSpaceY(vertex0.position.y, height) };
	vertex1.position.xy = FPoint2{ CalculateScreenSpaceX(vertex1.position.x, width), CalculateScreenSpaceY(vertex1.position.y, height) };
	vertex2.position.xy = FPoint2{ CalculateScreenSpaceX(vertex2.position.x, width), CalculateScreenSpaceY(vertex2.position.y, height) };
}
