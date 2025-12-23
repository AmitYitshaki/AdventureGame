#include "Screen.h"
#include "Utils.h" // For gotoxy
#include <iostream>

/*
    Screen.cpp
    Holds the implementation of the draw function.
    Constructors and setters are now inline in Screen.h
*/

void Screen::draw() const
{
    // הזזה לתחילת המסך וציור כל השורות
    gotoxy(0, 0);
    for (const auto& line : layout)
    {
        std::cout << line << '\n';
    }
}