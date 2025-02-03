#pragma once
#include "Engine.h"

class enemyProjectile : public GameObject {
public:
	enemyProjectile(bool visibility = true, bool isBullet = true, bool hasSense = true)
		: GameObject(visibility, isBullet, hasSense) {
	}

	float moveSpeed = -250.0f;

	void OnStart() override;

	void OnUpdate() override;
};


