#pragma once
#include "Piece.h"

class BoxShape : public Piece
{
public:
	BoxShape();
	~BoxShape();

	/// <summary>
	/// Updates the position of each block of the piece
	/// considering it's shape and the param rotation
	/// </summary>
	/// <param name="rotation"></param>
	virtual void Rotate(int rotation) override;
private:
};