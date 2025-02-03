#pragma once
#include <string>
#include <vector>
#include "GameObjects.h"
#include "LevelBackground.h"


class GameLevel
{
public:
	std::vector<GameObject*> levelObjects;
	std::vector<LevelBackground*> backgrounds;

	void AddBackground(LevelBackground* bg);
	void addObject(GameObject* obj);
	

};

