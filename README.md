# Game Engine and Xenon 2000 Clone

A small 2D game engine written from scratch in C++, paired with a playable clone of *Xenon 2000*, the vertically scrolling shoot 'em up. This is the earlier, SDL rendered version of the project, built as coursework for the Undergraduate in Games and Multimedia programme at Politecnico de Leiria, and later revisited to track down and fix a set of physics and collision bugs that were still open at submission time.

## Overview

The solution is split into two Visual Studio projects. `Engine` is a static library that owns the game loop, resource management, rendering, input, and physics, it has no dependency on anything outside itself. `Xenon2022` is the executable that consumes the engine's public API to build an actual game, and never touches SDL2 or Box2D directly.

Unlike the later version of this project, rendering here goes through `SDL_Renderer` and `SDL_RenderCopyEx` rather than raw OpenGL and GLSL shaders. The engine architecture, the object model, and the Box2D based collision system are otherwise the same idea, just an earlier and simpler pass at it.

The game puts the player in control of a spaceship at the bottom of the screen, firing missiles at waves of enemies descending from the top. The graphical assets are the original freeware set released by The Bitmap Brothers together with PC Format magazine for *Xenon 2000*.

## Features

**Engine**

- Component style game objects built around a shared `GameObject` base class, with position, collision box size, color modulation, rotation, and visibility handled generically.
- A `GameLevel` class that stores every object and background layer for a level, with `Engine::setLevel()` used to load one.
- Sprite tilemap rendering with both automatic frame cycling and manual, hand picked animation frames through the `Animation` class.
- Two layer parallax scrolling backgrounds through `LevelBackground`.
- Gamepad input handling through SDL2, including controller detection and button mapping.
- Physics and collision handling through Box2D.

**Xenon 2000 clone**

- A player spaceship that moves in all four directions and fires missiles.
- Two enemy types, rushers that move straight down the screen and loners that move horizontally while firing projectiles back at the player, each spawned by its own timer based spawner.
- An explosion effect on hit, giving the player visual feedback that a shot landed.
- A galaxy backdrop with a second scrolling starfield layer for the parallax effect.

This version does not yet have the health bar UI, companion ships, power ups, or drone packs that were added in the later OpenGL rewrite of the project.

## Tech stack

- C++ with Visual Studio 2022
- SDL2 for window creation, input, and rendering
- Box2D 3.1 for collision detection
- GLM for math

## Getting started

The project targets Windows and Visual Studio 2022.

1. Open `Xenon2022Engine.sln`.
2. Build the solution (this builds `Engine` as a static library first, then links it into `Xenon2022`).
3. Run `Xenon2022.exe`.

A gamepad is required to play, the game reads movement and firing through SDL's controller API and does not currently have a keyboard fallback. Move with the D-pad and fire missiles with the A button.

## Physics and collision fixes

The original report already flagged that objects were sometimes not detected during collisions, and guessed a missing fixed timestep was the likely cause. Going back through the physics code turned up several compounding issues, all now fixed:

- **Physics stepping.** The world step ran once per rendered frame using a hardcoded timestep, so the simulated time only matched real elapsed time as long as vsync kept frames landing exactly on schedule. It now runs through a fixed timestep accumulator built from the real frame delta, so the simulation speed stays consistent regardless of framerate.
- **Teleporting bodies.** Every object's Box2D body was moved with `b2Body_SetTransform`, which Box2D's own docs describe as an instant teleport with no notion of the path taken. That meant continuous collision could never catch a fast object sweeping past a thin collider between frames. Bodies are now driven with linear velocity computed from the object's own movement each frame, letting Box2D's continuous collision actually see the motion, then snapped back to the object's authoritative position after the physics step so the existing manual movement code still owns where everything ends up.
- **Off center hitboxes.** Collision shapes were built with an offset equal to a full half width and half height of the object, shifting every hitbox diagonally away from where its sprite was actually drawn, by different amounts depending on each object's size. Shapes are now centered on the body origin, matching the sprite.
- **One sided contact notification.** The contact listener only called `OnCollideEnter` on whichever object Box2D happened to place in the "A" slot of a contact, and Box2D does not guarantee that ordering. Both objects in a contact are now notified, matching how engines like Unity handle collision callbacks.
- **Uninitialized pointers.** `GameObject`'s Box2D body and shape pointers had no default value, leaving them holding garbage memory for any object type that never creates a Box2D body. They are now initialized to `nullptr`.

## Windows and SDL build note

Fixing the timestep introduced a build error that is worth documenting, since it can resurface if the accumulator code is touched again. SDL2 pulls in `windows.h` on the way into `Engine.cpp`, and `windows.h` defines `min` and `max` as macros unless `NOMINMAX` is set first. Those macros do plain text substitution, so a call written as `std::min(a, b)` gets rewritten by the preprocessor into `std::(a, b)` before the compiler ever sees it, since the macro has no idea `min` was meant to be scoped under `std::`. The clamp in the accumulator is written as a plain ternary instead, which sidesteps the problem entirely regardless of include order.

## Assets and credits

The *Xenon 2000* graphics are freeware, originally released by The Bitmap Brothers in association with PC Format magazine, archived at [archive.org](https://archive.org/details/Xenon_2000_Project_PCF). Box2D, SDL2, and GLM are used under their respective licenses, see `Engine/Dependencies` for each library's own terms. Parberry's *Introduction to Game Physics with Box2D* was used as a reference while working through the Box2D API.

## Authors

Xavier Martin Lopes and Hugo Ferreira, Undergraduate in Games and Multimedia, Politecnico de Leiria.

## License

This project is licensed under the Apache License 2.0, see `LICENSE` for details.
