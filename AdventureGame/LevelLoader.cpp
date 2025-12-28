#include "LevelLoader.h"
#include "Switch.h"
#include "Laser.h"
#include "Door.h"
#include "Key.h"
#include "Torch.h"
#include "Riddle.h"
#include "spring.h"
#include "Bomb.h"
#include "Obstacle.h" 
#include "Direction.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <utility>

// =========================================================
//             Helper Functions (Private)
// =========================================================

// הפונקציה המתוקנת שבודקת גם מיקום וגם ID של המסך
GameObject* LevelLoader::findObjectAt(int x, int y, ScreenId screenId, const std::vector<GameObject*>& gameObjects)
{
    for (GameObject* obj : gameObjects) {
        if (obj->isAtPosition(x, y) && obj->getScreenId() == screenId) return obj;
    }
    return nullptr;
}

// --- פונקציות הלייזר החסרות (הוספתי אותן כאן) ---

void LevelLoader::scanDirection(int startX, int startY, int dx, int dy, char type, ScreenId screenId,
    const std::vector<GameObject*>& allObjects,
    std::vector<GameObject*>& outputBeam)
{
    int currX = startX + dx;
    int currY = startY + dy;

    while (true)
    {
        // מעבירים את ה-screenId לחיפוש
        GameObject* obj = findObjectAt(currX, currY, screenId, allObjects);

        // אם אין אובייקט, עוצרים
        if (!obj) break;

        Laser* laser = dynamic_cast<Laser*>(obj);

        // אם זה לייזר מאותו סוג, מוסיפים וממשיכים
        if (laser && laser->getChar() == type)
        {
            outputBeam.push_back(laser);
            currX += dx;
            currY += dy;
        }
        else
        {
            break; // נתקלנו במשהו שאינו לייזר המשך
        }
    }
}

std::vector<GameObject*> LevelLoader::collectLaserBeam(Laser* startNode, const std::vector<GameObject*>& allObjects)
{
    std::vector<GameObject*> beam;
    if (!startNode) return beam;

    beam.push_back(startNode);

    int startX = startNode->getX();
    int startY = startNode->getY();
    char type = startNode->getChar();

    // שולפים את ה-ID מהלייזר הראשון כדי להעביר לסריקה
    ScreenId sid = startNode->getScreenId();

    if (type == '-')
    {
        scanDirection(startX, startY, -1, 0, '-', sid, allObjects, beam); // שמאלה
        scanDirection(startX, startY, 1, 0, '-', sid, allObjects, beam);  // ימינה
    }
    else if (type == '|')
    {
        scanDirection(startX, startY, 0, -1, '|', sid, allObjects, beam); // למעלה
        scanDirection(startX, startY, 0, 1, '|', sid, allObjects, beam);  // למטה
    }

    return beam;
}

// ---------------------------------------------------------

void LevelLoader::createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects)
{
    switch (c)
    {
    case '/': gameObjects.push_back(new Switch(x, y, screenId)); break;
    case '-': case '|': gameObjects.push_back(new Laser(x, y, c, screenId)); break;
    case 'D': // טיפול כללי בדלתות (אם יש D בקובץ)
        // ברירת מחדל, תעודכן אח"כ ב-parseDoorData
        gameObjects.push_back(new Door(x, y, c, screenId, -1, ScreenId::ROOM1, false));
        break;
        // טיפול במספרים שמייצגים דלתות בקובץ ASCII
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        gameObjects.push_back(new Door(x, y, c, screenId, -1, ScreenId::ROOM1, false)); break;

    case 'K': gameObjects.push_back(new Key(x, y, 'K', screenId, -1)); break;
    case '!': gameObjects.push_back(new Torch(x, y, '!', screenId)); break;
    case '?': gameObjects.push_back(new Riddle(x, y, screenId)); break;
    case '@': gameObjects.push_back(new Bomb(x, y, screenId)); break;

    case 'W':
        // יוצרים קפיץ זמני (ברירת מחדל אורך 3). 
        // האורך האמיתי והכיוון יעודכנו בפונקציה parseSpringData
        gameObjects.push_back(new Spring(Point(x, y, 'W'), Direction::STAY, screenId, 3));
        break;

    default: break;
    }
}

