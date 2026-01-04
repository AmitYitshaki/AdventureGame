#include "Riddle.h"
#include "Player.h"

bool Riddle::handleCollision(Player& p, const Screen&)
{
    if (solved) return true;
    p.setInteractingRiddle(this);
    return true;
}

std::string Riddle::getSaveData() const {
    // אם לחידה יש דאטה, נשמור את ה-ID שלה. אם לא, נשמור -1.
    int idToSave = -1;
    if (!data.textLines.empty()) { // בדיקה גסה אם יש דאטה
        idToSave = data.id;
    }
    // Format: X Y RiddleID
    return GameObject::getSaveData() + " " + std::to_string(idToSave);
}