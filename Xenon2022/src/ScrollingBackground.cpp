#include "ScrollingBackground.h"
#include "Engine.h"

extern GameEngine::Engine engine;

	void ScrollingBackground::OnUpdate()
	{
		position.y += moveSpeed * engine.deltaTime;
		if (position.y >= 480.f)
		{
			position.y = -480.f;
		}
	}
