#pragma once
#include <string>
#include <vector>
#include "Screen.h"
#include "GameObject.h"

// Forward Declaration - כדי שהדר לא יצטרך לייבא את כל Laser.h
class Laser;

class LevelLoader
{
public:
    // הפונקציה היחידה שחשופה החוצה
    static void loadLevelFromFile(const std::string& fileName, Screen& screen, std::vector<GameObject*>& gameObjects);
    static void loadScreenFromFile(const std::string& fileName, Screen& screen);

private:
    // --- פונקציות עזר פנימיות (לוגיקה) ---

    static void parseDoorData(const std::string& line, std::vector<GameObject*>& gameObjects);

    static  void parseKeyData(const std::string& line, std::vector<GameObject*>& gameObjects);

    static void parseSpringData(const std::string& line, std::vector<GameObject*>& gameObjects);

    // יצירת אובייקט בודד (לפי תו)
    static void createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects);

    // מציאת אובייקט בזיכרון לפי מיקום
    static GameObject* findObjectAt(int x, int y, const std::vector<GameObject*>& gameObjects);

    // לוגיקה לחיבור מתגים (CONNECT)
    static void parseConnectCommand(const std::string& line, std::vector<GameObject*>& gameObjects);

    // --- אלגוריתם שרשור הלייזרים ---

    // איסוף כל הקרן
    static std::vector<GameObject*> collectLaserBeam(Laser* startNode, const std::vector<GameObject*>& allObjects);

    // סריקה בכיוון מסוים
    static void scanDirection(int startX, int startY, int dx, int dy, char type,
        const std::vector<GameObject*>& allObjects,
        std::vector<GameObject*>& outputBeam);
};