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
// int32 velocityIterations = 8;
// int32 positionIterations = 3;

InputEnum Input::mapSDLKeyToInputEnum(SDL_Keycode key) {
	switch (key) {
	case SDLK_w: return InputEnum::North;
	case SDLK_s: return InputEnum::South;
	case SDLK_a: return InputEnum::West;
	case SDLK_d: return InputEnum::East;
	case SDLK_UP: return InputEnum::DNorth;
	case SDLK_DOWN: return InputEnum::DSouth;
	case SDLK_LEFT: return InputEnum::DWest;
	case SDLK_RIGHT: return InputEnum::DEast;
		// Add more mappings as needed
	default: return InputEnum::LeftThumbstick; // Default case
	}
}

InputEnum Input::mapSDLButtonToInputEnum(Uint8 button) {
	switch (button) {
	case SDL_CONTROLLER_BUTTON_A: return InputEnum::ButtonA;
	case SDL_CONTROLLER_BUTTON_B: return InputEnum::ButtonB;
	case SDL_CONTROLLER_BUTTON_X: return InputEnum::ButtonX;
	case SDL_CONTROLLER_BUTTON_Y: return InputEnum::ButtonY;
		// Add more mappings as needed
	default: return InputEnum::LeftThumbstick; // Default case
	}
}

InputEnum Input::getKeyPressed() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			return mapSDLKeyToInputEnum(event.key.keysym.sym);
		}
		if (event.type == SDL_CONTROLLERBUTTONDOWN) {
			return mapSDLButtonToInputEnum(event.cbutton.button);
		}
	}
	return inputEnum; // Return the current inputEnum if no key is pressed
}

void Input::setGameController(SDL_GameController* controller) {
	gameController = controller;
}

bool Input::IsGamepadButtonPressed(GamepadButton button, bool singleClick) {
	if (!gameController) return false;

	SDL_GameControllerButton sdlButton;
	switch (button) {
	case GamepadButton::A: sdlButton = SDL_CONTROLLER_BUTTON_A; break;
	case GamepadButton::B: sdlButton = SDL_CONTROLLER_BUTTON_B; break;
	case GamepadButton::X: sdlButton = SDL_CONTROLLER_BUTTON_X; break;
	case GamepadButton::Y: sdlButton = SDL_CONTROLLER_BUTTON_Y; break;
	case GamepadButton::DPadLeft: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_LEFT; break;
	case GamepadButton::DPadRight: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_RIGHT; break;
	case GamepadButton::DPadUp: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_UP; break;
	case GamepadButton::DPadDown: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_DOWN; break;
	default: return false;
	}

	static std::map<GamepadButton, bool> buttonState;
	bool isPressed = SDL_GameControllerGetButton(gameController, sdlButton) != 0;

	if (singleClick) {
		if (isPressed && !buttonState[button]) {
			buttonState[button] = true;
			return true;
		}
		if (!isPressed) {
			buttonState[button] = false;
		}
		return false;
	}
	return isPressed;
}

namespace GameEngine {

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


