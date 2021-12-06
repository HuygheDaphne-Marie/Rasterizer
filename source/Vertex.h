#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct Vertex
{
	FPoint4 position{0,0,0, 0};
	RGBColor color{1,1,1};
	FVector2 uv{};
	FVector3 normal{};
	float weight{0}; // barycentric
};
