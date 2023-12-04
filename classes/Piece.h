#pragma once

enum class Shape;
enum class Color;

class Piece
{
public:
	Piece();
	~Piece();

	/// <summary>
	/// Defines which piece function is called to rotate the piece
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
	void UpdateColumn(int& direction);

	/// <summary>
	/// Get the color of the piece
	/// </summary>
	/// <returns>Color</returns>
	Color GetPieceColor();

	/// <summary>
	/// Get the shape of the piece
	/// </summary>
	/// <returns></returns>
	Shape GetPieceShape();

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
	int rotation, previousRotation;

	// ---- Z SHAPE SPECIFIC METHODS

	void ZShapeInit();
	void ZShapeRotate(int& sector);

	// ---- Z SHAPE SPECIFIC METHODS

	// ---- T SHAPE SPECIFIC METHODS

	void TShapeInit();
	void TShapeRotate(int& sector);

	// ---- T SHAPE SPECIFIC METHODS

	// ---- I SHAPE SPECIFIC METHODS

	void IShapeInit();
	void IShapeRotate(int& sector);

	// ---- I SHAPE SPECIFIC METHODS

	// ---- L SHAPE SPECIFIC METHODS
	
	void LShapeInit();
	void LShapeRotate(int& sector);

	// ---- L SHAPE SPECIFIC METHODS

	// ---- BOX SHAPE SPECIFIC METHODS
	
	void BoxShapeInit();
	
	// ---- BOX SHAPE SPECIFIC METHODS
};