			for (int i = 0; i < getLevel().background.size(); ++i)
			{
				if (getLevel().background[i].scrollingDirection == getLevel().background[i].vertical) {

					getLevel().background[i].scrollRect.h += getLevel().background[i].scrollingSpeed * deltaTime;

					if (getLevel().background[i].scrollingSpeed > 0)
						getLevel().background[i].scrollRect.h2 = getLevel().background[i].scrollRect.h - windowDisplay.windowHeight;
					else if (getLevel().background[i].scrollingSpeed < 0)
						getLevel().background[i].scrollRect.h2 = getLevel().background[i].scrollRect.h + windowDisplay.windowHeight;
				}
				else {

					getLevel().background[i].scrollRect.w += getLevel().background[i].scrollingSpeed * deltaTime;

					if (getLevel().background[i].scrollingSpeed > 0)
						getLevel().background[i].scrollRect.w2 = getLevel().background[i].scrollRect.w - windowDisplay.windowWidth;
					else if (getLevel().background[i].scrollingSpeed < 0)
						getLevel().background[i].scrollRect.w2 = getLevel().background[i].scrollRect.w + windowDisplay.windowWidth;
				}

			}
			SDL_RenderClear(renderTarget);
			//Multiple background layers
			for (int i = 0; i < getLevel().background.size(); ++i)
			{
				background = LoadTexture(getLevel().background[i].background_path, renderTarget);

				SDL_Rect scrollRect;
				SDL_Rect scrollPosition;
				SDL_Rect scrollPosition2;

				scrollRect.x = 0;
				scrollRect.y = 0;

				SDL_QueryTexture(background, NULL, NULL, &scrollRect.w, &scrollRect.h);

				scrollPosition.x = getLevel().background[i].scrollRect.w;
				scrollPosition2.x = getLevel().background[i].scrollRect.w2;

				scrollPosition.y = getLevel().background[i].scrollRect.h;
				scrollPosition2.y = getLevel().background[i].scrollRect.h2;

				scrollPosition.w = scrollPosition2.w = windowDisplay.windowWidth;
				scrollPosition.h = scrollPosition2.h = windowDisplay.windowHeight;


				if (getLevel().background[i].scrollingDirection == getLevel().background[i].vertical) {

					if (getLevel().background[i].scrollRect.h >= scrollPosition.h || getLevel().background[i].scrollRect.h <= -scrollPosition.h)
					{
						getLevel().background[i].scrollRect.h = 0;
					}
				}
				else {

					if (getLevel().background[i].scrollRect.w >= scrollPosition.w || getLevel().background[i].scrollRect.w <= -scrollPosition.w)
					{
						getLevel().background[i].scrollRect.w = 0;
					}
				}
				SDL_RenderCopy(renderTarget, background, &scrollRect, &scrollPosition);
				SDL_RenderCopy(renderTarget, background, &scrollRect, &scrollPosition2);

				SDL_DestroyTexture(background);
			}



			// Delete GameObjects

			for (int i = getLevel().levelObjects.size() - 1; i >= 0; --i) {
				if (getLevel().levelObjects[i]->toBeDeleted == true) {
					getLevel().levelObjects[i]->OnDestroyed();

					if (i > 1)
					{
						b2DestroyBody(*getLevel().levelObjects[i]->bodyId);
						delete getLevel().levelObjects[i]->bodyDef;
						delete getLevel().levelObjects[i]->bodyId;
					}
					else
					{
						std::cout << "Spawner delete" << i << std::endl;
					}
					delete getLevel().levelObjects[i];
					getLevel().levelObjects.erase(getLevel().levelObjects.begin() + i);
				}
			}

			for (int i = getLevel().levelObjects.size()-1; i >= 0; --i)
			{
				auto obj = getLevel().levelObjects[i];
				if (obj->bodyId != nullptr)
				{
					b2DestroyBody(*obj->bodyId);
				}
			}

