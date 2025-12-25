#pragma once

#include "Point.h"
#include "ScreenID.h"
#include <vector>
#include <string>

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
    bool inBounds(int x, int y) const
    {
        return y >= 0 && y < (int)layout.size() &&
            x >= 0 && x < (int)layout[y].size();
    }

    bool inBounds(const Point& p) const
    {
        return inBounds(p.getX(), p.getY());
    }

    bool isWall(const Point& p) const
    {
        if (!inBounds(p)) return false;
        return layout[p.getY()][p.getX()] == '#';
    }


    ScreenId getScreenId() const
    {
        return screenID;
    }

    void setScreenId(ScreenId id)
    {
        screenID = id;
	}

    bool isDark() const
    {
        return dark;
    }

    void setDark(bool isDark)
    {
        dark = isDark;
    }

    void setLayout(const std::vector<std::string>& newLayout)
    {
        layout = newLayout;
	}

    // Return a raw C-string for a given row (used to init buffer)
    std::string getLine(int row) const
    {
		if (row >= 0 && row < layout.size())
            return layout[row];
        return "";
    }

    void setStartPos1(int x, int y) { startPos1.setPos(x, y); }
    void setStartPos2(int x, int y) { startPos2.setPos(x, y); }

    Point getStartPos1() const { return startPos1; }
    Point getStartPos2() const { return startPos2; }

    void setLine(int row, const std::string& line)
    {
        if (row >= 0 && row < HEIGHT) {
            if (layout.size() < HEIGHT)
                layout.resize(HEIGHT, std::string(WIDTH, ' '));
            layout[row] = line;
        }
    }
    void setChar(int x, int y, char c)
    {
        if (y >= 0 && y < layout.size() &&
            x >= 0 && x < layout[y].size()) {
            layout[y][x] = c;
        }
	}

private:
    ScreenId screenID = ScreenId::HOME;
	std::vector<std::string> layout; // vector of strings for dynamic access
    bool dark = false;                   // Used by lighting logic in Game
    Point startPos1 = { 1, 1, ' ' }; // נקודת התחלה לשחקן 1
    Point startPos2 = { 1, 1, ' ' }; // נקודת התחלה לשחקן 2
};
