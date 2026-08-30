#ifndef TETRIS_SCREEN_H
#define TETRIS_SCREEN_H

enum class State;

class Renderer;

class Screen
{
public:
    virtual ~Screen() = default;

    virtual void draw() = 0;
    virtual void handleEvents(State currentState) = 0;

private:
    Renderer *m_Renderer = nullptr;
};

#endif //TETRIS_SCREEN_H
