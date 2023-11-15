#pragma once
#include "Piece.h"

class TShape : public Piece
{
public:
	TShape();
	~TShape();

	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and the param rotation
	/// </summary>
	/// <param name="rotation"></param>
	virtual void Rotate(int rotation) override;
};
