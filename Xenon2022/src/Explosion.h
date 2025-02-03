#pragma once
#include "Engine.h"

class explosion : public GameObject {
public:
	explosion(bool visibility = true, bool isBullet = false, bool hasSense = false)
		: GameObject(visibility, isBullet, hasSense) {}

	void OnStart() override;

	void OnAnimationFinish() override;

};
