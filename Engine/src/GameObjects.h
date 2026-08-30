#pragma once
#include <string>
#include "Animation.h"

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

	~GameObject()
	{
		delete bodyId;
		delete bodyDef;
		delete shapeId;
		delete shapeDef;
		delete boxCollision;

		bodyId = nullptr;
		bodyDef = nullptr;
		shapeId = nullptr;
		shapeDef = nullptr;
		boxCollision = nullptr;
	}

	Animation animation;

	bool hasBox2d = true;
	bool box2dCreated = false;

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
	}colorChange;

	float rotation = 0.f;

	bool visible = true;
	bool isBullet = false;
	bool hasSense = false;

	float movementSpeed = 150.0f;

	virtual void OnStart() {};
	virtual void OnUpdate() {};
	virtual void OnAnimationFinish() {};
	virtual void OnCollideEnter(GameObject& contact) {};
	void Destroy();
	virtual void OnDestroyed() {};

	std::string objectGroup;

	b2BodyId* bodyId = nullptr;
	b2BodyDef* bodyDef = nullptr;
	b2ShapeId* shapeId = nullptr;
	b2ShapeDef* shapeDef = nullptr;
	b2Polygon* boxCollision = nullptr;


	bool toBeCreated = true;
	bool toBeDeleted = false;
};

