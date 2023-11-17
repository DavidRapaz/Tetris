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
	/// Update the position of each 
	/// block of the piece as it's falling
	/// </summary>
	void UpdatePosition();

	/// <summary>
	/// Updates the column in which every block of the piece is respresented
	/// using a param direction to choose between left and right
	/// </summary>
	/// <param name="direction"></param>
	void UpdateColumn(int direction);

	/// <summary>
	/// Get the color of the piece
	/// </summary>
	/// <returns>Color</returns>
	Color GetPieceColor();

	// Stores the coordinates for each block of the piece
	int position[4];
private:
	// Define the piece color
	Color pieceColor;

	// Store the piece shape
	Shape pieceShape;

	// Defines where the block most to the left is
	int column;

	// Stores the current piece rotation
	int rotation;

	// ---- Z SHAPE SPECIFIC METHODS

	void ZShapeInit();
	void ZShapeRotate(int& rotation);

	// ---- Z SHAPE SPECIFIC METHODS

	// ---- T SHAPE SPECIFIC METHODS

	void TShapeInit();
	void TShapeRotate(int& rotation);

	// ---- T SHAPE SPECIFIC METHODS

	// ---- I SHAPE SPECIFIC METHODS

	void IShapeInit();
	void IShapeRotate(int& rotation);

	// ---- I SHAPE SPECIFIC METHODS

	// ---- L SHAPE SPECIFIC METHODS
	
	void LShapeInit();
	void LShapeRotate();

	// ---- L SHAPE SPECIFIC METHODS

	// ---- BOX SHAPE SPECIFIC METHODS
	
	void BoxShapeInit();
	void BoxShapeRotate();
	
	// ---- BOX SHAPE SPECIFIC METHODS
};

