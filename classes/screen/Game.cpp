#include "Game.h"

// SDL Includes
#include "SDL_keycode.h"
#include "SDL_timer.h"

#include "../Renderer.h"
#include "../../State.h"
#include "../../Color.h"

// Piece Includes
#include "../piece/Piece.h"
#include "../piece/BoxShape.h"
#include "../piece/IShape.h"
#include "../piece/LShape.h"
#include "../piece/ZShape.h"
#include "../piece/TShape.h"

// Include standard libs
#include <iostream>
#include "stdlib.h"

// Define sizes and positions
#define PIECE_SIZE 30
#define BOARD_TOP_LEFT_X_POS 500
#define BOARD_TOP_LEFT_Y_POS 50

// Define the time in which the board is updated
#define MAX_PERIOD 800.f

Game::Game(Renderer* gameRenderer)
{
	// Associate the renderer
	m_Renderer = gameRenderer;

	// Load the Pieces textures
	m_Red    = m_Renderer->LoadTexture("./assets/images/red.bmp");
	m_Green  = m_Renderer->LoadTexture("./assets/images/green.bmp");
	m_Blue   = m_Renderer->LoadTexture("./assets/images/lightblue.bmp");
	m_Orange = m_Renderer->LoadTexture("./assets/images/orange.bmp");

	// Initialize a random piece
	std::srand(5);
	switch (rand() % 5 + 1)
	{
	case 1: // TShape
		currentPiece = new TShape();
		break;
	case 2: // ZShape
		currentPiece = new ZShape();
		break;
	case 3: // LShape
		currentPiece = new LShape();
		break;
	case 4: // IShape
		currentPiece = new IShape();
		break;
	case 5: // BoxShape
		currentPiece = new BoxShape();
		break;
	}
	//currentPiece = Piece();

	// Intialize time management variables
	lastFrame = 0;
	timeStep  = 0;

	// Initialize the board
	memset(&m_Board, (int) Color::None, sizeof(int) * 200);
}

Game::~Game()
{}

void Game::HandleEvents(State& gameState)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			gameState = State::Quit;
			break;
		default:
			break;
		}
	}
}

// GAME BOARD FUNCTIONS

/// <summary>
/// Updates the board with the current piece position
/// or clear the piece previous position
/// </summary>
void Game::UpdateBoard(bool clear)
{
	int pieceIndex    = 0;
	Color updateColor = clear ? Color::None : currentPiece->GetPieceColor();

	for (int index = 0; index < 4; index++)
	{
		pieceIndex = currentPiece->position[index];
		
		if (pieceIndex > -1)
		{
			m_Board[pieceIndex] = updateColor;
		}
	}
}

/// <summary>
/// Draws the game board
/// </summary>
void Game::DrawBoard()
{
	// Draw the current piece and the already positioned pieces
	for (int row = 0; row < 20; row++)
	{
		int offset = row * 10;

		for (int column = 0; column < 10; column++)
		{
			switch (m_Board[offset + column])
			{
			case Color::Red:
				m_Renderer->RenderTexture(
					m_Red,
					BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
					BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
					PIECE_SIZE,
					PIECE_SIZE
				);
				break;
			case Color::Green:
				m_Renderer->RenderTexture(
					m_Green,
					BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
					BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
					PIECE_SIZE,
					PIECE_SIZE
				);
				break;
			case Color::Blue:
				m_Renderer->RenderTexture(
					m_Blue,
					BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
					BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
					PIECE_SIZE,
					PIECE_SIZE
				);
				break;
			case Color::Orange:
				m_Renderer->RenderTexture(
					m_Orange,
					BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
					BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
					PIECE_SIZE,
					PIECE_SIZE
				);
				break;
			}

			// Renders a boarder
			m_Renderer->RenderRect(
				BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
				BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
				PIECE_SIZE,
				PIECE_SIZE
			);
		}
	}
}

/// <summary>
/// Draws the what is targeted to show in the screen
/// </summary>
void Game::Draw()
{ 
	// Manage time
	time     = (float)SDL_GetTicks();
	timeStep = time - lastFrame;

	// Updates the board with the current piece position
	UpdateBoard();
	DrawBoard();

	/*
	* Check if the time between the current time and the last frame
	* is greater than the max period given for a frame
	* if it is then execute each time base functionality
	*/
	if (timeStep > MAX_PERIOD)
	{
		lastFrame = time;

		// Clear the previous position
		UpdateBoard(true);

		// After draw update the current piece position
		currentPiece->UpdatePosition();
	}
}