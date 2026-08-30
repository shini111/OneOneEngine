#include "Engine.h"


#include <cstdint>
#include <algorithm>

#include <SDL.h>
#include <box2d/box2d.h>
#include "SDL_gamecontroller.h"

Input input;

SDL_Texture* windowSurface = nullptr;
SDL_Texture* background = nullptr;
SDL_Renderer* renderTarget = nullptr;
SDL_Window* window = nullptr;

//box2d setup
b2Vec2 gravity = { 0.0f, 0.0f };
b2WorldDef worldDef = b2DefaultWorldDef();
b2WorldId worldId = b2CreateWorld(&worldDef);

float timeStep = 1.0f / 60.0f;
int subStepCount = 2;
float physicsAccumulator = 0.0f;


namespace GameEngine {
	void Engine::Initialize(GameWindow windowSettings)
	{
		//Set Gravity
		worldDef.gravity = gravity;

		windowDisplay = windowSettings;
		SDL_GameController* controller;
		int i;

		SDL_Init(SDL_INIT_VIDEO );

		SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
		
		for (i = 0; i < SDL_NumJoysticks(); ++i) {
			if (SDL_IsGameController(i)) {
				char* mapping;
				std::cout << "Index '" << i << "' is a compatible controller, named '" << SDL_GameControllerNameForIndex(i) << "'" << std::endl;
				controller = SDL_GameControllerOpen(i);
				input.setGameController(controller);
				mapping = SDL_GameControllerMapping(controller);
				std::cout << "Controller " << i << " is mapped as \"" << mapping << std::endl;
				SDL_free(mapping);
			}
			else {
				std::cout << "Index '" << i << "' is not a compatible controller." << std::endl;
			}
		}
		window = SDL_CreateWindow(windowSettings.windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSettings.windowWidth, windowSettings.windowHeight, SDL_WINDOW_OPENGL);
		renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		b2World_EnableContinuous(worldId, true);

		Update();
	}

	void Engine::setLevel(GameLevel level)
	{
		mainLevel = level;
	}

	void Engine::print(std::string printText)
	{
		std::cout << printText << std::endl;
	}

	GameLevel& Engine::getLevel()
	{
		return mainLevel;
	}

	void Engine::sensorListener()
	{
		b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);

