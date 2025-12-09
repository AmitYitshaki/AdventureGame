#pragma once

#include "Utils.h"

/*
    Point:
    Represents a drawable position on the console with a character.
    Used by all world objects and the players.
*/
class Point
{
private:
    int x = 1;
    int y = 1;
    char ch = '*';

public:
    // --- Constructors ---
    Point() = default;

    Point(int x1, int y1, char c)
        : x(x1), y(y1), ch(c)
    {
    }

    // --- Drawing ---
    void draw() const
    {
        gotoxy(x, y);
        std::cout << ch;
    }


    // --- Position ---
    void setPos(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    void move(int dx, int dy);

    // --- Getters ---
    int getX() const { return x; }
    int getY() const { return y; }
    char getChar() const { return ch; }

    void setChar(char c) { ch = c; }
};
