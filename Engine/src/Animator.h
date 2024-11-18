#pragma once
#include <string>
#include <vector>

class AnimationCoord
{
public:

	struct
	{
		int x = 0;
		int y = 0;
	}coordPosition;


	AnimationCoord() = default;

	AnimationCoord(int xParam, int yParam) {

		coordPosition.x = xParam;
		coordPosition.y = yParam;

	}


};


class Animation {
public:

	std::string tilemapPath = "";
	int textureWidth = 0;
	int textureHeight = 0;
	int frameWidth = 0;
	int frameHeight = 0;

	int spriteIndex = 0;


	float frameTime = 0.0f;

	float frameDuration = 0.1f;
	bool loop = false;

	std::vector <AnimationCoord> manual;

	struct
	{
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
	}animationRect;

	struct
	{
		int w = 0;
		int h = 0;
	}tilemapSize;

	Animation() = default;

	Animation(std::string tilemapParam, float frameDurationParam, int tilemapSizeParam[2], bool loopParam, std::vector <AnimationCoord> manualMode) {
		tilemapPath = tilemapParam;
		frameDuration = frameDurationParam;
		tilemapSize.w = tilemapSizeParam[0];
		tilemapSize.h = tilemapSizeParam[1];
		loop = loopParam;
		manual = manualMode;
	}
	virtual void OnAnimationEnd() {};
	int GetSpriteWidth();
};