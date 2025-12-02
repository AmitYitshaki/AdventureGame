#include "Player.h"
#include "Screen.h"
#include <vector>
#include "GameObject.h"
#include "Key.h"
#include "Door.h"

using namespace std;

Player::~Player() {
    if (heldItem != nullptr) {
        delete heldItem;
    }
}

void Player::move(Screen& screen, vector<GameObject*>& gameObjects) {
    int dx = 0, dy = 0;

    switch (dir) {
    case Direction::UP: dy = -1; break;
    case Direction::DOWN: dy = 1; break;
    case Direction::LEFT: dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    case Direction::STAY: return;
    }

    Point next(point.getX() + dx * speed, point.getY() + dy * speed, ' ');

    if (screen.isWall(next)) {
        stopMovement();
        return;
    }
    point.move(dx * speed, dy * speed);

    ScreenId currentScreenId = screen.getScreenId();
    int playerX = point.getX();
    int playerY = point.getY();

    for (auto obj : gameObjects) {
        int objX = obj->getX();
        int objY = obj->getY();
        if (playerX == objX && playerY == objY &&
            obj->getScreenId() == currentScreenId &&
            !obj->getIsCollected())
        {
			bool allowed = obj->handleCollision(*this, screen, gameObjects);
            if (!allowed) {
				point.move(-dx * speed, -dy * speed);
				dir = Direction::STAY;
                return;
            }
        }
    }
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
