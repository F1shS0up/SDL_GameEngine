#include "Game.h"
#include "Entity.h"
#include <iostream>
#include <memory>
#include "Registry.h"
#include <chrono>
#include <random>
#include <SDL_ttf.h>

#define HandleError() \
	std::cout << SDL_GetError() << std::endl; \
	isRunning = false;

const int rectangleLineThickness = 10;
const SDL_Rect rect = { 920 / rectangleLineThickness, 80 / rectangleLineThickness , 2000 / rectangleLineThickness , 2000 / rectangleLineThickness };
const float gravityStrength = 1000;

Game::Game()
{
}
Game::~Game()
{
}

void Game::Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen)
{
	InitSDL(fullscreen, title, windowSize);

	Entity line1 = EntityManager::Instance()->CreateEntity();
	Entity line2 = EntityManager::Instance()->CreateEntity();
	Entity line3 = EntityManager::Instance()->CreateEntity();
	Entity line4 = EntityManager::Instance()->CreateEntity();
	Entity line5 = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->lineColliders[line2] = LineCollider_Component{ Vector2D(920, 80), Vector2D(920, 2080) };
	Registry::Instance()->lineColliders[line1] = LineCollider_Component{ Vector2D(2920, 2080), Vector2D(2920, 80) };
	Registry::Instance()->lineColliders[line3] = LineCollider_Component{ Vector2D(2920, 80), Vector2D(920, 80) };
	Registry::Instance()->lineColliders[line4] = LineCollider_Component{ Vector2D(920, 2080), Vector2D(2920, 2080) };

	/*Entity fluidSim = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->fluids[fluidSim] = Fluid_Component{ Vector2D(1920, 1080), Vector2D(0, gravityStrength), SDL_Rect{920, 80, 2000, 2000}, 27, 300, 80, 2, 20};*/

	//Entity square = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->filledRectangles[square] = FilledRectangle_Component{ 100, 100, {255, 255, 255, 1} };
	//Registry::Instance()->transforms[square] = Transform_Component{ Vector2D(2000, 1000) };
	//Registry::Instance()->rigidbodies[square] = Rigidbody_Component{ Vector2D(200, -400), Vector2D(0, gravityStrength) , 100 };
	//Registry::Instance()->AABBColliders[square] = AABBCollider_Component{ 100, 100 };

	//Entity square2 = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->filledRectangles[square2] = FilledRectangle_Component{ 200, 200, {255, 255, 255, 1} };
	//Registry::Instance()->transforms[square2] = Transform_Component{ Vector2D(1400, 200) };
	//Registry::Instance()->rigidbodies[square2] = Rigidbody_Component{ Vector2D(-500, 600), Vector2D(0, gravityStrength) , 200 };
	//Registry::Instance()->AABBColliders[square2] = AABBCollider_Component{ 200, 200 };

	Entity square = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->texts[square] = Text_Component{ "Hi", "consola.ttf", 300, SDL_Color{255, 255, 255, 1} };
	inputManager = InputManager::Instance();

	SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);

	Registry::Instance()->Init(renderer);
}

void Game::InitSDL(bool fullscreen, const char* title, SDL_Rect& windowSize)
{
	int flags = 0;
	isRunning = true;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
		flags += SDL_WINDOW_ALLOW_HIGHDPI;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Sdl initialized!" << std::endl;

		window = SDL_CreateWindow(title, windowSize.x, windowSize.y, windowSize.w, windowSize.h, flags);
		if (window != nullptr)
		{
			std::cout << "Window created! Size: " << windowSize.w << windowSize.h << std::endl;
		}
		else
		{
			HandleError();
		}

		SDL_SetWindowSize(window, windowSize.w, windowSize.h);

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (window != nullptr)
		{
			std::cout << "Renderer created!" << std::endl;
		}
		else
		{
			HandleError();
		}
	}
	else
	{
		HandleError();
	}

	if (TTF_Init() < 0)
	{
		std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
	}
}

#pragma region UpdateEveryFrame

void Game::HandleEvents()
{
	SDL_Event evt;
	SDL_PollEvent(&evt);
	switch (evt.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		switch (evt.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			isRunning = false;
		}
	default:
		break;
	}
}
void Game::Update(double* deltaTime)
{
	Registry::Instance()->Update(deltaTime);
}
void Game::Render()
{
	SDL_RenderClear(renderer);

	SDL_RenderSetScale(renderer, rectangleLineThickness, rectangleLineThickness);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_RenderSetScale(renderer, 1, 1);

	Registry::Instance()->Draw(renderer);
	SDL_RenderPresent(renderer);
}

#pragma endregion

void Game::Clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}