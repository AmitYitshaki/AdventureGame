#pragma once

#include "Point.h"
#include "ScreenID.h"
#include <vector>
#include <string>

class Screen
{
public:
    static constexpr int WIDTH = 80;
    static constexpr int HEIGHT = 25;

    Screen() : screenID(ScreenId::HOME) {}
    Screen(ScreenId screenID) : screenID(screenID) {}

    // --- Drawing ---
    void draw() const; // מימוש ב-cpp

    // --- Queries & Getters ---
    bool isWall(const Point& p) const {
        if (p.getY() >= 0 && p.getY() < layout.size() && p.getX() >= 0 && p.getX() < layout[p.getY()].size())
            return layout[p.getY()][p.getX()] == '#';
        return false;
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

    ScreenId getScreenId() const { return screenID; }
    void setScreenId(ScreenId id) { screenID = id; }

    ScreenId getScreenId() const
    {
        return screenID;
    }

    bool isDark() const { return dark; }
    void setDark(bool isDark) { dark = isDark; }

    // --- Layout Management ---
    void setLayout(const std::vector<std::string>& newLayout) { layout = newLayout; }

    std::string getLine(int row) const {
        if (row >= 0 && row < layout.size()) return layout[row];
        return "";
    }

    void setLine(int row, const std::string& line) {
        if (row >= 0 && row < HEIGHT) {
            if (layout.size() < HEIGHT) layout.resize(HEIGHT, std::string(WIDTH, ' '));
            layout[row] = line;
        }
    }

    void setChar(int x, int y, char c) {
        if (y >= 0 && y < layout.size() && x >= 0 && x < layout[y].size()) {
            layout[y][x] = c;
        }
    }

    // --- Player Start Positions ---
    void setStartPos1(int x, int y) { startPos1.setPos(x, y); }
    void setStartPos2(int x, int y) { startPos2.setPos(x, y); }
    Point getStartPos1() const { return startPos1; }
    Point getStartPos2() const { return startPos2; }

    // --- Legend Position (NEW) ---
    void setLegendStart(int x, int y) { legendStart.setPos(x, y); hasLegend = true; }
    Point getLegendStart() const { return legendStart; }
    bool hasLegendDefined() const { return hasLegend; }

private:
    ScreenId screenID = ScreenId::HOME;
    std::vector<std::string> layout;
    bool dark = false;

    Point startPos1 = { 1, 1, ' ' };
    Point startPos2 = { 1, 1, ' ' };

    // מיקום ה-Legend (ברירת מחדל 0,0)
    Point legendStart = { 0, 0, ' ' };
    bool hasLegend = false;
};