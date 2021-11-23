#pragma once
#include <vector>
#include "Geometry.h"
#include "Vertex.h"

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

	void Hit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels) const override;



private:
	const std::vector<Vertex> m_ModelVertices;
	std::vector<Vertex> m_WorldVertices;
	std::vector<unsigned> m_Indices;

	PrimitiveTopology m_Topology;

	void RecalculateWorldVertices();
	void OnRecalculateTransform() override;

	std::vector<Vertex> GetTriangleVertices(unsigned int triangleNumber, const std::vector<Vertex>& vertices) const;

	void TriangleHit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, std::vector<Vertex>& triangleVertices) const;
	bool PixelHit(Elite::FPoint3& pixel, RGBColor& finalColor, std::vector<Vertex>& vertices) const;
};
