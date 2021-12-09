#include "TriangleMesh.h"
#include "SceneManager.h"
#include "TransformationHelper.h"
#include <tuple>
#include <array>

TriangleMesh::TriangleMesh(const FPoint3& position, const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, PrimitiveTopology topology)
	: Geometry(position)
	, m_ModelVertices(vertices)
	, m_Indices(indices)
	, m_Topology(topology)
	, m_Texture("Resources/vehicle_diffuse.png")
	, m_NormalMap("Resources/vehicle_normal.png")
{
	m_WorldVertices.insert(m_WorldVertices.end(), m_ModelVertices.begin(), m_ModelVertices.end());
	RecalculateWorldVertices();
}

void TriangleMesh::Hit(const RenderInfo& renderInfo) const
{
	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	const Camera* pCamera{ activeScene.GetCamera() };

	// projection
	std::vector<Vertex> transformedVertices{ TransformVertexPositions(pCamera->GetProjection() * pCamera->GetWorldToView(), m_WorldVertices) };
	TransformVertexNormals(GetTransform(), transformedVertices);
	TransformVertexTangents(GetTransform(), transformedVertices);
	ApplyPerspectiveDivide(transformedVertices);
	// vertices are now in NDC

	// Index loop
	unsigned int maxIndex{};
	switch (m_Topology)
	{
	case PrimitiveTopology::TriangleList:
		maxIndex = static_cast<unsigned int>(m_Indices.size()) / 3;
		break;
	case PrimitiveTopology::TriangleStrip:
		maxIndex = static_cast<unsigned int>(m_Indices.size()) - 2;
		break;
	}
	for (unsigned int i{0}; i < maxIndex; ++i)
	{
		std::vector<Vertex> transformedTriangleVertices{ GetTriangleVertices(i, transformedVertices) };
		TriangleHit(renderInfo, transformedTriangleVertices);
	}

}

// The Great Rework
void TriangleMesh::Project(std::vector<Vertex>& vertices) const
{
	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	const Camera* pCamera{ activeScene.GetCamera() };

	// Positions
	TransformVertexPositionsNoCopy(pCamera->GetProjection() * pCamera->GetWorldToView() * GetTransform(), vertices);
	ApplyPerspectiveDivide(vertices);

	// Normal & Tangent
	TransformVertexNormals(GetTransform(), vertices);
	TransformVertexTangents(GetTransform(), vertices);

	// Todo: View Direction 
}

bool TriangleMesh::Rasterize(std::vector<Vertex>& triangleVertices, Vertex& vertexOut) const
{
	for (const Vertex& vertex : triangleVertices)
	{
		// if vertex doesn't fit in NDC space (outside the view of the screen)
		if (!IsInRange(vertex.position.x, 0.f, 1.f))
			return false;
		if (!IsInRange(vertex.position.y, 0.f, 1.f))
			return false;

		// closer than near plane or further than far plane
		if (!IsInRange(vertex.position.z, 0.f, 1.f))
			return false;
	}

	// Triangle is within the screen now for sure

	// Bring NDC space vertex positions to screenSpace 
	const Camera* pCamera{ SceneManager::GetInstance().GetActiveScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };
	VerticesToScreenSpace(pCamera->GetScreenWidth(), height, triangleVertices);

	// Make bounding box around triangle, so we check the least amount of pixels 
	const std::tuple<FPoint2, FPoint2> points{ GetBoundingBox(static_cast<float>(width), static_cast<float>(height), triangleVertices) };
	const FPoint2 topLeft{ std::get<0>(points) };
	const FPoint2 bottomRight{ std::get<1>(points) };

	return true;
}

void TriangleMesh::RecalculateWorldVertices()
{
	for (unsigned int i{0}; i < m_ModelVertices.size(); ++i)
	{
		m_WorldVertices[i].position = GetTransform() * m_ModelVertices[i].position;
	}
}
void TriangleMesh::OnRecalculateTransform()
{
	RecalculateWorldVertices();
}

