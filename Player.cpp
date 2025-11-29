#include "Player.h"
#include "Screen.h"
using namespace std;


void Player::move(const Screen& screen) {
    int dx = 0, dy = 0;

    switch (dir) {
    case Direction::UP:
        dy = -1;
        break;
    case Direction::DOWN:
        dy = 1;
        break;
    case Direction::LEFT:
        dx = -1;
        break;
    case Direction::RIGHT:
        dx = 1;
        break;
    case Direction::STAY:
        return; 
    }
	Point next(point.getX() + dx * speed, point.getY() + dy * speed, ' ');
	if (screen.isWall(next)) {
		dir = Direction::STAY; // Fix: use member variable directly
        return; 
	}
    point.move(dx * speed, dy * speed);
}