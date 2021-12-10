//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"

// Own includes
#include "MathHelper.h"
#include "SceneManager.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "EOBJParser.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Rasterizer - Henri-Thibault Huyghe - 2DAE06",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize own classes
	SceneManager& sceneManager{ SceneManager::GetInstance() };

	//Initialize "framework"
	Timer* pTimer = new Timer();
	Renderer* pRenderer = new Renderer(pWindow);

	// Set up Scene
	{
		SceneGraph& scene{ sceneManager.GetActiveScene() };
		scene.SetCamera(new Camera(width, height));

		{
			std::vector<Vertex> verticesFromFile{};
			std::vector<uint32_t> indices{};
			ParseOBJ("Resources/vehicle.obj", verticesFromFile, indices);

			scene.AddGeometryToScene(new TriangleMesh(FPoint3{ 0,0,0 }, verticesFromFile, indices));
		}
	}
	

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenShot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenShot = true;
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
					pRenderer->ToggleRenderDepthBuffer();
				break;
			}
		}

		sceneManager.GetActiveScene().GetCamera()->Update(pTimer->GetElapsed());

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenShot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenShot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}