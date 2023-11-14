#pragma once

enum class Shape;
enum class Color;

class Piece
{
public:
	Piece();
	~Piece();
	
	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and a given rotation
	/// </summary>
	/// <param name="rotation"></param>
	void Rotate(int rotation);

	/// <summary>
	/// Update the position of each piece as it's falling
	/// </summary>
	void UpdatePosition();

	/// <summary>
	/// Get the color of the piece
	/// </summary>
	/// <returns>Color</returns>
	Color GetPieceColor();

	int rotation = 0, // Controls the piece rotation
		position[4]; // Stores the coordinates for each block of the piece
private:
	// Define the piece shape
	Shape pieceShape;

	// Define the piece color
	Color pieceColor;

	int column;
};

