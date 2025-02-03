#pragma once
#include "Engine.h"
#include "Rusher.h"
#include "Enemy.h"

class rusherSpawner : public GameObject
{
public:
	rusherSpawner(bool visibility = false, bool isBullet = false, bool hasSense = false)
		: GameObject(visibility, isBullet, hasSense) {
	}

	float spawnCooldown = 2.0f;
	float time = 0.0f;

	void OnStart() override;

	void OnUpdate() override;
};