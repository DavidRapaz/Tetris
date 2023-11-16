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

	/// <summary>
	/// Get the color of the piece
	/// </summary>
	/// <returns>Color</returns>
	Color GetPieceColor()
	{
		return pieceColor;
	}

	// Stores the coordinates for each block of the piece
	int position[4];
protected:
	// Define the piece color
	Color pieceColor;

	// Defines where the block most to the left is
	int column;
};

