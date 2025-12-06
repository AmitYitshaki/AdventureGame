#pragma once
#include "Point.h"
#include "ScreenID.h"

class Screen
{
public:
	static constexpr int WIDTH = 80;
	static constexpr int HEIGHT = 25;
private:
	ScreenId screenID;
	const char* const* layout = nullptr; // Initialize to nullptr
	bool dark = false; 

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
	ScreenId getScreenId() {
		return screenID;
	}
	
	bool isDark() const {
		return dark;
	}

	void setDark(bool isDark) {
		dark = isDark;
	}


	const char* getLine(int row) const {
		if (row >= 0 && row < HEIGHT && layout != nullptr) {
			return layout[row];
		}
		return "";
	}
};

