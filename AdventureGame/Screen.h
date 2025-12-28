#pragma once
#include "Point.h"
#include "ScreenID.h"
#include <vector>
#include <string>

/*
 * ===================================================================================
 * Class: Screen
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Represents a single "Room" or "Level" in the game world.
 * It holds the static map layout (walls/floor) and room-specific properties.
 *
 * Behavior:
 * - Validates coordinates (bounds checking).
 * - Checks for wall collisions.
 * - Stores start positions for players.
 * - Manages room attributes like "Darkness" (isDark) or Legend position.
 *
 * Implementation:
 * - Stores the map as a vector of strings (2D character grid).
 * - Does NOT own the GameObjects (those are managed by the Game class).
 * ===================================================================================
 */

class Screen
{
public:
    static constexpr int WIDTH = 80;
    static constexpr int HEIGHT = 25;
    static constexpr int LEGEND_WIDTH = 75;

    Screen() : screenID(ScreenId::HOME) {}
    Screen(ScreenId screenID) : screenID(screenID) {}

    // --- Drawing ---
    void draw() const;

    // --- Queries ---
    bool inBounds(int x, int y) const
    {
        return y >= 0 && y < (int)layout.size() &&
            x >= 0 && x < (int)layout[y].size();
    }

    bool inBounds(const Point& p) const { return inBounds(p.getX(), p.getY()); }

    bool isWall(const Point& p) const
    {
        if (!inBounds(p)) return false;
        return layout[p.getY()][p.getX()] == '#';
    }

    // --- Accessors ---
    ScreenId getScreenId() const { return screenID; }
    void setScreenId(ScreenId id) { screenID = id; }

    bool isDark() const { return dark; }
    void setDark(bool isDark) { dark = isDark; }

    void setLayout(const std::vector<std::string>& newLayout) { layout = newLayout; }

    // Optimized: returns const reference
    const std::string& getLine(int row) const;

    void setLine(int row, const std::string& line);
    void setChar(int x, int y, char c);

    // --- Setup Positions ---
    void setStartPos1(int x, int y) { startPos1.setPos(x, y); }
    void setStartPos2(int x, int y) { startPos2.setPos(x, y); }
    Point getStartPos1() const { return startPos1; }
    Point getStartPos2() const { return startPos2; }

    void setLegendStart(int x, int y) { legendStart.setPos(x, y); hasLegend = true; }
    Point getLegendStart() const { return legendStart; }
    bool hasLegendDefined() const { return hasLegend; }

private:
    ScreenId screenID = ScreenId::HOME;
    std::vector<std::string> layout;
    bool dark = false;
    Point startPos1 = { 1, 1, ' ' };
    Point startPos2 = { 1, 1, ' ' };
    Point legendStart = { 0, 0, ' ' };
    bool hasLegend = false;
};