std::vector<Vertex> TriangleMesh::GetTriangleVertices(unsigned triangleNumber, const std::vector<Vertex>& vertices) const
{
	std::vector<Vertex> output{};
	switch (m_Topology)
	{
		case PrimitiveTopology::TriangleList:
			{
				const unsigned int firstIndex{ triangleNumber * 3 };
				output.push_back(vertices[m_Indices[firstIndex]]);
				output.push_back(vertices[m_Indices[firstIndex + 1]]);
				output.push_back(vertices[m_Indices[firstIndex + 2]]);
			}
		break;
		case PrimitiveTopology::TriangleStrip:
			output.push_back(vertices[m_Indices[triangleNumber]]);
			if (triangleNumber % 2 == 0) // is even
			{
				output.push_back(vertices[m_Indices[triangleNumber + 1]]);
				output.push_back(vertices[m_Indices[triangleNumber + 2]]);
			}
			else
			{
				output.push_back(vertices[m_Indices[triangleNumber + 2]]);
				output.push_back(vertices[m_Indices[triangleNumber + 1]]);
			}
		break;
	}
	return output;
}

void TriangleMesh::TriangleHit(const RenderInfo& renderInfo, std::vector<Vertex>& triangleVertices) const
{
	// I truly hate this function/setup, it's incredibly flawed
	// my geometry should not be handling this, but how else do I call Hit() on geometry once and color all my pixels
	// if I could redo this I'd make more functions which receive data from vertices and then do
	// 1. projection
	// 2. optimization
	// 3. rasterization
	// then leave the shading for the renderer once it gets the vertex info
	// Todo: IMPORTANT, see if this can be changed around to work like this

	// frustum culling
	for (const Vertex& vertex : triangleVertices)
	{
		if (vertex.position.z < 0) // closer than near
			return;
		if (vertex.position.z > 1)
			return;
	}

	const Camera* pCamera{ SceneManager::GetInstance().GetActiveScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };

	// rasterization
	VerticesToScreenSpace(pCamera->GetScreenWidth(), height, triangleVertices);

	const std::tuple<FPoint2, FPoint2> points{ GetBoundingBox(static_cast<float>(width), static_cast<float>(height), triangleVertices) };
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
				if (pixel.z < renderInfo.depthBuffer[PixelToBufferIndex(col, row, width)])
				{
					renderInfo.depthBuffer[PixelToBufferIndex(col, row, width)] = pixel.z;

					if (renderInfo.depthBufferRenderMode) // Render depth buffer
					{
						const float depthColorValue{ Remap(pixel.z, 0.985f, 1.f) };
						renderInfo.pBackBufferPixels[PixelToBufferIndex(col, row, width)] = SDL_MapRGB(renderInfo.pBackBuffer->format,
							static_cast<Uint8>(depthColorValue * 255.f),
							static_cast<Uint8>(depthColorValue * 255.f),
							static_cast<Uint8>(depthColorValue * 255.f));
						continue;
					}

					// Interpolation & assigning color
					const float wInterpolated
					{
						1 / 
						(
							1 / triangleVertices[0].position.w * triangleVertices[0].weight +
							1 / triangleVertices[1].position.w * triangleVertices[1].weight +
							1 / triangleVertices[2].position.w * triangleVertices[2].weight
						)
					};
					const FVector2 uvInterpolated
					{
						Interpolate
						(
							std::array<FVector2, 3>{triangleVertices[0].uv, triangleVertices[1].uv, triangleVertices[2].uv},
							std::array<const Vertex*, 3>{&triangleVertices[0],& triangleVertices[1],& triangleVertices[2]},
							wInterpolated
						)
					};
					const FVector3 normalInterpolated
					{
						GetNormalized(
							Interpolate
							(
								std::array<FVector3, 3>{triangleVertices[0].normal, triangleVertices[1].normal, triangleVertices[2].normal},
								std::array<const Vertex*, 3>{&triangleVertices[0],& triangleVertices[1],& triangleVertices[2]},
								wInterpolated
							)
						)
					};
					const FVector3 tangentInterpolated
					{
						GetNormalized(
							Interpolate
							(
								std::array<FVector3, 3>{triangleVertices[0].tangent, triangleVertices[1].tangent, triangleVertices[2].tangent},
								std::array<const Vertex*, 3>{&triangleVertices[0],& triangleVertices[1],& triangleVertices[2]},
								wInterpolated
							)
						)
					};

					// Color
					finalColor = m_Texture.Sample(uvInterpolated);
					finalColor = PixelShading(normalInterpolated, tangentInterpolated, uvInterpolated, finalColor);
					finalColor.MaxToOne();

					renderInfo.pBackBufferPixels[PixelToBufferIndex(col, row, width)] = SDL_MapRGB(renderInfo.pBackBuffer->format,
						static_cast<Uint8>(finalColor.r * 255.f),
						static_cast<Uint8>(finalColor.g * 255.f),
						static_cast<Uint8>(finalColor.b * 255.f));
				}
			}
		}
	}
}

