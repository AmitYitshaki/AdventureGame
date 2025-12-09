#include "Point.h"

/*
    Moves the point by (dx, dy) without drawing.
*/
void Point::move(int dx, int dy)
{
    x += dx;
    y += dy;
}
