#pragma once
#include "Engine.h"

class missile : public GameObject {
public:

	missile(bool visibility = true, bool isBullet = false, bool hasSense = true)
		: GameObject(visibility, isBullet, hasSense) {
	}

	float moveSpeed = -250.0f;

	int firePower = 0;
	int missileDamage = 1;

	void OnStart() override;

	int getMissileDamage();

	void OnUpdate() override;
};
