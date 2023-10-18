#include "Game.h"
#include "Entity.h"
#include <iostream>
#include <memory>
#include "Registry.h"
#include <chrono>
#include <random>
#include <SDL_ttf.h>
#include "SDL_filesystem.h"

#define HandleError() \
	std::cout << SDL_GetError() << std::endl; \
	isRunning = false;

const int rectangleLineThickness = 10;
const SDL_Rect rect = { 920 / rectangleLineThickness, 80 / rectangleLineThickness , 2000 / rectangleLineThickness , 2000 / rectangleLineThickness };
float* gravityY;
float* gravityX;
std::string assetPath = "";

Game::Game()
{
}
Game::~Game()
{
}
TextBox_Component* txt;
void Game::Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen)
{
	InitSDL(fullscreen, title, windowSize);

	assetPath = SDL_GetBasePath();
	assetPath += "Assets/";

	gravityX = new float(0);
	gravityY = new float(0);

	Entity e = EntityManager::Instance()->CreateEntity();

	std::string font = assetPath + "Fonts/consola.ttf";
	Registry::Instance()->sliderBoxes[e] = SliderBox_Component{ "Gravity:",Vector2DInt(50,100), SDL_Color{255, 255, 255, 255}, font.c_str(), 40, 
		SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255}, SDL_Rect{250, 100, 350, 350}, SDL_Rect{240, 90, 360, 50}, true, 3 };
	gravityY = &Registry::Instance()->sliderBoxes[e].value;

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

	Entity square = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->filledRectangles[square] = FilledRectangle_Component{ 200, 200, {255, 255, 255, 1} };
	Registry::Instance()->transforms[square] = Transform_Component{ Vector2D(2000, 1000) };
	Registry::Instance()->rigidbodies[square] = Rigidbody_Component{ Vector2D(200, -400), gravityX, gravityY , 200 };
	Registry::Instance()->AABBColliders[square] = AABBCollider_Component{ 200, 200 };

	Entity circle = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->filledCircles[circle] = FilledCircle_Component{ 50, {255, 255, 255, 1} };
	Registry::Instance()->transforms[circle] = Transform_Component{ Vector2D(2000, 200) };
	Registry::Instance()->rigidbodies[circle] = Rigidbody_Component{ Vector2D(-500, 600), gravityX, gravityY , 50 };
	Registry::Instance()->circleColliders[circle] = CircleCollider_Component{ 50 };

	Entity circle2 = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->filledCircles[circle2] = FilledCircle_Component{ 100, {255, 255, 255, 1} };
	Registry::Instance()->transforms[circle2] = Transform_Component{ Vector2D(1700, 200) };
	Registry::Instance()->rigidbodies[circle2] = Rigidbody_Component{ Vector2D(-100, 600), gravityX, gravityY , 100 };
	Registry::Instance()->circleColliders[circle2] = CircleCollider_Component{ 100 };

	inputManager = InputManager::Instance();

	SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);

	SDL_StopTextInput();
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
	case SDL_TEXTINPUT:
		/* Add new text onto the end of our text */
		
		activeTextInput->append(evt.text.text);
		break;
	case SDL_KEYDOWN:
		switch (evt.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			if (SDL_IsTextInputActive())
			{
				SDL_StopTextInput();
			}
			else
			{
				isRunning = false;
			}
			break;
		case SDLK_BACKSPACE:
			if (SDL_IsTextInputActive())
			{
				activeTextInput->pop_back();
			}
			break;
		}


	default:
		break;
	}
}
void Game::Update(double* deltaTime)
{
	inputManager->Update();
	Registry::Instance()->Update(deltaTime, this);
	inputManager->UpdatePrevInput();
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

void Game::SetTextInputString(std::string* textToPutInputTo)
{
	activeTextInput = textToPutInputTo;
}
