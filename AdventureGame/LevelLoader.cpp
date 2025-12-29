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
#include "GameException.h" // Added for error handling

#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <stdexcept>

GameObject* LevelLoader::findObjectAt(int x, int y, ScreenId screenId, const std::vector<GameObject*>& gameObjects)
{
    for (const auto& obj : gameObjects) {
        if (obj->isAtPosition(x, y) && obj->getScreenId() == screenId) return obj;
    }
    return nullptr;
}

void LevelLoader::scanDirection(int startX, int startY, int dx, int dy, char type, ScreenId screenId,
    const std::vector<GameObject*>& allObjects,
    std::vector<GameObject*>& outputBeam)
{
    int currX = startX + dx;
    int currY = startY + dy;

    while (true)
    {
        // Safe check: Ensure we don't scan outside bounds (infinite loops safety)
        if (currX < 0 || currX >= Screen::WIDTH || currY < 0 || currY >= Screen::HEIGHT) break;

        GameObject* obj = findObjectAt(currX, currY, screenId, allObjects);
        if (!obj) break;

        Laser* laser = dynamic_cast<Laser*>(obj);
        if (laser && laser->getChar() == type)
        {
            outputBeam.push_back(laser);
            currX += dx;
            currY += dy;
        }
        else
        {
            break;
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
    ScreenId sid = startNode->getScreenId();

    if (type == '-')
    {
        scanDirection(startX, startY, -1, 0, '-', sid, allObjects, beam);
        scanDirection(startX, startY, 1, 0, '-', sid, allObjects, beam);
    }
    else if (type == '|')
    {
        scanDirection(startX, startY, 0, -1, '|', sid, allObjects, beam);
        scanDirection(startX, startY, 0, 1, '|', sid, allObjects, beam);
    }

    return beam;
}

// --- Parsing ---

void LevelLoader::parseConnectCommand(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int switchX, switchY, targetX, targetY;

    ss >> command >> switchX >> switchY >> targetX >> targetY;

    // Validation: Check if the line format is correct
    GameException::ensureStreamGood(ss, "Corrupt CONNECT command format: " + line, "LevelLoader::parseConnectCommand");

    // Validation: Check bounds
    GameException::ensureInBounds(switchX, switchY, "LevelLoader::parseConnectCommand (Switch Pos)");
    GameException::ensureInBounds(targetX, targetY, "LevelLoader::parseConnectCommand (Target Pos)");

    GameObject* objSwitch = findObjectAt(switchX, switchY, screenId, gameObjects);
    GameObject* objTarget = findObjectAt(targetX, targetY, screenId, gameObjects);

    // Validation: Ensure objects actually exist
    GameException::ensureNotNull(objSwitch, "Switch not found at specified coordinates", "LevelLoader::parseConnectCommand");
    GameException::ensureNotNull(objTarget, "Target object not found at specified coordinates", "LevelLoader::parseConnectCommand");

    Switch* mySwitch = dynamic_cast<Switch*>(objSwitch);
    // Validation: Ensure the object is indeed a Switch
    GameException::ensureNotNull(mySwitch, "Object at coordinates is not a Switch", "LevelLoader::parseConnectCommand");

    // Logic: Connect switch to target (or laser beam)
    if (mySwitch && objTarget)
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
}

void LevelLoader::parseSpringData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int dataX, dataY;
    char dirChar;
    int screenIdInt;
    int length = 3;

    ss >> command >> dataX >> dataY >> dirChar >> screenIdInt;

    // Validation: Stream integrity
    GameException::ensureStreamGood(ss, "Corrupt SPRING_DATA format: " + line, "LevelLoader::parseSpringData");
    // Validation: Bounds
    GameException::ensureInBounds(dataX, dataY, "LevelLoader::parseSpringData");

    // Optional length parsing
    if (ss >> length) { /* Length parsed successfully */ }
    else { ss.clear(); length = 3; } // Default length if missing

    GameObject* obj = findObjectAt(dataX, dataY, screenId, gameObjects);

    // Validation: Object existence
    GameException::ensureNotNull(obj, "Spring object not found at (" + std::to_string(dataX) + "," + std::to_string(dataY) + ")", "LevelLoader::parseSpringData");

    Spring* spring = dynamic_cast<Spring*>(obj);

    // Validation: Type safety
    GameException::ensureNotNull(spring, "Object at coordinates is not a Spring", "LevelLoader::parseSpringData");

    Direction dir = Direction::STAY;
    switch (dirChar) {
    case 'U': dir = Direction::UP; break;
    case 'D': dir = Direction::DOWN; break;
    case 'L': dir = Direction::LEFT; break;
    case 'R': dir = Direction::RIGHT; break;
    default:
        // Validation: Invalid Direction
        throw GameException("Invalid direction char '" + std::string(1, dirChar) + "' in line: " + line, "LevelLoader::parseSpringData");
    }

    spring->rebuild(dir, length);
}

void LevelLoader::parseDoorData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id, targetScreenInt, lockedInt;

    ss >> command >> x >> y >> id >> targetScreenInt >> lockedInt;

    // Validation
    GameException::ensureStreamGood(ss, "Corrupt DOOR_DATA format: " + line, "LevelLoader::parseDoorData");
    GameException::ensureInBounds(x, y, "LevelLoader::parseDoorData");

    GameObject* obj = findObjectAt(x, y, screenId, gameObjects);

    // Validation
    GameException::ensureNotNull(obj, "Door object not found at (" + std::to_string(x) + "," + std::to_string(y) + ")", "LevelLoader::parseDoorData");

    Door* door = dynamic_cast<Door*>(obj);

    // Validation
    GameException::ensureNotNull(door, "Object is not a Door", "LevelLoader::parseDoorData");

    ScreenId targetId = static_cast<ScreenId>(targetScreenInt);
    door->setDoorProperties(id, targetId, (lockedInt == 1));
}

