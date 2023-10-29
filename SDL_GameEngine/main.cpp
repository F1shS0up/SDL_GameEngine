#include <iostream>
#include "Game.h"
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <chrono>

const int WND_WIDTH = 3840, WND_HEIGHT = 2160;
const int REN_WIDTH = 3840, REN_HEIGHT = 2160;
const int MAX_FPS = 0;

Game* game = nullptr;

std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point TickTack;

int main(int argc, char* argv[])
{
	game = new Game();

	SDL_Rect windowRect = { 0, 0, WND_WIDTH, WND_HEIGHT };

	game->Init("Game window", windowRect, REN_WIDTH, REN_HEIGHT, true);
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	//deltaTimeLogic

	int desiredDeltaTime = MAX_FPS != 0 ? 1000 / MAX_FPS : 1;

	double deltaTime = 0.1;

	while (game->Running())
	{
		if (MAX_FPS != 0 && deltaTime < desiredDeltaTime)
		{
			SDL_Delay(desiredDeltaTime - deltaTime);
		}

		game->HandleEvents();
		game->Update(&deltaTime);
		game->Render();

		TickTack = std::chrono::steady_clock::now();

		deltaTime = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(TickTack - Start).count() / 1'000'000'000.0;
		Start = TickTack;
	}

	game->Clean();

	return 0;
}