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

	// Set up Scenes
	{
		SceneGraph& scene{ sceneManager.GetActiveScene() };
		scene.SetCamera(new Camera(width, height));

		// Week 1
		//{
		//	const FPoint3 v0{ 0.f, 2.0f, 0.0f };
		//	const FPoint3 v1{ -1.0f, 0.0f, 0.0f };
		//	const FPoint3 v2{ 1.0f, 0.0f, 0.0f };
		//	scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
		//		v0, RGBColor{ 1,1,1 },
		//		v1, RGBColor{ 1,1,1 },
		//		v2, RGBColor{ 1,1,1 }));
		//}
		//{
		//	const FPoint3 v0{0.f, 4.0f, -2.0f};
		//	const FPoint3 v1{-3.0f, -2.0f, -2.0f};
		//	const FPoint3 v2{3.0f, -2.0f, -2.0f};
		//	scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
		//		v0, RGBColor{ 1,0,0 },
		//		v1, RGBColor{ 0,1,0 },
		//		v2, RGBColor{ 0,0,1 }));
		//}

		// Week 2
		{
			const FPoint3 v0{-3, 3, -2};
			const FPoint3 v1{0, 3, -2};
			const FPoint3 v2{3, 3, -2};
			const FPoint3 v3{-3, 0, -2};
			const FPoint3 v4{0, 0, -2};
			const FPoint3 v5{3, 0, -2};
			const FPoint3 v6{-3, -3, -2};
			const FPoint3 v7{0, -3, -2};
			const FPoint3 v8{3, -3, -2};

			const RGBColor white{ 1,1,1 };

			std::vector<Vertex> vertices
			{
				{v0, white, FVector2{0,0}},
				{v1, white, FVector2{0.5f, 0}},
				{v2, white, FVector2{1,0}},
				{v3, white, FVector2{0, 0.5f}},
				{v4, white, FVector2{0.5f, 0.5f}},
				{v5, white, FVector2{1, 0.5f}},
				{v6, white, FVector2{0, 1}},
				{v7, white, FVector2{0.5f, 1}},
				{v8, white, FVector2{1, 1}}
			};
			//std::vector<unsigned int> indices
			//{
			//	0,3,1,
			//	3,4,1,
			//	1,4,2,
			//	4,5,2,
			//	3,6,4,
			//	6,7,4,
			//	4,7,5,
			//	7,8,5
			//};

			std::vector<unsigned int> indices
			{
				0,3,1,4,2,5,5,3,3,6,4,7,5,8
			};

			//scene.AddGeometryToScene(new TriangleMesh(FPoint3{ 0,0,0 }, vertices, indices));
			scene.AddGeometryToScene(new TriangleMesh(FPoint3{ 0,0,0 }, vertices, indices, PrimitiveTopology::TriangleStrip));

			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v0, RGBColor{ 1,1,1 },
			//	v3, RGBColor{ 1,1,1 },
			//	v1, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v3, RGBColor{ 1,1,1 },
			//	v4, RGBColor{ 1,1,1 },
			//	v1, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v1, RGBColor{ 1,1,1 },
			//	v4, RGBColor{ 1,1,1 },
			//	v2, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v4, RGBColor{ 1,1,1 },
			//	v5, RGBColor{ 1,1,1 },
			//	v2, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v3, RGBColor{ 1,1,1 },
			//	v6, RGBColor{ 1,1,1 },
			//	v4, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v6, RGBColor{ 1,1,1 },
			//	v7, RGBColor{ 1,1,1 },
			//	v4, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v4, RGBColor{ 1,1,1 },
			//	v7, RGBColor{ 1,1,1 },
			//	v5, RGBColor{ 1,1,1 }));
			//
			//scene.AddGeometryToScene(new Triangle(FPoint3{ 0,0,0 },
			//	v7, RGBColor{ 1,1,1 },
			//	v8, RGBColor{ 1,1,1 },
			//	v5, RGBColor{ 1,1,1 }));
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