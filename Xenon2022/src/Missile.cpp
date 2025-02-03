#include "Missile.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	float moveSpeed = -250.0f;

	int firePower = 0;
	int missileDamage = 1;

	void missile::OnStart() {
		int textureDimentions[2] = { 2,3 };

		switch (firePower) {
		case 0:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0) });
			break;
		case 1:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,1),AnimationCoord(1,1) });
			break;
		case 2:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,2),AnimationCoord(1,2) });
			break;
		default:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0) });
		}

		collisionBoxSize.w = collisionBoxSize.h = 16.0f;

		objectGroup = "bullet";
	}

	int missile::getMissileDamage() {
		int damage = 1;

		switch (firePower) {
		case 0:
			damage = 1;
			break;
		case 1:
			damage = 2;
			break;
		case 2:
			damage = 4;
			break;
		}
		return damage;
	}

	void missile::OnUpdate()  {
		position.y += moveSpeed * engine.deltaTime;

		if (position.y < -50) {
			Destroy();
		}
	}
