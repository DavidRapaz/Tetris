#pragma once
#include "Piece.h"

class LShape : public Piece
{
public:
	LShape();
	~LShape();

	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and the param rotation
	/// </summary>
	/// <param name="rotation"></param>
	virtual void Rotate(int rotation) override;
};
