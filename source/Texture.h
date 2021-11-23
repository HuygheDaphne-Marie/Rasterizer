#pragma once
#include <SDL_image.h>
#include <string>
#include "ERGBColor.h"
#include "EMath.h"

class Texture final
{
public:
	explicit Texture(const std::string& path);
	~Texture();

	Elite::RGBColor Sample(const Elite::FVector2& uv) const;

private:
	SDL_Surface* m_pTexture{nullptr};
};

