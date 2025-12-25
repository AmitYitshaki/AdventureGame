#pragma once
#include "Point.h"
#include "ScreenID.h"
#include "GameObject.h"
#include <vector>
#include <string>

// forward declarations (כדי שה-header יתקמפל מהר ונקי)
class Player;
class Screen;

class Bomb : public GameObject
{
private:
	bool active = false;        // האם הפצצה פעילה (סופרת לאחור)
	bool exploded = false;
	static constexpr int COUNTDOWN_TICKS = 24; // מספר הטיקים עד הפיצוץ	
	int ticksLeft = COUNTDOWN_TICKS; // טיקים שנותרו עד הפיצוץ
	static constexpr int EXPLOSION_RADIUS = 3; // רדיוס הפיצוץ
public:
	// Constructor
	Bomb(int x, int y, ScreenId screen);
		
	bool handleCollision(Player& p, const Screen& screen) override;

	void activate(); // Activate the bomb (start countdown)

	bool isactive() const { return active; }

	bool isExploded() const { return exploded; }
	
	static constexpr int getExplosionRadius() { return EXPLOSION_RADIUS; }

	bool tick(); // Returns true if exploded this tick

};

