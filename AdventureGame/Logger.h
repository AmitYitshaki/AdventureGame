#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "ScreenID.h"

class Game;
class Player;
class Screen;
class GameObject;
class Key;
class Door;
class Switch;
class Laser;
class Torch;
class Spring;


class Logger
{
public:
    static void Init(const std::string& fileName = "game_log.txt");
    static void Log(const std::string& message);
    static void Error(const std::string& message);

    static const std::vector<std::string>& GetRecentLogs();

	static std::string getPlayerInfo(const Player& );
	static std::string getScreenInfo(const Screen&);
	static std::string getGameObjectInfo(const GameObject* obj);
    static std::vector<std::string> getScreenObjectsList(const std::vector<GameObject*>& allObjects, ScreenId currentScreenId);


private: 
    static std::ofstream logFile;
    static std::vector<std::string> recentLogs;
	static std::string GetTimestamp();
};

