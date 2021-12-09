#pragma once
#include "EMath.h"
#include "ERGBColor.h"

using namespace Elite;
struct Vertex
{
	FPoint4 position{0,0,0, 0};
	RGBColor color{1,1,1};
	FVector2 uv{};
	FVector3 normal{};
	FVector3 tangent{};
	float weight{0}; // barycentric
};
