#pragma once
#include "Engine.h"
#include "Missile.h"

class Enemy : public GameObject {
public:
	Enemy(bool visibility = true, bool isBullet = false, bool hasSense = false)
		: GameObject(visibility, isBullet, hasSense) {
	}

	int healthPoints = 1;

	void TakeDamage(int paramFirePower);

};
