#include "Screen.h"
#include <iostream>

/*
    Screen.cpp
    Holds the ASCII layouts of all screens and implements
    basic drawing and initialization logic.
*/

Screen::Screen() // Default constructor → HOME screen
{
    screenID = ScreenId::HOME;
}

Screen::Screen(ScreenId id)
    : screenID(id)
{}

void Screen::draw() const
{
    gotoxy(0, 0);
    for (const auto& line : layout)
    {
        std::cout << line<< '\n';
    }
}


