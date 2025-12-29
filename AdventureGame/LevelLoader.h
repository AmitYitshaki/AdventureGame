#pragma once
#include <string>
#include <vector>
#include "Screen.h"
#include "GameObject.h"
#include "GameException.h"

class Laser;

/*
 * ===================================================================================
 * Class: LevelLoader
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A static utility class responsible for parsing level files (.txt) and converting
 * them into in-game objects and map layouts.
 *
 * Behavior:
 * - Reads ASCII art from text files to define walls and floor plans.
 * - Parses metadata lines (e.g., "DOOR_DATA", "CONNECT") to configure complex objects.
 * - Instantiates specific GameObject subclasses (Bomb, Enemy, etc.) based on characters.
 *
 * Implementation:
 * - Entirely static; no instance is needed.
 * - Uses helper functions to parse specific data formats (coordinate extraction).
 * ===================================================================================
 */

class LevelLoader
{
public:
    static void loadLevelFromFile(const std::string& fileName, Screen& screen, std::vector<GameObject*>& gameObjects);
    static void loadScreenFromFile(const std::string& fileName, Screen& screen);

private:
    static void parseDoorData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId);
    static void parseKeyData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId);
    static void parseSpringData(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId);
    static void parseConnectCommand(const std::string& line, std::vector<GameObject*>& gameObjects, ScreenId screenId);

    static void createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects);
    static GameObject* findObjectAt(int x, int y, ScreenId screenId, const std::vector<GameObject*>& gameObjects);

    static std::vector<GameObject*> collectLaserBeam(Laser* startNode, const std::vector<GameObject*>& allObjects);
    static void scanDirection(int startX, int startY, int dx, int dy, char type, ScreenId screenId,
        const std::vector<GameObject*>& allObjects, std::vector<GameObject*>& outputBeam);
};