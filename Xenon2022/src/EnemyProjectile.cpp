#include "EnemyProjectile.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void enemyProjectile::OnStart() {
		int textureDimentions[2] = { 8,1 };

		animation = Animation("resources/graphics/EnWeap6.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "enemyBullet";

		collisionBoxSize.w = collisionBoxSize.h = 16.0f;
	}

	void enemyProjectile::OnUpdate() {
		position.y -= moveSpeed * engine.deltaTime;

		if (position.y > 500.f) {
			Destroy();
		}
	}