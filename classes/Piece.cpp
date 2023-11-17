#include "Piece.h"

#include "../Shape.h"
#include "../Color.h"

// STD Includes
#include "stdlib.h"

#include <ctime>

// ---- INITIALIZATION METHODS

Piece::Piece()
{
	// Define start column
	srand(std::time(nullptr));
	column = rand() % 10; // each row goes from 0 to 9
	// Define piece color
	srand(std::time(nullptr));
	pieceColor = static_cast<Color>(rand() % 4 + 1);

	// Define and initialize piece shape
	srand(std::time(nullptr));
	pieceShape = static_cast<Shape>(rand() % 5 + 1);

	switch (pieceShape)
	{
	case Shape::TShape:
		TShapeInit();
		break;
	case Shape::ZShape:
		ZShapeInit();
		break;
	case Shape::LShape:
		LShapeInit();
		break;
	case Shape::IShape:
		IShapeInit();
		break;
	case Shape::BoxShape:
		BoxShapeInit();
		break;
	}
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the block shape
/// </summary>
void Piece::TShapeInit()
{
	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -10 + column; // This represents the block of the piece most to the left side
	position[1] = -9 + column;
	position[2] = -8 + column;
	position[3] = 1 + column;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the Z shape
/// </summary>
void Piece::ZShapeInit()
{
	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -10 + column; // This represents the block of the piece most to the left side
	position[1] = -9 + column;
	position[2] = 1 + column;
	position[3] = 2 + column;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the L shape
/// </summary>
void Piece::LShapeInit()
{
	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -20 + column; // This represents the block of the piece most to the left side
	position[1] = -10 + column;
	position[2] = -0 + column;
	position[3] = 1 + column;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the I shape
/// </summary>
void Piece::IShapeInit()
{	
	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -30 + column; // This represents the block of the piece most to the left side
	position[1] = -20 + column;
	position[2] = -10 + column;
	position[3] = 0 + column;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the Box shape
/// </summary>
void Piece::BoxShapeInit()
{
	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -10 + column; // This represents the block of the piece most to the left side
	position[1] = -9 + column;
	position[2] = 0 + column;
	position[3] = 1 + column;
}

// ---- INITIALIZATION METHODS

Piece::~Piece()
{}

// ---- PIECE MOVEMENT METHODS

void Piece::UpdateColumn(int direction)
{
	/*
	 * Each row goes from 0 to 9
	 * which means it's 10 indexes per row
	 * so if we do the modulus of the current position
	 * by 10 we know if it's in the edge if the result is either 0 or 9
	 */
	for (int index = 0; index < 4; index++)
	{
		if (position[0] % 10 == 0 || position[0] % 10 == 9)
			return;
	}

	// Update piece position
	for (int index = 0; index < 4; index++)
	{
		position[index] += direction;
	}
}

void Piece::UpdatePosition()
{
	for (int index = 0; index < 4; index++)
	{
		/*
		 * Validate if the new coordinate won't be bigger
		 * than the max coordinate of the game board
		 * declared in the Game class
		 */
		if (position[index] + 10 <= 199)
		{
			position[index] += 10;
		}
	}
}

void Piece::Rotate(int rotation)
{

}

// ---- PIECE MOVEMENT METHODS

Color Piece::GetPieceColor()
{
	return pieceColor;
}