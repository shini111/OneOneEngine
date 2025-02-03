#include "Spaceship.h"
#include "Explosion.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void spaceship::OnStart() {

		int textureDimentions[2] = { 7,1 };


		shipHealthMax = 5;
		shipHealth = 5;
		keyPressed = false;
		firePower = 0;

		movementSpeed = 200.0f;

		bulletOffset.x = 24;
		bulletOffset.y = 0;

		animationState = 0;
		objectGroup = "player";

		position.x = 280.0f;
		position.y = 400.0f;

		collisionBoxSize.w = collisionBoxSize.h = 64.0f;
	}

	void spaceship::OnUpdate() {

		if (isGameOver == false)
		{
			float moveX = 0.0f;
			float moveY = 0.0f;

			ShootCheck();
			checkDamageCooldown();

			if (input.IsGamepadButtonPressed(GamepadButton::DPadLeft, false)) {
				moveX = -1.0f;
				animationState = 2;
			}
			else if (input.IsGamepadButtonPressed(GamepadButton::DPadRight, false)) {
				moveX = 1.0f;
				animationState = 1;
			}
			else {
				animationState = 0;
			}
			if (input.IsGamepadButtonPressed(GamepadButton::DPadUp, false)) {
				moveY = 1.0f;
			}
			else if (input.IsGamepadButtonPressed(GamepadButton::DPadDown, false)) {
				moveY = -1.0f;
			}

			// Normalize the movement vector if moving diagonally
			float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
			if (magnitude > 0.0f) {
				moveX /= magnitude; // Normalize X
				moveY /= magnitude; // Normalize Y
			}

			// Apply the movement
			float auxX = position.x;
			float auxY = position.y;

			auxX += moveX * movementSpeed * engine.deltaTime;
			auxY -= moveY * movementSpeed * engine.deltaTime;

			if (auxX > limits[0] && auxX < limits[2])
			{
				position.x += moveX * movementSpeed * engine.deltaTime;
			}

			if (auxY > limits[1])
			{
				if (auxY < limits[3])
				{
					position.y -= moveY * movementSpeed * engine.deltaTime;
				}
			}



		}

		if (animationState == 1 && currentAnimation != "Right")
		{
			currentAnimation = "Right";

			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0) });
			animation.spriteIndex = 0;
		}
		else if (animationState == 2 && currentAnimation != "Left")
		{
			currentAnimation = "Left";
			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(2,0),AnimationCoord(1,0),AnimationCoord(0,0) });
			animation.spriteIndex = 0;
		}
		else if (animationState == 0 && currentAnimation != "Idle")
		{
			currentAnimation = "Idle";
			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(3,0) });
			animation.spriteIndex = 0;
		}


		if (shipHealth <= 0 && isGameOver == false) {
			isGameOver = true;

			position.x = 1000.0f;
			position.y = 1000.0f;
		}
	}

	void spaceship::OnCollideEnter(GameObject& contact) {

		int textureDimentions[2] = { 7,3 };

		if (contact.objectGroup == "enemyBullet") {
			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			if (animationState == 1 && currentAnimation != "Right")
			{
				currentAnimation = "Up";

				animation = Animation("resources/graphics/Ship2.bmp", 0.1f, textureDimentions, false,
					{
					AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0), AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0), AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0),
					AnimationCoord(6,1),AnimationCoord(6,1),AnimationCoord(6,1), AnimationCoord(4,1),AnimationCoord(5,1),AnimationCoord(6,1), AnimationCoord(4,1),AnimationCoord(5,1),AnimationCoord(6,1),
					AnimationCoord(6,2),AnimationCoord(6,2),AnimationCoord(6,2), AnimationCoord(4,2),AnimationCoord(5,2),AnimationCoord(6,2), AnimationCoord(4,2),AnimationCoord(5,2),AnimationCoord(6,2)
					}
				);
				animation.spriteIndex = 0;
			}
			else if (animationState == 2 && currentAnimation != "Left")
			{
				currentAnimation = "Down";
				animation = Animation("resources/graphics/Ship2.bmp", 0.1f, textureDimentions, false,
					{
					AnimationCoord(2,0),AnimationCoord(1,0),AnimationCoord(0,0) , AnimationCoord(2,0),AnimationCoord(1,0),AnimationCoord(0,0), AnimationCoord(2,0),AnimationCoord(1,0),AnimationCoord(0,0),
					AnimationCoord(2,1),AnimationCoord(1,1),AnimationCoord(0,1), AnimationCoord(2,1),AnimationCoord(1,1),AnimationCoord(0,1), AnimationCoord(2,1),AnimationCoord(1,1),AnimationCoord(0,1),
					AnimationCoord(2,2),AnimationCoord(1,2),AnimationCoord(0,2), AnimationCoord(2,2),AnimationCoord(1,2),AnimationCoord(0,2), AnimationCoord(2,2),AnimationCoord(1,2),AnimationCoord(0,2)
					}
				);
				animation.spriteIndex = 0;
			}
			else if (animationState == 0 && currentAnimation != "Idle")
			{
				currentAnimation = "Idle";
				animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false,
					{
						AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2),AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2),AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2)
					}
				);
				animation.spriteIndex = 0;
			}
			engine.getLevel().addObject(boom);
			TakeShipDamage();
			contact.Destroy();
		}

		if (contact.objectGroup == "enemy") {
			animation = Animation("resources/graphics/Ship2.bmp", 0.1f, textureDimentions, false, { AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2),AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2),AnimationCoord(3,0), AnimationCoord(3,1), AnimationCoord(3,2) });
			animation.spriteIndex = 0;
			TakeShipDamage();
		}
	}
