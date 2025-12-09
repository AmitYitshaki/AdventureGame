#pragma once

// Core includes
#include <vector>
#include <string>

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
    Player* currentRiddlePlayer = nullptr;

    static std::string statusMessage;

    // ============================================================
    //                     RENDERING SYSTEM
    // ============================================================

    std::vector<std::string> initBuffer();
    void writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c);
    void writeTextToBuffer(std::vector<std::string>& buffer, int x, int y, const std::string& text);

    void drawObjectsToBuffer(std::vector<std::string>& buffer);
    void drawPlayersToBuffer(std::vector<std::string>& buffer);
    void drawStatusToBuffer(std::vector<std::string>& buffer);
    void drawRiddle(std::vector<std::string>& buffer);
    void renderBuffer(const std::vector<std::string>& buffer);

    void applyLighting(std::vector<std::string>& buffer);

    // ============================================================
    //                        INPUT SYSTEM
    // ============================================================

    void ChangeDirection(char c);
    void handleRiddleInput(char key);

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

    // Navigation
    void goToScreen(ScreenId id) {
        currentScreen = &screens[(int)id];
    }

    // Pause screen
    void pauseScreen();
};
