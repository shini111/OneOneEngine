#include "Engine.h"
#include "Input.h"

#include "Ally.h"
#include "Enemy.h"
#include "EnemyProjectile.h"
#include "ScrollingBackground.h"
#include "RusherSpawner.h"
#include "Spaceship.h"
#include "LonerSpawner.h"


#include <random>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#undef main

GameEngine::Engine engine;
float globalRotation = 0.0f;

//To use this fuction we just call *GetGlobalRotation() and get the value of the global rotation
float* GetGlobalRotation() {
	return &globalRotation;
}

int main()
{
	GameWindow gameWindow;
	gameWindow.windowName = "Xenon 2000";
	gameWindow.windowWidth = 640;
	gameWindow.windowHeight = 480;

	GameLevel level;

	LevelBackground* backgroundLayer1 = new LevelBackground("resources/graphics/galaxy2.bmp");

	ScrollingBackground* backgroundLayer2 = new ScrollingBackground("resources/graphics/GAster96.bmp");

	ScrollingBackground* backgroundLayer3 = new ScrollingBackground("resources/graphics/MAster96.bmp");
	backgroundLayer3->position.y = -480.f;

	engine.setLevel(level);

	engine.getLevel().AddBackground(backgroundLayer1);
	engine.getLevel().AddBackground(backgroundLayer2);
	engine.getLevel().AddBackground(backgroundLayer3);

	spaceship* ship = new spaceship();

	rusherSpawner* spawner = new rusherSpawner();
	engine.getLevel().addObject(spawner);

	lonerSpawner* spawner2 = new lonerSpawner();
	engine.getLevel().addObject(spawner2);


	rusher* enemy = new rusher(true, false, true);


	engine.getLevel().addObject(ship);

	enemy->position.x = 400.0f;
	enemy->position.x = rand() % 540 + 100;
	enemy->position.y = -100.0f;
	engine.getLevel().addObject(enemy);

	engine.Initialize(gameWindow);
}
