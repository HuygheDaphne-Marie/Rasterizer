#include "TriangleMesh.h"
#include "SceneManager.h"
#include "TransformationHelper.h"
#include <tuple>
#include <array>

#include "Triangle.h"

TriangleMesh::TriangleMesh(const FPoint3& position, const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, PrimitiveTopology topology)
	: Geometry(position)
	, m_ModelVertices(vertices)
	, m_Indices(indices)
	, m_Topology(topology)
{
	m_WorldVertices.insert(m_WorldVertices.end(), m_ModelVertices.begin(), m_ModelVertices.end());
	RecalculateWorldVertices();
}

std::vector<Vertex> TriangleMesh::GetModelVertices() const
{
	return m_ModelVertices;
}

void TriangleMesh::Project(std::vector<Vertex>& vertices) const
{
	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };
	const Camera* pCamera{ activeScene.GetCamera() };

	// Positions
	TransformVertexPositions(pCamera->GetProjection() * pCamera->GetWorldToView() * GetTransform(), vertices);
	ApplyPerspectiveDivide(vertices);
	// Vertices now in NDC space

	// Normal & Tangent
	TransformVertexNormals(GetTransform(), vertices);
	TransformVertexTangents(GetTransform(), vertices);

	// Todo: View Direction 
}
bool TriangleMesh::Rasterize(std::vector<Vertex>& vertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const
{
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

	for (unsigned int i{ 0 }; i < maxIndex; ++i)
	{
		std::vector<Vertex> triangleVertices{ GetTriangleVertices(i, vertices) };
		RasterizeSingleTriangle(triangleVertices, depthBuffer, outVertices);
	}

	return !outVertices.empty();
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

bool TriangleMesh::RasterizeSingleTriangle(std::vector<Vertex>& triangleVertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const
{
	for (const Vertex& vertex : triangleVertices)
	{
		// This has been disabled, so that triangles which only have a tiny part of them outside the screen, still get rendered
		// cause if one vertex is out of the screen, the whole triangle is culled
		
		// if vertex doesn't fit in NDC space (outside the view of the screen)
		//if (!IsInRange(vertex.position.x, -1.f, 1.f))
		//	return false;
		//if (!IsInRange(vertex.position.y, -1.f, 1.f))
		//	return false;

		// closer than near plane or further than far plane
		if (!IsInRange(vertex.position.z, 0.f, 1.f))
			return false;
	}

	// Triangle is within the screen now for sure

	// Bring NDC space vertex positions to screenSpace 
	const Camera* pCamera{ SceneManager::GetInstance().GetActiveScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };
	VerticesToScreenSpace(width, height, triangleVertices);

	// Make bounding box around triangle, so we check the least amount of pixels 
	const std::tuple<FPoint2, FPoint2> points{ GetBoundingBox(static_cast<float>(width), static_cast<float>(height), triangleVertices) };
	const FPoint2 topLeft{ std::get<0>(points) };
	const FPoint2 bottomRight{ std::get<1>(points) };

	FPoint2 pixel{};
	for (auto row = static_cast<uint32_t>(std::ceilf(topLeft.y)); row < static_cast<uint32_t>(std::ceilf(bottomRight.y)); ++row)
	{
		pixel.y = static_cast<float>(row);

		for (auto col = static_cast<uint32_t>(std::ceilf(topLeft.x)); col < static_cast<uint32_t>(std::ceilf(bottomRight.x)); ++col)
		{
			pixel.x = static_cast<float>(col);
			if (Triangle::Hit(pixel, triangleVertices))
			{
				const float zInterpolated
				{
					1 /
					(
						1 / triangleVertices[0].position.z * triangleVertices[0].weight +
						1 / triangleVertices[1].position.z * triangleVertices[1].weight +
						1 / triangleVertices[2].position.z * triangleVertices[2].weight
					)
				};

				if (zInterpolated > depthBuffer[PixelToBufferIndex(col, row, width)])
					continue; // our pixel is not the closest, so we don't have an out (at least not a meaningful one without opacity)

				depthBuffer[PixelToBufferIndex(col, row, width)] = zInterpolated;

				const float wInterpolated // Made here cause of frequent use
				{
					1 /
					(
						1 / triangleVertices[0].position.w * triangleVertices[0].weight +
						1 / triangleVertices[1].position.w * triangleVertices[1].weight +
						1 / triangleVertices[2].position.w * triangleVertices[2].weight
					)

				};

				Vertex outVertex{};
				// Depth test success, we have an outVertex
				outVertex.position.x = pixel.x;
				outVertex.position.y = pixel.y;
				outVertex.position.z = zInterpolated;
				outVertex.position.w = wInterpolated;

				// Attribute interpolation
				const std::array<const Vertex*, 3> triangleVertexPointerArray{ &triangleVertices[0],&triangleVertices[1],&triangleVertices[2] };
				outVertex.uv = Interpolate
				(
					std::array<FVector2, 3>{triangleVertices[0].uv, triangleVertices[1].uv, triangleVertices[2].uv},
					triangleVertexPointerArray,
					wInterpolated
				);
				outVertex.color = Interpolate
				(
					std::array<RGBColor, 3>{triangleVertices[0].color, triangleVertices[1].color, triangleVertices[2].color},
					triangleVertexPointerArray,
					wInterpolated
				);
				outVertex.normal = Interpolate
				(
					std::array<FVector3, 3>{triangleVertices[0].normal, triangleVertices[1].normal, triangleVertices[2].normal},
					triangleVertexPointerArray,
					wInterpolated
				);
				outVertex.tangent = Interpolate
				(
					std::array<FVector3, 3>{triangleVertices[0].tangent, triangleVertices[1].tangent, triangleVertices[2].tangent},
					triangleVertexPointerArray,
					wInterpolated
				);
				// Todo: interpolate viewDirection

				outVertices.push_back(outVertex);
			}
		}
	}

	return !outVertices.empty();
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