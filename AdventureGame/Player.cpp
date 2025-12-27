#include "Player.h"
#include "Screen.h"
#include "GameObject.h"
#include "Key.h"
#include "Torch.h"
#include "Riddle.h"
#include "Bomb.h"
#include "Door.h"
#include "Game.h"
#include "Obstacle.h"
#include <vector>

/*
    Player.cpp
    Implements normal movement, flying (spring launch),
    item handling, collisions, and life system.
*/

Player::~Player()
{
	heldItem = nullptr; // Do not delete held item; managed by Game
}

void Player::resetForNewGame(int x, int y, char c, int hudPos, ScreenId startLevel)
{
    // Position & char
    point.setPos(x, y);   // לפי השינוי שעשית (setPos)
    point.setChar(c);

    // HUD location
    hudX = hudPos;
    hudY = 0;

    // Basic movement state
    dir = Direction::STAY;
    speed = 1;
    force = 1;

    // Life & score & level
	score = 0;
    live = 3;
    currentLevel = startLevel;

    // Spring / flying state
    flying = false;
    loaded = false;
    springTicksLeft = 0;
    lastLoadedSpringLength = 0;
    launchDirection = Direction::STAY;

    // Inventory
    heldItem = nullptr;
}



/* ----------------------------------------------------
                     MOVEMENT
   ---------------------------------------------------- */

void Player::calculateMovementDelta(int& dx, int& dy) const
{
    dx = 0; dy = 0;

    // לוגיקה לתעופה (Spring)
    if (isFlying())
    {
        // כיוון בסיסי לפי השיגור
        switch (launchDirection)
        {
        case Direction::UP:    dy = -1; break;
        case Direction::DOWN:  dy = 1;  break;
        case Direction::LEFT:  dx = -1; break;
        case Direction::RIGHT: dx = 1;  break;
        }

        // הטיה (Steering) תוך כדי תעופה
        if (dir == Direction::UP)    dy -= 1;
        else if (dir == Direction::DOWN) dy += 1;
    }
    // לוגיקה להליכה רגילה
    else
    {
        switch (dir)
        {
        case Direction::UP:    dy = -1; break;
        case Direction::DOWN:  dy = 1;  break;
        case Direction::LEFT:  dx = -1; break;
        case Direction::RIGHT: dx = 1;  break;
        }
    }
}

bool Player::isBlockedByStatic(int x, int y, const Screen& screen, const Player* otherPlayer) const
{
    // 1. בדיקת קירות
    if (screen.isWall(Point(x, y, ' '))) {
        return true;
    }

    // 2. בדיקת שחקן אחר (רק אם הוא קיים ונמצא ב-STAY)
    if (otherPlayer)
    {
        if (otherPlayer->getX() == x &&
            otherPlayer->getY() == y &&
            otherPlayer->getDirection() == Direction::STAY)
        {
            return true;
        }
    }

    return false;
}

bool Player::handleObjectInteractions(const Screen& screen, std::vector<GameObject*>& gameObjects, const Player* otherPlayer)
{
    ScreenId currentScreenId = screen.getScreenId();
    int px = point.getX();
    int py = point.getY();

    for (auto obj : gameObjects)
    {
        // סינונים מהירים
        if (!obj || obj->getScreenId() != currentScreenId || obj->isCollected())
            continue;

        // האם דרכנו על האובייקט?
        if (obj->isAtPosition(px, py))
        {
            bool allowed = true;

            // טיפול ספציפי למכשול (דורש את השחקן השני לדחיפה)
            if (auto obstacle = dynamic_cast<Obstacle*>(obj))
            {
                allowed = obstacle->handleCollision(*this, screen, otherPlayer, gameObjects);
            }
            else
            {
                allowed = obj->handleCollision(*this, screen);
            }

            // אם האובייקט חסם אותנו
            if (!allowed) return false;
        }
    }
    return true; // הכל עבר בשלום
}

void Player::handleStop()
{
    if (isFlying()) {
        stopSpringEffect();
    }
    else {
        stopMovement(); // מאפס כיוון ל-STAY
    }
}

