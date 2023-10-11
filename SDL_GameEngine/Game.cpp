#include "Game.h"
#include "Entity.h"
#include <iostream>
#include <memory>
#include "Registry.h"
#include <chrono>
#include <random>

#define HandleError() \
	std::cout << SDL_GetError() << std::endl; \
	isRunning = false; 

const int rectangleLineThickness = 10;
const SDL_Rect rect = { 920 / rectangleLineThickness, 80 / rectangleLineThickness , 2000 / rectangleLineThickness , 2000 / rectangleLineThickness };
const float gravityStrength = 0;





Game::Game()
{

}
Game::~Game()
{

}

void Game::Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen)
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
	

	Entity line1 = EntityManager::Instance()->CreateEntity();
	Entity line2 = EntityManager::Instance()->CreateEntity();
	Entity line3 = EntityManager::Instance()->CreateEntity();
	Entity line4 = EntityManager::Instance()->CreateEntity();
	Entity line5 = EntityManager::Instance()->CreateEntity();
	


	Registry::Instance()->lineCollider[line2] = LineCollider_Component{ Vector2D(920, 80), Vector2D(920, 2080) };
	Registry::Instance()->lineCollider[line1] = LineCollider_Component{ Vector2D(2920, 2080), Vector2D(2920, 80) };
	Registry::Instance()->lineCollider[line3] = LineCollider_Component{ Vector2D(2920, 80), Vector2D(920, 80) };
	Registry::Instance()->lineCollider[line4] = LineCollider_Component{ Vector2D(920, 2080), Vector2D(2920, 2080) };
#ifdef StartWithRandom
	int max = 5;
	for (int i = 0; i < max; i++)
	{
		Entity entity = EntityManager::Instance()->CreateEntity();
		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(0, 150); // Guaranteed unbiased
		std::uniform_int_distribution<int> uni2(0, 800); // Guaranteed unbiased
		std::uniform_int_distribution<int> uni3(1000, 2000); // Guaranteed unbiased

		float size = uni(rng);
		Vector2D rngDir = Vector2D(uni2(rng), uni2(rng));
		Vector2D rngPos = Vector2D(uni3(rng), uni3(rng));

		unsigned int temp = 10101; // seed - this seeds the random number generator

		temp = temp * (1 + 1 + 1); // this performs a multiplication with the number itself and y
		// which is incremented upon each loop cycle
		temp = (temp ^ 0xffffff) >> 2; // this reduces the generated random number
		// in order it to be less than to 2 ^ 2
		// perhaps it interprets `temp` as an RGB 3-byte color value

		Registry::Instance()->circlesFilled[entity] = CircleFilled_Component{ (int)size, {255, 255, 255, 1} };
		Registry::Instance()->rigidbodies[entity] = Rigidbody_Component{ rngDir, Vector2D(0, gravityStrength),  size * 5 };
		Registry::Instance()->transforms[entity] = Transform_Component{ rngPos };
		Registry::Instance()->circleCollider[entity] = CircleCollider_Component{ size };
	}

#else
	Entity entity = EntityManager::Instance()->CreateEntity();
	Entity entity2 = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->circlesFilled[entity] = CircleFilled_Component{ 50, {255, 255, 255, 1} };
	Registry::Instance()->rigidbodies[entity] = Rigidbody_Component{ Vector2D(500, 500), Vector2D(0, gravityStrength) , 50 };
	Registry::Instance()->transforms[entity] = Transform_Component{ Vector2D(1000, 900) };
	Registry::Instance()->circleCollider[entity] = CircleCollider_Component{ 50 };

	Registry::Instance()->circlesFilled[entity2] = CircleFilled_Component{ 100, {255, 255, 255, 1} };
	Registry::Instance()->rigidbodies[entity2] = Rigidbody_Component{ Vector2D(200, 500), Vector2D(0, gravityStrength),  100 };
	Registry::Instance()->transforms[entity2] = Transform_Component{ Vector2D(2000, 700) };
	Registry::Instance()->circleCollider[entity2] = CircleCollider_Component{ 100 };
#endif// StartWithRandom





	inputManager = InputManager::Instance();

	SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);

	Registry::Instance()->Init();
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