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
	// Define piece color
	srand(std::time(nullptr));
	pieceColor = static_cast<Color>(rand() % 4 + 1);

	// Define and initialize piece shape
	srand(std::time(nullptr));
	pieceShape = static_cast<Shape>(rand() % 5 + 1);

	// Define starting column of the piece
	srand(std::time(nullptr));
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
	column = rand() % 8; // due to it's shape can only be inserted between 0 and 7 columns

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
	column = rand() % 8; // due to it's shape can only be inserted between 0 and 7 columns

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
	column = rand() % 9; // due to it's shape can only be inserted between 0 and 9 columns

	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -20 + column; // This represents the block of the piece most to the left side
	position[1] = -10 + column;
	position[2] = 0 + column;
	position[3] = 1 + column;
}

/// <summary>
/// Initializes the positions of the 
/// pieces for the I shape
/// </summary>
void Piece::IShapeInit()
{
	column = rand() % 10; // can start at any column

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
	column = rand() % 9; // due to it's shape can only be inserted between 0 and 8 columns

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
	// Update the previous rotation
	this->previousRotation = this->rotation;

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
		TShapeRotate(sector);
		break;
	case Shape::ZShape:
		ZShapeRotate(sector);
		break;
	case Shape::LShape:
		LShapeRotate(sector);
		break;
	case Shape::IShape:
		IShapeRotate(sector);
		break;
	}
}

/// <summary>
/// Rotates the T Shape piece since -1 = -90º and 1 = 90º
/// In the shape every piece rotates around the base of the T
/// which means only the 3 first indexes need to be updated
/// </summary>
/// <param name="rotation"></param>
void Piece::TShapeRotate(int& sector)
{
	// Check if the position of the base is almost at
	if (
		(position[3] % 10 >= 8 && sector == 1) ||
		(position[3] % 10 <= 1 && sector == 3)
	)
	{
		this->rotation = this->previousRotation;
		return;
	}

	// Control how much to increment to each position
	// Make them with the default value of the base of the T shape
	// Then we will use that position to consider the rotation values
	int topLeft   = position[3],
		topCenter = position[3],
		topRight  = position[3];

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
/// <param name="sector"></param>
void Piece::ZShapeRotate(int& sector)
{
	// Check if the position of the base is almost at
	if (
		(position[2] % 10 >= 9 && sector == 1) ||
		(position[2] % 10 <= 0 && sector == 3)
		)
	{
		this->rotation = this->previousRotation;
		return;
	}

	// The center of the Z Shape piece is at the index 2
	int topLeft     = position[2], // The piece at the top most to the left
		topCenter   = position[2], // The piece at the top on the center
		bottomRight = position[2]; // The piece at the bottom most to the right

	topLeft     += sector == 1 ? -9 : sector == 2 ? 11 : sector == 3 ? 9 : -11;
	topCenter   += sector == 1 ? 1 : sector == 2 ? 10 : sector == 3 ? -1 : -10;
	bottomRight += sector == 1 ? 10 : sector == 2 ? -1 : sector == 3 ? -10 : 1;

	position[0] = topLeft;
	position[1] = topCenter;
	position[3] = bottomRight;
}

/// <summary>
/// Rotates the L Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::LShapeRotate(int& sector)
{
	// Check if the position of the base is almost at
	if (
		(position[2] % 10 >= 8 && sector == 1) ||
		(position[2] % 10 <= 1 && sector == 3)
	)
	{
		this->rotation = this->previousRotation;
		return;
	}

	// The center of the L Shape piece is at the index 2
	int topMax      = position[2],
		topCenter   = position[2],
		bottomRight = position[2];

	topMax      += sector == 1 ? 2 : sector == 2 ? 20 : sector == 3 ? -2 : -20;
	topCenter   += sector == 1 ? 1 : sector == 2 ? 10 : sector == 3 ? -1 : -10;
	bottomRight += sector == 1 ? 10 : sector == 2 ? -1 : sector == 3 ? -10 : 1;

	position[0] = topMax;
	position[1] = topCenter;
	position[3] = bottomRight;
}

/// <summary>
/// Rotates the I Shape piece since -1 = -90º and 1 = 90º
/// </summary>
/// <param name="rotation"></param>
void Piece::IShapeRotate(int& sector)
{
	// Check if the position of the base is almost at
	if (
		(position[3] % 10 >= 7 && sector == 1) || 
		(position[3] % 10 <= 2 && sector == 3)
	)
	{
		this->rotation = this->previousRotation;
		return;
	}

	// The center of the I Shape piece is at the index 3
	int topMax    = position[3],
		topCenter = position[3],
		topMin    = position[3];

	topMax    += sector == 1 ? 3 : sector == 2 ? 30 : sector == 3 ? -3 : -30;
	topCenter += sector == 1 ? 2 : sector == 2 ? 20 : sector == 3 ? -2 : -20;
	topMin    += sector == 1 ? 1 : sector == 2 ? 10 : sector == 3 ? -1 : -10;

	position[0] = topMax;
	position[1] = topCenter;
	position[2] = topMin;
}

// ---- PIECE MOVEMENT METHODS

Color Piece::GetPieceColor()
{
	return pieceColor;
}