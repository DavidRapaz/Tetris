#pragma once
#include "screen.h"

class Menu : public Screen
{
public:
	Menu(Renderer* gameRenderer);
	~Menu();

	/// <summary>
	/// Handle the screen events
	/// </summary>
	virtual void HandleEvents(State& gameState) override;

	/// <summary>
	/// Draws the what is targeted to show in the screen
	/// </summary>
	virtual void Draw() override;
private:
	SDL_Texture* m_Logo;
	SDL_Texture* m_InfoText;
};