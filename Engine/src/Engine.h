#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <Windows.h>

#include "Animator.h"
#include "GameLevel.h"
#include "GameObjects.h"


typedef int SDL_Keycode;

// Forward declaration of SDL_GameController
struct _SDL_GameController;
typedef _SDL_GameController SDL_GameController;

// Typedef for Uint8
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

extern Input input;


class Game {
public:

	void start();


private:
	int prevTime = currentTime;
	int currentTime = 0;
};


class GameWindow
{
public:
	const char* windowName = "Xenon 2022";
	int windowWidth = 640;
	int windowHeight = 480;
};

namespace GameEngine {
	class Engine
	{
	public:
		float deltaTime = 0.0f;

		void setLevel(GameLevel level);
		GameLevel& getLevel();
		void print(std::string printText);

		void Update();
		void Initialize(GameWindow windowSettings);
	private:
		GameLevel mainLevel;
		GameWindow windowDisplay;
		int prevTime = currentTime;
		int currentTime = 0;

	};

}
