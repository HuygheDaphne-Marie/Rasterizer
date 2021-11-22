#include "TriangleMesh.h"
#include "SceneManager.h"
#include "TransformationHelper.h"
#include <tuple>

TriangleMesh::TriangleMesh(const FPoint3& position, const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
	: Geometry(position)
	, m_ModelVertices(vertices)
	, m_Indices(indices)
{
	m_WorldVertices.insert(m_WorldVertices.end(), m_ModelVertices.begin(), m_ModelVertices.end());
	RecalculateWorldVertices();
}

void TriangleMesh::Hit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels) const
{
	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	const Camera* pCamera{ activeScene.GetCamera() };
	std::vector<Vertex> transformedVertices{ TransformVertices(pCamera->GetWorldToView(), m_WorldVertices) };

	// for each triangle
		// check the mode (normal or strip)
		// get right indices
		// call triangle hit
}

void TriangleMesh::RecalculateWorldVertices()
{
	for (unsigned int i{0}; i < m_ModelVertices.size(); ++i)
	{
		m_WorldVertices[i].position = (GetTransform() * FPoint4 { m_ModelVertices[i].position, 1 }).xyz;
	}
}
void TriangleMesh::OnRecalculateTransform()
{
	RecalculateWorldVertices();
}

void TriangleMesh::TriangleHit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, 
	std::vector<Vertex>& triangleVertices) const
{
	const Camera* pCamera{ SceneManager::GetInstance().GetActiveScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };

	const std::tuple<Elite::FPoint2, Elite::FPoint2> points{ GetBoundingBox(static_cast<float>(width), static_cast<float>(height), triangleVertices) };
	const FPoint2 topLeft{ std::get<0>(points) };
	const FPoint2 bottomRight{ std::get<1>(points) };

	//Loop over all the pixels in triangle
	FPoint3 pixel{};
	for (auto row = static_cast<uint32_t>(std::ceilf(topLeft.y)); row < static_cast<uint32_t>(std::ceilf(bottomRight.y)); ++row)
	{
		pixel.y = static_cast<float>(row);

		for (auto col = static_cast<uint32_t>(std::ceilf(topLeft.x)); col < static_cast<uint32_t>(std::ceilf(bottomRight.x)); ++col)
		{
			pixel.x = static_cast<float>(col);

			RGBColor finalColor{};
			if (PixelHit(pixel, finalColor, triangleVertices))
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

bool TriangleMesh::PixelHit(Elite::FPoint3& pixel, RGBColor& finalColor, std::vector<Vertex>& vertices) const
{
	// Hit check
	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel.xy - vertices[0].position.xy };
	FVector3 edge{ vertices[1].position - vertices[0].position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeA
		return false;

	pixelToVertex = pixel.xy - vertices[1].position.xy;
	edge = { vertices[2].position - vertices[1].position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeB
		return false;

	pixelToVertex = pixel.xy - vertices[2].position.xy;
	edge = { vertices[0].position - vertices[2].position };
	if (Cross(edge.xy, pixelToVertex) > 0.f) // edgeC
		return false;

	// Triangle has been hit at this point

	CalculateBarycentricWeights(pixel.xy, vertices[0], vertices[1], vertices[2]);

	// Depth test
	const float pixelDepth{ vertices[0].position.z * vertices[0].weight + 
							vertices[1].position.z * vertices[1].weight + 
							vertices[2].position.z * vertices[2].weight };
	pixel.z = pixelDepth;

	finalColor = vertices[0].color * vertices[0].weight + 
				vertices[1].color * vertices[1].weight + 
				vertices[2].color * vertices[2].weight;

	return true;
}
