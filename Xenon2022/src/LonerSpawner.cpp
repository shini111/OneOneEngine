#include "LonerSpawner.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void lonerSpawner::OnStart() {
		objectGroup = "LSpwaner";

		position.x = 0;
	}


	void lonerSpawner::OnUpdate() {
		time += 1 * engine.deltaTime;
		if (time > spawnCooldown) {
			loner* enemy = new loner(true, false, true);
			enemy->position.x = -100.0f;
			enemy->position.y = rand() % 200 + 40;
			engine.getLevel().addObject(enemy);
			time = 0;
		}
	}