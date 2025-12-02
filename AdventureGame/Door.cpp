#include "Door.h"
#include "Player.h"
#include "Key.h"

Door::Door(int x, int y, char c, ScreenId screen, int id, ScreenId leadsToScreen, bool isLocked)
	: GameObject(x, y, c, screen, true), DoorID(id), leadsTo(leadsToScreen), locked(isLocked) {
}

bool Door::unlock(Player& player) {
	if (!locked) {
		return true; // Door is already unlocked
	}
	if (player.getHeldKeyID() == DoorID) { 
		locked = false; // Unlock the door
		player.removeHeldItem(); // Remove the key from the player

		return true; // Successfully unlocked
	}
	return false; // Failed to unlock
}
bool Door::handleCollision(Player& p, const Screen& screen, std::vector<GameObject*>& objects)
{
    if (!isLocked())
    {
        // הדלת פתוחה - מתירים מעבר
        p.setCurrentLevel(leadsTo);
		p.setDirection(Direction::STAY); // עצירת התנועה לאחר המעבר
        return true; // מאשרים לשחקן להישאר במשבצת
	}
    // לוגיקה ייחודית לדלת:
    // מנסים לפתוח (הפונקציה unlock כבר בודקת מפתחות ומטפלת בהכל)
    if (unlock(p))
    {
        // הצלחנו להיכנס!
        // הדלת מעדכנת את השחקן שהוא עבר מסך
        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY); // עצירת התנועה לאחר המעבר
        return true; // מאשרים לשחקן להישאר במשבצת
    }

    // נכשלנו (נעול בלי מפתח) -> חוסמים
    return false;
}