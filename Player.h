#pragma once
#include "Point.h"
#include "Direction.h"

class Screen;

class Player
{
private:
	Point point;
	Direction dir = Direction::STAY;
	int speed = 1;
	int live = 3;
public:
	Player(int x, int y, char c) //member initializer list
		: point(x, y, c) {
	}
	void draw() const
	{
		point.draw();
	}
	void setDirection(Direction direction)
	{
		dir = direction;
	}
	int getX() const
	{
		return point.getX();
	}
	int getY() const
	{
		return point.getY();
	}
	void move(const Screen& screen);
	
	Direction getDirection() const
	{
		return dir;
	}

	void updatepoint(int x, int y)
	{
		point.setpoint(x, y);
	}
	
};


