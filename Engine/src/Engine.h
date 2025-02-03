#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <Windows.h>
#include <cstdint>

#include "Animation.h"
#include "GameLevel.h"
#include "GameObjects.h"
#include "Input.h"
#include "LevelBackground.h"


typedef int SDL_Keycode;

extern Input input;

// Forward declaration of SDL_GameController
struct _SDL_GameController;
typedef _SDL_GameController SDL_GameController;

typedef struct SDL_Texture;
typedef struct SDL_Surface;
typedef struct SDL_Renderer;

typedef struct b2ShapeId;
typedef struct b2Manifold;

// Typedef for Uint8
typedef unsigned char Uint8;

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
		static SDL_Texture* LoadTexture(std::string filePath, SDL_Renderer* renderTarget);
		static SDL_Surface* OptimizedSurface(std::string filePath, SDL_Surface* windowSurface);
	private:
		void sensorListener();
		void contactListener();

		GameLevel mainLevel;
		GameWindow windowDisplay;
		int prevTime = currentTime;
		int currentTime = 0;

	};

}