void LevelLoader::parseKeyData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId)
{
    std::stringstream ss(line);
    std::string command;
    int x, y, id;

    ss >> command >> x >> y >> id;

    // Validation
    GameException::ensureStreamGood(ss, "Corrupt KEY_DATA format: " + line, "LevelLoader::parseKeyData");
    GameException::ensureInBounds(x, y, "LevelLoader::parseKeyData");

    GameObject* obj = findObjectAt(x, y, screenId, gameObjects);

    // Validation
    GameException::ensureNotNull(obj, "Key object not found at (" + std::to_string(x) + "," + std::to_string(y) + ")", "LevelLoader::parseKeyData");

    Key* key = dynamic_cast<Key*>(obj);

    // Validation
    GameException::ensureNotNull(key, "Object is not a Key", "LevelLoader::parseKeyData");

    key->setKeyID(id);
}

void LevelLoader::createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects)
{
    switch (c)
    {
    case '/': gameObjects.push_back(new Switch(x, y, screenId)); break;
    case '-': case '|': gameObjects.push_back(new Laser(x, y, c, screenId)); break;
    case 'D':
        gameObjects.push_back(new Door(x, y, c, screenId, -1, ScreenId::ROOM1, false));
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        gameObjects.push_back(new Door(x, y, c, screenId, -1, ScreenId::ROOM1, false)); break;

    case 'K': gameObjects.push_back(new Key(x, y, 'K', screenId, -1)); break;
    case '!': gameObjects.push_back(new Torch(x, y, '!', screenId)); break;
    case '?': gameObjects.push_back(new Riddle(x, y, screenId)); break;
    case '@': gameObjects.push_back(new Bomb(x, y, screenId)); break;

    case 'W':
        gameObjects.push_back(new Spring(Point(x, y, 'W'), Direction::STAY, screenId, 3));
        break;

    default: break;
    }
}

// --- Main ---

