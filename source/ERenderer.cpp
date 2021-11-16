//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"

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
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			//Fill the pixels - pixel access demo
			float cw = (255.0f / m_Width) * c;
			float ch = (255.0f / m_Height) * r;
			uint8_t uColor = static_cast<uint8_t>((cw + ch) / 2.0f);
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(uColor),
				static_cast<uint8_t>(uColor),
				static_cast<uint8_t>(uColor));
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
