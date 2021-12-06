#pragma once
#include <SDL_surface.h>
#include <vector>

struct RenderInfo
{
	std::vector<float>& depthBuffer;
	SDL_Surface* pBackBuffer;
	uint32_t* pBackBufferPixels;
	bool depthBufferRenderMode;
};
