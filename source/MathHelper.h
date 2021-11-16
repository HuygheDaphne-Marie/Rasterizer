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