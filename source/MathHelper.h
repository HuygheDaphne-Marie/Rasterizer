#pragma once
#include "EMath.h"

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

inline unsigned int PixelToBufferIndex(int col, int row, int screenWidth)
{
	return col + (row * screenWidth);
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