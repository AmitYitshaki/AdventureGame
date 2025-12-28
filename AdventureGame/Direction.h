#pragma once

/*
 * ===================================================================================
 * Enum: Direction
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Represents movement vectors (UP, DOWN, LEFT, RIGHT, STAY).
 * Used by Player logic, Physics calculations, and Input handling.
 * ===================================================================================
 */

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STAY
};
