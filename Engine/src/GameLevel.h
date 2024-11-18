#pragma once
#include <string>
#include <vector>
#include "GameObjects.h"


class LevelBackground
{
public:
	std::string background_path = "";
	float scrollingSpeed = 0;

	enum scrollingDirectionEnum {
		vertical,
		horizontal,
	};

	int scrollingDirection = vertical;

	struct
	{
		float w = 0.0f;
		float h = 0.0f;

		float w2 = 0.0f;
		float h2 = 0.0f;
	}scrollRect;
};

class GameLevel
{
public:
	std::vector<GameObject*> levelObjects;
	std::vector<LevelBackground> background;

	void setLayerSize(int layerSize);
	void addObject(GameObject* obj);
};

