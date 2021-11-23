#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct Vertex
{
	FPoint3 position{0,0,0};
	RGBColor color{1,1,1};
	FVector2 uv{};
	float weight{0}; // barycentric
};
