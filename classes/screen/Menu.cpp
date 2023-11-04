#include "Menu.h"
#include "SDL_keycode.h"
#include "../Renderer.h"
#include "../../State.h"

Menu::Menu(Renderer* gameRenderer, State* gameState)
{
	m_Renderer     = gameRenderer;
	m_CurrentState = gameState;
	m_Logo         = m_Renderer->LoadTexture("assets/images/tetrisLogo.png");
	m_InfoText     = m_Renderer->LoadText("Press arrow down to start or esc to quit");
}

Menu::~Menu()
{ }

void Menu::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			*m_CurrentState = State::Quit;
			break;
		case SDL_KEYDOWN:
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