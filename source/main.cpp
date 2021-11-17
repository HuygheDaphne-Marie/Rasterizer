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
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);

	// Set up Scenes
	{
		//const FPoint3 v0{ NDCPointToScreenSpace(FPoint3{0.f, 0.5f, -1.0f}, width, height) };
		//const FPoint3 v1{ NDCPointToScreenSpace(FPoint3{-0.5f, -0.5f, -1.0f}, width, height) };
		//const FPoint3 v2{ NDCPointToScreenSpace(FPoint3{0.5f, -0.5f, -1.0f}, width, height) };

		SceneGraph& scene{ sceneManager.GetActiveScene() };
		scene.SetCamera(new Camera(width, height));

		const FPoint3 v0{ FPoint3{0.f, 2.0f, 0.0f} };
		const FPoint3 v1{ FPoint3{-1.0f, 0.0f, 0.0f} };
		const FPoint3 v2{ FPoint3{1.0f, 0.0f, 0.0f} };
		scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 }, v0, v1, v2));
	}
	

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
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
					takeScreenshot = true;
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
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}