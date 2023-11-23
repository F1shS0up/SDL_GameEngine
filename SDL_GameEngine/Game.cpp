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
#include "KDTree.h"

namespace Engine
{
#define HandleError() \
	std::cout << SDL_GetError() << std::endl; \
	isRunning = false;

	Game::Game()
	{
	}
	Game::~Game()
	{
	}

	void Game::Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen, bool windowedBorderless, SDL_Color clearColor)
	{
		this->clearColor = clearColor;
		InitSDL(fullscreen, windowedBorderless, title, windowSize);

		assetPath = SDL_GetBasePath();
		assetPath += "Assets/";
		inputManager = InputManager::Instance();
		gravity = new double(0);
		clearColor = clearColor;

		SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);

		SDL_StopTextInput();
	
	}

	void Game::InitSDL(bool fullscreen, bool windowedBorderless, const char* title, SDL_Rect& windowSize)
	{
		int flags = 0;
		isRunning = true;

		if (fullscreen)
		{
			flags = SDL_WINDOW_FULLSCREEN;
		}
		else if(windowedBorderless)
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
		if(fullscreen)SDL_SetWindowGrab(window, SDL_TRUE);
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
		Registry::Instance()->StartUpdate(deltaTime, this);
		Registry::Instance()->Update(deltaTime, this);

		inputManager->UpdatePrevInput();
	}
	void Game::Render()
	{
		SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		SDL_RenderClear(renderer);

		Registry::Instance()->Draw(renderer, &cam->resultRect, this);
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
}