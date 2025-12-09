#pragma once

#include "Point.h"
#include "ScreenID.h"

/*
    Screen:
    Holds the static ASCII layout for a specific room / UI screen.
    Provides wall queries, dark flag for lighting, and row access
    for the double-buffered renderer in Game.
*/
class Screen
{
public:
    static constexpr int WIDTH = 80;
    static constexpr int HEIGHT = 25;

    // --- Constructors ---
    Screen();                 // Default: HOME layout
    Screen(ScreenId screenID); // Specific screen

    // --- Drawing ---
    void draw() const;

    // --- Queries ---
    bool isWall(const Point& p) const
    {
        return layout[p.getY()][p.getX()] == '#';
    }

    ScreenId getScreenId() const
    {
        return screenID;
    }

    bool isDark() const
    {
        return dark;
    }

    void setDark(bool isDark)
    {
        dark = isDark;
    }

    // Return a raw C-string for a given row (used to init buffer)
    const char* getLine(int row) const
    {
        if (row >= 0 && row < HEIGHT && layout != nullptr)
            return layout[row];

        return "";
    }

private:
    ScreenId screenID = ScreenId::HOME;
    const char* const* layout = nullptr; // Pointer to the static layout table
    bool dark = false;                   // Used by lighting logic in Game

    // Static layouts for all screens
    static const char* HOME_LAYOUT[HEIGHT];
    static const char* INSTRUCTIONS_LAYOUT[HEIGHT];
    static const char* ROOM1_LAYOUT[HEIGHT];
    static const char* ROOM2_LAYOUT[HEIGHT];
    static const char* ROOM3_LAYOUT[HEIGHT];
};
