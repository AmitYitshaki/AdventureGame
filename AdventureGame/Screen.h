#pragma once
#include "Point.h"
#include "ScreenID.h"

class Screen
{
public:
	static constexpr int WIDTH = 80;
	static constexpr int HEIGHT = 25;
private:
	const char* const* layout = nullptr; // Initialize to nullptr

	// כאן נגדיר את כל ה-layouts
	static const char* HOME_LAYOUT[HEIGHT];
	static const char* INSTRUCTIONS_LAYOUT[HEIGHT];
	static const char* ROOM1_LAYOUT[HEIGHT];
	static const char* ROOM2_LAYOUT[HEIGHT];
	static const char* ROOM3_LAYOUT[HEIGHT];
public:
	Screen(); //defult ctor
	Screen(ScreenId screenID); //ctor

	void draw() const;

	bool isWall(const Point& p) const {
		return layout[p.getY()][p.getX()] == '#';
	}
};

