#include "Game.h"

// SDL Includes
#include "SDL_keycode.h"
#include "SDL_timer.h"

#include "../Renderer.h"
#include "../../State.h"
#include "../../Color.h"
#include "../../Shape.h"

// Piece Includes
#include "../Piece.h"

// Include standard libs
#include <iostream>
#include "stdlib.h"

#include <ctime>

#pragma warning(disable:6385)

// Define sizes and positions
#define PIECE_SIZE 30
#define NEXT_PIECE_SIZE 15
#define BOARD_TOP_LEFT_X_POS 500
#define BOARD_TOP_LEFT_Y_POS 50

// Define the time in which the board is updated
#define NORMAL_VELOCITY 400.f
#define SPEEDING_VELOCITY 200.f

// CONTROL VARS
bool newPiece    = false,
	updateColumn = false,
	rotatePiece  = false,
	isOnPause    = false,
	gameOver     = false;

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
{
	// Free up the memory occupied
	delete m_Red, m_Green, m_Blue, m_Orange;
}

// ---- INITIALIZATION METHODS

/// <summary>
/// Generates a new piece
/// </summary>
void Game::GenerateNewPiece()
{
	newPiece = false;

	// If the next pieces are not init the current piece and the next pieces
	if (m_NextPieces[0] == nullptr)
	{
		// Initialize a random new piece
		m_CurrentPiece = new Piece();
	
		m_NextPieces[0] = new Piece();
		m_NextPieces[1] = new Piece();
		m_NextPieces[2] = new Piece();
	} else
	{
		m_CurrentPiece = m_NextPieces[0];

		m_NextPieces[0] = m_NextPieces[1];
		m_NextPieces[1] = m_NextPieces[2];
		m_NextPieces[2] = new Piece();
	}
}

// ---- INITIALIZATION METHODS

// ---- VALIDATIONS METHODS

bool Game::CheckIfColumnAvailable()
{
	for (int index = 0; index < 4; index++)
	{
		int pieceNextColumn = m_CurrentPiece->position[index] + targetDirection;

		// Validates if there exists a block on the target column and if that block doesn't belong to the current piece
		if (
			m_Board[pieceNextColumn] != Color::None &&
			m_CurrentPiece->position[0] != pieceNextColumn &&
			m_CurrentPiece->position[1] != pieceNextColumn &&
			m_CurrentPiece->position[2] != pieceNextColumn &&
			m_CurrentPiece->position[3] != pieceNextColumn
		)
			return false;
	}

	return true;
}

