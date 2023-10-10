#pragma once

#include <SDL.h>
#include "Vector2DInt.h"

class InputManager
{
public:

	enum MOUSE_BUTTONS { left = 0, right, middle, back, forward };

private:

	static InputManager* instance;

	Uint8* previousKeyState;
	const Uint8* keyboardState;
	int keyLength;

	Uint32 prevMouseState;
	Uint32 mouseState;

	Vector2DInt mousePos;

public:

	static InputManager* Instance();
	static void Release();

	bool KeyDown(SDL_Scancode scanCode);
	bool KeyPressed(SDL_Scancode scanCode);
	bool KeyReleased(SDL_Scancode scanCode);

	bool MouseButtonDown(MOUSE_BUTTONS button);
	bool MouseButtonPressed(MOUSE_BUTTONS button);
	bool MouseButtonReleased(MOUSE_BUTTONS button);

	Vector2DInt MousePos();

	void Update();
	void UpdatePrevInput();

private:

	InputManager();
	~InputManager();
};
