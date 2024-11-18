#include "Engine.h"
#include <random>

#undef main

GameEngine::Engine engine;
Input input;
float globalRotation = 0.0f;

//To use this fuction we just call *GetGlobalRotation() and get the value of the global rotation
float* GetGlobalRotation() {
	return &globalRotation;
}

class Enemy : public GameObject {
public:
	int healthPoints = 1;

	void showDamageFeedback() {
		modulate.r = 255;
		modulate.g = 0;
		modulate.b = 0;
	}

	void hideDamageFeedback() {
		modulate.r = 255;
		modulate.g = 255;
		modulate.b = 255;
	}

	void checkDamageFeedback() {
		if (damageFeedbackTimer > 0) {
			damageFeedbackTimer -= damageFeedbackDuration * damageFeedbackSpeed * engine.deltaTime;

			if (int(damageFeedbackTimer) % 2 == 0) {
				showDamageFeedback();
			}
			else {
				hideDamageFeedback();
			}

		}
		else
		{
			hideDamageFeedback();
			damageFeedbackTimer = 0;
		}
	}

	void TakeDamage(int paramFirePower) {
		healthPoints -= paramFirePower;



		if (healthPoints <= 0) {
			Destroy();

		}
		else {

			showDamageFeedback();
			damageFeedbackTimer = damageFeedbackDuration;

		}
	}
private:
	float damageFeedbackTimer = 0;
	float damageFeedbackDuration = 5;
	float damageFeedbackSpeed = 2;

};
class explosion : public GameObject {
public:

	void OnStart() override {

		int textureDimentions[2] = { 5,2 };

		animation = Animation("resources/graphics/explode64.bmp", 0.1f, textureDimentions, false, {});
	}

	void OnAnimationFinish() override {
		Destroy();
	}

};


class missile : public GameObject {
public:
	float moveSpeed = -250.0f;

	int firePower = 0;
	int missileDamage = 1;

	void OnStart() override {
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

		// 		rotation = globalRotation;
		rotation = *GetGlobalRotation();
	}

