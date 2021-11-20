//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "SceneManager.h"

Elite::Renderer::Renderer(SDL_Window * pWindow)
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
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };

	for (const Geometry* geometry : activeScene.GetGeometries())
	{
		//Loop over all the pixels
		FPoint2 pixel{};

		for (uint32_t row = 0; row < m_Height; ++row)
		{
			pixel.y = static_cast<float>(row);

			for (uint32_t col = 0; col < m_Width; ++col)
			{
				pixel.x = static_cast<float>(col);

				RGBColor finalColor{};
				if (geometry->Hit(pixel, finalColor))
				{
					m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<Uint8>(finalColor.r * 255.f),
						static_cast<Uint8>(finalColor.g * 255.f),
						static_cast<Uint8>(finalColor.b * 255.f));
				}
				else
				{
					m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						0,
						0,
						0);
				}
			}
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}
