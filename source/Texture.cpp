#include "Texture.h"

#include "MathHelper.h"

Texture::Texture(const std::string& path)
{
	m_pTexture = IMG_Load(path.c_str());
}

Texture::~Texture()
{
	SDL_FreeSurface(m_pTexture);
	m_pTexture = nullptr;
}

Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) const
{
	Uint8 r, g, b;
	SDL_GetRGB(
		static_cast<Uint32*>(m_pTexture->pixels)
		[
			PixelToBufferIndex
			(
				static_cast<int>(uv.x * static_cast<float>(m_pTexture->w)),
				static_cast<int>(uv.y * static_cast<float>(m_pTexture->h)),
				m_pTexture->w)
		], 
		m_pTexture->format,
		&r, &g, &b);
	return Elite::RGBColor{static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f };
}
