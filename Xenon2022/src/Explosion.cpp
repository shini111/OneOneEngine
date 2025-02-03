#include "Explosion.h"

	void explosion::OnStart(){

		int textureDimentions[2] = { 5,2 };

		animation = Animation("resources/graphics/explode64.bmp", 0.1f, textureDimentions, false, {});
	}

	void explosion::OnAnimationFinish() {
		Destroy();
	}