	int getMissileDamage() {
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

	void OnUpdate() override {
		position.y += moveSpeed * engine.deltaTime;

		if (position.y < -50) {
			Destroy();
		}
	}
};

class rusher : public Enemy {
public:
	float moveSpeed = -150.0f;
	void OnStart() override {
		healthPoints = 2;

		int textureDimentions[2] = { 4,6 };

		animation = Animation("resources/graphics/rusher.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";
		collisionBoxSize.w = 48.0f;
		collisionBoxSize.h = 32.0f;

		rotation = *GetGlobalRotation();
	}

	void OnUpdate() override {
		position.y -= moveSpeed * engine.deltaTime;

		if (position.y > 520) {
			Destroy();
		}

		checkDamageFeedback();
	}

	void OnCollideEnter(GameObject& contact) override {
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

};

class enemyProjectile : public GameObject {
public:
	float moveSpeed = -250.0f;

	void OnStart() override {
		int textureDimentions[2] = { 8,1 };

		animation = Animation("resources/graphics/EnWeap6.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "enemyBullet";

		collisionBoxSize.w = collisionBoxSize.h = 16.0f;
	}

	void OnUpdate() override {
		position.y -= moveSpeed * engine.deltaTime;

		if (position.y > 500.f) {
			Destroy();
		}
	}

};

class loner : public Enemy {
public:
	float moveSpeed = 70.0f;
	float time = 0.0f;
	float timeCooldown = 2.0f;

	void OnStart() override {

		healthPoints = 3;

		int textureDimentions[2] = { 4,4 };

		animation = Animation("resources/graphics/LonerA.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";

		collisionBoxSize.w = collisionBoxSize.h = 64.0f;
		rotation = *GetGlobalRotation();
	}

	void OnCollideEnter(GameObject& contact) override {
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
	void OnUpdate() override {
		time += 1 * engine.deltaTime;

		if (time > timeCooldown) {
			enemyProjectile* enemyProj = new enemyProjectile();
			enemyProj->position.x = position.x + 20;
			enemyProj->position.y = position.y + 48;
			engine.getLevel().addObject(enemyProj);
			time = 0;
		}

		position.x += moveSpeed * engine.deltaTime;

		checkDamageFeedback();

		if (position.y > 480) {
			Destroy();
		}

	}

};

class ally : public Pawn {
public:

	int shipHealthMax = 5;
	int shipHealth = 5;

	bool keyPressed = false;

	int firePower = 0;

	int positionOffset = 0;

	struct
	{
		int x = 0;
		int y = 0;
	}bulletOffset;


	void TakeShipDamage() {
		if (damageCooldown <= 0)
		{
			shipHealth -= 1;
			damageCooldown = damageCooldownDefault;
		}
	}

	void checkDamageCooldown() {
		if (damageCooldown > 0)
		{
			damageCooldown -= 1 * engine.deltaTime;
		}
		else {
			damageCooldown = 0;
		}
	}

	void ShootCheck() {

		if (GetAsyncKeyState(input.ShootAction[0]) & 0x8000) {
			if (!keyPressed) {
				missile* bullet = new missile();
				bullet->position.x = position.x + bulletOffset.x;
				bullet->position.y = position.y + bulletOffset.y;
				bullet->firePower = firePower;
				engine.getLevel().addObject(bullet);

				keyPressed = true;
			}
		}
		else
		{
			keyPressed = false;
		}

	};
private:
	float damageCooldownDefault = 1;
	float damageCooldown = 0;
};

class spaceship : public ally {
public:

	std::string currentAnimation = "";
	int animationState = 0;

	bool isGameOver = false;

	void OnStart() override {
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
		rotation = *GetGlobalRotation();
	}

	void OnUpdate() override {

		int textureDimentions[2] = { 7,1 };


		if (isGameOver == false)
		{
			ShootCheck();
			checkDamageCooldown();

			if (GetKeyState(input.MoveLeft[0]) & 0x8000)
			{
				position.x -= movementSpeed * engine.deltaTime;
			}
			else if (GetKeyState(input.MoveRight[0]) & 0x8000)
			{
				position.x += movementSpeed * engine.deltaTime;

			}


			if (GetKeyState(input.MoveUp[0]) & 0x8000)
			{
				position.y -= movementSpeed * engine.deltaTime;
				animationState = 1;
			}
			else if (GetKeyState(input.MoveDown[0]) & 0x8000)
			{
				position.y += movementSpeed * engine.deltaTime;
				animationState = 2;
			}
			else
			{
				animationState = 0;
			}

		}

		if (animationState == 1 && currentAnimation != "Up")
		{
			currentAnimation = "Up";

			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0) });
			animation.spriteIndex = 0;
		}
		else if (animationState == 2 && currentAnimation != "Down")
		{
			currentAnimation = "Down";
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

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "enemyBullet") {
			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);
			TakeShipDamage();
			contact.Destroy();
		}

		if (contact.objectGroup == "enemy") {
			TakeShipDamage();
		}
	}

};

class rusherSpawner : public GameObject
{
public:
	float spawnCooldown = 2.0f;
	float time = 0.0f;
	void OnUpdate() override {
		time += 1 * engine.deltaTime;
		if (time > spawnCooldown) {
			rusher* enemy = new rusher();

			enemy->position.x = 400.0f;
			enemy->position.x = rand() % 540 + 100;
			enemy->position.y = -100.0f;
			engine.getLevel().addObject(enemy);
			time = 0;
		}
	}
};

class lonerSpawner : public GameObject
{
public:
	float spawnCooldown = 4.0f;
	float time = 0.0f;
	void OnUpdate() override {
		time += 1 * engine.deltaTime;
		if (time > spawnCooldown) {
			loner* enemy = new loner();
			enemy->position.x = -100.0f;
			enemy->position.y = rand() % 200 + 40;
			engine.getLevel().addObject(enemy);
			time = 0;
		}
	}
};

int main()
{
	GameWindow gameWindow;
	gameWindow.windowName = "Xenon 2000";
	gameWindow.windowWidth = 640;
	gameWindow.windowHeight = 480;

	// Input Mapping

	input.ShootAction = { VK_SPACE };
	input.MoveDown = { VK_DOWN };
	input.MoveLeft = { VK_LEFT };
	input.MoveRight = { VK_RIGHT };
	input.MoveUp = { VK_UP };

	GameLevel level;

	LevelBackground backgroundLayer1;
	backgroundLayer1.background_path = "resources/graphics/galaxy2.bmp";
	backgroundLayer1.scrollingSpeed = 0;

	LevelBackground backgroundLayer2;

	backgroundLayer2.background_path = "resources/graphics/GAster96.bmp";
	backgroundLayer2.scrollingSpeed = -10;
	backgroundLayer2.scrollingDirection = backgroundLayer2.vertical;

	level.setLayerSize(2);
	level.background[0] = backgroundLayer1;
	level.background[1] = backgroundLayer2;

	engine.setLevel(level);

	spaceship* ship = new spaceship();

	rusherSpawner* spawner = new rusherSpawner();
	engine.getLevel().addObject(spawner);
	lonerSpawner* spawner2 = new lonerSpawner();
	engine.getLevel().addObject(spawner2);

	engine.getLevel().addObject(ship);

	engine.Initialize(gameWindow);
	engine.getLevel().addObject(ship);

	engine.Initialize(gameWindow);

}
