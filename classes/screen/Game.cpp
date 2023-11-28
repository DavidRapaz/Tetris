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

#pragma warning(disable:6385)

// Define sizes and positions
#define PIECE_SIZE 30
#define BOARD_TOP_LEFT_X_POS 500
#define BOARD_TOP_LEFT_Y_POS 50

// Define the time in which the board is updated
#define NORMAL_VELOCITY 400.f
#define SPEEDING_VELOCITY 200.f

// CONTROL VARS
bool newPiece    = false,
	updateColumn = false,
	rotatePiece  = false,
	isOnPause    = false;

// NEW VALUES TARGET VARS
int targetDirection = 0, 
	targetRotation  = 0;

float targetUpdateTime;

Game::Game(Renderer* gameRenderer)
{
	// Set the default value of target update speed
	targetUpdateTime = NORMAL_VELOCITY;

	// Associate the renderer
	m_Renderer = gameRenderer;

	// Load the Pieces textures
	m_Red    = m_Renderer->LoadTexture("./assets/images/red.bmp");
	m_Green  = m_Renderer->LoadTexture("./assets/images/green.bmp");
	m_Blue   = m_Renderer->LoadTexture("./assets/images/lightblue.bmp");
	m_Orange = m_Renderer->LoadTexture("./assets/images/orange.bmp");

	GenerateNewPiece();

	// Intialize time management variables
	m_LastFrame = 0;
	m_TimeStep  = 0;

	// Initialize the board
	memset(&m_Board, static_cast<int>(Color::None), sizeof(int) * 200);
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

// ---- VALIDATIONS METHODS

bool Game::CheckIfColumnAvailable()
{
	for (int index = 0; index < 4; index++)
	{
		int pieceNextColumn = currentPiece->position[index] + targetDirection;

		// Validates if there exists a block on the target column and if that block doesn't belong to the current piece
		if (
			m_Board[pieceNextColumn] != Color::None &&
			currentPiece->position[0] != pieceNextColumn &&
			currentPiece->position[1] != pieceNextColumn &&
			currentPiece->position[2] != pieceNextColumn &&
			currentPiece->position[3] != pieceNextColumn
		)
			return false;
	}

	return true;
}

bool Game::CheckIfGameOver()
{
	return false;
}

// ---- VALIDATIONS METHODS

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
	case SDLK_SPACE:
		targetUpdateTime = keyUp ? NORMAL_VELOCITY : SPEEDING_VELOCITY;
		break;
	case SDLK_RIGHT: // Piece movement to the right
	case SDLK_d:
		if (keyUp) return;
		
		updateColumn    = true;
		targetDirection = 1;
		break;
	case SDLK_LEFT: // Piece movement to the left
	case SDLK_a:
		if (keyUp) return;
		
		updateColumn    = true;
		targetDirection = -1;
		break;
	case SDLK_q: // Rotate left
		if (keyUp) return;
		
		rotatePiece    = true;
		targetRotation = -1;
		break;
	case SDLK_e: // Rotate right
		if (keyUp) return;
		
		rotatePiece    = true;
		targetRotation = 1;
		break;
	case SDLK_ESCAPE:
		if (keyUp) return;

		isOnPause = !isOnPause;
		break;
	}
}

/// <summary>
/// Handle the screen events
/// </summary>
/// <param name="gameState"></param>
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
		case SDL_KEYUP:
			HandleKeyEvents(event.key.keysym, true);
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// Check where the piece is going to land
/// </summary>
void Game::PreviewPiecePosition()
{
	int highestRow  = 19, // Initialize with the last row
		blockRow    = -1, // Stores the row of the block that has a collision first
		lowestBlock = 0;  // Stores the lowest block of the piece

	/*
	* Go through every block of the piece,
	* check and store the row of the block and 
	* the row where the collision occurs
	*/
	for (int index = 0; index < 4; index++)
	{
		int indexBlockRow    = currentPiece->position[index] / 10,
			indexBlockColumn = currentPiece->position[index] % 10,
			nextRow          = currentPiece->position[index] + 10;

		// If there exists a block that bellongs to the current piece a row bellow then skip this block
		if (
			nextRow == currentPiece->position[0] ||
			nextRow == currentPiece->position[1] ||
			nextRow == currentPiece->position[2] ||
			nextRow == currentPiece->position[3]
		)
			continue;
		
		// Update every time it finds a block where the row is bellow than the current one stored in the lowest block
		if (indexBlockRow > lowestBlock)
		{
			lowestBlock = indexBlockRow;
		}

		/*
		* First we will find we will find in which column
		* the first collision occurs
		*/
		for (int row = indexBlockRow + 1; row < 20; row++)
		{
			int boardPos    = row * 10 + indexBlockColumn,
				previousRow = row - 1;
			
			/*
			* Every time that a position is occupied by another block
			* that doesn't belong to the current piece
			* and that the previous row is in a row higher than the highestRow stored
			* then we update the highestRow and the blockRow
			*/
			if (
				m_Board[boardPos] != Color::None &&
				boardPos != currentPiece->position[0] &&
				boardPos != currentPiece->position[1] &&
				boardPos != currentPiece->position[2] &&
				boardPos != currentPiece->position[3] &&
				previousRow <= highestRow
			)
			{
				if (previousRow != highestRow)
				{
					highestRow = previousRow;
					blockRow   = indexBlockRow;
				}
				else if (blockRow < indexBlockRow)
				{
					blockRow = indexBlockRow;
				}
			}
		}
	}

	// If it didn't found any position that is occupied then insert as default the lowest block of the piece
	if (blockRow == -1)
		blockRow = lowestBlock;

	/*
	* Now for every block of the piece
	* we add the difference between the first block to collide row
	* and the row previous to where there will be a collision
	*/
	m_PreviewedPositions[0] = currentPiece->position[0] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[1] = currentPiece->position[1] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[2] = currentPiece->position[2] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[3] = currentPiece->position[3] + (highestRow - blockRow) * 10;
}

