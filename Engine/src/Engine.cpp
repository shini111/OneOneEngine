#include "Engine.h"


#include <cstdint>

#include <SDL.h>
#include <box2d/box2d.h>
#include "SDL_gamecontroller.h"


SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, Uint32 flags);
SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface);

Input input;

static SDL_Texture* LoadTexture(std::string filePath, SDL_Renderer* renderTarget) {
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

static SDL_Surface* OptimizedSurface(std::string filePath, SDL_Surface* windowSurface) {
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
			if (myUserData)
			{
				GameObject* m = static_cast<GameObject*>(myUserData);
				//std::cout << m->objectGroup << std::endl;
				void* myUserData2 = b2Shape_GetUserData(beginTouch->shapeIdB);
				//std::cout << "Collision A: " << m->objectGroup << " " << m->collisionBoxSize.w << " " << m->collisionBoxSize.h;
				
				if (myUserData2)
				{
					GameObject* m2 = static_cast<GameObject*>(myUserData2);
					m->OnCollideEnter(*m2);
					//std::cout << " Collision B: " << m2->objectGroup << " " << m2->collisionBoxSize.w << " " << m2->collisionBoxSize.h << std::endl;
				}
			}
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

			for (int i = getLevel().levelObjects.size() - 1; i >= 0; --i)
			{
				auto obj = getLevel().levelObjects[i];
				if (obj->bodyId != nullptr)
				{
					b2DestroyBody(*obj->bodyId);
					delete obj->bodyDef;
					delete obj->bodyId;
					delete obj->boxCollision;
					delete obj->shapeId;
					delete obj->shapeDef;
				}
			}

			//Manage Created Objects
			for (int i = 0; i < getLevel().levelObjects.size(); ++i) {
				GameObject* obj = getLevel().levelObjects[i];

				obj->OnUpdate();

				Animation* spriteAnimation = &obj->animation;

				if (getLevel().levelObjects[i]->hasBox2d)
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

					b2Vec2 bodyCenter{ bodyWidth, bodyHeight };
					float angle = 4.0f;

					b2Polygon* dynamicBox = new b2Polygon;
					*dynamicBox = b2MakeOffsetBox(bodyWidth, bodyHeight, bodyCenter, b2MakeRot(angle * b2_pi));


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


							SDL_Color myColor = { getLevel().levelObjects[i]->modulate.r, getLevel().levelObjects[i]->modulate.g, getLevel().levelObjects[i]->modulate.b,255 };

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
							SDL_Color myColor = { getLevel().levelObjects[i]->modulate.r, getLevel().levelObjects[i]->modulate.g, getLevel().levelObjects[i]->modulate.b,255 };

							SDL_SetTextureColorMod(sprite, myColor.r, myColor.g, myColor.b);
							SDL_RenderCopyEx(renderTarget, sprite, &spriteRect, &spritePos, getLevel().levelObjects[i]->rotation, NULL, SDL_FLIP_NONE);
						}
						SDL_DestroyTexture(sprite);
					}
				}


			}

			b2World_Step(worldId, timeStep, subStepCount);
			contactListener();

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




