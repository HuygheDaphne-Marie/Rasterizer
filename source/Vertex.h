#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct Vertex
{
	FPoint3 position;
	RGBColor color;
	float weight; // barycentric
};
