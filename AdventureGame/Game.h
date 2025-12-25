#pragma once

// Core includes
#include <vector>
#include <string>
#include <algorithm> // std::shuffle, std::sort, std::reverse
#include <random>    // std::default_random_engine
#include <chrono>    // std::chrono
#include <fstream>
#include <sstream>

// Project includes
#include "Player.h"
#include "Screen.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Key.h"
#include "Door.h"
#include "Spring.h"
#include "Torch.h"
#include "Riddle.h"
#include "Obstacle.h"
#include "LevelLoader.h"
#include "Logger.h"
#include "Bomb.h"


/*
    Game:
    Central controller for the entire game loop.
    Handles rendering, input, world updates,
    level transitions, riddles, lighting, and UI.
*/
class Game
{
public:
    // --- Constructor & Destructor ---
    Game();
    ~Game();

    // --- Main Loop ---
    void start();
    void draw();
    void update();
    void handleInput();

    // --- Game State ---
    void initGame();
    void resetGame();
    bool running() const { return isRunning; }
    void end() { isRunning = false; }

    // --- UI / Status ---
    static void setStatusMessage(const std::string& msg);

private:
    // ============================================================
    //                INTERNAL GAME STATE & DATA
    // ============================================================
    static constexpr int TICK_MS = 128; // Game tick duration in milliseconds(for the sleep function)

    Player player1;
    Player player2;

    Screen screens[(int)ScreenId::NumOfScreens];
    Screen* currentScreen = nullptr;

    std::vector<GameObject*> gameObjects;

    bool isRunning = true;
    bool RiddleMode = false;

    Riddle* currentRiddle = nullptr;
    std::vector<RiddleData> riddlesPool;
    Player* currentRiddlePlayer = nullptr;

    bool debugMode = false;

    static std::string statusMessage;

    // ============================================================
    //                     RENDERING SYSTEM
    // ============================================================

    std::vector<std::string> initBuffer();
    void writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c);
    void writeTextToBuffer(std::vector<std::string>& buffer, int x, int y, const std::string& text);
    void writeHudText(std::vector<std::string>& buffer, int x, int y, const std::string& text);
    void drawLegendToBuffer(std::vector<std::string>& buffer);
    void drawObjectsToBuffer(std::vector<std::string>& buffer);
    void drawPlayersToBuffer(std::vector<std::string>& buffer);
    void drawStatusMessageAt(std::vector<std::string>& buffer, int x, int y, int maxWidth);
    void drawRiddle(std::vector<std::string>& buffer);
    void renderBuffer(const std::vector<std::string>& buffer);
    void drawDebugDashboard();
    bool isDebugMode() { return debugMode; }
    void applyLighting(std::vector<std::string>& buffer);

    // ============================================================
    //                        INPUT SYSTEM
    // ============================================================

    void ChangeDirection(char c);
    void handleRiddleInput(char key);
    void setDebugMode(bool adminUse);
    // ============================================================
    //                     GAMEPLAY MECHANICS
    // ============================================================

    void stopMovement();
    void checkIsPlayerLoaded();
    bool checkPlayerHasRiddle();

    // Level transitions
    void resetPlayersForNewLevel();
    void checkLevelTransition();

    // Riddle activation
    void startRiddle(Riddle* riddle, Player& p);
    void loadRiddlesFromFile(const std::string& filename);
    void assignRiddlesToLevel();
    // Navigation
    void goToScreen(ScreenId id);

    // Pause screen
    void pauseScreen();

    void explodeCell(int x, int y, Screen& screen);

    void updateBombs();
    void applyBombEffects(int cx, int cy, Screen& curr_screen, int R);
};
