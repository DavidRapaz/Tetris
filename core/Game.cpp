#include "Game.h"
#include "Renderer.h"
#include "../screens/Menu.h"
#include "../screens/Board.h"

#include <SDL.h>
#include <iostream>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 760;

Game::Game()
{
    constexpr int windowFlags = 0;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    m_Window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

    if (!m_Window)
    {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    m_Screen   = new Menu();
    m_Renderer = new Renderer(m_Window);
}

Game::~Game()
{}

void Game::init()
{
    while (m_CurrentState != GameState::QUIT)
    {
        m_Renderer->prepareScene();

        m_Screen->handleEvents();
        m_Screen->draw();

        m_Renderer->presentScene();
		SDL_Delay(16);
    }
}

void Game::close()
{}

void Game::changeState()
{}