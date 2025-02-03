#include "Enemy.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void Enemy::TakeDamage(int paramFirePower) {
		healthPoints -= paramFirePower;

		if (healthPoints <= 0) {
			Destroy();
		}
	}
