#pragma once
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

struct SDL_Color;
struct SDL_rect;

class Renderer
{
public:
	Renderer(SDL_Window* window);
	~Renderer();

	/// <summary>
	/// Loads and returns the target texture
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SDL_Texture* LoadTexture(const char* filename);

	/// <summary>
	/// Loads and returns the target text as a texture
	/// </summary>
	/// <param name="text"></param>
	/// <param name="textColor"></param>
	SDL_Texture* LoadText(const char* text, SDL_Color textColor = { 255, 255, 255, 0 });

	/// <summary>
	/// Renders a given texture in the specified position, 
	/// can also define the texture width and height
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void RenderTexture(SDL_Texture* texture, int posX, int posY);
	
	/// <summary>
	/// Renders a given texture in the specified position,
	/// with a defined width and heigh
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void RenderTexture(SDL_Texture* texture, int posX, int posY, int width, int height);

	/// <summary>
	/// Renders a rectangle with the given "coordinates"
	/// </summary>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void RenderRect(int posX, int posY, int width, int height, SDL_Color border = { 211, 211, 211, 255 });

	/// <summary>
	/// Resets the scene with the color black
	/// </summary>
	void PrepareScene();

	/// <summary>
	/// Presentes everything needed for the scene
	/// </summary>
	void PresentScene();

private:
	SDL_Renderer* renderer;
	TTF_Font* font;
};