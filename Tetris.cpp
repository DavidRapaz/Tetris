#include "classes/Renderer.h"
#include "classes/screen/Screen.h"
#include "classes/screen/Game.h"
#include "classes/screen/Menu.h"
#include "State.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>

#undef main // Happens because SDL has a main definition

// CONSTANTS

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 760

// GLOBAL VARS

SDL_Window* window;

State currentState  = State::InMenu;
State previousState = State::InMenu;

int main()
{
	int windowFlags;
	windowFlags = 0;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
	{
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!window)
	{
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	Renderer renderer(window);
	Screen* currentScreen = new Menu(&renderer, &currentState);

	atexit(SDL_Quit);

	// Execute while the users doesn't quit
	while (currentState != State::Quit)
	{
		renderer.PrepareScene();

		currentScreen->HandleEvents();
		currentScreen->Draw();

		renderer.PresentScene();
		SDL_Delay(16);
	}

	return 0;
}