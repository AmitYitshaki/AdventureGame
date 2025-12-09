#include "Utils.h"

/*
    Utils.cpp
    Console helper functions:
    - move cursor
    - hide cursor
    - clear screen
*/

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;

    std::cout.flush();
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdOut, &curInfo);

    curInfo.bVisible = FALSE; // Set TRUE to show cursor again
    SetConsoleCursorInfo(hStdOut, &curInfo);
}

void cls()
{
    system("cls");
}
