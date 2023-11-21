#include "Piece.h"

#include "../Shape.h"
#include "../Color.h"

// STD Includes
#include "stdlib.h"

#include <algorithm>
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

	rotation = 0;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the block shape
/// </summary>
void Piece::TShapeInit()
{
	if (column > 7)
		column = 7;

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
	if (column > 8)
		column = 8;

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
	if (column > 8)
		column = 8;

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
	if (column > 8)
		column = 8;

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

void Piece::UpdateColumn(int& direction)
{
	/*
	 * Each row goes from 0 to 9
	 * which means it's 10 indexes per row
	 * so by doing the modulus of the current position
	 * by 10 we know it's in the edge if the result is either 0 or 9
	 */
	for (int index = 0; index < 4; index++)
	{
		if (
			(position[index] % 10 == 0 && direction == -1) || 
			(position[index] % 10 == 9 && direction == 1) ||
			(position[index] + direction > 199)
		)
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

/// <summary>
/// Defines which piece function is called to rotate the piece
/// </summary>
/// <param name="rotation"></param>
void Piece::Rotate(int rotation)
{
	// Force rotation to be between 0 and 360 never negative numbers
	if (this->rotation == 0 && rotation == -1)
		this->rotation = 270;
	else
		this->rotation += rotation * 90;

	// Reset to 0 when does a full circle
	if (this->rotation == 360)
		this->rotation = 0;

	/*
	* Sector 0 = 0º
	* Sector 1 = 90º
	* Sector 2 = 180º
	* Sector 3 = 270º
	*/

	int sector = this->rotation / 90;

	switch (pieceShape)
	{
	case Shape::TShape:
		TShapeRotate(rotation, sector);
		break;
	case Shape::ZShape:
		ZShapeRotate(rotation, sector);
		break;
	case Shape::LShape:
		LShapeRotate(rotation, sector);
		break;
	case Shape::IShape:
		IShapeRotate(rotation, sector);
		break;
	}
}

/// <summary>
/// Rotates the T Shape piece since -1 = -90º and 1 = 90º
/// In the shape every piece rotates around the base of the T
/// which means only the 3 first indexes need to be updated
/// </summary>
/// <param name="rotation"></param>
void Piece::TShapeRotate(int& rotation, int& sector)
{
	// Control how much to increment to each position
	// Make them with the default value of the base of the T shape
	// Then we will use that position to consider the rotation values
	int topLeft   = position[3],
		topCenter = position[3],
		topRight  = position[3];

	// The sectors 1 and 3 change are different depending if it's rotating to the right or to the left
	topLeft   += sector == 1 ? -9 : sector == 2 ? 11 : sector == 3 ? 9 : -11;
	topCenter += sector == 1 ? 1 : sector == 2 ? 10 : sector == 3 ? -1 : -10;
	topRight  += sector == 1 ? 11 : sector == 2 ? 9 : sector == 3 ? -11 : -9;

	position[0] = topLeft;
	position[1] = topCenter;
	position[2] = topRight;
}

/// <summary>
/// Rotates the Z Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::ZShapeRotate(int& rotation, int& sector)
{

}

/// <summary>
/// Rotates the L Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::LShapeRotate(int& rotation, int& sector)
{

}

/// <summary>
/// Rotates the I Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::IShapeRotate(int& rotation, int& sector)
{

}

/// <summary>
/// Rotates the Box Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::BoxShapeRotate(int& rotation, int& sector)
{

}

// ---- PIECE MOVEMENT METHODS

Color Piece::GetPieceColor()
{
	return pieceColor;
}