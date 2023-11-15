#pragma once
#include "Piece.h"

class ZShape : public Piece
{
public:
	ZShape();
	~ZShape();

	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and the param rotation
	/// </summary>
	/// <param name="rotation"></param>
	virtual void Rotate(int rotation) override;
};