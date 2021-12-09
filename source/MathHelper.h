#pragma once
#include "EMath.h"
#include <array>
#include "Vertex.h"

inline float NDCSpaceXToScreenSpaceX(float x, float screenWidth)
{
	return (x + 1) / 2 * screenWidth;
}

inline float NDCSpaceYToScreenSpaceY(float y, float screenHeight)
{
	return (1 - y) / 2 * screenHeight;
}

inline Elite::FPoint3 NDCPointToScreenSpace(const Elite::FPoint3& NDCPoint, float screenWidth, float screenHeight)
{
	Elite::FPoint3 screenSpacePoint{NDCPoint};
	screenSpacePoint.x = NDCSpaceXToScreenSpaceX(NDCPoint.x, screenWidth);
	screenSpacePoint.y = NDCSpaceYToScreenSpaceY(NDCPoint.y, screenHeight);
	return screenSpacePoint;
}

inline float CalculateScreenSpaceX(const float x, const int screenWidth) // x
{
	return (x + 1) / 2 * screenWidth;
}
inline float CalculateScreenSpaceY(const float y, const int screenHeight) // y
{
	return (1 - y) / 2 * screenHeight;
}

inline unsigned int PixelToBufferIndex(unsigned int x, unsigned int y, unsigned int width)
{
	return x + (y * width);
}

inline void LimitPointToScreenBoundaries(Elite::FPoint2& point, float width, float height)
{
	if (point.x < 0)
		point.x = 0;
	if (point.x >= width)
		point.x = width - 1.f;

	if (point.y < 0)
		point.y = 0;
	if (point.y >= height)
		point.y = height - 1.f;
}

template<typename Type>
inline Type Interpolate(const std::array<Type, 3>& attributesToInterpolate,
	const std::array<const Vertex*, 3>& pOwningVertices)
{
	return
		1 / (
			attributesToInterpolate[0] / pOwningVertices[0]->position.w * pOwningVertices[0]->weight +
			attributesToInterpolate[1] / pOwningVertices[1]->position.w * pOwningVertices[1]->weight +
			attributesToInterpolate[2] / pOwningVertices[2]->position.w * pOwningVertices[2]->weight
		);
}

template<typename Type>
inline Type Interpolate(const std::array<Type, 3>& attributesToInterpolate, 
						const std::array<const Vertex*, 3>& pOwningVertices, 
						const float interpolatedLinearDepth)
{
	return
	(
		attributesToInterpolate[0] / pOwningVertices[0]->position.w * pOwningVertices[0]->weight +
		attributesToInterpolate[1] / pOwningVertices[1]->position.w * pOwningVertices[1]->weight +
		attributesToInterpolate[2] / pOwningVertices[2]->position.w * pOwningVertices[2]->weight
	) * interpolatedLinearDepth;
}



template<typename Type>
inline bool IsInRange(Type value, Type minRange, Type maxRange)
{
	return value >= minRange && value <= maxRange;
}