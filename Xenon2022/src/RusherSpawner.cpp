#include "RusherSpawner.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void rusherSpawner::OnStart() {
		objectGroup = "RSpwaner";
	}

	void rusherSpawner::OnUpdate()  {
		time += 1 * engine.deltaTime;
		if (time > spawnCooldown) {
			rusher* enemy = new rusher(true, false, true);

			enemy->position.x = 400.0f;
			enemy->position.x = rand() % 540 + 100;
			enemy->position.y = -100.0f;
			engine.getLevel().addObject(enemy);
			time = 0;
		}
	}