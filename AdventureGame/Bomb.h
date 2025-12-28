#pragma once
#include "GameObject.h"

/*
 * ===================================================================================
 * Class: Bomb
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A collectible item that can be placed and activated to destroy nearby objects.
 *
 * Behavior:
 * - Starts as a collectible item ('@').
 * - When dropped, it activates a countdown timer.
 * - Visual changes during countdown (3..2..1).
 * - Explodes, triggering destruction logic in the Game class.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Uses an internal `tick()` counter updated by the game loop.
 * ===================================================================================
 */

class Bomb : public GameObject
{
private:
	bool active = false;
	int ticksLeft;
	static constexpr int COUNTDOWN_TICKS = 24;
	static constexpr int EXPLOSION_RADIUS = 3;

public:
	Bomb(int x, int y, ScreenId screen);
	bool handleCollision(Player& p, const Screen& screen) override;
	void activate();
	static constexpr int getExplosionRadius() { return EXPLOSION_RADIUS; }
	bool tick();
};