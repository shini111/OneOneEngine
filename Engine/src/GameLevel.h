#pragma once
#include <string>
#include <vector>
#include "GameObjects.h"


class LevelBackground
{
public:

	LevelBackground(std::string filepath) : background_path(filepath)
	{
	};

	std::string background_path = "";
	float scrollingSpeed = 0;

	enum scrollingDirectionEnum {
		vertical,
		horizontal,
	};

	int scrollingDirection = vertical;

	struct
	{
		float w = 1.0f;
		float h = 1.0f;

		float w2 = 1.0f;
		float h2 = 1.0f;
	}scrollRect;

	virtual void OnUpdate() {};
};

class GameLevel
{
public:
	std::vector<GameObject*> levelObjects;
	std::vector<LevelBackground*> backgrounds;

	void AddBackground(LevelBackground* bg);
	void addObject(GameObject* obj);
	

};

