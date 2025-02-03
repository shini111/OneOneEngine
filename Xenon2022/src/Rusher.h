#pragma once
#include "Enemy.h"

class rusher : public Enemy {
public:

	rusher(bool visibility = true, bool isBullet = false, bool hasSense = true)
		: Enemy(visibility, isBullet, hasSense) {
	}

	float moveSpeed = -150.0f;
	void OnStart() override;

	void OnUpdate() override;

	void OnCollideEnter(GameObject& contact) override;

};
