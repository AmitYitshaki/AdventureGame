#pragma once
#include <vector>
#include <string>
#include "Screen.h"
#include "Game.h"
#include "ScreenID.h"
#include "Player.h"
#include "GameObject.h"

// LevelLoader:Responsible for loading form files the game levels (screens) including game objects.

class LevelLoader
{
public:
	static void loadLevelFromFile(
		const std::string& filename,
		Screen& screen,
		std::vector<GameObject*>& gameObjects);

	static void loadScreenFromFile(
		const std::string& fileName,
		Screen& screenToFill
	);

private:
	static void createObject(
		char c,
		int x, int y,
		ScreenId screenId,
		std::vector<GameObject*>& gameObjects);
	static void handlePlayerStartPosition(
		char c,
		int x, int y,
		Screen& screen);
};

