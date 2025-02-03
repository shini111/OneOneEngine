#pragma once
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
		float x = 1.0f;
		float y = 1.0f;

		float w2 = 1.0f;
		float h2 = 1.0f;
	}position;

	virtual void OnUpdate() {};
};