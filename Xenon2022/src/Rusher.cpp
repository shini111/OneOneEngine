#include "Rusher.h"
#include "Engine.h"
#include "Animation.h"
#include "Explosion.h"

extern GameEngine::Engine engine;

	void rusher::OnStart() {
		healthPoints = 2;

		int textureDimentions[2] = { 4,6 };

		animation = Animation("resources/graphics/rusher.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";
		collisionBoxSize.w = 48.0f;
		collisionBoxSize.h = 32.0f;
	}

	void rusher::OnUpdate() {
		position.y -= moveSpeed * engine.deltaTime;

		if (position.y > 520) {
			Destroy();
		}

	}

	void rusher::OnCollideEnter(GameObject& contact) {
		if (contact.objectGroup == "bullet") {

			explosion* boom = new explosion();
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