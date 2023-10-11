#pragma once

#include "SDL.h"
#include "InputManager.h"

class Game
{
public:
	Game();
	~Game();

	void Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen);

	void InitSDL(bool fullscreen, const char* title, SDL_Rect& windowSize);

	void HandleEvents();
	void Update(double* deltaTime);
	void Render();

	void Clean();
	bool Running() { return isRunning; };

	SDL_Renderer* renderer;
private:

	bool isRunning;
	SDL_Window* window;
	InputManager* inputManager;
};
