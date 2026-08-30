#include "Screen.h"

#ifndef TETRIS_MENU_H
#define TETRIS_MENU_H

enum class State;

class Menu : public Screen
{
    public:
        Menu();
        ~Menu() override;

        void draw() override;
        void handleEvents(State currentState) override;
};


#endif //TETRIS_MENU_H
