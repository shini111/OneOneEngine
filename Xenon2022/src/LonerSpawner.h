#pragma once
#include "Engine.h"
#include "Enemy.h"
#include "Explosion.h"
#include "Loner.h"

class lonerSpawner : public GameObject
{
public:
	lonerSpawner(bool visibility = false, bool isBullet = false, bool hasSense = false)
		: GameObject(visibility, isBullet, hasSense) {
	}
	float spawnCooldown = 4.0f;
	float time = 0.0f;
	void OnStart() override;
	void OnUpdate() override;
};