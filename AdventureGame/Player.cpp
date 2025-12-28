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

Player::~Player()
{
    heldItem = nullptr;
}

void Player::resetForNewGame(int x, int y, char c, int hudPos, ScreenId startLevel)
{
    point.setPos(x, y);
    point.setChar(c);
    hudX = hudPos;
    hudY = 0;
    dir = Direction::STAY;
    speed = 1;
    force = 1;
    score = 0;
    live = 3;
    currentLevel = startLevel;
    flying = false;
    loaded = false;
    springTicksLeft = 0;
    lastLoadedSpringLength = 0;
    launchDirection = Direction::STAY;
    heldItem = nullptr;
}

void Player::stopMovement()
{
    dir = Direction::STAY;
}

void Player::calculateMovementDelta(int& dx, int& dy) const
{
    dx = 0; dy = 0;

    if (isFlying())
    {
        switch (launchDirection)
        {
        case Direction::UP:    dy = -1; break;
        case Direction::DOWN:  dy = 1;  break;
        case Direction::LEFT:  dx = -1; break;
        case Direction::RIGHT: dx = 1;  break;
        default: break;
        }

        // Steering while flying
        if (dir == Direction::UP)    dy -= 1;
        else if (dir == Direction::DOWN) dy += 1;
    }
    else
    {
        switch (dir)
        {
        case Direction::UP:    dy = -1; break;
        case Direction::DOWN:  dy = 1;  break;
        case Direction::LEFT:  dx = -1; break;
        case Direction::RIGHT: dx = 1;  break;
        default: break;
        }
    }
}

bool Player::isBlockedByStatic(int x, int y, const Screen& screen, Player* otherPlayer) const
{
    if (screen.isWall(Point(x, y, ' '))) {
        return true;
    }

    if (otherPlayer)
    {
        if (otherPlayer->getX() == x && otherPlayer->getY() == y)
        {
            return true;
        }
    }
    return false;
}

bool Player::handleObjectInteractions(const Screen& screen, std::vector<GameObject*>& gameObjects, Player* otherPlayer)
{
    ScreenId currentScreenId = screen.getScreenId();
    int px = point.getX();
    int py = point.getY();

    for (auto obj : gameObjects)
    {
        if (!obj || obj->getScreenId() != currentScreenId || obj->isCollected())
            continue;

        if (obj->isAtPosition(px, py))
        {
            bool allowed = true;

            if (auto obstacle = dynamic_cast<Obstacle*>(obj))
            {
                allowed = obstacle->handleCollision(*this, screen, otherPlayer, gameObjects);
            }
            else
            {
                allowed = obj->handleCollision(*this, screen);
            }

            if (!allowed) return false;
        }
    }
    return true;
}

void Player::handleStop()
{
    if (isFlying()) {
        stopSpringEffect();
    }
    else {
        stopMovement();
    }
}

void Player::move(Screen& screen, std::vector<GameObject*>& gameObjects, Player* otherPlayer)
{
    if (point.getX() == -1 || isLoaded()) return;

    int dx = 0, dy = 0;
    calculateMovementDelta(dx, dy);

    if (dx == 0 && dy == 0) return;

    Point nextPos(point.getX() + dx, point.getY() + dy, ' ');

    if (isBlockedByStatic(nextPos.getX(), nextPos.getY(), screen, otherPlayer))
    {
        if (otherPlayer && otherPlayer->getX() == nextPos.getX() && otherPlayer->getY() == nextPos.getY())
        {
            if (isFlying())
            {
                // Momentum transfer logic
                otherPlayer->setLaunchDirection(this->dir);
                otherPlayer->startSpringEffect(this->speed);
                this->stopSpringEffect();
                Game::setStatusMessage("CRASH! Momentum transferred!");
            }
            else
            {
                handleStop();
            }
        }
        else
        {
            handleStop();
        }
        return;
    }

    point.move(dx, dy);

    if (!handleObjectInteractions(screen, gameObjects, otherPlayer))
    {
        point.move(-dx, -dy); // Undo move

        if (isFlying()) {
            stopSpringEffect();
        }
    }
}

void Player::launch(int springLength)
{
    startSpringEffect(springLength);
}

void Player::startSpringEffect(int springLength)
{
    springTicksLeft = springLength * springLength;
    flying = true;
    loaded = false;
    speed = springLength;
    force = springLength;
    dir = launchDirection;
}

void Player::updateSpringEffect()
{
    if (!flying) return;

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

bool Player::collectItem(GameObject* item)
{
    if (!hasItem())
    {
        heldItem = item;
        item->Collected();
        item->setPosition(hudX + 13, hudY);
        Game::setStatusMessage(std::string("Collected item '") + item->getChar() + "'");
        return true;
    }
    return false;
}

GameObject* Player::dropItemToScreen(ScreenId currentScreenID)
{
    if (!hasItem()) return nullptr;

    GameObject* itemToDrop = heldItem;
    itemToDrop->setPosition(point.getX(), point.getY());
    itemToDrop->setScreenId(currentScreenID);
    itemToDrop->drop();

    if (itemToDrop->getChar() == '@')
    {
        static_cast<Bomb*>(itemToDrop)->activate();
        Game::setStatusMessage("Bomb activated! RUN!");
    }
    else {
        Game::setStatusMessage(std::string("Dropped item '") + itemToDrop->getChar() + "'");
    }

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

void Player::initForLevel(int x, int y, char c, ScreenId levelID)
{
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

void Player::resetStats() {
    score = 0;
    live = 3;
    heldItem = nullptr;
    savedLife = 3;
    savedScore = 0;
}

void Player::restoreState() {
    live = savedLife;
    score = savedScore;
    heldItem = nullptr;
    flying = false;
    loaded = false;
}