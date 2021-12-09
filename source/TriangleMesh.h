#pragma once
#include <vector>
#include "Geometry.h"
#include "Vertex.h"
#include "Texture.h"

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

class TriangleMesh final : public Geometry
{
public:
	TriangleMesh(const FPoint3& position, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
		PrimitiveTopology topology = PrimitiveTopology::TriangleList);
	~TriangleMesh() override = default;

	void Hit(const RenderInfo& renderInfo) const override;

private:
	const std::vector<Vertex> m_ModelVertices;
	std::vector<Vertex> m_WorldVertices;
	std::vector<unsigned> m_Indices;

	PrimitiveTopology m_Topology;

	// Dirty texture add (Temp) Todo: remove
	Texture m_Texture;
	Texture m_NormalMap;

	void RecalculateWorldVertices();
	void OnRecalculateTransform() override;

	std::vector<Vertex> GetTriangleVertices(unsigned int triangleNumber, const std::vector<Vertex>& vertices) const;

	void TriangleHit(const RenderInfo& renderInfo, std::vector<Vertex>& triangleVertices) const;
	bool PixelHit(Elite::FPoint3& pixel, RGBColor& finalColor, std::vector<Vertex>& vertices) const;

	RGBColor PixelShading(const FVector3& normal, const FVector3& tangent, const FVector2& uv, const RGBColor& finalColor) const;
};
