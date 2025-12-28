#include "Screen.h"
#include "Utils.h"
#include <iostream>

void Screen::draw() const
{
    gotoxy(0, 0);
    for (const auto& line : layout)
    {
        std::cout << line << '\n';
    }
}

const std::string& Screen::getLine(int row) const
{
    static const std::string emptyLine = "";
    if (row >= 0 && row < (int)layout.size()) return layout[row];
    return emptyLine;
}

void Screen::setLine(int row, const std::string& line)
{
    if (row >= 0 && row < HEIGHT) {
        if (layout.size() < HEIGHT) layout.resize(HEIGHT, std::string(WIDTH, ' '));
        layout[row] = line;
    }
}

void Screen::setChar(int x, int y, char c)
{
    if (inBounds(x, y)) {
        layout[y][x] = c;
    }
}