void Player::move(Screen& screen, std::vector<GameObject*>& gameObjects, const Player* otherPlayer)
{
    // 1. בדיקות מקדימות
    if (point.getX() == -1 || isLoaded()) return;

    // 2. חישוב לאן רוצים לזוז
    int dx = 0, dy = 0;
    calculateMovementDelta(dx, dy);

    // אם אין תזוזה, אין מה להמשיך
    if (dx == 0 && dy == 0) return;

    Point nextPos(point.getX() + dx, point.getY() + dy, ' ');

    // 3. בדיקת חסימות סטטיות (קירות / שחקן עומד)
    // אנחנו בודקים *לפני* שמבצעים את התזוזה (Look-Ahead)
    if (isBlockedByStatic(nextPos.getX(), nextPos.getY(), screen, otherPlayer))
    {
        handleStop();
        return;
    }

    // 4. ביצוע התזוזה הפיזית
    point.move(dx, dy);

    // 5. אינטראקציה עם אובייקטים (Collision Response)
    // אם האינטראקציה נכשלה (למשל מכשול כבד מדי), אנחנו מבטלים את התזוזה
    if (!handleObjectInteractions(screen, gameObjects, otherPlayer))
    {
        // ביטול צעד (Undo)
        point.move(-dx, -dy);
        handleStop();
    }
}



/* ----------------------------------------------------
                    SPRING LAUNCH
   ---------------------------------------------------- */

void Player::launch(int springLength)
{
    // springLength = N, startSpringEffect computes N^2
    startSpringEffect(springLength);
}

void Player::startSpringEffect(int springLength)
{
    // Duration = N^2 game cycles
    springTicksLeft = springLength * springLength;

    flying = true;
    loaded = false;
	speed = springLength;
    force = springLength;
    dir = launchDirection; // lock initial launch direction
}

void Player::updateSpringEffect()
{
    if (!flying)
        return;

    if (springTicksLeft > 0)
        springTicksLeft--;

    if (springTicksLeft <= 0)
        stopSpringEffect();
}

void Player::stopSpringEffect()
{
    flying = false;
    springTicksLeft = 0;
    speed = 1;
	force = 1;

    launchDirection = Direction::STAY;
    dir = Direction::STAY;
}

/* ----------------------------------------------------
                    ITEM HANDLING
   ---------------------------------------------------- */

bool Player::collectItem(GameObject* item)
{
    if (!hasItem())
    {
        heldItem = item;
        item->Collected();

        // Attach to HUD position
        item->setPosition(hudX+13, hudY);

        Game::setStatusMessage(std::string("Collected item '") + item->getChar() + "'");
        return true;
    }
    return false;
}

GameObject* Player::dropItemToScreen(ScreenId currentScreenID)
{
    if (!hasItem())
        return nullptr;

    GameObject* itemToDrop = heldItem;

    itemToDrop->setPosition(point.getX(), point.getY());
    itemToDrop->setScreenId(currentScreenID);
    itemToDrop->drop();

    if (itemToDrop->getChar() == '@')
    {
        static_cast<Bomb*>(itemToDrop)->activate();
        Game::setStatusMessage("Bomb activated! RUN!"); 
    }

    else 
        Game::setStatusMessage(std::string("Dropped item '") + itemToDrop->getChar() + "'");

    heldItem = nullptr;
    return itemToDrop;
}

void Player::removeHeldItem()
{
    if (heldItem)
    {
        heldItem->removeFromGame();
        heldItem = nullptr;
    }
}

/* ----------------------------------------------------
                    INVENTORY QUERIES
   ---------------------------------------------------- */

int Player::getHeldKeyID() const
{
    if (heldItem)
    {
        if (auto k = dynamic_cast<Key*>(heldItem))
            return k->getKeyID();
    }
    return -1;
}

bool Player::hasTorch() const
{
    return (heldItem && dynamic_cast<Torch*>(heldItem) != nullptr);
}

bool Player::hasRiddle() const
{
    return (heldItem && dynamic_cast<Riddle*>(heldItem) != nullptr);
}

Riddle* Player::getHeldRiddle() const
{
    if (hasRiddle())
        return dynamic_cast<Riddle*>(heldItem);

    return nullptr;
}

char Player::getItemChar() const
{
    return heldItem ? heldItem->getChar() : ' ';
}

/* ----------------------------------------------------
                        LIFE SYSTEM
   ---------------------------------------------------- */

void Player::initForLevel(int x, int y, char c, ScreenId levelID)
{
    // 1. מיקום פיזי (Physical Position)
    point.setPos(x, y);
    point.setChar(c);
    dir = Direction::STAY;
    speed = 1;
    force = 1;
    flying = false;
    loaded = false;
    springTicksLeft = 0;
    lastLoadedSpringLength = 0;
    launchDirection = Direction::STAY;
    currentLevel = levelID;
}

void Player::resetStats()
{
    score = 0;
    live = 3;
    heldItem = nullptr; // במשחק חדש מתחילים בלי כלום
}




