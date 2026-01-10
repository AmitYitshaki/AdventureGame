#include "Riddle.h"
#include "Player.h"

// === Collision Handling ===
bool Riddle::handleCollision(Player& p, const Screen&)
{
    if (solved) {
        return true;
    }
    p.setInteractingRiddle(this);
    return true;
}

// === Serialization ===
std::string Riddle::getSaveData() const
{
    // Save the riddle ID if we have data; otherwise, store -1.
    int idToSave = -1;
    if (!data.textLines.empty()) {
        idToSave = data.id;
    }
    // Format: X Y RiddleID
    return GameObject::getSaveData() + " " + std::to_string(idToSave);
}
