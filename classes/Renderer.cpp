#include "Renderer.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"

Renderer::Renderer(SDL_Window* window)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		throw("Could not create renderer");
	}

	// Initialize SDL_ttf lib
	TTF_Init();
	font = TTF_OpenFont("./assets/fonts/CrimsonText-Regular.ttf", 20);

	// Intialize SDL_image lib
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
}

Renderer::~Renderer()
{ }

/// <summary>
/// Loads and returns the target texture
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SDL_Texture* Renderer::LoadTexture(const char* filename)
{ 
	return IMG_LoadTexture(renderer, filename); 
}

/// <summary>
/// Prepares the text to be loaded to the render
/// </summary>
/// <param name="text"></param>
/// <param name="textColor"></param>
SDL_Texture* Renderer::LoadText(const char* text, SDL_Color textColor)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, textColor);
	SDL_Texture* message        = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_FreeSurface(surfaceMessage);
	return message;
}

/// <summary>
/// Renders a given texture in the specified position
/// </summary>
/// <param name="texture"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Renderer::RenderTexture(SDL_Texture* texture, int posX, int posY)
{
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;

	SDL_QueryTexture(texture, NULL, NULL, &destination.w, &destination.h);
	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

/// <summary>
/// Renders a given texture in the specified position,
/// with a defined width and heigh
/// </summary>
/// <param name="texture"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void Renderer::RenderTexture(SDL_Texture* texture, int posX, int posY, int width, int height)
{
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;
	destination.h = height;
	destination.w = width;

	SDL_RenderCopyEx(renderer, texture, NULL, &destination, 0, NULL, SDL_FLIP_NONE);
}

/// <summary>
/// Renders a rectangle with the given "coordinates"
/// </summary>
/// <param name="posX"></param>
/// <param name="posY"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void Renderer::RenderRect(int posX, int posY, int width, int height, SDL_Color border, bool fill)
{
	SDL_Rect rect;

	rect.x = posX;
	rect.y = posY;
	rect.w = width;
	rect.h = height;

	SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
	SDL_RenderDrawRect(renderer, &rect);

	if (fill)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(renderer, &rect);
	}
}

/// <summary>
/// Resets the scene with the color black
/// </summary>
void Renderer::PrepareScene()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

/// <summary>
/// Presentes everything needed for the scene
/// </summary>
void Renderer::PresentScene()
{
	SDL_RenderPresent(renderer);
}