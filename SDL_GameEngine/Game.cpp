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
float* gravity;
float* dragCoeficient;
std::string assetPath = "";

int x;

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

	gravity = new float(0);
	dragCoeficient = new float(0);

	Entity e = EntityManager::Instance()->CreateEntity();

	std::string font = assetPath + "Fonts/consola.ttf";
	Registry::Instance()->sliderBoxes[e] = SliderBox_Component{ "Gravity:",Vector2DInt(50,100), SDL_Color{255, 255, 255, 255}, font.c_str(), 40, 
		SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255}, SDL_Rect{250, 100, 350, 350}, SDL_Rect{240, 90, 360, 50}, true, 0 , 100, -100000, 100000};
	gravity = &Registry::Instance()->sliderBoxes[e].value;

	Entity coef = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->sliderBoxes[coef] = SliderBox_Component{ "Drag:",Vector2DInt(50,200), SDL_Color{255, 255, 255, 255}, font.c_str(), 40,
	SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255}, SDL_Rect{250, 200, 350, 350}, SDL_Rect{240, 190, 360, 50}, true, 0 , 0.0001f, 0, 1};
	dragCoeficient = &Registry::Instance()->sliderBoxes[coef].value;

	//Entity line1 = EntityManager::Instance()->CreateEntity();
	//Entity line2 = EntityManager::Instance()->CreateEntity();
	//Entity line3 = EntityManager::Instance()->CreateEntity();
	//Entity line4 = EntityManager::Instance()->CreateEntity();
	//Entity line5 = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->AABBColliders[line1] = AABBCollider_Component{ 920, 2000 };
	//Registry::Instance()->filledRectangles[line1] = FilledRectangle_Component{ 920, 2000, SDL_Color{255, 255, 255, 1} };
	//Registry::Instance()->transforms[line1] = Transform_Component{ Vector2D(2920, 80) };
	//Registry::Instance()->AABBColliders[line3] = AABBCollider_Component{ 2000, 80 };
	//Registry::Instance()->filledRectangles[line3] = FilledRectangle_Component{ 2000, 80, SDL_Color{255, 255, 255, 1} };
	//Registry::Instance()->transforms[line3] = Transform_Component{ Vector2D(920, 0) };
	//Registry::Instance()->AABBColliders[line4] = AABBCollider_Component{ 2000, 80 };
	//Registry::Instance()->filledRectangles[line4] = FilledRectangle_Component{ 2000, 80, SDL_Color{255, 255, 255, 1} };
	//Registry::Instance()->transforms[line4] = Transform_Component{ Vector2D(920, 2080) };
	Entity softbody = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[softbody] = Softbody_Component{ 16,
		{
			/*MassPoint{Vector2D(1400, 1400), 1}, MassPoint{Vector2D(1450, 1400), 1}, MassPoint{Vector2D(1500, 1400), 1}, MassPoint{Vector2D(1550, 1400), 1}, MassPoint{Vector2D(1600, 1400), 1}, 
			MassPoint{Vector2D(1400, 1450), 1}, MassPoint{Vector2D(1450, 1450), 1}, MassPoint{Vector2D(1500, 1450), 1}, MassPoint{Vector2D(1550, 1450), 1}, MassPoint{Vector2D(1600, 1450), 1},
			MassPoint{Vector2D(1400, 1500), 1}, MassPoint{Vector2D(1450, 1500), 1}, MassPoint{Vector2D(1500, 1500), 1}, MassPoint{Vector2D(1550, 1500), 1}, MassPoint{Vector2D(1600, 1500), 1},
			MassPoint{Vector2D(1400, 1550), 1}, MassPoint{Vector2D(1450, 1550), 1}, MassPoint{Vector2D(1500, 1550), 1}, MassPoint{Vector2D(1550, 1550), 1}, MassPoint{Vector2D(1600, 1550), 1},
			MassPoint{Vector2D(1400, 1600), 1}, MassPoint{Vector2D(1450, 1600), 1}, MassPoint{Vector2D(1500, 1600), 1}, MassPoint{Vector2D(1550, 1600), 1}, MassPoint{Vector2D(1600, 1600), 1}*/
			MassPoint{Vector2D(1400, 1400), 1}, MassPoint{Vector2D(1450, 1400), 1}, MassPoint{Vector2D(1500, 1400), 1}, MassPoint{Vector2D(1550, 1400), 1}, MassPoint{Vector2D(1600, 1400), 1},
			MassPoint{Vector2D(1600, 1450), 1}, MassPoint{Vector2D(1600, 1500), 1},MassPoint{Vector2D(1600, 1550), 1}, MassPoint{Vector2D(1600, 1600), 1},
			MassPoint{Vector2D(1550, 1600), 1}, MassPoint{Vector2D(1500, 1600), 1},MassPoint{Vector2D(1450, 1600), 1}, MassPoint{Vector2D(1400, 1600), 1},
			MassPoint{Vector2D(1400, 1550), 1}, 
			MassPoint{Vector2D(1400, 1500), 1},
			MassPoint{Vector2D(1400, 1450), 1}
		},
		{ },gravity, dragCoeficient, 500, .8 };

	x = softbody;


	
	float d = 0;
	Entity ground = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[ground] = Softbody_Component{ 22,
		{
			MassPoint{Vector2D(920, 1680), 1, true}, MassPoint{Vector2D(1120, 1680), 1}, MassPoint{Vector2D(1320, 1680), 1}, MassPoint{Vector2D(1520, 1680), 1}, MassPoint{Vector2D(1720, 1680), 1}, MassPoint{Vector2D(1920, 1680), 1},
			MassPoint{Vector2D(2120, 1680), 1}, MassPoint{Vector2D(2320, 1680), 1}, MassPoint{Vector2D(2520, 1680), 1}, MassPoint{Vector2D(2720, 1680), 1},MassPoint{Vector2D(2920, 1680), 1, true},
			MassPoint{Vector2D(2920, 1880), 1, true}, MassPoint{Vector2D(2720, 1880), 1}, MassPoint{Vector2D(2520, 1880), 1}, MassPoint{Vector2D(2320, 1880), 1}, MassPoint{Vector2D(2120, 1880), 1}, MassPoint{Vector2D(1920, 1880), 1},
			MassPoint{Vector2D(1720, 1880), 1}, MassPoint{Vector2D(1520, 1880), 1}, MassPoint{Vector2D(1320, 1880), 1}, MassPoint{Vector2D(1120, 1880), 1},MassPoint{Vector2D(920, 1880), 1, true}
		},



		{/*Spring{0, 1, 100, 100, 5}, Spring{1, 2, 100, 100, 5}, Spring{2, 3, 100, 100, 5}, Spring{3, 4, 100, 100, 5}, Spring{4, 5, 100, 100, 5},
		Spring{5, 6, 100, 100, 5}, Spring{6, 7, 100, 100, 5}, Spring{7, 8, 100, 100, 5}, Spring{8, 9, 100, 100, 5}, Spring{9, 10, 100, 100, 5},
		Spring{10, 11, 100, 100, 5}, Spring{11, 12, 100, 100, 5}, Spring{12, 13, 100, 100, 5}, Spring{13, 14, 100, 100, 5}, Spring{14, 15, 100, 100, 5},
		Spring{15, 16, 100, 100, 5}, Spring{16, 17, 100, 100, 5}, Spring{17, 18, 100, 100, 5}, Spring{18, 19, 100, 100, 5}, Spring{19, 20, 100, 100, 5}, Spring{20, 21, 100, 100, 5}, Spring{21, 0, 100, 100, 5}*/},
		gravity, dragCoeficient, 500, 10 };




	/*Entity fluidSim = EntityManager::Instance()->CreateEntity();

	Registry::Instance()->fluids[fluidSim] = Fluid_Component{ Vector2D(1920, 1080), Vector2D(0, gravityStrength), SDL_Rect{920, 80, 2000, 2000}, 27, 300, 80, 2, 20};*/

	//Entity square = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->filledRectangles[square] = FilledRectangle_Component{ 200, 200, {255, 255, 255, 1} };
	//Registry::Instance()->transforms[square] = Transform_Component{ Vector2D(2000, 100) };
	//Registry::Instance()->rigidbodies[square] = Rigidbody_Component{ Vector2D(200, -400), gravity, 200 };
	//Registry::Instance()->AABBColliders[square] = AABBCollider_Component{ 200, 200 };

	//Entity circle = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->filledCircles[circle] = FilledCircle_Component{ 50, {255, 255, 255, 1} };
	//Registry::Instance()->transforms[circle] = Transform_Component{ Vector2D(2000, 200) };
	//Registry::Instance()->rigidbodies[circle] = Rigidbody_Component{ Vector2D(-500, 600), gravity, 50 };
	//Registry::Instance()->circleColliders[circle] = CircleCollider_Component{ 50 };

	//Entity circle2 = EntityManager::Instance()->CreateEntity();

	//Registry::Instance()->filledCircles[circle2] = FilledCircle_Component{ 100, {255, 255, 255, 1} };
	//Registry::Instance()->transforms[circle2] = Transform_Component{ Vector2D(1700, 200) };
	//Registry::Instance()->rigidbodies[circle2] = Rigidbody_Component{ Vector2D(-100, 600), gravity, 100 };
	//Registry::Instance()->circleColliders[circle2] = CircleCollider_Component{ 100 };



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
int massPointI;
void Game::Update(double* deltaTime)
{
	inputManager->Update();
	Registry::Instance()->Update(deltaTime, this);
	if (InputManager::Instance()->MouseButtonPressed(InputManager::right))
	{
		float dist = 0;
		for (int i = 0; i < Registry::Instance()->softbodies[x].massPoints.size(); i++)
		{
			Vector2D dirVec = Vector2D(Registry::Instance()->softbodies[x].massPoints[i].position.x - InputManager::Instance()->MousePos().x, Registry::Instance()->softbodies[x].massPoints[i].position.y - InputManager::Instance()->MousePos().y);
			float currentDist = dirVec.x * dirVec.x + dirVec.y * dirVec.y;
			if (dist > currentDist || i == 0)
			{
				dist = currentDist;
				massPointI = i;
			}
		}
		
		Registry::Instance()->softbodies[x].massPoints[massPointI].Lock();
	}
	else if (InputManager::Instance()->MouseButtonDown(InputManager::right))
	{
		
		Registry::Instance()->softbodies[x].massPoints[massPointI].position = Vector2D(InputManager::Instance()->MousePos().x, InputManager::Instance()->MousePos().y);
	}
	else if (InputManager::Instance()->MouseButtonReleased(InputManager::right))
	{
		
		Registry::Instance()->softbodies[x].massPoints[massPointI].Unlock();
	}
	inputManager->UpdatePrevInput();
}
void Game::Render()
{
	SDL_RenderClear(renderer);

	SDL_RenderSetScale(renderer, 1, 1);

	Registry::Instance()->Draw(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
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
