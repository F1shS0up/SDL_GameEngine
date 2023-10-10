#include "InputManager.h"

InputManager* InputManager::instance = NULL;

InputManager* InputManager::Instance()
{
	if (instance == NULL)
	{
		instance = new InputManager();
	}

	return instance;
}

void InputManager::Release()
{
	delete instance;
	instance = NULL;
}

bool InputManager::KeyDown(SDL_Scancode scanCode)
{
	return keyboardState[scanCode];
}
bool InputManager::KeyPressed(SDL_Scancode scanCode)
{
	return !previousKeyState[scanCode] && keyboardState[scanCode];
}
bool InputManager::KeyReleased(SDL_Scancode scanCode)
{
	return previousKeyState[scanCode] && !keyboardState[scanCode];
}


Vector2DInt InputManager::MousePos()
{
	return Vector2DInt(mousePos);
}

bool InputManager::MouseButtonDown(MOUSE_BUTTONS button)
{
	Uint32 mask = 0;

	switch (button)
	{
	case left:
		mask = SDL_BUTTON_LMASK;
		break;
	case right:
		mask = SDL_BUTTON_RMASK;
		break;
	case middle:
		mask = SDL_BUTTON_MMASK;
		break;
	case back:
		mask = SDL_BUTTON_X1MASK;
		break;
	case forward:
		mask = SDL_BUTTON_X2MASK;
		break;
	}

	return (mouseState & mask);
}
bool InputManager::MouseButtonPressed(MOUSE_BUTTONS button)
{
	Uint32 mask = 0;

	switch (button)
	{
	case left:
		mask = SDL_BUTTON_LMASK;
		break;
	case right:
		mask = SDL_BUTTON_RMASK;
		break;
	case middle:
		mask = SDL_BUTTON_MMASK;
		break;
	case back:
		mask = SDL_BUTTON_X1MASK;
		break;
	case forward:
		mask = SDL_BUTTON_X2MASK;
		break;
	}

	return !(prevMouseState & mask) && (mouseState & mask);
}
bool InputManager::MouseButtonReleased(MOUSE_BUTTONS button)
{
	Uint32 mask = 0;

	switch (button)
	{
	case left:
		mask = SDL_BUTTON_LMASK;
		break;
	case right:
		mask = SDL_BUTTON_RMASK;
		break;
	case middle:
		mask = SDL_BUTTON_MMASK;
		break;
	case back:
		mask = SDL_BUTTON_X1MASK;
		break;
	case forward:
		mask = SDL_BUTTON_X2MASK;
		break;
	}

	return (prevMouseState & mask) && !(mouseState & mask);
}

void InputManager::Update()
{
	mouseState = SDL_GetMouseState(mousePos.getX(), mousePos.getY());
}
void InputManager::UpdatePrevInput()
{
	SDL_memcpy(previousKeyState, keyboardState, keyLength);
	prevMouseState = mouseState;
}

InputManager::InputManager()
{
	keyboardState = SDL_GetKeyboardState(&keyLength);
	previousKeyState = new Uint8[keyLength];
	SDL_memcpy(previousKeyState, keyboardState, keyLength);
}

InputManager::~InputManager()
{
	delete[] previousKeyState;
	previousKeyState = NULL;
}
