#pragma once

#include "SDL.h"
#include "InputManager.h"
#include <string>
#include "Vector2D.h"
namespace Engine
{
	class Game
	{
	public:
		Game();
		~Game();

		virtual void Init(const char* title, SDL_Rect windowSize, int renderWidth, int renderHeight, bool fullscreen, bool windowedBorderless, SDL_Color clearColor);

		void InitSDL(bool fullscreen, bool windowedBorderless, const char* title, SDL_Rect& windowSize);

		virtual void HandleEvents();
		virtual void Update(double* deltaTime);
		virtual void Render();

		void Clean();
		bool Running() { return isRunning; };

		void SetTextInputString(std::string* textToPutInputTo);
		std::string* activeTextInput;

		SDL_Renderer* renderer;

		SDL_Color clearColor;

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
}
