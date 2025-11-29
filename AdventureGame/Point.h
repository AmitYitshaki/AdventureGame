#pragma once
#include "Utils.h"
using namespace std;


class Point {
	int x = 1, y = 1;
	char ch = '*';
public:
	Point() {} //default constructor
	Point(int x1, int y1, char c) //member initializer list
		: x(x1), y(y1), ch(c) {}

	void draw() const {
		gotoxy(x, y);
		cout << ch;
		cout.flush();
	}

	void setpoint(int x1, int y1) {
		x = x1;
		y = y1;
	}

	int getX() const {
		return x;
	}
	int getY() const {
		return y;
	}

	void move(int dx, int dy);
};