#include "Ally.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void ally::TakeShipDamage() {
		if (damageCooldown <= 0)
		{
			std::cout << "Ship Damaged" << std::endl;
			shipHealth -= 1;
			damageCooldown = damageCooldownDefault;
		}
	}

	void ally::checkDamageCooldown() {
		if (damageCooldown > 0)
		{
			damageCooldown -= 1 * engine.deltaTime;
		}
		else {
			damageCooldown = 0;
		}
	}
	void ally::ShootCheck() {
		if (input.IsGamepadButtonPressed(GamepadButton::A, false)) {
			if (!keyPressed) {
				missile* bullet = new missile(true, true, true);
				bullet->position.x = position.x + bulletOffset.x;
				bullet->position.y = position.y + bulletOffset.y;
				bullet->firePower = firePower;
				engine.getLevel().addObject(bullet);
				keyPressed = true;
			}
		}
		else {
			keyPressed = false;
		}
	};
