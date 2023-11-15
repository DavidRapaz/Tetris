#pragma once
#include "Screen.h"
#include "../piece/Piece.h"

class Renderer;

enum class State;
enum class Color;

class Game : public Screen
{
public:
	Game(Renderer* gameRenderer);
	~Game();

	/// <summary>
	/// Handle the screen events
	/// </summary>
	virtual void HandleEvents(State& gameState) override;

	/// <summary>
	/// Draws the what is targeted to show in the screen
	/// </summary>
	virtual void Draw() override;

private:
	Color m_Board[200]; // 10 column * 20 rows

	// Store the textures of each color
	SDL_Texture* m_Red, 
		* m_Blue, 
		* m_Green, 
		* m_Orange;

	// Stores the current playing piece
	Piece* currentPiece;

	// Time management variables
	float time,
		lastFrame,
		timeStep;

	/// <summary>
	/// Updates the board with the current piece position
	/// or clears the previous piece position
	/// </summary>
	void UpdateBoard(bool clear = false);

	/// <summary>
	/// Draws the game board
	/// </summary>
	void DrawBoard();
};