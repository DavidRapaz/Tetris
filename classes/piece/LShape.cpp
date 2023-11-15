#include "LShape.h"

#include "../../Color.h"

// STD Includes
#include "stdlib.h"

LShape::LShape()
{

	//Define start column 
	column = rand() % 9 + 1;

	// Define which color is the piece
	switch (rand() % 4 + 1)
	{
	case 1:
		pieceColor = Color::Red;
		break;
	case 2:
		pieceColor = Color::Blue;
		break;
	case 3:
		pieceColor = Color::Green;
		break;
	case 4:
		pieceColor = Color::Orange;
		break;
	}

	// Define piece positions
	// Minor than 0 means it's not to be showned at
	position[0] = -20 + column; // This represents the block of the piece most to the left side
	position[1] = -10 + column;
	position[2] = -0 + column;
	position[3] = 1 + column;
}

LShape::~LShape()
{}

/// <summary>
/// Updates the position of each block of the piece
/// considering it's shape and the param rotation
/// </summary>
/// <param name="rotation"></param>
void LShape::Rotate(int rotation)
{}