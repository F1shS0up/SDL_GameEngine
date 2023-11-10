#pragma once

#include "SDL.h"
#include "InputManager.h"
#include <string>
#include "Vector2D.h"

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

	class Camera_Component* cam;

	double* gravity;
	double* dragCoeficient;
	double* camSize;

	std::string assetPath = "";
private:

	bool isRunning;
	SDL_Window* window;
	InputManager* inputManager;
};
