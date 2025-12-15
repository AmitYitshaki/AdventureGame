#include "Door.h"
#include "Player.h"
#include "Key.h"
#include <iostream>
#include <string>

// שים לב: וודא שיש לך כאן את ה-include לפונקציה gotoxy
// בדרך כלל זה אחד מאלה:
// #include "io_utils.h" 
// #include "Utils.h"
// או אם זה מוגדר ב-Screen:
#include "Screen.h" 

// פונקציית עזר קטנה לדיבאג נקי (אם אין לך gotoxy גלובלי, תצטרך להוסיף את ה-include המתאים)
void printDebugBelow(const std::string& msg)
{
    // נניח שהגובה הוא 24, נדפיס בשורה 26 כדי לא להפריע
    // אם gotoxy לא מזוהה, תוסיף את ה-include המתאים מהפרויקט שלך
    gotoxy(0, 26);

    // מדפיסים את ההודעה + הרבה רווחים כדי למחוק טקסט קודם שהיה שם
    std::cout << "DOOR DEBUG: " << msg << "                                         " << std::endl;
}

Door::Door(int x, int y, char c, ScreenId screen,
    int id, ScreenId leadsToScreen, bool isLocked)
    : GameObject(x, y, c, screen, true),
    doorID(id),
    leadsTo(leadsToScreen),
    locked(isLocked)
{
}

void Door::setDoorProperties(int id, ScreenId targetScreen, bool isLocked)
{
    this->doorID = id;
    this->leadsTo = targetScreen;
    this->locked = isLocked;
}

bool Door::unlock(Player& player)
{
    if (!locked) return true;

    if (player.getHeldKeyID() == doorID)
    {
        printDebugBelow("Key MATCH! Unlocking door.");
        locked = false;
        player.removeHeldItem();
        return true;
    }

    printDebugBelow("Locked! Player has KeyID: " + std::to_string(player.getHeldKeyID()) + " | Door needs: " + std::to_string(doorID));
    return false;
}

bool Door::handleCollision(Player& p, const Screen& screen)
{
    // מקרה 1: הדלת פתוחה
    if (!locked)
    {
        printDebugBelow("OPEN! Moving to Level: " + std::to_string((int)leadsTo));

        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    // מקרה 2: הדלת נעולה - מנסים לפתוח
    if (unlock(p))
    {
        printDebugBelow("Unlocked & Moving to Level: " + std::to_string((int)leadsTo));

        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    return false; // חוסם מעבר
}