void LevelLoader::loadLevelFromFile(const std::string& fileName, Screen& screen, std::vector<GameObject*>& gameObjects)
{
    std::ifstream file(fileName);

    // Validation: Ensure file opened successfully
    GameException::ensureFileOpen(file, fileName, "LevelLoader::loadLevelFromFile");

    std::vector<std::string> rawLines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.size() < Screen::WIDTH) line.resize(Screen::WIDTH, ' ');
        rawLines.push_back(line);
    }
    file.close();

    screen.setDark(false);
    std::vector<std::string> mapLayout;
    std::vector<std::string> metadata;

    for (size_t i = 0; i < rawLines.size(); ++i) {
        if (i < Screen::HEIGHT) mapLayout.push_back(rawLines[i]);
        else metadata.push_back(rawLines[i]);
    }
    // Pad the map if file is short
    while (mapLayout.size() < Screen::HEIGHT) mapLayout.push_back(std::string(Screen::WIDTH, ' '));

    ScreenId currentScreenId = screen.getScreenId();

    // Pass 1: Static objects
    for (size_t row = 0; row < mapLayout.size(); ++row) {
        for (size_t col = 0; col < mapLayout[row].size(); ++col) {
            char c = mapLayout[row][col];
            if (c == ' ') continue;

            if (c == '$') { screen.setStartPos1((int)col, (int)row); mapLayout[row][col] = ' '; }
            else if (c == '&') { screen.setStartPos2((int)col, (int)row); mapLayout[row][col] = ' '; }
            else if (c == 'L') { screen.setLegendStart((int)col, (int)row); mapLayout[row][col] = ' '; }
            else if (c == '*' || c == '#') { continue; }
            else { createObject(c, (int)col, (int)row, currentScreenId, gameObjects); mapLayout[row][col] = ' '; }
        }
    }

    // Pass 2: Obstacles (BFS)
    std::vector<std::vector<bool>> visited(Screen::HEIGHT, std::vector<bool>(Screen::WIDTH, false));
    auto inBounds = [](int x, int y) { return x >= 0 && x < Screen::WIDTH && y >= 0 && y < Screen::HEIGHT; };

    for (int y = 0; y < Screen::HEIGHT; ++y) {
        for (int x = 0; x < Screen::WIDTH; ++x) {
            if (mapLayout[y][x] != '*' || visited[y][x]) continue;

            std::queue<std::pair<int, int>> q;
            std::vector<Point> cluster;
            q.push({ x, y });
            visited[y][x] = true;

            while (!q.empty()) {
                auto cell = q.front(); q.pop();
                int cx = cell.first; int cy = cell.second;
                cluster.emplace_back(cx, cy, '*');
                mapLayout[cy][cx] = ' ';

                const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
                for (auto& d : dirs) {
                    int nx = cx + d[0]; int ny = cy + d[1];
                    if (inBounds(nx, ny) && !visited[ny][nx] && mapLayout[ny][nx] == '*') {
                        visited[ny][nx] = true; q.push({ nx, ny });
                    }
                }
            }
            if (!cluster.empty()) gameObjects.push_back(new Obstacle(cluster, currentScreenId));
        }
    }

    screen.setLayout(mapLayout);

    // Pass 3: Metadata
    for (const auto& meta : metadata) {
        if (meta.find("CONNECT") != std::string::npos) parseConnectCommand(meta, gameObjects, currentScreenId);
        else if (meta.find("DOOR_DATA") != std::string::npos) parseDoorData(meta, gameObjects, currentScreenId);
        else if (meta.find("KEY_DATA") != std::string::npos) parseKeyData(meta, gameObjects, currentScreenId);
        else if (meta.find("DARK") != std::string::npos) screen.setDark(true);
        else if (meta.find("SPRING_DATA") != std::string::npos) parseSpringData(meta, gameObjects, currentScreenId);
    }
}

void LevelLoader::loadScreenFromFile(const std::string& fileName, Screen& screen)
{
    std::ifstream file(fileName);

    // Validation: Ensure file opened successfully
    GameException::ensureFileOpen(file, fileName, "LevelLoader::loadScreenFromFile");

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < Screen::HEIGHT) {
        screen.setLine(row, line);
        row++;
    }
}