bool Game::CheckIfGameOver()
{
	int lowestBlockPos = 0;

	//Fetch the lowest block position of the piece
	for (int index = 0; index < 4; index++)
	{
		if (lowestBlockPos < m_CurrentPiece->position[index])
			lowestBlockPos = m_CurrentPiece->position[index];
	}

	/*
	* The game is over when the next position of the lowest block is occupied and 
	* at least one of the indexes isn't even on the board
	*/
	return m_Board[lowestBlockPos + 10] != Color::None && (
		m_CurrentPiece->position[0] < 0 || 
		m_CurrentPiece->position[1] < 0 || 
		m_CurrentPiece->position[2] < 0 || 
		m_CurrentPiece->position[3] < 0
	);
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
		int pieceNextPos = m_CurrentPiece->position[index] + 10;

		// Validate if the next movement is going to be outside of the board
		if (pieceNextPos > 199)
		{
			newPiece = true;
			return;
		}

		if (pieceNextPos < 0)
			return;

		/*
		 * Validates if the next movement will be on top 
		 * of an already inserted block that doesn't belong 
		 * to the current piece
		 */
		if (
			m_Board[pieceNextPos] != Color::None &&
			m_CurrentPiece->position[0] != pieceNextPos &&
			m_CurrentPiece->position[1] != pieceNextPos &&
			m_CurrentPiece->position[2] != pieceNextPos &&
			m_CurrentPiece->position[3] != pieceNextPos
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
		int indexBlockRow    = m_CurrentPiece->position[index] / 10,
			indexBlockColumn = m_CurrentPiece->position[index] % 10,
			nextRow          = m_CurrentPiece->position[index] + 10;
		
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
			* The quickest way found to prevent not counting the last row as a "limit"
			* the L Shape piece preview was being drawned outside of the board when rotated
			*/
			if (
				row == 19 && 
				m_Board[boardPos] == Color::None &&
				row - indexBlockRow < highestRow + 1 - blockRow
			)
			{
				highestRow = row;
				blockRow   = indexBlockRow;
			}

			/*
			* Ignore current row if the board pos is empty and it's not the last row
			* or if the current board pos is occupied 
			* by a block of the current piece
			*/
			if (
				m_Board[boardPos] == Color::None ||
				boardPos == m_CurrentPiece->position[0] ||
				boardPos == m_CurrentPiece->position[1] ||
				boardPos == m_CurrentPiece->position[2] ||
				boardPos == m_CurrentPiece->position[3]
			)
				continue;
			
			/*
			* Here we compare the difference between the current block row and selected row with the difference
			* between the highest row stored and the block row stored
			* with the difference comparison we will know which block first colides with a inserted piece
			*/
			if (previousRow - indexBlockRow < highestRow - blockRow)
			{
				highestRow = previousRow;
				blockRow   = indexBlockRow;
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
	m_PreviewedPositions[0] = m_CurrentPiece->position[0] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[1] = m_CurrentPiece->position[1] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[2] = m_CurrentPiece->position[2] + (highestRow - blockRow) * 10;
	m_PreviewedPositions[3] = m_CurrentPiece->position[3] + (highestRow - blockRow) * 10;
}

// ---- EVENTS HANDLING METHODS

// ---- NEXT PIECE METHODS

/// <summary>
/// Draws the board on the right side that contains the next pieces
/// </summary>
void Game::DrawNextPiecesBoard()
{
	int topLeftXPos = BOARD_TOP_LEFT_X_POS + (10 * PIECE_SIZE) + 30;

	// Render the boarders of the preview next pieces board
	m_Renderer->RenderRect(
		topLeftXPos,
		BOARD_TOP_LEFT_Y_POS,
		NEXT_PIECE_SIZE * 9,
		NEXT_PIECE_SIZE * 18
	);

	/*
	* Draw the pieces previews
	*/
	SDL_Texture* pieceColor;
	for (int index = 0; index < 3; index++)
	{
		int start = (index * 6 + 1) * NEXT_PIECE_SIZE;

		switch (m_NextPieces[index]->GetPieceColor())
		{
		case Color::Red:
			pieceColor = m_Red;
			break;
		case Color::Green:
			pieceColor = m_Green;
			break;
		case Color::Blue:
			pieceColor = m_Blue;
			break;
		default:
			pieceColor = m_Orange;
			break;
		}

		/*
		* Define the positions for each block
		*/
		int block01XPos = 0, 
			block01YPos = 0, 
			block02XPos = 0, 
			block02YPos = 0, 
			block03XPos = 0, 
			block03YPos = 0, 
			block04XPos = 0,
			block04YPos = 0;

		switch (m_NextPieces[index]->GetPieceShape())
		{
		case Shape::TShape:
			// Define the positions in the x axis
			block01XPos = NEXT_PIECE_SIZE * 3;
			block02XPos = NEXT_PIECE_SIZE * 4;
			block03XPos = NEXT_PIECE_SIZE * 5;
			block04XPos = block02XPos;

			// Define the positions in the y axis
			block01YPos = start;
			block02YPos = start;
			block03YPos = start;
			block04YPos = start + NEXT_PIECE_SIZE;
			break;
		case Shape::ZShape:
			// Define the positions in the x axis
			block01XPos = NEXT_PIECE_SIZE * 3;
			block02XPos = NEXT_PIECE_SIZE * 4;
			block03XPos = block02XPos;
			block04XPos = NEXT_PIECE_SIZE * 5;

			// Define the positions in the y axis
			block01YPos = start;
			block02YPos = start;
			block03YPos = start + NEXT_PIECE_SIZE;
			block04YPos = start + NEXT_PIECE_SIZE;
			break;
		case Shape::LShape:
			// Define the positions in the x axis
    		block01XPos = NEXT_PIECE_SIZE * 4;
			block02XPos = block01XPos;
			block03XPos = block01XPos;
			block04XPos = NEXT_PIECE_SIZE * 5;

			// Define the positions in the y axis
			block01YPos = start;
			block02YPos = start + NEXT_PIECE_SIZE;
			block03YPos = start + NEXT_PIECE_SIZE * 2;
			block04YPos = block03YPos;
			break;
		case Shape::IShape:
			// Define the positions in the x axis
			block01XPos = NEXT_PIECE_SIZE * 4;
			block02XPos = block01XPos;
			block03XPos = block01XPos;
			block04XPos = block01XPos;

			// Define the positions in the y axis
			block01YPos = start;
			block02YPos = start + NEXT_PIECE_SIZE;
			block03YPos = start + NEXT_PIECE_SIZE * 2;
			block04YPos = start + NEXT_PIECE_SIZE * 3;
			break;
		case Shape::BoxShape:
			// Define the positions in the x axis
			block01XPos = NEXT_PIECE_SIZE * 4;
			block02XPos = NEXT_PIECE_SIZE * 5;
			block03XPos = block01XPos;
			block04XPos = block02XPos;

			// Define the positions in the y axis
			block01YPos = start;
			block02YPos = start;
			block03YPos = start + NEXT_PIECE_SIZE;
			block04YPos = block03YPos;
			break;
		default:
			break;
		}

		// Render the textures
		m_Renderer->RenderTexture(pieceColor, topLeftXPos + block01XPos, BOARD_TOP_LEFT_Y_POS + block01YPos, NEXT_PIECE_SIZE, NEXT_PIECE_SIZE);
		m_Renderer->RenderTexture(pieceColor, topLeftXPos + block02XPos, BOARD_TOP_LEFT_Y_POS + block02YPos, NEXT_PIECE_SIZE, NEXT_PIECE_SIZE);
		m_Renderer->RenderTexture(pieceColor, topLeftXPos + block03XPos, BOARD_TOP_LEFT_Y_POS + block03YPos, NEXT_PIECE_SIZE, NEXT_PIECE_SIZE);
		m_Renderer->RenderTexture(pieceColor, topLeftXPos + block04XPos, BOARD_TOP_LEFT_Y_POS + block04YPos, NEXT_PIECE_SIZE, NEXT_PIECE_SIZE);
	}
}

// ---- NEXT PIECE METHODS

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
	Color updateColor = clear ? Color::None : m_CurrentPiece->GetPieceColor();

	for (int index = 0; index < 4; index++)
	{
		pieceIndex = m_CurrentPiece->position[index];
		
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
	if (m_CurrentPiece->position[0] < 0 && m_CurrentPiece->position[1] < 0 && m_CurrentPiece->position[2] < 0 && m_CurrentPiece->position[3] < 0)
		return;

	unsigned char red = 0,
		green         = 0, 
		blue          = 0;

	switch (m_CurrentPiece->GetPieceColor())
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
/// Draws in the screen the info 
/// that informs the user the game is on pause
/// </summary>
void Game::DrawOnPause()
{
	if (m_OnPauseText == nullptr)
	{
		m_OnPauseText = m_Renderer->LoadText("Paused");
	}

	m_Renderer->RenderRect(BOARD_TOP_LEFT_X_POS - 100, BOARD_TOP_LEFT_Y_POS + 200, 500, 200, { 200, 200, 200, 200 }, true);
	m_Renderer->RenderTexture(m_OnPauseText, BOARD_TOP_LEFT_X_POS + 120, BOARD_TOP_LEFT_Y_POS + 290);
}

/// <summary>
/// Draws in the screen the info
/// that informs the user the game is over and 
/// what to do to restart or exit to menu
/// </summary>
void Game::DrawGameOver()
{
	if (m_GameOverText == nullptr)
	{
		m_GameOverText     = m_Renderer->LoadText("Game Over!");
		m_GameOverInfoText = m_Renderer->LoadText("Press esc to quit or arrow down to restart");
	}

	m_Renderer->RenderRect(BOARD_TOP_LEFT_X_POS - 100, BOARD_TOP_LEFT_Y_POS + 200, 500, 200, { 200, 200, 200, 200 }, true);
	m_Renderer->RenderTexture(m_GameOverText, BOARD_TOP_LEFT_X_POS + 100, BOARD_TOP_LEFT_Y_POS + 290);
	m_Renderer->RenderTexture(m_GameOverInfoText, BOARD_TOP_LEFT_X_POS + 140, BOARD_TOP_LEFT_Y_POS + 290);
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
	DrawBoard();
	DrawNextPiecesBoard();

	// Check if the game is over
	if (gameOver)
	{
		DrawGameOver();
		return;
	}

	// Check if the game is on pause
	if (isOnPause)
	{
		DrawOnPause();
		return;
	}

	/*
	* Update the column if the current 
	* piece is not already locked
	*/
	if (!newPiece && updateColumn)
	{
		if (CheckIfColumnAvailable())
		{
			UpdateBoard(true);
			m_CurrentPiece->UpdateColumn(targetDirection);
		}

		// Reset control variables
		updateColumn    = false;
		targetDirection = 0;
	}

	if (!newPiece && rotatePiece)
	{
		UpdateBoard(true);
		m_CurrentPiece->Rotate(targetRotation);

		// Validate if with the new rotation the target is over an already inserted piece
		// If it exists then rotate back
		for (int index = 0; index < 4; index++)
		{
			int pieceIndex = m_CurrentPiece->position[index];
			if (m_Board[pieceIndex] != Color::None)
			{
				m_CurrentPiece->Rotate(-(targetRotation));
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
			m_CurrentPiece->UpdatePosition();
		} else
		{
			GenerateNewPiece();
			ClearFilledRows();
			memset(&m_PreviewedPositions, -1, sizeof(int) * 4);
		}

		CheckForPieceLocked();
		CheckIfGameOver();
	}

	UpdateBoard();
}

// ---- GAME BOARD METHODS