#pragma once
#include <iostream>
#include <vector>
#include <tuple>
#include "EMath.h"
#include "MathHelper.h"
#include "Vertex.h"

inline void TransformVertexPositions(const Elite::FMatrix4& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.position = transform * vertex.position;
	}
}

inline void TransformVertexNormals(const Elite::FMatrix3& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.normal = transform * vertex.normal;
	}
}

inline void TransformVertexTangents(const Elite::FMatrix3& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.tangent = transform * vertex.tangent;
	}
}

inline void ApplyPerspectiveDivide(std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.x /= vertex.position.w;
		vertex.position.y /= vertex.position.w;
		vertex.position.z /= vertex.position.w;
	}
}

inline void VerticesToScreenSpace(const int width, const int height, std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.xy = Elite::FPoint2{ CalculateScreenSpaceX(vertex.position.x, width), CalculateScreenSpaceY(vertex.position.y, height) };
	}
	
}

inline std::tuple<Elite::FPoint2, Elite::FPoint2> GetBoundingBox(float width, float height, const std::vector<Vertex>& vertices)
{
	if (vertices.empty())
		return std::make_tuple(Elite::FPoint2{}, Elite::FPoint2{});

	FPoint2 topLeft{ vertices[0].position.xy };
	FPoint2 bottomRight{ vertices[0].position.xy };

	for (unsigned int i{1}; i < vertices.size(); ++i)
	{
		topLeft.x = std::min(topLeft.x, vertices[i].position.x);
		topLeft.y = std::min(topLeft.y, vertices[i].position.y);

		bottomRight.x = std::max(bottomRight.x, vertices[i].position.x);
		bottomRight.y = std::max(bottomRight.y, vertices[i].position.y);
	}

	LimitPointToScreenBoundaries(topLeft, width, height);
	LimitPointToScreenBoundaries(bottomRight, width, height);

	return std::make_tuple(topLeft, bottomRight);
}
