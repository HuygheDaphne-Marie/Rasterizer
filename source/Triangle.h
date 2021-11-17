#pragma once
#include "Geometry.h"
class Triangle final : public Geometry
{
public:
	Triangle(const FPoint3& position, const FPoint3& v0, const FPoint3& v1, const FPoint3& v2);

	bool Hit(const FPoint2& pixel, const FMatrix4& worldToView) const override;

private:
	const FPoint3 m_ModelVertex0;
	const FPoint3 m_ModelVertex1;
	const FPoint3 m_ModelVertex2;

	FPoint3 m_WorldVertex0{};
	FPoint3 m_WorldVertex1{};
	FPoint3 m_WorldVertex2{};

	void RecalculateWorldVertices();

	void OnRecalculateTransform() override;
};