bool TriangleMesh::PixelHit(FPoint3& pixel, RGBColor& finalColor, std::vector<Vertex>& vertices) const
{
	// Hit check
	// crosses point out of the screen cause of right hand rule
	FVector2 pixelToVertex{ pixel.xy - vertices[0].position.xy };
	FVector3 edge{ vertices[1].position.xyz - vertices[0].position.xyz };
	const float crossA = Cross(edge.xy, pixelToVertex);
	if (crossA > 0.f) // edgeA
		return false;

	pixelToVertex = pixel.xy - vertices[1].position.xy;
	edge = vertices[2].position.xyz - vertices[1].position.xyz;
	const float crossB = Cross(edge.xy, pixelToVertex);
	if (crossB > 0.f) // edgeB
		return false;

	pixelToVertex = pixel.xy - vertices[2].position.xy;
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

	//const float totalWeights{ fabs(vertices[0].weight + vertices[1].weight + vertices[2].weight) };
	//if (!AreEqual(totalWeights, 1.0f))
	//{
	//	const float weightError{ totalWeights - 1.0f };
	//	std::cout << "Absolute value of all weights should == 1, difference is: " << std::to_string(weightError) << std::endl;
	//}

	// Non linear depth (Used in depthBuffer)
	const float zInterpolated
	{
		1 / 
		(
			1 / vertices[0].position.z * vertices[0].weight +
			1 / vertices[1].position.z * vertices[1].weight +
			1 / vertices[2].position.z * vertices[2].weight
		)
	};

	pixel.z = zInterpolated;
	// z check
	//if (zInterpolated < 0.f || zInterpolated > 1.0f)
	//{
	//	std::cout << "zInterpolated not in [0, 1] range, value: " << zInterpolated << std::endl;
	//}

	return true;
}

// TODO: IMPORTANT, move this for god sakes
RGBColor TriangleMesh::PixelShading(const FVector3& normal, const FVector3& tangent, const FVector2& uv, const RGBColor& finalColor) const
{
	const FVector3 lightDirection = { .577f, -.577f, -.577f };
	const RGBColor lightColor = { 0.7f,0.7f,0.7f };
	constexpr float intensity{ 10.f };

	const FVector3 binormal{ Cross(tangent, normal) };
	const FMatrix3 tangentSpaceAxis{ tangent, binormal, normal };
	
	RGBColor sampledValue{ m_NormalMap.Sample(uv) };
	sampledValue /= 255.f;
	sampledValue.r = 2.f * sampledValue.r - 1.f;
	sampledValue.g = 2.f * sampledValue.g - 1.f;
	sampledValue.b = 2.f * sampledValue.b - 1.f;
	FVector3 newNormal{sampledValue.r, sampledValue.g, sampledValue.b};
	newNormal = tangentSpaceAxis * newNormal;

	const float observedArea{ Dot(-normal, lightDirection) };

	return lightColor * intensity * finalColor * observedArea;
}
