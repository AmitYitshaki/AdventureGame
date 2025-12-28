#pragma once

/*
 * ===================================================================================
 * Enum: ScreenId
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Defines unique identifiers for all game screens (Levels, Menu, Instructions).
 * Used by the ScreenManager/Game class to handle transitions and array indexing.
 * ===================================================================================
 */

enum class ScreenId
{
    HOME = 0,
    INSTRUCTIONS = 1,
    ROOM1 = 2,
    ROOM2 = 3,
    ROOM3 = 4,
    ROOM4 = 5,
    ROOM5 = 6,
    ROOM6 = 7,
    ROOM7 = 8,
    ROOM8 = 9,
    NumOfScreens = 10
};