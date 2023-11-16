#include "ZShape.h"

#include "../../Color.h"

// STD Includes
#include "stdlib.h"

#include <ctime>

ZShape::ZShape()
{
	// Define a new seed of the column selection
	srand(std::time(nullptr));

	//Define start column 
	column = rand() % 9 + 1;

	// Define a new seed for the piece color render
	srand(std::time(nullptr));

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
	position[0] = -10 + column; // This represents the block of the piece most to the left side
	position[1] = -9 + column;
	position[2] = 1 + column;
	position[3] = 2 + column;
}

ZShape::~ZShape()
{}

/// <summary>
/// Updates the position of each block of the piece
/// considering it's shape and the param rotation
/// </summary>
/// <param name="rotation"></param>
void ZShape::Rotate(int rotation)
{}