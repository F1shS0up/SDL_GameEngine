#pragma once

#include "SDL.h"
#include "InputManager.h"
#include <string>

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

	void SetTextInputString(std::string* textToPutInputTo);
	std::string* activeTextInput;

	SDL_Renderer* renderer;

	const int rectangleLineThickness = 10;
	const SDL_Color backroundColor = SDL_Color{ 214, 206, 194, 255 };
	const SDL_Rect rect = { 920 / rectangleLineThickness, 80 / rectangleLineThickness , 2000 / rectangleLineThickness , 2000 / rectangleLineThickness };
	float* gravity;
	float* dragCoeficient;
	std::string assetPath = "";
	bool stopSimulation = false;
private:

	bool isRunning;
	SDL_Window* window;
	InputManager* inputManager;
};
