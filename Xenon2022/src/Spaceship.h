#pragma once
#include "Engine.h"
#include "Ally.h"

class spaceship : public ally {
public:
	spaceship(bool visibility = true, bool isBullet = false, bool hasSense = false)
		: ally(visibility, isBullet, hasSense) {
	}

	int textureDimentions[2] = { 7,1 };

	float limits[4] = { 0.f, 0.f,580.f, 420.f };

	std::string currentAnimation = "";
	int animationState = 0;

	bool isGameOver = false;

	bool canTakeDamage = true;
	float damageCooldown = 0;


	void OnStart() override;

	void OnUpdate() override;

	void OnCollideEnter(GameObject& contact) override;

};
