#include "Animation.h"

int Animation::GetSpriteWidth()
{
	int ret = animationRect.w / tilemapSize.w;
	return ret;
}