void LevelLoader::parseConnectCommand(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int switchX, switchY, targetX, targetY;

    ss >> command >> switchX >> switchY >> targetX >> targetY;

    // חיפוש המתג והמטרה רק במסך הרלוונטי
    GameObject* objSwitch = findObjectAt(switchX, switchY, screenId, gameObjects);
    GameObject* objTarget = findObjectAt(targetX, targetY, screenId, gameObjects);

    Switch* mySwitch = dynamic_cast<Switch*>(objSwitch);

    if (mySwitch == nullptr) {
        return;
    }

    if (objTarget != nullptr)
    {
        Laser* laserTarget = dynamic_cast<Laser*>(objTarget);

        // --- מקרה 1: המטרה היא לייזר ---
        if (laserTarget != nullptr)
        {
            auto beam = collectLaserBeam(laserTarget, gameObjects);

            for (GameObject* part : beam) {
                mySwitch->addTarget(part);
            }

            // === הדפסה ללייזר ===
            std::cout << "[DEBUG] CONNECT SUCCESS: Switch(" << switchX << "," << switchY
                << ") -> Laser Beam (Size: " << beam.size() << ")" << std::endl;
        }
        // --- מקרה 2: המטרה היא אובייקט רגיל ---
        else
        {
            mySwitch->addTarget(objTarget);

            // === הדפסה לאובייקט רגיל ===
            std::cout << "[DEBUG] CONNECT SUCCESS: Switch(" << switchX << "," << switchY
                << ") -> Object(" << targetX << "," << targetY << ")" << std::endl;
        }
    }
    else {
        // אופציונלי: הדפסה אם המטרה לא נמצאה
        std::cerr << "[ERROR] CONNECT FAILED: Target not found at (" << targetX << "," << targetY << ")" << std::endl;
    }
}

void LevelLoader::parseSpringData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int dataX, dataY;
    char dirChar;
    int screenIdInt; // (אנחנו קוראים אותו מהקובץ אבל משתמשים ב-screenId שקיבלנו מהפונקציה לאמינות)
    int length = 3;

    ss >> command >> dataX >> dataY >> dirChar >> screenIdInt >> length;

    GameObject* obj = findObjectAt(dataX, dataY, screenId, gameObjects);

    if (!obj) return;
    Spring* spring = dynamic_cast<Spring*>(obj);
    if (!spring) return;

    Direction dir = Direction::STAY;
    switch (dirChar) {
    case 'U': dir = Direction::UP; break;
    case 'D': dir = Direction::DOWN; break;
    case 'L': dir = Direction::LEFT; break;
    case 'R': dir = Direction::RIGHT; break;
    }

    spring->rebuild(dir, length);
}

void LevelLoader::parseDoorData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id, targetScreenInt, lockedInt;

    ss >> command >> x >> y >> id >> targetScreenInt >> lockedInt;

    GameObject* obj = findObjectAt(x, y, screenId, gameObjects);
    Door* door = dynamic_cast<Door*>(obj);

    if (door != nullptr)
    {
        ScreenId targetId = static_cast<ScreenId>(targetScreenInt);
        door->setDoorProperties(id, targetId, (lockedInt == 1));
    }
}

void LevelLoader::parseKeyData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id;

    ss >> command >> x >> y >> id;

    GameObject* obj = findObjectAt(x, y, screenId, gameObjects);
    Key* key = dynamic_cast<Key*>(obj);

    if (key != nullptr)
    {
        key->setKeyID(id);
    }
}


// =========================================================
//            Main Implementation (Public)
// =========================================================

