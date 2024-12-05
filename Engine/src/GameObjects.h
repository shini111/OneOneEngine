#pragma once
#include <string>
#include "Animator.h"

typedef struct b2BodyId;
typedef struct b2BodyDef;
typedef struct b2ShapeId;
typedef struct b2ShapeDef;
typedef struct b2Polygon;

class GameObject
{
public:
	GameObject(bool visibility, bool isBullet, bool hasSense)
			: visible(visibility), isBullet(isBullet), hasSense(hasSense) {
	}

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
	bool isBullet = false;
	bool hasSense = false;

	virtual void OnStart() {};
	virtual void OnUpdate() {};
	virtual void OnAnimationFinish() {};
	virtual void OnCollideEnter(GameObject& contact) {};
	void Destroy();
	virtual void OnDestroyed() {};

	std::string objectGroup;

	b2BodyId* bodyId;
	b2BodyDef* bodyDef;
	b2ShapeId* shapeId;
	b2ShapeDef* shapeDef;
	b2Polygon* boxCollision;


	bool toBeCreated = true;
	bool toBeDeleted = false;
};

class Pawn : public GameObject
{
public:
	Pawn(bool visibility = true, bool isBullet = false, bool hasSense = true)
		: GameObject(visibility, isBullet, hasSense) {
	}
	float movementSpeed = 150.0f;
};