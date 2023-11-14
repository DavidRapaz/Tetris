#pragma once
#include "SDL_events.h"
#include <SDL_render.h>
#include <SDL_keycode.h>

class Renderer;
enum class State;

class Screen
{
public:
	Screen() {};
	~Screen() {};

	/// <summary>
	/// Handle the screen events
	/// </summary>
	virtual void HandleEvents(State& gameState) = 0;

	/// <summary>
	/// Draws the what is targeted to show in the screen
	/// </summary>
	virtual void Draw() = 0;
	
	/// <summary>
	/// Checks if the screen was initialized
	/// </summary>
	/// <returns></returns>
	bool hasInitialized() { return m_Initialized; }

protected:
	bool m_Initialized = false;

	Renderer* m_Renderer;
	State* m_CurrentState;
};