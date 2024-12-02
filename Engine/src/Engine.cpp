#include "Engine.h"

#include <SDL.h>
#include <box2d.h>
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

class MyContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact) {

		void* userData = (void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		if (userData)
		{
			GameObject* m = (GameObject*)userData;

			void* userData2 = (void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
			if (userData2)
			{
				GameObject* m2 = (GameObject*)userData2;

				m->OnCollideEnter(*m2);
			}


			contact->GetFixtureA()->GetBody()->GetUserData().pointer = (uintptr_t) nullptr;
			contact->GetFixtureB()->GetBody()->GetUserData().pointer = (uintptr_t) nullptr;
		}


	}
};

SDL_Texture* windowSurface = nullptr;
SDL_Texture* background = nullptr;
SDL_Renderer* renderTarget = nullptr;
SDL_Window* window = nullptr;

//box2d setup
b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);
MyContactListener myContactListenerInstance;

float timeStep = 1.0f / 60.0f;
int32 velocityIterations = 8;
int32 positionIterations = 3;

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


		world.SetContactListener(&myContactListenerInstance);

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


			//Delete GameObjects
			for (int i = 0; i < getLevel().levelObjects.size(); ++i) {
				if (getLevel().levelObjects[i]->toBeDeleted == true) {
					getLevel().levelObjects[i]->OnDestroyed();
					getLevel().levelObjects.erase(getLevel().levelObjects.begin() + i);
				}
			}


			
			b2Body* bodyList = world.GetBodyList();

			std::vector<b2Body*> bodiesToDestroy;

			while (bodyList != nullptr)
			{

				bodiesToDestroy.push_back(bodyList);
				bodyList = bodyList->GetNext();

			}
			for (b2Body* body : bodiesToDestroy)
			{
				world.DestroyBody(body);
			}

			bodiesToDestroy.clear();

			//Update GameObjects
			for (int i = 0; i < getLevel().levelObjects.size(); ++i) {
				getLevel().levelObjects[i]->OnUpdate();

				Animation* spriteAnimation = &getLevel().levelObjects[i]->animation;

				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;

				bodyDef.position.Set(getLevel().levelObjects[i]->position.x, getLevel().levelObjects[i]->position.y);


				bodyDef.userData.pointer = (uintptr_t)getLevel().levelObjects[i];
				b2Body* body = world.CreateBody(&bodyDef);


				b2PolygonShape dynamicBox;


				float bodyWidth = getLevel().levelObjects[i]->collisionBoxSize.w;
				float bodyHeight = getLevel().levelObjects[i]->collisionBoxSize.h;

				bodyWidth = bodyWidth / 2;
				bodyHeight = bodyHeight / 2;

				b2Vec2 collisionCenter;
				collisionCenter.Set(bodyWidth, bodyHeight);

				dynamicBox.SetAsBox(bodyWidth, bodyHeight, collisionCenter, 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &dynamicBox;
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.3f;
				fixtureDef.isSensor = true;
				body->CreateFixture(&fixtureDef);

				for (int32 i = 0; i < 60; ++i)
				{
					world.Step(timeStep, velocityIterations, positionIterations);
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

		SDL_Quit();
	}

	void Engine::Initialize(GameWindow windowSettings)
	{
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


