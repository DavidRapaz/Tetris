#pragma once

enum class Shape;
enum class Color;

class Piece
{
public:
	Piece() {};
	~Piece() {};
	
	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and a given rotation
	/// </summary>
	/// <param name="rotation"></param>
	virtual void Rotate(int rotation) = 0;

	/// <summary>
	/// Update the position of each 
	/// block of the piece as it's falling
	/// </summary>
	void UpdatePosition()
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
	/// Updates the column in which every block of the piece is respresented
	/// using a param direction to choose between left and right
	/// </summary>
	/// <param name="direction"></param>
	void UpdateColumn(int direction)
	{
		/*
		 * TODO:
		 * 
		 * - Validade if the piece is already 
		 * on the edge of the board
		 * 
		 * - If the piece is not on the edge
		 * update the piece column
		 */
	}

	/// <summary>
	/// Get the color of the piece
	/// </summary>
	/// <returns>Color</returns>
	Color GetPieceColor()
	{
		return pieceColor;
	}

	int position[4]; // Stores the coordinates for each block of the piece
protected:
	// Define the piece color
	Color pieceColor;

	// Defines where the block most to the left is
	int column;
};

