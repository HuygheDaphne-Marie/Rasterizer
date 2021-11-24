#pragma once
#include <vector>
#include <tuple>
#include "EMath.h"
#include "MathHelper.h"
#include "Vertex.h"

inline std::vector<Vertex> TransformVertices(const Elite::FMatrix4& transform, const std::vector<Vertex>& verticesToTransform)
{
	std::vector<Vertex> output{};
	for (const Vertex& vertex : verticesToTransform)
	{
		output.push_back(vertex);
		output[output.size() - 1].position = transform * vertex.position;
	}
	return output;
}

inline void ApplyCameraCorrection(float fov, float aspectRatio, std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.x /= aspectRatio * fov;
		vertex.position.y /= fov;
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

inline void CalculateBarycentricWeights(const FPoint2& pixel, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2)
{
	const float area{ Cross(FVector2{vertex0.position.xy - vertex1.position.xy}, FVector2{vertex0.position.xy - vertex2.position.xy}) };

	// swapped compared to slides so sign is positive
	vertex0.weight = Cross(FVector2{ vertex2.position.xy - vertex1.position.xy }, FVector2{ pixel - vertex1.position.xy }) / area;
	vertex1.weight = Cross(FVector2{ vertex0.position.xy - vertex2.position.xy }, FVector2{ pixel - vertex2.position.xy }) / area;
	vertex2.weight = Cross(FVector2{ vertex1.position.xy - vertex0.position.xy }, FVector2{ pixel - vertex0.position.xy }) / area;
}