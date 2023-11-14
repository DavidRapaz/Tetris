#include "Menu.h"
#include "SDL_keycode.h"
#include "../Renderer.h"
#include "../../State.h"

Menu::Menu(Renderer* gameRenderer)
{
	m_Renderer = gameRenderer;
	m_Logo     = m_Renderer->LoadTexture("./assets/images/tetrisLogo.png");
	m_InfoText = m_Renderer->LoadText("Press arrow down to start or esc to quit");
}

Menu::~Menu()
{ }

void Menu::HandleEvents(State& gameState)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			gameState = State::Quit;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				gameState = State::InGame;
			} else if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				gameState = State::Quit;
			}
			break;
		default:
			break;
		}
	}
}

void Menu::Draw()
{
	m_Renderer->RenderTexture(m_Logo, 335, 164);
	m_Renderer->RenderTexture(m_InfoText, 480, 600);
}