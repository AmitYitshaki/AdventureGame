#pragma once
#include <windows.h>
#include <iostream>

/*
 * ===================================================================================
 * Module: Utils
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Provides static helper functions for low-level Console interactions.
 *
 * Behavior:
 * - gotoxy: Moves the cursor to specific coordinates for non-linear drawing.
 * - hideCursor: Disables the blinking console cursor for a cleaner UI.
 * - cls: Clears the terminal screen.
 *
 * Implementation:
 * - Wraps Windows API (<windows.h>) calls to control the command prompt.
 * ===================================================================================
 */

void gotoxy(int x, int y);
void hideCursor();
void cls();