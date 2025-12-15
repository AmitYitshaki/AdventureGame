#include "LevelLoader.h"
#include "Switch.h"
#include "Laser.h"
#include "Door.h"
#include "Key.h"
#include "Torch.h"
#include "spring.h"
#include "Direction.h"
// #include "Riddle.h"

#include <fstream>
#include <iostream>
#include <sstream>

// =========================================================
//            מימוש פונקציות העזר (Private)
// =========================================================

GameObject* LevelLoader::findObjectAt(int x, int y, const std::vector<GameObject*>& gameObjects)
{
    for (GameObject* obj : gameObjects) {
        if (obj->isAtPosition(x, y)) {
            return obj;
        }
    }
    return nullptr;
}

void LevelLoader::scanDirection(int startX, int startY, int dx, int dy, char type,
    const std::vector<GameObject*>& allObjects,
    std::vector<GameObject*>& outputBeam)
{
    int currX = startX + dx;
    int currY = startY + dy;

    while (true)
    {
        GameObject* obj = findObjectAt(currX, currY, allObjects);
        Laser* neighbor = dynamic_cast<Laser*>(obj);

        // אם מצאנו שכן שהוא לייזר מאותו סוג
        if (neighbor != nullptr && neighbor->getChar() == type)
        {
            outputBeam.push_back(neighbor);
            currX += dx;
            currY += dy;
        }
        else
        {
            break; // שרשרת נקטעה
        }
    }
}

std::vector<GameObject*> LevelLoader::collectLaserBeam(Laser* startNode, const std::vector<GameObject*>& allObjects)
{
    std::vector<GameObject*> beam;
    beam.push_back(startNode);

    char type = startNode->getChar();
    int x = startNode->getX();
    int y = startNode->getY();

    if (type == '-') {
        scanDirection(x, y, -1, 0, '-', allObjects, beam);
        scanDirection(x, y, 1, 0, '-', allObjects, beam);
    }
    else if (type == '|') {
        scanDirection(x, y, 0, -1, '|', allObjects, beam);
        scanDirection(x, y, 0, 1, '|', allObjects, beam);
    }

    return beam;
}

// בתוך LevelLoader.cpp -> parseConnectCommand

void LevelLoader::parseConnectCommand(const std::string& line, std::vector<GameObject*>& gameObjects)
{
    std::stringstream ss(line);
    std::string command;
    int switchX, switchY, targetX, targetY;

    ss >> command >> switchX >> switchY >> targetX >> targetY;

    GameObject* objSwitch = findObjectAt(switchX, switchY, gameObjects);
    GameObject* objTarget = findObjectAt(targetX, targetY, gameObjects);

    Switch* mySwitch = dynamic_cast<Switch*>(objSwitch);

    // הדפסה 2: האם מצאנו את המתג?
    if (mySwitch == nullptr) {
        return;
    }

    if (objTarget != nullptr)
    {
        Laser* laserTarget = dynamic_cast<Laser*>(objTarget);
        if (laserTarget != nullptr)
        {
            auto beam = collectLaserBeam(laserTarget, gameObjects);

            for (GameObject* part : beam) {
                mySwitch->addTarget(part);
            }
        }
        else
        {
            mySwitch->addTarget(objTarget);
        }
    }
    else {
    }
}
void LevelLoader::parseSpringData(const std::string& line, std::vector<GameObject*>& gameObjects)
{
    std::stringstream ss(line);
    std::string command;
    int dataX, dataY;
    char dirChar;
    int screenIdInt;

    // קריאת הנתונים מהשורה
    ss >> command >> dataX >> dataY >> dirChar >> screenIdInt;

    // 1. חיפוש האובייקט (שנוצר קודם כ-'W' גולמי)
    GameObject* obj = findObjectAt(dataX, dataY, gameObjects);

    if (obj == nullptr) {
        return;
    }

    // 2. המרה ל-Spring
    Spring* spring = dynamic_cast<Spring*>(obj);
    if (spring == nullptr) {
        return;
    }

    // 3. המרת התו לכיוון
    Direction dir = Direction::STAY;
    switch (dirChar)
    {
    case 'U': dir = Direction::UP; break;
    case 'D': dir = Direction::DOWN; break;
    case 'L': dir = Direction::LEFT; break;
    case 'R': dir = Direction::RIGHT; break;
    }

    // 4. עדכון המקפצה (כאן נקראת setDirection החדשה שפורסת את המקפצה)
    spring->setDirection(dir);

    // אופציונלי: הדפסת אישור שקטה ללוג
    // std::cout << "Spring loaded at (" << dataX << "," << dataY << ")" << std::endl;
}
// פונקציה לקריאת נתוני דלת: DOOR_DATA <x> <y> <id> <target_screen_id> <locked(0/1)>
void LevelLoader::parseDoorData(const std::string& line, std::vector<GameObject*>& gameObjects)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id, targetScreenInt, lockedInt;

    ss >> command >> x >> y >> id >> targetScreenInt >> lockedInt;


    // מציאת הדלת במיקום הזה
    GameObject* obj = findObjectAt(x, y, gameObjects);
    Door* door = dynamic_cast<Door*>(obj);

    if (door != nullptr)
    {
        // המרת int ל-ScreenId (בהנחה שזה Enum או int)
        ScreenId targetId = static_cast<ScreenId>(targetScreenInt);

        // עדכון הדלת
        door->setDoorProperties(id, targetId, (lockedInt == 1));
    }
}

