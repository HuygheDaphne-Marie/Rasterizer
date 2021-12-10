//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "SceneManager.h"
#include "MathHelper.h"

Renderer::Renderer(SDL_Window* pWindow)
	: m_Texture("Resources/vehicle_diffuse.png")
	, m_NormalMap("Resources/vehicle_normal.png")
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

	for (const Geometry* geometry : activeScene.GetGeometries())
	{
		std::vector<Vertex> geometryVertices{ geometry->GetModelVertices() };
		geometry->Project(geometryVertices);

		std::vector<Vertex> outVertices{};
		geometry->Rasterize(geometryVertices, m_DepthBuffer, outVertices);

		for (const Vertex& vertex : outVertices)
		{
			RGBColor finalPixelColor;
			if (m_RenderDepthBuffer)
			{
				finalPixelColor = RGBColor{ vertex.position.z, vertex.position.z , vertex.position.z };
			}
			else
			{
				// PixelShading is broken for now Todo: fix it
				//finalPixelColor = PixelShading(vertex);
				finalPixelColor = m_Texture.Sample(vertex.uv);
			}

			m_pBackBufferPixels
			[
				PixelToBufferIndex
				(
					static_cast<unsigned int>(roundf(vertex.position.x)),
					static_cast<unsigned int>(roundf(vertex.position.y)),
					m_Width
				)
			] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<Uint8>(finalPixelColor.r * 255.f),
				static_cast<Uint8>(finalPixelColor.g * 255.f),
				static_cast<Uint8>(finalPixelColor.b * 255.f));
		}
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

RGBColor Renderer::PixelShading(const Vertex& outVertex) const
{
	// WIP, broken

	// Light constants
	const FVector3 lightDirection{ .577f, -.577f, -.577f };
	const RGBColor lightColor{ 1.f, 1.f, 1.f };
	constexpr float intensity{ 2.f };

	// Normal Mapping
	//const FVector3 binormal{ Cross(outVertex.tangent, outVertex.normal) };
	//const FMatrix3 tangentSpaceAxis{ outVertex.tangent, binormal, outVertex.normal };
	//
	//RGBColor sampledValue{ m_NormalMap.Sample(outVertex.uv) };
	//sampledValue /= 255.f;
	//sampledValue.r = 2.f * sampledValue.r - 1.f;
	//sampledValue.g = 2.f * sampledValue.g - 1.f;
	//sampledValue.b = 2.f * sampledValue.b - 1.f;
	//FVector3 newNormal{ sampledValue.r, sampledValue.g, sampledValue.b };
	//newNormal = tangentSpaceAxis * newNormal;
	//
	//const float observedArea{ Dot(-newNormal, lightDirection) };

	// Normal mapping seems to have an issue *somewhere*, below is the old method as a temporary fix

	const float observedArea{ Dot(-outVertex.normal, lightDirection) };

	// Diffuse map color
	// Todo: lambert
	const RGBColor diffuseColor{ m_Texture.Sample(outVertex.uv) };

	// Todo: Specular
	// Todo: Ambient

	return lightColor * intensity * diffuseColor * observedArea;
}

void Renderer::ToggleRenderDepthBuffer()
{
	m_RenderDepthBuffer = !m_RenderDepthBuffer;
}
