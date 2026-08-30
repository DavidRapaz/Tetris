#pragma once
#include <unordered_map>
#include <string>

#include <SDL_ttf.h>

struct SDL_rect;
struct SDL_Texture;
struct SDL_Window;

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
	SDL_Texture* LoadTexture(const char* filename) const;

	/// <summary>
	/// Loads and returns the target text as a texture
	/// </summary>
	/// <param name="text"></param>
	/// <param name="textColor"></param>
	SDL_Texture* LoadText(const char* text, SDL_Color textColor = { 255, 255, 255, 0 }) const;

	/// <summary>
	/// Renders a given texture in the specified position
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	void renderTexture(SDL_Texture* texture, int posX, int posY) const;
	
	/// <summary>
	/// Renders a given texture in the specified position,
	/// with a defined width and heigh
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void renderTexture(SDL_Texture* texture, int posX, int posY, int width, int height) const;

	/// <summary>
	/// Renders a cached texture
	/// </summary>
	/// <param name="keyName"></param>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void renderTexture(std::string keyName, int posX, int posY, int width, int height) const;

	/// <summary>
	/// Renders a rectangle with the given "coordinates"
	/// </summary>
	/// <param name="posX"></param>
	/// <param name="posY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="border"></param>
	/// <param name="fill"></param>
	void RenderRect(int posX, int posY, int width, int height, SDL_Color border = { 211, 211, 211, 255 }, bool fill = false) const;

	/// <summary>
	/// Resets the scene with the color black
	/// </summary>
	void prepareScene() const;

	/// <summary>
	/// Presents everything needed for the scene
	/// </summary>
	void presentScene() const;
private:
	SDL_Renderer* m_Renderer;
	TTF_Font *m_Font;
	std::unordered_map<std::string, SDL_Texture*> m_Textures;
};