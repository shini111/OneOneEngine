#include "GameLevel.h"

void GameLevel::AddBackground(LevelBackground* bg)
{
	backgrounds.push_back(bg);
}

void GameObject::Destroy()
{
	toBeDeleted = true;
}

void GameLevel::addObject(GameObject* obj)
{
	levelObjects.push_back(obj);
	obj->OnStart();
}