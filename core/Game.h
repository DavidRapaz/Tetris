#pragma once
#include <SDL_video.h>
#include "../enums/State.h"

class Screen;
class Renderer;

class Game
{
	public: 
		void init();
		void close();
		void changeState();

		Game();
		~Game();

	private:
		SDL_Window *m_Window;

		State m_CurrentState  = State::InMenu;
		State m_PreviousState = State::InMenu;

		Screen* m_Screen;
		Renderer* m_Renderer;
};