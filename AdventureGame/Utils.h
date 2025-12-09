#pragma once

#include <windows.h>
#include <iostream>

/*
    Console utility functions:
    - gotoxy : moves cursor to (x,y)
    - hideCursor : hides blinking caret
    - cls : clears screen
*/

void gotoxy(int x, int y);
void hideCursor();
void cls();
