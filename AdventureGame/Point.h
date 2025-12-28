#pragma once
#include "Utils.h"
#include <iostream>

/*
 * ===================================================================================
 * Class: Point
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Represents a single coordinate (X, Y) on the console grid, associated with a
 * specific visual character.
 *
 * Behavior:
 * - Serves as the atomic unit for position and rendering in the game.
 * - Handles the actual "Draw" operation.
 * ===================================================================================
 */

class Point
{
public:
    Point() = default;
    Point(int x1, int y1, char c) : x(x1), y(y1), ch(c) {}

    // --- Drawing ---
    void draw() const
    {
        gotoxy(x, y);
        std::cout << ch;
    }

    // --- Movement ---
    void setPos(int newX, int newY) { x = newX; y = newY; }

    void move(int dx, int dy) { x += dx; y += dy; }

    // --- Accessors ---
    int getX() const { return x; }
    int getY() const { return y; }
    char getChar() const { return ch; }
    void setChar(char c) { ch = c; }

private:
    int x = 1;
    int y = 1;
    char ch = '*';
};