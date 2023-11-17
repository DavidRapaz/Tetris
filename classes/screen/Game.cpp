#include "Game.h"

// SDL Includes
#include "SDL_keycode.h"
#include "SDL_timer.h"

#include "../Renderer.h"
#include "../../State.h"
#include "../../Color.h"

// Piece Includes
#include "../Piece.h"

// Include standard libs
#include <iostream>
#include "stdlib.h"

#include <ctime>

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

	GenerateNewPiece();

	// Intialize time management variables
	lastFrame = 0;
	timeStep  = 0;

	// Initialize the board
	memset(&m_Board, (int) Color::None, sizeof(int) * 200);
}

Game::~Game()
{}

// ---- INITIALIZATION METHODS

/// <summary>
/// Generates a new piece
/// </summary>
void Game::GenerateNewPiece()
{
	newPiece = false;

	// Initialize a random new piece
	currentPiece = new Piece();
}

// ---- INITIALIZATION METHODS

// ---- GAME LOGIC METHODS

/// <summary>
/// Validates if the current piece can still move or 
/// if it's movement is locked and updates newPiece variable
/// </summary>
void Game::CheckForPieceLocked()
{
	for (int index = 0; index < 4; index++)
	{
		int pieceNextPos = currentPiece->position[index] + 10;

		// Validate if the next movement is going to be outside of the board
		if (pieceNextPos > 199)
		{
			newPiece = true;
			return;
		}

		/*
		 * Validates if the next movement will be on top 
		 * of an already inserted block that doesn't belong 
		 * to the current piece
		 */
		if (
			m_Board[pieceNextPos] != Color::None &&
			currentPiece->position[0] != pieceNextPos &&
			currentPiece->position[1] != pieceNextPos &&
			currentPiece->position[2] != pieceNextPos &&
			currentPiece->position[3] != pieceNextPos
		)
		{
			newPiece = true;
			return;
		}
	}
}

// ---- GAME LOGIC METHODS

// ---- EVENTS HANDLING METHODS

/// <summary>
/// Handle the events when a user presses a key or 
/// stops pressing it
/// </summary>
/// <param name="key"></param>
/// <param name="keyUp"></param>
void Game::HandleKeyEvents(SDL_Keysym key, bool keyUp)
{
	switch (key.sym)
	{
	case SDLK_DOWN: // Speed piece movement down 
	case SDLK_s:
		break;
	case SDLK_RIGHT: // Piece movement to the right
	case SDLK_d:
		// Clear the previous positions
		UpdateBoard(true);

		// Update the piece to the new column
		currentPiece->UpdateColumn(1);
		break;
	case SDLK_LEFT: // Piece movement to the left
	case SDLK_a:
		// Clear the previous positions
		UpdateBoard(true);

		// Update the piece to the new column
		currentPiece->UpdateColumn(-1);
		break;
	default:
		break;
	}
}

/// <summary>
/// Handle the screen events
/// </summary>
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
		case SDL_KEYDOWN:
			HandleKeyEvents(event.key.keysym);
			break;
		default:
			break;
		}
	}
}

// ---- EVENTS HANDLING METHODS

// ---- GAME BOARD METHODS

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
	CheckForPieceLocked();

	/*
	* Check if the time between the current time and the last frame
	* is greater than the max period given for a frame
	* if it is then execute each time base functionality
	*/
	if (timeStep > MAX_PERIOD)
	{
		lastFrame = time;

		if (!newPiece)
		{
			// Clear the previous position
			UpdateBoard(true);

			// After draw update the current piece position
			currentPiece->UpdatePosition();
		} else
		{
			GenerateNewPiece();
		}
	}
}

// ---- GAME BOARD METHODS