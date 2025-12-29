#pragma once

// =============================================================
//                     STANDARD LIBRARIES
// =============================================================
#include <vector>
#include <string>
#include <algorithm> 
#include <random>    
#include <chrono>    
#include <fstream>
#include <sstream>
#include <windows.h>
#include <memory> 

// =============================================================
//                     PROJECT INCLUDES
// =============================================================

#include "ScreenID.h"
#include "Utils.h"
#include "Point.h"

#include "GameObject.h"
#include "Screen.h"
#include "Player.h"
#include "LevelLoader.h"


#include "Bomb.h"
#include "Key.h"
#include "Door.h"
#include "Spring.h"
#include "Torch.h"
#include "Riddle.h"   
#include "Obstacle.h"
#include "Switch.h"
#include "Laser.h"

/*
 * ===================================================================================
 * Class: Game
 * -----------------------------------------------------------------------------------
 * Purpose:
 * The central engine/controller of the application. It manages the main game loop,
 * game state, rendering pipeline, and input processing.
 *
 * Behavior:
 * - Initializes the game world and loads levels.
 * - Runs the "Game Loop" (Input -> Update -> Draw).
 * - Manages transitions between screens (Levels, Home, Instructions).
 * - Handles global systems like Lighting, HUD, and Message Logs.
 *
 * Implementation:
 * - Holds the instances of Player1 and Player2.
 * - Maintains an array of 'Screen' objects representing the world map.
 * - Uses a double-buffer approach (via std::vector<string>) for flicker-free rendering.
 * ===================================================================================
 */

class Game
{
public:
    Game();
    ~Game();

    // מניעת העתקה (Singleton-like behavior)
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void start();

private:
    // --- Core Loop ---
    void update();
    void draw();
    void handleInput();
    void initGame();
    void resetGame();
    void end() { isRunning = false; }

    // --- UI Helpers ---
    std::vector<std::string> initBuffer();
    void writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c);
    void writeHudText(std::vector<std::string>& buffer, int x, int y, const std::string& text);
    void renderBuffer(const std::vector<std::string>& buffer);

    // --- Drawing Components ---
    void drawLegendToBuffer(std::vector<std::string>& buffer);
    void drawObjectsToBuffer(std::vector<std::string>& buffer);
    void drawPlayersToBuffer(std::vector<std::string>& buffer);
    void drawStatusMessageAt(std::vector<std::string>& buffer, int x, int y, int maxWidth);
    void drawRiddle(std::vector<std::string>& buffer);
    void drawHomeMessage(std::vector<std::string>& buffer);
    void applyLighting(std::vector<std::string>& buffer);

    // --- Color System ---
    void toggleColor() { colorEnabled = !colorEnabled; }
    void toggleSound() { soundEnabled = !soundEnabled; }
    bool isColorEnabled() const { return colorEnabled; }
    bool isSoundEnabled() const { return soundEnabled; }
    void setConsoleColor(int colorCode);
    int getColorForChar(char c);
    int resolveColor(char c, int x, int y);

    // --- Logic & Mechanics ---
    void playSound(int frequency, int duration);
    void ChangeDirection(char c);
    void handleRiddleInput(char key);
    void stopMovement();
    void checkIsPlayerLoaded();
    bool checkPlayerHasRiddle();
    void checkLevelTransition();
    void gameOverScreen(const std::string& message);
    void restartCurrentLevel();
    void startRiddle(Riddle* riddle, Player& p);
    void loadRiddlesFromFile(const std::string& filename);
    void assignRiddlesToLevel();
    void goToScreen(ScreenId id);
    void pauseScreen();
    void explodeCell(int x, int y, Screen& screen);
    void visualizeExplosion(int cx, int cy, int radius);
    void updateBombs();
    void applyBombEffects(int cx, int cy, Screen& curr_screen, int R);

public:
    static void setStatusMessage(const std::string& msg);

private:
    static constexpr int TICK_MS = 64;

    Player player1;
    Player player2;
    Screen screens[(int)ScreenId::NumOfScreens];
    Screen* currentScreen = nullptr;
    std::vector<GameObject*> gameObjects;

    bool isRunning = true;
    bool RiddleMode = false;
    Riddle* currentRiddle = nullptr;
    Player* currentRiddlePlayer = nullptr;
    std::vector<RiddleData> riddlesPool;

    // Flags
    bool exitToMainMenu = false;
    bool pendingRestart = false;
    bool colorEnabled = true;
    bool soundEnabled = true;

    static std::string statusMessage;
};