// ---- EVENTS HANDLING METHODS

// ---- GAME BOARD METHODS

/// <summary>
/// Checks if a row is total filled, if it is then clears it and 
/// updates the position of the pieces above
/// </summary>
void Game::ClearFilledRows()
{
	int firstRowCleared = -1,
		rowsCleared     = 0;

	// Go from bottom to top to clear the rows
	for (int row = 19; row >= 0; row--)
	{
		int totalPositionsFilled = 0,
			offset               = row * 10;

		// Check if all columns are filled
		for (int column = 0; column < 10; column++)
		{
			if (m_Board[offset + column] != Color::None)
				totalPositionsFilled++;
		}

		// Store the row, clear it and stores how many where cleared
		if (totalPositionsFilled == 10)
		{
			memset(&m_Board[offset], static_cast<int>(Color::None), sizeof(int) * 10);
			
			if (firstRowCleared == -1)
				firstRowCleared = row;

			rowsCleared++;
		}
	}

	/*
	* Starting from the first row cleared
	* we update every row above because the fist row cleared
	* will always be the row most bellow
	*/
	for (int row = firstRowCleared; row > rowsCleared; row--)
	{
		int offset         = row * 10,
			offsetToUpdate = (row - rowsCleared) * 10;

		memcpy(&m_Board[offset], &m_Board[offsetToUpdate], sizeof(int) * 10);
	}
}

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
/// Draws on the screen the piece previewed position
/// </summary>
void Game::DrawPiecePreviewedPosition()
{
	unsigned char red = 0,
		green         = 0, 
		blue          = 0, 
		alpha         = 255;

	switch (currentPiece->GetPieceColor())
	{
	case Color::Red:
		red = 255;
		break;
	case Color::Green:
		green = 255;
		break;
	case Color::Blue:
		blue = 255;
		break;
	case Color::Orange:
		red   = 255;
		green = 140;
		blue  = 0;
		break;
	}

	for (int index = 0; index < 4; index++)
	{
		int column = m_PreviewedPositions[index] % 10,
			row    = m_PreviewedPositions[index] / 10;

		m_Renderer->RenderRect(
			BOARD_TOP_LEFT_X_POS + (column * PIECE_SIZE),
			BOARD_TOP_LEFT_Y_POS + (row * PIECE_SIZE),
			PIECE_SIZE,
			PIECE_SIZE,
			{ red, green, blue, 255 }
		);
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
	m_Time     = (float)SDL_GetTicks();
	m_TimeStep = m_Time - m_LastFrame;

	// Updates the board with the current piece position
	UpdateBoard();
	DrawBoard();
	CheckForPieceLocked();

	/*
	* Update the column if the current 
	* piece is not already locked
	*/
	if (!newPiece && updateColumn && !isOnPause)
	{
		if (CheckIfColumnAvailable())
		{
			UpdateBoard(true);
			currentPiece->UpdateColumn(targetDirection);
		}

		// Reset control variables
		updateColumn    = false;
		targetDirection = 0;
	}

	if (!newPiece && rotatePiece && !isOnPause)
	{
		UpdateBoard(true);
		currentPiece->Rotate(targetRotation);

		// Validate if with the new rotation the target if over an already inserted piece
		// If it exists then rotate back
		for (int index = 0; index < 4; index++)
		{
			int pieceIndex = currentPiece->position[index];
			if (m_Board[pieceIndex] != Color::None)
			{
				currentPiece->Rotate(-(targetRotation));
			}
		}

		rotatePiece    = false;
		targetRotation = 0;
	}

	// Preview and draw where the current piece will fall
	PreviewPiecePosition();
	DrawPiecePreviewedPosition();

	/*
	* Check if the time between the current time and the last frame
	* is greater than the max period given for a frame
	* if it is then execute each time base functionality
	*/
	if (m_TimeStep > targetUpdateTime && !isOnPause)
	{
		m_LastFrame = m_Time;

		if (!newPiece)
		{
			// Clear the previous position
			UpdateBoard(true);

			// After draw update the current piece position
			currentPiece->UpdatePosition();
		} else
		{
			GenerateNewPiece();
			ClearFilledRows();
			memset(&m_PreviewedPositions, -1, sizeof(int) * 4);
		}
	}
}

// ---- GAME BOARD METHODS