#pragma once
#include "EMath.h"
#include "ERGBColor.h"

using namespace Elite;
struct Vertex
{
	FPoint4 position{};
	RGBColor color{1,1,1};
	FVector2 uv{};
	FVector3 normal{};
	FVector3 tangent{};
	float weight{}; // barycentric
};