void LevelLoader::loadLevelFromFile(const std::string& fileName, Screen& screen, std::vector<GameObject*>& gameObjects)
{
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error loading level: " << fileName << std::endl;
        return;
    }

    std::vector<std::string> rawLines;
    std::string line;

    // Read entire file first
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); // ניקוי ווינדוס
        if (line.size() < Screen::WIDTH) line.resize(Screen::WIDTH, ' ');
        rawLines.push_back(line);
    }
    file.close();

    screen.setDark(false); // Default

    std::vector<std::string> mapLayout;
    std::vector<std::string> metadata;

    // Split into Map Layout vs Metadata
    for (size_t i = 0; i < rawLines.size(); ++i) {
        if (i < Screen::HEIGHT) mapLayout.push_back(rawLines[i]);
        else metadata.push_back(rawLines[i]);
    }

    // Ensure map is full height
    while (mapLayout.size() < Screen::HEIGHT) {
        mapLayout.push_back(std::string(Screen::WIDTH, ' '));
    }

    ScreenId currentScreenId = screen.getScreenId();

    // ------------------------------------------------------------------
    // PASS 1: Static objects
    // ------------------------------------------------------------------
    for (size_t row = 0; row < mapLayout.size(); ++row)
    {
        for (size_t col = 0; col < mapLayout[row].size(); ++col)
        {
            char c = mapLayout[row][col];

            if (c == ' ') continue;

            if (c == '$') {
                screen.setStartPos1((int)col, (int)row);
                mapLayout[row][col] = ' ';
            }
            else if (c == '&') {
                screen.setStartPos2((int)col, (int)row);
                mapLayout[row][col] = ' ';
            }
            else if (c == 'L') {
                screen.setLegendStart((int)col, (int)row);
                std::cout << "DEBUG: Found L at " << col << "," << row << std::endl;
                mapLayout[row][col] = ' ';
            }
            else if (c == '*' || c == '#') {
                continue;
            }
            else {
                createObject(c, (int)col, (int)row, currentScreenId, gameObjects);
                mapLayout[row][col] = ' ';
            }
        }
    }

    // ------------------------------------------------------------------
    // PASS 2: Connected Components (Obstacles)
    // ------------------------------------------------------------------
    std::vector<std::vector<bool>> visited(Screen::HEIGHT, std::vector<bool>(Screen::WIDTH, false));
    auto inBounds = [](int x, int y) {
        return x >= 0 && x < Screen::WIDTH && y >= 0 && y < Screen::HEIGHT;
        };

    for (int y = 0; y < Screen::HEIGHT && y < (int)mapLayout.size(); ++y)
    {
        for (int x = 0; x < Screen::WIDTH && x < (int)mapLayout[y].size(); ++x)
        {
            if (mapLayout[y][x] != '*' || visited[y][x])
                continue;

            std::queue<std::pair<int, int>> q;
            std::vector<Point> cluster;

            q.push({ x, y });
            visited[y][x] = true;

            while (!q.empty())
            {
                auto cell = q.front();
                int cx = cell.first;
                int cy = cell.second;
                q.pop();

                cluster.emplace_back(cx, cy, '*');
                mapLayout[cy][cx] = ' ';

                const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
                for (auto& d : dirs)
                {
                    int nx = cx + d[0];
                    int ny = cy + d[1];

                    if (inBounds(nx, ny) && !visited[ny][nx] && mapLayout[ny][nx] == '*')
                    {
                        visited[ny][nx] = true;
                        q.push({ nx, ny });
                    }
                }
            }

            if (!cluster.empty()) {
                gameObjects.push_back(new Obstacle(cluster, currentScreenId));
            }
        }
    }

    // ------------------------------------------------------------------
    // FINALIZATION
    // ------------------------------------------------------------------

    screen.setLayout(mapLayout);

    // Pass 3: Metadata parsing
    // כאן התיקון הגדול - מעבירים את ה-currentScreenId לכל פונקציית Parse
    for (const auto& meta : metadata)
    {
        if (meta.find("CONNECT") != std::string::npos)      parseConnectCommand(meta, gameObjects, currentScreenId);
        else if (meta.find("DOOR_DATA") != std::string::npos) parseDoorData(meta, gameObjects, currentScreenId);
        else if (meta.find("KEY_DATA") != std::string::npos)  parseKeyData(meta, gameObjects, currentScreenId);
        else if (meta.find("DARK") != std::string::npos) screen.setDark(true);
        else if (meta.find("SPRING_DATA") != std::string::npos) parseSpringData(meta, gameObjects, currentScreenId);
    }
}

void LevelLoader::loadScreenFromFile(const std::string& fileName, Screen& screen)
{
    std::ifstream file(fileName);
    if (!file) return;
    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < Screen::HEIGHT) {
        screen.setLine(row, line);
        row++;
    }
    file.close();
}