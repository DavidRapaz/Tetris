#include "Piece.h"
#include "../Color.h"
#include "../Shape.h"

Piece::Piece()
{
	pieceColor = Color::Blue;
	pieceShape = Shape::BoxShape;
	
	// Define which column the piece most to the left starts
	column = 2;
	
	// Initialize the position
	// Minor than 0 means it's not to be showned at
	position[0] = -10 + column; // This represents the piece to the left side
	position[1] = -10 + column + 1;
	position[2] = 0 + column;
	position[3] = 1 + column;
	
	// Initialize Rotation
	rotation = 0;
}

Piece::~Piece()
{ }

/// <summary>
/// Updates the position of each block of the piece
/// considering it's shape and a given rotation
/// </summary>
/// <param name="rotation"></param>
void Piece::Rotate(int rotation)
{ }

/// <summary>
/// Update the position of each piece as it's falling
/// </summary>
void Piece::UpdatePosition()
{
	for (int index = 0; index < 4; index++)
	{
		if (position[index] + 10 <= 199)
		{
			position[index] += 10;
		}
	}
}

/// <summary>
/// Get the color of the piece
/// </summary>
/// <returns>Color</returns>
Color Piece::GetPieceColor()
{
	return pieceColor;
}