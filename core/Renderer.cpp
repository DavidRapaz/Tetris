#include "Renderer.h"
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_video.h>

Renderer::Renderer(SDL_Window* window)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!m_Renderer)
	{
		throw("Could not create renderer");
	}

	// Initialize SDL_ttf lib
	TTF_Init();
	m_Font = TTF_OpenFont("assets/fonts/CrimsonText-Regular.ttf", 20);

	// Initialize SDL_image lib
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	m_Textures.insert({ "background", LoadTexture("assets/images/background.png") });
	m_Textures.insert({ "logo", LoadTexture("assets/images/logo.png") });
	m_Textures.insert({ "up", LoadTexture("assets/images/keys/key_up.png") });
	m_Textures.insert({ "down", LoadTexture("assets/images/keys/key_down.png") });
	m_Textures.insert({ "left", LoadTexture("assets/images/keys/key_left.png") });
	m_Textures.insert({ "right", LoadTexture("assets/images/keys/key_right.png") });
	m_Textures.insert({ "space", LoadTexture("assets/images/keys/key_space.png") });
	m_Textures.insert({ "esc", LoadTexture("assets/images/keys/key_esc.png") });
}

Renderer::~Renderer()
{
	TTF_CloseFont(m_Font);
	IMG_Quit();
	m_Textures.clear();
}

/// <summary>
/// Loads and returns the target texture
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SDL_Texture* Renderer::LoadTexture(const char* filename) const
{ 
	return IMG_LoadTexture(m_Renderer, filename);
}

/// <summary>
/// Prepares the text to be loaded to the render
/// </summary>
/// <param name="text"></param>
/// <param name="textColor"></param>
SDL_Texture* Renderer::LoadText(const char* text, const SDL_Color textColor) const
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_Font, text, textColor);
	SDL_Texture* message        = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);

	SDL_FreeSurface(surfaceMessage);
	return message;
}

/// <summary>
/// Renders a given texture in the specified position
/// </summary>
/// <param name="texture"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Renderer::renderTexture(SDL_Texture* texture, const int posX, const int posY) const
{
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;

	SDL_QueryTexture(texture, nullptr, nullptr, &destination.w, &destination.h);
	SDL_RenderCopy(m_Renderer, texture, nullptr, &destination);
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
void Renderer::renderTexture(SDL_Texture* texture, const int posX, const int posY, const int width, const int height) const
{
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;
	destination.h = height;
	destination.w = width;

	SDL_RenderCopyEx(m_Renderer, texture, nullptr, &destination, 0, nullptr, SDL_FLIP_NONE);
}

/// <summary>
/// Renders a cached texture
/// </summary>
/// <param name="keyName"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void Renderer::renderTexture(std::string keyName, int posX, int posY, int width, int height) const
{
	SDL_Texture* texture = m_Textures.at(keyName);
	SDL_Rect destination;

	destination.x = posX;
	destination.y = posY;
	destination.h = height;
	destination.w = width;

	SDL_RenderCopyEx(m_Renderer, texture, nullptr, &destination, 0, nullptr, SDL_FLIP_NONE);
}

/// <summary>
/// Renders a rectangle with the given "coordinates"
/// </summary>
/// <param name="posX"></param>
/// <param name="posY"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="border"></param>
/// <param name="fill"></param>
void Renderer::RenderRect(const int posX, const int posY, const int width, const int height, const SDL_Color border, const bool fill) const
{
	SDL_Rect rect;

	rect.x = posX;
	rect.y = posY;
	rect.w = width;
	rect.h = height;

	SDL_SetRenderDrawColor(m_Renderer, border.r, border.g, border.b, border.a);
	SDL_RenderDrawRect(m_Renderer, &rect);

	if (fill)
	{
		SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(m_Renderer, &rect);
	}
}

/// <summary>
/// Resets the scene with the color black
/// </summary>
void Renderer::prepareScene() const
{
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);

	int width, height;
	SDL_GetRendererOutputSize(m_Renderer, &width, &height);
	RenderTexture(m_Textures.at("background"), 0, 0, width, height);
	RenderTexture(m_Textures.at("logo"), 500, 250, 300, 100);
}

/// <summary>
/// Presents everything needed for the scene
/// </summary>
void Renderer::presentScene() const
{
	SDL_RenderPresent(m_Renderer);
}