			//Manage Created Objects
			for (int i = 0; i < getLevel().levelObjects.size(); ++i) {
				GameObject* obj = getLevel().levelObjects[i];

				obj->OnUpdate();

				Animation* spriteAnimation = &obj->animation;


				//THIS IS TO IGNORE SPAWNERS. THE FIRST TWO OBJECTS IN THE LEVEL OBJECTS VECTOR ARE SPAWNERS
					//This is a just a workaround for now. I will implement a better way to handle this later, because i need to create
					//a bool variable for objects for the user to want or not a box2d body but right now i dont have time for that.

				if (i > 1)
				{
					float bodyWidth = getLevel().levelObjects[i]->collisionBoxSize.w;
					float bodyHeight = getLevel().levelObjects[i]->collisionBoxSize.h;
					bodyWidth = bodyWidth / 2.0f;
					bodyHeight = bodyHeight / 2.0f;

					
					b2BodyDef* bodyDef = new b2BodyDef;
					*bodyDef = b2DefaultBodyDef();
					bodyDef->type = b2_dynamicBody;
					bodyDef->position = { getLevel().levelObjects[i]->position.x, getLevel().levelObjects[i]->position.y };
					//bodyDef-> = getLevel().levelObjects[i]->isBullet;
					bodyDef->userData = getLevel().levelObjects[i];


					b2BodyId* bodyId = new b2BodyId;
					*bodyId = b2CreateBody(worldId, bodyDef);

					b2Vec2 bodyCenter{ bodyWidth, bodyHeight };
					float angle = 4.0f;

					b2Polygon* dynamicBox = new b2Polygon;
					//*dynamicBox = b2MakeBox(bodyWidth, bodyHeight);
					*dynamicBox = b2MakeOffsetBox(bodyWidth, bodyHeight, bodyCenter, b2MakeRot(angle * b2_pi));


					b2ShapeDef* shapeDef = new b2ShapeDef;
					*shapeDef = b2DefaultShapeDef();
					shapeDef->density = 1.0f;
					shapeDef->friction = 0.3f;

					//shapeDef->enableSensorEvents = getLevel().levelObjects[i]->hasSense;

					//shapeDef->enableSensorEvents = true;
					//shapeDef->isSensor = getLevel().levelObjects[i]->hasSense;

					//shapeDef->enableContactEvents = true;

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
				
				//WORLD STEP DOESNT MAKE SENSE USING IT IN A OBJECT UPDATE LOOP IT SHOULD BE IN WORLD UPDATE
				

// 				b2World_Step(worldId, timeStep, subStepCount);
// 				sensorListener();
// 				contactListener();


// 				for (int32_t i = 0; i < 90; ++i) {
// 					if (B2_IS_NULL(worldId) != 0) {
// 						std::cerr << "Invalid worldId detected." << std::endl;
// 						break;
// 					}
// 					else {
// 						try {
// 							b2World_Step(worldId, timeStep, subStepCount);
// 							sensorListener();
// 							contactListener();
// 						}
// 						catch (const std::exception& e) {
// 							std::cerr << "Exception during b2World_Step: " << e.what() << std::endl;
// 							__debugbreak();
// 						}
// 					}
// 				}

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

// 			for (int32_t i = 0; i < 60; ++i) {
// 				if (B2_IS_NULL(worldId) != 0) {
// 					std::cerr << "Invalid worldId detected." << std::endl;
// 					break;
// 				}
// 				else {
// 					try {
// 						b2World_Step(worldId, timeStep, subStepCount);
// 						contactListener();
// 						//sensorListener();
// 					}
// 					catch (const std::exception& e) {
// 						std::cerr << "Exception during b2World_Step: " << e.what() << std::endl;
// 						__debugbreak();
// 					}
// 				}
// 			}

			SDL_RenderPresent(renderTarget);

			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_QUIT) {
					isRunning = false;
				}
			}


			/////////////////////////DEBUG TESTING/////////////////////////
			//You can use this to debug the player position, i was using this to test if the box2d setup was working 
			// and it seems to be updating the box2d variables correctly
			//Debug player position
			//std::cout << "Position absolute: " << getLevel().levelObjects[2]->position.x << " " << getLevel().levelObjects[2]->position.y << std::endl;
			//std::cout << "Position box: " << getLevel().levelObjects[2]->bodyDef->position.x << " " << getLevel().levelObjects[2]->bodyDef->position.y << std::endl;
			//Debug player group
			//std::cout << "Position: " << static_cast<GameObject*>(getLevel().levelObjects[2]->bodyDef->userData)->objectGroup << std::endl;
			
			//Debug enemy position - Always debugs one of the enemies positions. I was using this to test if the box2d setup was working
			// on new objects created after the game started and it seems to be working correctly, so the problem is related to the sensor itself i think
			//if (getLevel().levelObjects.size() > 4)
			//{
			//	std::cout << "Position: " << getLevel().levelObjects[3]->bodyDef->position.x << " " << getLevel().levelObjects[3]->bodyDef->position.y << std::endl;
			//}


			//Debug collision box size from player its being created with 64 by 64. Its bigger than the other collision that is being detected that has the size of 32x32
			//std::cout << "Collision Box: " << getLevel().levelObjects[2]->collisionBoxSize.w << " " << getLevel().levelObjects[2]->collisionBoxSize.h << std::endl;


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


// 	bool Engine::b2OverlapResultFcn(b2ShapeId id) {
// 		
// 		GameObject* obj = static_cast<GameObject*>(b2Shape_GetUserData(id));
// 		if (obj != nullptr && obj->objectGroup)
// 		{
// 			return true;
// 		}
// 		// continue the query
// 		return true;
// 	}
}

void GameLevel::setLayerSize(int layerSize)
{
	background.resize(layerSize);
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

int Animation::GetSpriteWidth()
{
	int ret = animationRect.w / tilemapSize.w;
	return ret;
}