void LevelLoader::parseKeyData(const std::string& line, std::vector<GameObject*>& gameObjects)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id;

    ss >> command >> x >> y >> id;

    // 1. מציאת האובייקט במיקום
    GameObject* obj = findObjectAt(x, y, gameObjects);

    if (obj == nullptr)
    {
        return;
    }

    // 2. המרה למפתח
    Key* key = dynamic_cast<Key*>(obj);

    // 3. עדכון ה-ID
    if (key != nullptr)
    {
        key->setKeyID(id);
    }
    else
    {}
}
void LevelLoader::createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects)
{
    switch (c)
    {
    case '/':
        gameObjects.push_back(new Switch(x, y, screenId));
        break;
    case '-':
    case '|':
        gameObjects.push_back(new Laser(x, y, c, screenId));
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': // דלתות ממוספרות
        // יוצרים דלת עם ערכי ברירת מחדל (יתעדכנו ב-metadata)
        gameObjects.push_back(new Door(x, y, c, screenId, -1, ScreenId::ROOM1, false));
        break;
    case 'K':
        // יוצרים מפתח עם ID זמני (-1). הנתונים האמיתיים יגיעו בסוף הקובץ.
        gameObjects.push_back(new Key(x, y, 'K', screenId, -1));
        break;
	case '!':
		 gameObjects.push_back(new Torch(x, y,'!' ,screenId));
		 break;
    case 'W':
        // יוצרים מקפצה "גולמית" - כל החלקים באותה משבצת, ללא כיוון מוגדר.
        // הפונקציה parseSpringData תעדכן אותה אחר כך ותפרוס אותה לכיוון הנכון.
        gameObjects.push_back(new Spring(
            Point(x, y, 'W'),   // Start
            Point(x, y, 'w'),   // Middle (זמני - באותו מקום)
            Point(x, y, 'w'),   // End (זמני - באותו מקום)
            Direction::STAY,    // Direction (זמני)
            screenId));
        break;
    default:
        break;
    }
}

// =========================================================
//            המ implementation הראשי (Public)
// =========================================================

void LevelLoader::loadLevelFromFile(const std::string& fileName, Screen& screen, std::vector<GameObject*>& gameObjects)
{
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error loading level: " << fileName << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    ScreenId currentScreenId = screen.getScreenId();
	if (std::getline(file, line)) {
		screen.setLine(0, line); // קריאת השורה הראשונה מבלי לחפש אובייקטים
		row++;
	}

    while (std::getline(file, line))
    {
        if (row < Screen::HEIGHT)
        {
            screen.setLine(row, line);
            for (size_t col = 0; col < line.size(); ++col)
            {
                char c = line[col];

                if (c != ' ' && c != '#') {
                    if (c == '$') { screen.setStartPos1((int)col, row); screen.setChar((int)col, row, ' ');}
                    else if (c == '&') { screen.setStartPos2((int)col, row); screen.setChar((int)col, row, ' ');}
                    else { createObject(c, (int)col, row, currentScreenId, gameObjects); screen.setChar((int)col, row, ' ');}
                }
            }
        }
        else // Metadata Area
        {
            if (line.find("CONNECT") == 0)
            {
                parseConnectCommand(line, gameObjects);
            }
            else if (line.find("DOOR_DATA") == 0) { // זיהוי הפקודה החדשה
                parseDoorData(line, gameObjects);
            }
            else if (line.find("KEY_DATA") == 0) {
                parseKeyData(line, gameObjects);
            }
            else if (line.find("DARK") != std::string::npos)
            {
                screen.setDark(true);
            }
            else if (line.find("SPRING_DATA") == 0) {
                parseSpringData(line, gameObjects);
            }
        }
        row++;
    }
    file.close();
}

// פונקציה לטעינת מסך "טיפש" (רק גרפיקה, בלי לוגיקה)
void LevelLoader::loadScreenFromFile(const std::string& fileName, Screen& screen)
{
    std::ifstream file(fileName);
    if (!file) {
        // אם לא מצאנו את הקובץ, אפשר להדפיס שגיאה או פשוט להתעלם
        return;
    }

    std::string line;
    int row = 0;

    // קוראים שורה-שורה ומדביקים למסך
    while (std::getline(file, line) && row <= Screen::HEIGHT)
    {
        screen.setLine(row, line);
        row++;
    }

    file.close();
}
