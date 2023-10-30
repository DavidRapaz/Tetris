#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>

#undef main // Happens because SDL has a main definition

// ENUMS

enum class State { InMenu, InGame, Quit };

// CONSTANTS

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 760

// GLOBAL VARS

SDL_Window* window;
SDL_Renderer* renderer;

State gameState = State::InMenu;

int currentScreenInit = 0; // Controls if the current screen was initialized

SDL_Texture* logo = nullptr;

// METHODS

// Common Methods

SDL_Texture* loadTexture(const char* filename)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	return IMG_LoadTexture(renderer, filename);
}

void blit(SDL_Texture* texture, int posX, int posY)
{
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;

	SDL_QueryTexture(texture, NULL, NULL, &destination.w, &destination.h);
	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

// Main Menu Methods

void initMenu()
{
	currentScreenInit = 1;
	logo              = loadTexture("./assets/images/tetrisLogo.png");
}

void handleMenuEvents(SDL_Event& event)
{

}

void drawMenu()
{
	blit(logo, SCREEN_WIDTH / 2 - 305, SCREEN_HEIGHT / 2 - 216);
}

// Game Methods

void initGame()
{
	currentScreenInit = 1;
}

void handleGameEvents(SDL_Event& event)
{

}

void drawGame()
{

}

// Handle the window events
void eventsHandler()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			gameState = State::Quit;
			break;
		default:
			break;
		}
	}
}

// Clear the screen
void prepareScene()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

// Present the screen infomation
void presentScene(void)
{
	SDL_RenderPresent(renderer);
}

int main()
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;
	windowFlags   = 0;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!window)
	{
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	renderer = SDL_CreateRenderer(window, -1, rendererFlags);

	if (!renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	atexit(SDL_Quit);

	// Execute while the users doesn't quit
	while (gameState != State::Quit)
	{
		prepareScene();
		eventsHandler();

		switch (gameState)
		{
		case State::InMenu:
			if (currentScreenInit == 0)
				initMenu();

			drawMenu();
			break;
		case State::InGame:
			if (currentScreenInit == 0)
				initGame();

			drawGame();
			break;
		}

		presentScene();
		SDL_Delay(16);
	}

	return 0;
}