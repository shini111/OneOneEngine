#pragma once
#include "Engine.h"

class ScrollingBackground : public LevelBackground
{
public:
	ScrollingBackground(std::string filepath) : LevelBackground(filepath)
	{
	}

	float moveSpeed = 100.f;

	void OnUpdate() override;
};
