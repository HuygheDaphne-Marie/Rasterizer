//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "SceneManager.h"
#include "MathHelper.h"

Renderer::Renderer(SDL_Window * pWindow)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);

	m_DepthBuffer.resize(m_Width * m_Height, 1.0f);
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };

	// Clear buffer, could this be done better?
	for (uint32_t row = 0; row < m_Height; ++row)
	{
		for (uint32_t col = 0; col < m_Width; ++col)
		{
			m_pBackBufferPixels[PixelToBufferIndex(col, row, m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				0,
				0,
				0);
		}
	}

	const RenderInfo renderInfo{ m_DepthBuffer, m_pBackBuffer, m_pBackBufferPixels, m_RenderDepthBuffer};
	for (const Geometry* geometry : activeScene.GetGeometries())
	{
		geometry->Hit(renderInfo);
	}

	std::fill(m_DepthBuffer.begin(), m_DepthBuffer.end(), 1.0f);
	
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Renderer::ToggleRenderDepthBuffer()
{
	m_RenderDepthBuffer = !m_RenderDepthBuffer;
}