		for (int i = 0; i < sensorEvents.beginCount; ++i)
		{
			b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
			void* myUserData = b2Shape_GetUserData(beginTouch->visitorShapeId);
			if (myUserData)
			{
				GameObject* m = static_cast<GameObject*>(myUserData);

				void* myUserData2 = b2Shape_GetUserData(beginTouch->sensorShapeId);
				std::cout << "Sensor detected collision with object group: " << m->objectGroup << std::endl;

				if (myUserData2)
				{
					GameObject* m2 = static_cast<GameObject*>(myUserData2);
					m->OnCollideEnter(*m2);
					if (m2->objectGroup == "player")
					{
						std::cout << "Sensor detected collision with object group: " << m2->objectGroup << std::endl;
					}
				}
			}
		}
	}

	void Engine::contactListener() {
		b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

		if (contactEvents.beginCount > 0) {
			//std::cout << "Contact Events Begin Count: " << contactEvents.beginCount << std::endl;
		}

		for (int i = 0; i < contactEvents.beginCount; ++i)
		{
			b2ContactBeginTouchEvent* beginTouch = contactEvents.beginEvents + i;
			void* myUserData = b2Shape_GetUserData(beginTouch->shapeIdA);
			void* myUserData2 = b2Shape_GetUserData(beginTouch->shapeIdB);

			if (myUserData && myUserData2)
			{
				GameObject* m = static_cast<GameObject*>(myUserData);
				GameObject* m2 = static_cast<GameObject*>(myUserData2);

				// Box2D doesn't guarantee which shape ends up as A vs B for a given
				// contact, so both sides need to be notified. Only calling
				// m->OnCollideEnter(*m2) meant whichever object type has no reaction to
				// the contact (e.g. a bullet, which never overrides OnCollideEnter) "won"
				// purely by chance whenever Box2D happened to put it in the A slot, and
				// the object that should have reacted never even found out about it.
				m->OnCollideEnter(*m2);
				m2->OnCollideEnter(*m);
			}
		}
	}

	SDL_Texture* Engine::LoadTexture(std::string filePath, SDL_Renderer* renderTarget) {
		SDL_Texture* texture = nullptr;
		SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
		if (surface == NULL)
			std::cout << "Error1" << std::endl;
		else
		{
			SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
			texture = SDL_CreateTextureFromSurface(renderTarget, surface);
			if (texture == NULL)
				std::cout << "Error2" << std::endl;
		}
		SDL_FreeSurface(surface);

		return texture;
	}

	SDL_Surface* Engine::OptimizedSurface(std::string filePath, SDL_Surface* windowSurface) {
		SDL_Surface* optimizedSurface = nullptr;
		SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());

		if (surface == nullptr) {
			std::cout << "Error loading image: " << filePath << std::endl;
		}
		else {
			optimizedSurface = SDL_ConvertSurface(surface, windowSurface->format, 0);
			if (optimizedSurface == nullptr) {
				std::cout << "Error optimizing surface: " << filePath << std::endl;
			}
			SDL_FreeSurface(surface);
			return optimizedSurface;
		}
	}

	void Engine::Update()
	{
		int prevTime = 0;
		int currentTime = 0;
		bool isRunning = true;
		SDL_Event event;

		while (isRunning) {
			prevTime = currentTime;
			currentTime = SDL_GetTicks();
			deltaTime = (currentTime - prevTime) / 1000.0f;


			for (int i = 0; i < getLevel().backgrounds.size(); ++i)
			{
				getLevel().backgrounds[i]->OnUpdate();
			}

			SDL_RenderClear(renderTarget);

			//Multiple background layers
			for (int i = 0; i < getLevel().backgrounds.size(); ++i)
			{
				background = LoadTexture(getLevel().backgrounds[i]->background_path, renderTarget);

				SDL_Rect position;
				SDL_Rect scrollPosition;


				position.x = 0;
				position.y = 0;

				if (SDL_QueryTexture(background, NULL, NULL, &position.w, &position.h) != 0) {
					std::cerr << "SDL_QueryTexture failed: " << SDL_GetError() << std::endl;
					continue;
				}

				scrollPosition.x = getLevel().backgrounds[i]->position.x;

				scrollPosition.y = getLevel().backgrounds[i]->position.y;

				scrollPosition.w = windowDisplay.windowWidth;
				scrollPosition.h = windowDisplay.windowHeight;


				SDL_RenderCopy(renderTarget, background, &position, &scrollPosition);

				SDL_DestroyTexture(background);
			}

			// Delete GameObjects

			for (int i = getLevel().levelObjects.size() - 1; i >= 0; --i) {
				if (getLevel().levelObjects[i]->toBeDeleted == true) {
					getLevel().levelObjects[i]->OnDestroyed();

					if (getLevel().levelObjects[i]->bodyId != nullptr)
					{
						b2DestroyBody(*getLevel().levelObjects[i]->bodyId);
					}
					else
					{
						std::cout << "Spawner delete" << i << std::endl;
					}
					delete getLevel().levelObjects[i];
					getLevel().levelObjects.erase(getLevel().levelObjects.begin() + i);
				}
			}

			//Manage Created Objects
			for (int i = 0; i < getLevel().levelObjects.size(); ++i) {
				GameObject* obj = getLevel().levelObjects[i];

				obj->OnUpdate();

				Animation* spriteAnimation = &obj->animation;

				if (getLevel().levelObjects[i]->hasBox2d && !getLevel().levelObjects[i]->box2dCreated)
				{
					float bodyWidth = getLevel().levelObjects[i]->collisionBoxSize.w;
					float bodyHeight = getLevel().levelObjects[i]->collisionBoxSize.h;
					bodyWidth = bodyWidth / 2.0f;
					bodyHeight = bodyHeight / 2.0f;


					b2BodyDef* bodyDef = new b2BodyDef;
					*bodyDef = b2DefaultBodyDef();
					bodyDef->type = b2_dynamicBody;
					bodyDef->position = { getLevel().levelObjects[i]->position.x, getLevel().levelObjects[i]->position.y };
					bodyDef->userData = getLevel().levelObjects[i];


					b2BodyId* bodyId = new b2BodyId;
					*bodyId = b2CreateBody(worldId, bodyDef);

					// Centered on the body origin. The body origin is set to
					// obj->position above, and that is also exactly where the sprite is
					// drawn (spritePos below is built straight from position). This used
					// to be offset by a full half-width and half-height
					// ({bodyWidth, bodyHeight}, with an extra 4*pi "rotation" that was
					// actually a no-op), shifting every object's hitbox away from its
					// visible sprite by an amount that depended on that object's own
					// collision size. Two sprites could overlap on screen while their
					// real hitboxes were nowhere near each other, or the reverse.
					b2Polygon* dynamicBox = new b2Polygon;
					*dynamicBox = b2MakeBox(bodyWidth, bodyHeight);


					b2ShapeDef* shapeDef = new b2ShapeDef;
					*shapeDef = b2DefaultShapeDef();
					shapeDef->density = 1.0f;
					shapeDef->friction = 0.3f;

					shapeDef->userData = getLevel().levelObjects[i];

					shapeDef->enableContactEvents = true;

					b2ShapeId* shapeId = new b2ShapeId;
					*shapeId = b2CreatePolygonShape(*bodyId, shapeDef, dynamicBox);

					getLevel().levelObjects[i]->bodyId = bodyId;
					getLevel().levelObjects[i]->bodyDef = bodyDef;
					getLevel().levelObjects[i]->shapeId = shapeId;
					getLevel().levelObjects[i]->shapeDef = shapeDef;
					getLevel().levelObjects[i]->boxCollision = dynamicBox;

					obj->box2dCreated = true;
				}

				if (spriteAnimation->tilemapPath != "") {

					if (spriteAnimation->manual.empty() == true)
					{
						SDL_Texture* sprite = LoadTexture(spriteAnimation->tilemapPath, renderTarget);

						SDL_QueryTexture(sprite, NULL, NULL, &spriteAnimation->textureWidth, &spriteAnimation->textureHeight);

						spriteAnimation->frameWidth = spriteAnimation->textureWidth / spriteAnimation->tilemapSize.w;
						spriteAnimation->frameHeight = spriteAnimation->textureHeight / spriteAnimation->tilemapSize.h;

						spriteAnimation->animationRect.w = spriteAnimation->frameWidth;
						spriteAnimation->animationRect.h = spriteAnimation->frameHeight;

						SDL_Rect spriteRect;

						SDL_Rect spritePos;
						spritePos.x = getLevel().levelObjects[i]->position.x;
						spritePos.y = getLevel().levelObjects[i]->position.y;
						spritePos.w = spriteAnimation->frameWidth;
						spritePos.h = spriteAnimation->frameHeight;


						spriteAnimation->frameTime += deltaTime;

						if (spriteAnimation->frameTime > spriteAnimation->frameDuration) {
							spriteAnimation->frameTime = 0;

							spriteAnimation->animationRect.x += spriteAnimation->frameWidth;

							if (spriteAnimation->animationRect.x >= spriteAnimation->textureWidth) {
								spriteAnimation->animationRect.x = 0;
								spriteAnimation->animationRect.y += spriteAnimation->frameHeight;

								if (spriteAnimation->animationRect.y >= spriteAnimation->textureHeight) {
									if (spriteAnimation->loop) {
										spriteAnimation->animationRect.y = 0;
									}
									else {
										spriteAnimation->animationRect.x = spriteAnimation->textureWidth - spriteAnimation->frameWidth;
										spriteAnimation->animationRect.y = spriteAnimation->textureHeight - spriteAnimation->frameHeight;
									}
									getLevel().levelObjects[i]->OnAnimationFinish();
								}
							}

						}

						spriteRect.x = spriteAnimation->animationRect.x;
						spriteRect.y = spriteAnimation->animationRect.y;
						spriteRect.w = spriteAnimation->animationRect.w;
						spriteRect.h = spriteAnimation->animationRect.h;

						if (getLevel().levelObjects[i]->visible) {


							SDL_Color myColor = { getLevel().levelObjects[i]->colorChange.r, getLevel().levelObjects[i]->colorChange.g, getLevel().levelObjects[i]->colorChange.b,255 };

							SDL_SetTextureColorMod(sprite, myColor.r, myColor.g, myColor.b);

							SDL_RenderCopyEx(renderTarget, sprite, &spriteRect, &spritePos, getLevel().levelObjects[i]->rotation, NULL, SDL_FLIP_NONE);
						}

						SDL_DestroyTexture(sprite);
					}
					else if (spriteAnimation->manual.empty() == false)
					{

						SDL_Texture* sprite = LoadTexture(spriteAnimation->tilemapPath, renderTarget);

						SDL_QueryTexture(sprite, NULL, NULL, &spriteAnimation->textureWidth, &spriteAnimation->textureHeight);

						spriteAnimation->frameWidth = spriteAnimation->textureWidth / spriteAnimation->tilemapSize.w;
						spriteAnimation->frameHeight = spriteAnimation->textureHeight / spriteAnimation->tilemapSize.h;

						spriteAnimation->animationRect.w = spriteAnimation->frameWidth;
						spriteAnimation->animationRect.h = spriteAnimation->frameHeight;

						SDL_Rect spriteRect;

						SDL_Rect spritePos;
						spritePos.x = getLevel().levelObjects[i]->position.x;
						spritePos.y = getLevel().levelObjects[i]->position.y;
						spritePos.w = spriteAnimation->frameWidth;
						spritePos.h = spriteAnimation->frameHeight;

						spriteAnimation->frameTime += deltaTime;


						if (spriteAnimation->frameTime > spriteAnimation->frameDuration)
						{
							spriteAnimation->frameTime = 0;

							if (spriteAnimation->spriteIndex < spriteAnimation->manual.size() - 1)
							{
								spriteAnimation->spriteIndex++;
							}
							else
							{
								if (spriteAnimation->loop) {
									spriteAnimation->spriteIndex = 0;
								}
								getLevel().levelObjects[i]->OnAnimationFinish();
							}
						}

						if (spriteAnimation->spriteIndex < spriteAnimation->manual.size())
						{
							spriteAnimation->animationRect.x = spriteAnimation->manual[spriteAnimation->spriteIndex].coordPosition.x * spriteAnimation->frameWidth;
							spriteAnimation->animationRect.y = spriteAnimation->manual[spriteAnimation->spriteIndex].coordPosition.y * spriteAnimation->frameHeight;
						}


						spriteRect.x = spriteAnimation->animationRect.x;
						spriteRect.y = spriteAnimation->animationRect.y;
						spriteRect.w = spriteAnimation->animationRect.w;
						spriteRect.h = spriteAnimation->animationRect.h;

						if (getLevel().levelObjects[i]->visible) {
							SDL_Color myColor = { getLevel().levelObjects[i]->colorChange.r, getLevel().levelObjects[i]->colorChange.g, getLevel().levelObjects[i]->colorChange.b,255 };

							SDL_SetTextureColorMod(sprite, myColor.r, myColor.g, myColor.b);
							SDL_RenderCopyEx(renderTarget, sprite, &spriteRect, &spritePos, getLevel().levelObjects[i]->rotation, NULL, SDL_FLIP_NONE);
						}
						SDL_DestroyTexture(sprite);
					}
				}

				// Drive the box2D body from the object's own authoritative position using
				// velocity instead of an instant SetTransform "teleport". Box2D's own docs
				// say SetTransform "acts as a teleport" -- it has no notion of the path
				// taken between frames, so continuous collision never sees a fast object
				// sweep past a thin collider between two teleports, and the hit is just
				// missed. Setting velocity instead lets Box2D actually integrate the
				// motion during the physics step below, so continuous collision can catch
				// it. The body gets snapped back to the exact authoritative position
				// afterwards (see below the step loop), so the game's own movement code
				// still fully owns where things end up -- Box2D is only used here to
				// detect what the object would have hit along the way.
				if (obj->bodyId != nullptr)
				{
					if (b2Body_IsValid(*obj->bodyId))
					{
						b2Vec2 targetPosition{ obj->position.x, obj->position.y };
						b2Vec2 currentBodyPosition = b2Body_GetPosition(*obj->bodyId);

						b2Vec2 velocity{ 0.f, 0.f };
						if (deltaTime > 0.0f)
						{
							velocity.x = (targetPosition.x - currentBodyPosition.x) / deltaTime;
							velocity.y = (targetPosition.y - currentBodyPosition.y) / deltaTime;
						}

						b2Body_SetLinearVelocity(*obj->bodyId, velocity);
					}
				}
			}

			// Step physics on a fixed-timestep accumulator built from the real frame
			// delta, so simulated time matches real elapsed time regardless of framerate.
			// This used to always advance exactly one hardcoded timeStep per rendered
			// frame, which only lined up with real time as long as vsync kept frames at
			// exactly that rate -- any stall or refresh-rate mismatch let it drift.
			// Clamp so a single slow frame (a stall, asset loading) can't dump a huge
			// deltaTime into the accumulator and trigger a burst of world steps at once.
			physicsAccumulator += (deltaTime < 0.25f) ? deltaTime : 0.25f;
			while (physicsAccumulator >= timeStep)
			{
				b2World_Step(worldId, timeStep, subStepCount);
				contactListener();
				physicsAccumulator -= timeStep;
			}

			// Now that Box2D has had a chance to sweep each body's velocity-driven motion
			// (so continuous collision could catch anything a fast object would have
			// hit), snap every body's transform back to its object's own authoritative
			// position. This keeps next frame's velocity calculation exactly in sync with
			// the game's manual position tracking, and stops any leftover velocity from
			// making a body drift on its own between frames.
			for (int i = 0; i < getLevel().levelObjects.size(); ++i)
			{
				GameObject* syncObj = getLevel().levelObjects[i];
				if (syncObj->bodyId != nullptr && b2Body_IsValid(*syncObj->bodyId))
				{
					b2Vec2 position{ syncObj->position.x, syncObj->position.y };
					b2Rot rotation{ syncObj->bodyDef->rotation.c, syncObj->bodyDef->rotation.s };

					b2Body_SetTransform(*syncObj->bodyId, position, rotation);
					b2Body_SetLinearVelocity(*syncObj->bodyId, b2Vec2{ 0.f, 0.f });
				}
			}

			SDL_RenderPresent(renderTarget);

			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_QUIT) {
					isRunning = false;
				}
			}
		}

		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderTarget);

		window = nullptr;
		windowSurface = nullptr;
		background = nullptr;
		renderTarget = nullptr;

		b2DestroyWorld(worldId);
		worldId = b2_nullWorldId;

		SDL_Quit();
	}
}




