#pragma once
#include <string>
#include "Animator.h"

class GameObject
{
public:
	Animation animation;

	struct {
		float x = 0.0f;
		float y = 0.0f;
	}position;

	struct {
		float w = 32.0f;
		float h = 32.0f;
	}collisionBoxSize;

	struct {
		int r = 255;
		int g = 255;
		int b = 255;
	}modulate;

	float rotation = 0;

	bool visible = true;

	virtual void OnStart() {};
	virtual void OnUpdate() {};
	virtual void OnAnimationFinish() {};
	virtual void OnCollideEnter(GameObject& contact) {};
	void Destroy();
	virtual void OnDestroyed() {};

	std::string objectGroup;

	bool toBeDeleted = false;


};

class Pawn : public GameObject
{
public:
	float movementSpeed = 150.0f;
};