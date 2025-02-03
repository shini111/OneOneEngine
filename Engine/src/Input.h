#pragma once
#include <string>

typedef int SDL_Keycode;

// Forward declaration of SDL_GameController
struct _SDL_GameController;
typedef _SDL_GameController SDL_GameController;
typedef unsigned char Uint8;

enum class InputEnum
{
	East,
	West,
	North,
	South,
	DNorth,
	DSouth,
	DEast,
	DWest,
	LeftThumbstick,
	RightThumbstick,
	ButtonA,
	ButtonB,
	ButtonX,
	ButtonY
};

enum class GamepadButton
{
	A,
	B,
	X,
	Y,
	DPadLeft,
	DPadRight,
	DPadUp,
	DPadDown
};

class Input {

public:
	std::string key;
	InputEnum inputEnum;
	SDL_GameController* gameController = nullptr;

	//void init();
	InputEnum getKeyPressed();
	void setGameController(SDL_GameController* controller);
	bool IsGamepadButtonPressed(GamepadButton button, bool singleClick);
private:
	InputEnum mapSDLKeyToInputEnum(SDL_Keycode key);
	InputEnum mapSDLButtonToInputEnum(Uint8 button);
};
