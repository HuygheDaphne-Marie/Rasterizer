#pragma once
#include "Geometry.h"
#include "Vertex.h"

class Triangle final : public Geometry
{
public:
	Triangle(const FPoint3& position, 
		const FPoint3& v0, const RGBColor& c0, 
		const FPoint3& v1, const RGBColor& c1, 
		const FPoint3& v2, const RGBColor& c2);

	bool Hit(FPoint3& pixel, RGBColor& finalColor) const override;

private:
	const FPoint3 m_ModelVertex0;
	const FPoint3 m_ModelVertex1;
	const FPoint3 m_ModelVertex2;
	const RGBColor m_Color0;
	const RGBColor m_Color1;
	const RGBColor m_Color2;

	FPoint3 m_WorldVertex0{};
	FPoint3 m_WorldVertex1{};
	FPoint3 m_WorldVertex2{};

	void RecalculateWorldVertices();

	void OnRecalculateTransform() override;

	void ApplyCameraCorrection(float fov, float aspectRatio, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const; // Make math helper function? (cause they're possible statics)
	void ApplyPerspectiveDivide(Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const; // Make math helper function?
	void CalculateBarycentricWeights(const FPoint2& pixel, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const;
};

