#pragma once
#include "Screen.h"

class Renderer;
class Piece;

struct SDL_Keysym;

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

	// Store the textures of each screen text
	SDL_Texture* m_OnPauseText = nullptr, 
		* m_GameOverText       = nullptr,
		* m_GameOverInfoText   = nullptr;

	// Stores the current playing piece
	Piece* m_CurrentPiece,
		* m_NextPieces[3] = { nullptr };

	// Time management variables
	float m_Time,
		m_LastFrame,
		m_TimeStep;

	// Stores the previewed positions where the piece will fall
	int m_PreviewedPositions[4];

	/// <summary>
	/// Checks if a row is total filled, if it is then clears it and 
	/// updates the position of the pieces above
	/// </summary>
	void ClearFilledRows();

	/// <summary>
	/// Updates the board with the current piece position
	/// or clears the previous piece position
	/// </summary>
	void UpdateBoard(bool clear = false);

	/// <summary>
	/// Handle the events when a user presses a key or 
	/// stops pressing it
	/// </summary>
	/// <param name="key"></param>
	/// <param name="keyUp"></param>
	void HandleKeyEvents(SDL_Keysym key, bool keyUp = false);

	/// <summary>
	/// Generates a new piece
	/// </summary>
	void GenerateNewPiece();

	/// <summary>
	/// Validates if the current piece can still move or 
	/// if it's movement is locked and updates newPiece variable
	/// </summary>
	void CheckForPieceLocked();

	/// <summary>
	/// Checks if the current piece 
	/// can be updated to the new column
	/// </summary>
	/// <returns></returns>
	bool CheckIfColumnAvailable();

	/// <summary>
	/// Validades if the user cannot make any more moves
	/// </summary>
	void CheckIfGameOver();

	/// <summary>
	/// Check where the piece is going to land
	/// </summary>
	void PreviewPiecePosition();

	/// <summary>
	/// Draws on the screen the piece previewed position
	/// </summary>
	void DrawPiecePreviewedPosition();

	/// <summary>
	/// Draws the game board
	/// </summary>
	void DrawBoard();

	/// <summary>
	/// Draws the board on the right side that contains the next pieces
	/// </summary>
	void DrawNextPiecesBoard();

	/// <summary>
	/// Draws in the screen the info 
	/// that informs the user the game is on pause
	/// </summary>
	void DrawOnPause();

	/// <summary>
	/// Draws in the screen the info
	/// that informs the user the game is over and 
	/// what to do to restart or exit to menu
	/// </summary>
	void DrawGameOver();
};