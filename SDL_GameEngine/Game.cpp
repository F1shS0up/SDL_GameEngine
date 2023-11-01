#include "Game.h"
#include "Entity.h"
#include <iostream>
#include <memory>
#include "Registry.h"
#include <chrono>
#include <random>
#include <SDL_ttf.h>
#include "SDL_filesystem.h"
#include <functional>

#define HandleError() \
	std::cout << SDL_GetError() << std::endl; \
	isRunning = false;

Game::Game()
{
}
Game::~Game()
{
}

void StartStop(Game* game)
{
	game->stopSimulation = !game->stopSimulation;
}

Entity carBody;
void Game::Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen)
{
	InitSDL(fullscreen, title, windowSize);
	w = windowSize.w;
	h = windowSize.h;

	stopSimulation = true;

	assetPath = SDL_GetBasePath();
	assetPath += "Assets/";

	gravity = new double(0);
	dragCoeficient = new double(0);

	std::string font = assetPath + "Fonts/consola.ttf";
	std::string img = assetPath + "Textures/BackGround.png";

	Entity gravityEntity = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->sliderBoxes[gravityEntity] = SliderBox_Component{ "Gravity:",Vector2DInt(50,100), SDL_Color{255, 255, 255, 255}, font.c_str(), 40,
		SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255}, SDL_Rect{250, 100, 350, 350}, SDL_Rect{240, 90, 360, 50}, true, 3500 , 100, -100000, 100000 };
	gravity = &Registry::Instance()->sliderBoxes[gravityEntity].value;

	Entity dragEntity = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->sliderBoxes[dragEntity] = SliderBox_Component{ "Drag:",Vector2DInt(50,200), SDL_Color{255, 255, 255, 255}, font.c_str(), 40,
	SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255}, SDL_Rect{250, 200, 350, 350}, SDL_Rect{240, 190, 360, 50}, true, 0.0001 , 0.0001f, 0, 1 };
	dragCoeficient = &Registry::Instance()->sliderBoxes[dragEntity].value;

	Entity startButtonEntity = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->buttons[startButtonEntity] = Button_Component{ "Start/Stop",  font.c_str() , 40, SDL_Color{255, 255, 255, 255}, SDL_Color{60, 60, 60, 255}, SDL_Color{80, 80, 80, 255}, SDL_Color{150, 150, 150, 255},
		SDL_Rect{250, 300, 350, 350}, SDL_Rect{240, 290, 360, 50}, &StartStop };

	Entity t = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->tiledSprites[t] = TiledSprite_Component{ img.c_str(), 3840, 2560, false };
	Registry::Instance()->transforms[t] = Transform_Component{ };

	//Entity softbody = EntityManager::Instance()->CreateEntity();
	//Registry::Instance()->softbodies[softbody] = Softbody_Component{ 16,
	//	{
	//		MassPoint{Vector2D(1400, 1400), 1}, MassPoint{Vector2D(1450, 1400), 1}, MassPoint{Vector2D(1500, 1400), 1}, MassPoint{Vector2D(1550, 1400), 1}, MassPoint{Vector2D(1600, 1400), 1},
	//		MassPoint{Vector2D(1600, 1450), 1}, MassPoint{Vector2D(1600, 1500), 1},MassPoint{Vector2D(1600, 1550), 1}, MassPoint{Vector2D(1600, 1600), 1},
	//		MassPoint{Vector2D(1550, 1600), 1}, MassPoint{Vector2D(1500, 1600), 1},MassPoint{Vector2D(1450, 1600), 1}, MassPoint{Vector2D(1400, 1600), 1},
	//		MassPoint{Vector2D(1400, 1550), 1},
	//		MassPoint{Vector2D(1400, 1500), 1},
	//		MassPoint{Vector2D(1400, 1450), 1}
	//	},
	//	{ },gravity, dragCoeficient, 250, .5, true, true, 300, 0.8f, 60, 1, A, Layers(),SDL_Color{131, 156, 169, 255} };

	//Entity wheel2 = EntityManager::Instance()->CreateEntity();
	//Registry::Instance()->softbodies[wheel2] = Softbody_Component{ 12,
	//	{
	//		MassPoint{Vector2D(2000, 1000), 1}, MassPoint{Vector2D(2033, 983), 1}, MassPoint{Vector2D(2066, 983), 1},
	//		MassPoint{Vector2D(2100, 1000), 1}, MassPoint{Vector2D(2116, 1033), 1}, MassPoint{Vector2D(2116, 1066), 1},
	//		MassPoint{Vector2D(2100, 1100), 1}, MassPoint{Vector2D(2066, 1116), 1}, MassPoint{Vector2D(2033, 1116), 1},
	//		MassPoint{Vector2D(2000, 1100), 1}, MassPoint{Vector2D(1983, 1066), 1}, MassPoint{Vector2D(1983, 1033), 1}
	//	},
	//	{ },gravity, dragCoeficient, 500, .5, true, true, 100, 0.5f, 60, 50, SDL_Color{200, 126, 126, 255} };

	//Registry::Instance()->softbodyRotatingMovements[wheel2] = SoftbodyRotatingMovement_Component{ &Registry::Instance()->softbodies[wheel2],  new double(5000) };

	carBody = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[carBody] = Softbody_Component{ 14,
		{
			/*MassPoint{Vector2D(1400, 800), 1, 3}, MassPoint{Vector2D(1500, 800), 1, 3}, MassPoint{Vector2D(1600, 800), 1, 3}, MassPoint{Vector2D(1700, 800), 1, 3}, MassPoint{Vector2D(1800, 800), 1, 3},
			MassPoint{Vector2D(1800, 900), 1, 3}, MassPoint{Vector2D(1800, 1000), 1, 3},
			MassPoint{Vector2D(1700, 1000), 1, 3}, MassPoint{Vector2D(1600, 1000), 1, 3},MassPoint{Vector2D(1500, 1000), 1, 3}, MassPoint{Vector2D(1400, 1000), 1, 3},
			MassPoint{Vector2D(1400, 900), 1, 3}*/
			MassPoint{Vector2D(1300, 750), 1, 1},MassPoint{Vector2D(1400, 750), 1, 1}, MassPoint{Vector2D(1400, 680), 1, 1}, MassPoint{Vector2D(1550, 680), 1, 1}, MassPoint{Vector2D(1575, 750), 1, 1}, MassPoint{Vector2D(1700, 750), 1, 1}, MassPoint{Vector2D(1720, 800), 1, 1},
			MassPoint{Vector2D(1720, 820), 1, 1}, MassPoint{Vector2D(1700, 840), 1, 1},  MassPoint{Vector2D(1670, 840), 1, 1}, MassPoint{Vector2D(1540, 840), 1, 1}, MassPoint{Vector2D(1450, 840), 1, 1},  MassPoint{Vector2D(1350, 840), 1, 1}, MassPoint{Vector2D(1300, 840), 1, 1}
		},
		{ },gravity, dragCoeficient, 300, .5, true, true, false, 300, 0.5f, 0, 60, 0, 0.8, B, C, SDL_Color{255, 195, 132, 255} };
	Registry::Instance()->transforms[carBody] = Transform_Component{ Vector2D(0, 0) };
	Registry::Instance()->cameras[carBody] = Camera_Component{ w, h, 100, 200,Vector2D(0, 0) };
	cam = &Registry::Instance()->cameras[carBody];

	Entity wheel1 = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[wheel1] = Softbody_Component{ 12,
		{
			MassPoint{Vector2D(1000, 1000), 1, 3}, MassPoint{Vector2D(1033, 983), 1, 3}, MassPoint{Vector2D(1066, 983), 1, 3},
			MassPoint{Vector2D(1100, 1000), 1, 3}, MassPoint{Vector2D(1116, 1033), 1, 3}, MassPoint{Vector2D(1116, 1066), 1, 3},
			MassPoint{Vector2D(1100, 1100), 1, 3}, MassPoint{Vector2D(1066, 1116), 1, 3}, MassPoint{Vector2D(1033, 1116), 1, 3},
			MassPoint{Vector2D(1000, 1100), 1, 3}, MassPoint{Vector2D(983, 1066), 1, 3}, MassPoint{Vector2D(983, 1033), 1, 3}
		},
		{ },gravity, dragCoeficient, 4000, .75, true, false, true, 700, 1, 8000, 60,.7, 0, C, B, SDL_Color{40, 40, 40, 255} };

	Entity wheel2 = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[wheel2] = Softbody_Component{ 12,
		{
			MassPoint{Vector2D(2000, 1000), 1, 3}, MassPoint{Vector2D(2033, 983), 1, 3}, MassPoint{Vector2D(2066, 983), 1, 3},
			MassPoint{Vector2D(2100, 1000), 1, 3}, MassPoint{Vector2D(2116, 1033), 1, 3}, MassPoint{Vector2D(2116, 1066), 1, 3},
			MassPoint{Vector2D(2100, 1100), 1, 3}, MassPoint{Vector2D(2066, 1116), 1, 3}, MassPoint{Vector2D(2033, 1116), 1, 3},
			MassPoint{Vector2D(2000, 1100), 1, 3}, MassPoint{Vector2D(1983, 1066), 1, 3}, MassPoint{Vector2D(1983, 1033), 1, 3}
		},
		{ },gravity, dragCoeficient,4000, .75, true, false, true, 700, 1, 8000, 60, .7, 0, C, B, SDL_Color{40, 40, 40, 255} };

	Registry::Instance()->softbodyRotatingMovements[wheel1] = SoftbodyRotatingMovement_Component{ &Registry::Instance()->softbodies[wheel1],  new double(10000) };
	Registry::Instance()->softbodyRotatingMovements[wheel2] = SoftbodyRotatingMovement_Component{ &Registry::Instance()->softbodies[wheel2],  new double(10000) };

	Registry::Instance()->softbodyJoints[carBody] = SoftbodyJoint_Component{ {SoftbodyJoint_Values{SpringJoint, &Registry::Instance()->softbodies[wheel1], Vector2D(0, 30), {}, {&Registry::Instance()->softbodies[carBody].massPoints[12]}},
	SoftbodyJoint_Values{SpringJoint, &Registry::Instance()->softbodies[wheel2], Vector2D(0, 30), {}, {&Registry::Instance()->softbodies[carBody].massPoints[9]}}} };

	Entity ground = EntityManager::Instance()->CreateEntity();
	Registry::Instance()->softbodies[ground] = Softbody_Component{ 4,
		{
			//MassPoint{Vector2D(920, 1600), 1, 1, true}, /*MassPoint{Vector2D(1320, 1500), 1, 1}, MassPoint{Vector2D(1720, 1400), 1, 1}, MassPoint{Vector2D(2120, 1300), 1, 1}, MassPoint{Vector2D(2520, 1200), 1, 1},*/
			//MassPoint{Vector2D(5020, 1100), 1, 1, true}, /*MassPoint{Vector2D(3200, 1100), 1, 1}, MassPoint{Vector2D(3500, 1100), 1, 1}, MassPoint{Vector2D(3750, 1100), 1, 1}, MassPoint{Vector2D(4000, 1100), 1, 1, true}, MassPoint{Vector2D(4000, 1380), 1, 1, true},*/
			//MassPoint{Vector2D(5020, 1380), 1, 1, true},
			//MassPoint{Vector2D(920, 1880), 1, 1, true}
		},
		{ },
		gravity, dragCoeficient, 2000, 40, false, true, false, 6000, 0.8f, 60, 0.5, 0, 0.4, A, Layers(), SDL_Color{135, 168, 137, 255} };

	Registry::Instance()->randomSoftbodyGenerators[ground] = RandomSoftbodyGenerator_Component{ HorizontalLinePolygon, 100, Vector2D(0, 2000), 100, 100, 0, 0, true, Vector2D(0, 200) };

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
int x = 0;
void Game::Update(double* deltaTime)
{
	inputManager->Update();
	Registry::Instance()->StartUpdate(deltaTime, this);
	Registry::Instance()->Update(deltaTime, this);

	if (InputManager::Instance()->MouseButtonPressed(InputManager::right))
	{
		x = 0;
		double dist = 1000000000000000000;
		for (int y = 1; y <= EntityManager::Instance()->num_entities; y++)
		{
			if (Registry::Instance()->softbodies.count(y))
			{
				for (int i = 0; i < Registry::Instance()->softbodies[y].massPoints.size(); i++)
				{
					Vector2D dirVec = Vector2D(Registry::Instance()->softbodies[y].massPoints[i].position.x - InputManager::Instance()->MousePos().x - cam->resultRect.x, Registry::Instance()->softbodies[y].massPoints[i].position.y - InputManager::Instance()->MousePos().y - cam->resultRect.y);
					double currentDist = dirVec.x * dirVec.x + dirVec.y * dirVec.y;
					if (dist > currentDist)
					{
						x = y;
						dist = currentDist;
						massPointI = i;
					}
				}
			}
		}

		Registry::Instance()->softbodies[x].massPoints[massPointI].Lock();
	}
	else if (InputManager::Instance()->MouseButtonDown(InputManager::right))
	{
		Registry::Instance()->softbodies[x].massPoints[massPointI].position = Vector2D(InputManager::Instance()->MousePos().x + cam->resultRect.x, InputManager::Instance()->MousePos().y + cam->resultRect.y);
	}
	else if (InputManager::Instance()->MouseButtonReleased(InputManager::right))
	{
		Registry::Instance()->softbodies[x].massPoints[massPointI].Unlock();
	}
	inputManager->UpdatePrevInput();
}
void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, backroundColor.r, backroundColor.g, backroundColor.b, backroundColor.a);
	SDL_RenderClear(renderer);

	SDL_RenderSetScale(renderer, 1, 1);

	Registry::Instance()->Draw(renderer, &cam->resultRect);
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