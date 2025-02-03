#include "Loner.h"
#include "Explosion.h"
#include "Engine.h"
#include "EnemyProjectile.h"

extern GameEngine::Engine engine;

	void loner::OnStart() {

		healthPoints = 3;

		int textureDimentions[2] = { 4,4 };

		animation = Animation("resources/graphics/LonerA.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";

		collisionBoxSize.w = collisionBoxSize.h = 64.0f;
	}

	void loner::OnCollideEnter(GameObject& contact) {
		if (contact.objectGroup == "bullet") {

			explosion* boom = new explosion(true, false, false);

			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);

			if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

				int missileFirePower = missileContact->getMissileDamage();

				TakeDamage(missileFirePower);
			}
			contact.Destroy();
		}
	}
	void loner::OnUpdate() {
		time += 1 * engine.deltaTime;

		if (time > timeCooldown) {
			enemyProjectile* enemyProj = new enemyProjectile();
			enemyProj->position.x = position.x + 20;
			enemyProj->position.y = position.y + 48;
			engine.getLevel().addObject(enemyProj);
			time = 0;
		}

		position.x += moveSpeed * engine.deltaTime;

		if (position.x > 700) {
			Destroy();
		}

	}
