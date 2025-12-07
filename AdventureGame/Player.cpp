#include "Player.h"
#include "Screen.h"
#include <vector>
#include "GameObject.h"
#include "Key.h"
#include "Torch.h"
#include "Riddle.h"
#include "Door.h"


Player::~Player() {
    if (heldItem != nullptr) {
        delete heldItem;
    }
}

void Player::move(Screen& screen, std::vector<GameObject*>& gameObjects) {

    if (isFlying())
    {
        moveFlying(screen, gameObjects);
        return;
    }
    
    int dx = 0, dy = 0;

    switch (dir) {
    case Direction::UP: dy = -1; dx = 0; break;
    case Direction::DOWN: dy = 1; dx = 0; break;
    case Direction::LEFT: dx = -1; dy = 0; break;
    case Direction::RIGHT: dx = 1; dy = 0; break;
    case Direction::STAY: return;
    }

    Point next(point.getX() + dx, point.getY() + dy, ' ');

    if (screen.isWall(next)) {
        stopMovement();
        return;
    }

    point.move(dx,dy);

    ScreenId currentScreenId = screen.getScreenId();
    int playerX = point.getX();
    int playerY = point.getY();

    for (auto obj : gameObjects) {
        if (obj->getScreenId() != currentScreenId)
			continue; // Skip objects not in the current screen
        int objX = obj->getX();
        int objY = obj->getY();
        if (playerX == objX && playerY == objY && !obj->isCollected())
            {
			bool allowed = obj->handleCollision(*this, screen);
            if (!allowed) {
                point.move(-dx * speed, -dy * speed);
                dir = Direction::STAY;
                }
            }
        }
    }

void Player::moveFlying(Screen& screen, std::vector<GameObject*>& gameObjects)
{
    int dx = 0, dy = 0;

    // כיוון בסיס מהקפיץ
    switch (launchDirection) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1;  break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1;  break;
    default: return;
    }

    // סטייה אנכית לפי dir
    if (dir == Direction::UP)        dy -= 1;
    else if (dir == Direction::DOWN) dy += 1;

    Point next(point.getX() + dx, point.getY() + dy, ' ');

    if (screen.isWall(next)) {
        flying = false;
        speed = 1;
        launchDirection = Direction::STAY;
        dir = Direction::STAY;
        return;
    }

    point.move(dx, dy);

    ScreenId currentScreenId = screen.getScreenId();
    int playerX = point.getX();
    int playerY = point.getY();

    for (auto obj : gameObjects) {
        if (obj->getScreenId() != currentScreenId)
            continue;

        if (playerX == obj->getX() && playerY == obj->getY() && !obj->isCollected()) {
            bool allowed = obj->handleCollision(*this, screen);
            if (!allowed) {
                point.move(-dx, -dy);
                flying = false;
                speed = 1;
                launchDirection = Direction::STAY;
                dir = Direction::STAY;
                return;
            }
        }
    }
}

void Player::launch()
{
	setFlying(true);
	dir = launchDirection;
    setLoaded(false);
}


bool Player::collectItem(GameObject* item)
{
    if (!hasItem())
    {
        heldItem = item;
        item->Collected();
		item->setPosition(hudX, hudY);
        return true;
	}
    return false;
}

GameObject* Player::dropItemToScreen(ScreenId currentScreenID)
{
    if (hasItem())
    {
        GameObject* itemToDrop = heldItem;

        // 1. החפץ חוזר למיקום השחקן
        itemToDrop->setPosition(point.getX(), point.getY());

        // 2. החפץ משויך לחדר הנוכחי (קריטי למעבר חדרים)
        itemToDrop->setScreenId(currentScreenID);

        // 3. החפץ מסומן כ"לא בתיק"
        itemToDrop->drop();

        // 4. ניתוק מהשחקן
        heldItem = nullptr;

        return itemToDrop;
    }
    return nullptr;
}

void Player::removeHeldItem()
{
    heldItem->removeFromGame();
	heldItem = nullptr;
}

int Player::getHeldKeyID() const
{
    if (heldItem != nullptr) {
        Key* k = dynamic_cast<Key*>(heldItem);
        if (k != nullptr)
        {
            return k->getKeyID();
        }
    }
    return -1;
} // לא מחזיק מפתח או מחזיק משהו אחר

bool Player::hasTorch() const
{
    if (heldItem == nullptr)
        return false;

    return dynamic_cast<Torch*>(heldItem) != nullptr;
}

bool Player::hasRiddle() const
{
    if (heldItem == nullptr)
        return false;

    return dynamic_cast<Riddle*>(heldItem) != nullptr;
}

Riddle* Player::getHeldRiddle() const
{
    if(hasRiddle())
		return dynamic_cast<Riddle*>(heldItem);
	return nullptr;
}

void Player::decreaseLife() {
    if (live > 0)
        live--;
}