#pragma once
#include "EMath.h"
#include "ERGBColor.h"

using namespace Elite;

class Geometry
{
public:
	Geometry(const FPoint3& position, const RGBColor& color);
	virtual ~Geometry() = default;

	virtual bool Hit(const FPoint2& pixel) const = 0;

private:
	FMatrix4 m_Transform;
	RGBColor m_Color; // Todo: remove this
};

