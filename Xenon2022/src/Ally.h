#pragma once
#include "Missile.h"

class ally : public GameObject {
public:
	ally(bool visibility = true, bool isBullet = false, bool hasSense = false)
		: GameObject(visibility, isBullet, hasSense) {
	}
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

	void TakeShipDamage();
	void checkDamageCooldown();
	void ShootCheck();

private:
	float damageCooldownDefault = 1;
	float damageCooldown = 0;
};
