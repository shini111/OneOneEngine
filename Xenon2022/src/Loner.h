#pragma once
#include "Engine.h"
#include "Enemy.h"

class loner : public Enemy {
public:

	loner(bool visibility = true, bool isBullet = false, bool hasSense = true)
		: Enemy(visibility, isBullet, hasSense) {
	}

	float moveSpeed = 70.0f;
	float time = 0.0f;
	float timeCooldown = 2.0f;

	void OnStart() override;

	void OnCollideEnter(GameObject& contact) override;
	void OnUpdate() override;

};
