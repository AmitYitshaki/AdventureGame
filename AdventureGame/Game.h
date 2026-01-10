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
#include <cctype>
#include <thread>
#include <deque>
#include <iostream>
#include <conio.h>

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
#include "GameException.h"

// Game Objects
#include "Bomb.h"
#include "Key.h"
#include "Door.h"
#include "Spring.h"
#include "Torch.h"
#include "Riddle.h"   
#include "Obstacle.h"
#include "Switch.h"
#include "Laser.h"

// Helper struct for game events
struct GameEvent {
    long cycle;
    std::string type;
    std::string details;
};

// =============================================================
//               BASE CLASS: Game (Abstract)
// =============================================================
class Game
{
public:
    Game();
    virtual ~Game();

    // Delete copy constructor/assignment to prevent logic errors
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Main entry point to start the game loop
    void start();

    // Static accessors for global functionality
    static void playSound(int frequency, int duration);
    static void setStatusMessage(const std::string& msg);

protected:
    // =========================================================
    //                    VIRTUAL INTERFACE
    // =========================================================

    // Abstract: Must implement input source (Keyboard vs File)
    virtual char getNextChar() = 0;

    // Abstract: Must implement event handling (Recording vs Verification)
    virtual void handleEventReport(const std::string& type, const std::string& details) = 0;

    // Abstract: Session lifecycle hooks
    virtual void initSession() = 0;
    virtual void endSession() = 0;

    // Virtual: Can be overridden for specific behavior
    virtual void resetGame(); // Overridden by KeyBoardGame to reset recording
    virtual void handleSleep(); // Overridden by FileGame for speed
    virtual void outputGraphics(); // Overridden by FileGame for silent mode

    // Configuration Hooks
    virtual bool allowSaveLoad() const { return true; }
    virtual bool isSilentMode() const { return false; }

protected:
    // =========================================================
    //                     CORE LOGIC
    // =========================================================

    void update();
    void handleInput();
    void draw(); // Physical rendering to buffer
    void initGame();
    void end() { isRunning = false; }

    // Wrapper to invoke virtual event reporting
    void reportEvent(const std::string& type, const std::string& details);

    // =========================================================
    //                    GAME MECHANICS
    // =========================================================

    // Entity Management
    void cleanupDeadObjects();
    void processPlayerMovement(Player& p, Player* other);
    void handleInteractions();
    void checkGameStatus();
    void handleFlowControl();
    void updateBombs();

    // Mechanics & Effects
    void visualizeExplosion(int cx, int cy, int radius);
    void applyBombEffects(int cx, int cy, Screen& curr_screen, int R);
    void explodeCell(int x, int y, Screen& screen);

    // Level Management
    void checkLevelTransition();
    void goToScreen(ScreenId id);
    void restartCurrentLevel();
    void gameOverScreen(const std::string& message);
    void pauseScreen();

    // =========================================================
    //                    RIDDLE SYSTEM
    // =========================================================

    void handleRiddleInput(char key);
    void startRiddle(Riddle* riddle, Player& p);
    void loadRiddlesFromFile(const std::string& filename);
    void assignRiddlesToLevel();
    bool checkPlayerHasRiddle();
    const RiddleData* getRiddleDataById(int id) const;

    // =========================================================
    //                  SAVE / LOAD SYSTEM
    // =========================================================

    void saveGameState(const std::string& filename);
    void loadGameState(const std::string& filename);
    GameObject* createObjectFromSave(const std::string& type, std::stringstream& ss);
    GameObject* findObjectAt(int x, int y);

    // =========================================================
    //                  GRAPHICS & RENDERING
    // =========================================================

    std::vector<std::string> initBuffer();
    void writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c);
    void writeHudText(std::vector<std::string>& buffer, int x, int y, const std::string& text);
    void renderBuffer(const std::vector<std::string>& buffer);

    void drawObjectsToBuffer(std::vector<std::string>& buffer);
    void drawPlayersToBuffer(std::vector<std::string>& buffer);
    void drawLegendToBuffer(std::vector<std::string>& buffer);
    void drawStatusMessageAt(std::vector<std::string>& buffer, int x, int y, int maxWidth);
    void drawRiddle(std::vector<std::string>& buffer);
    void drawHomeMessage(std::vector<std::string>& buffer);
    void applyLighting(std::vector<std::string>& buffer);

    int resolveColor(char c, int x, int y);
    void setConsoleColor(int colorCode);
    int getColorForChar(char c);
    void ChangeDirection(char c);
    void stopMovement();

    // Toggles
    void toggleColor() { colorEnabled = !colorEnabled; }
    void toggleSound() { soundEnabled = !soundEnabled; }
    bool isColorEnabled() const { return colorEnabled; }
    static bool isSoundEnabled();

protected:
    // =========================================================
    //                     DATA MEMBERS
    // =========================================================

    static constexpr int TICK_MS = 64;
    static constexpr ScreenId FINAL_LEVEL = ScreenId::ROOM4;

    // Entities
    Player player1;
    Player player2;
    Screen screens[(int)ScreenId::NumOfScreens];
    Screen* currentScreen = nullptr;
    std::vector<GameObject*> gameObjects;

    // State
    bool isRunning = true;
    unsigned long cycleCounter = 0;
    unsigned int randomSeed = 0;
    bool isGameSessionActive = false;

    // Flags
    bool RiddleMode = false;
    Riddle* currentRiddle = nullptr;
    Player* currentRiddlePlayer = nullptr;
    std::vector<RiddleData> riddlesPool;
    bool exitToMainMenu = false;
    bool pendingRestart = false;
    bool colorEnabled = true;

    // Statics
    static bool soundEnabled;
    static std::string statusMessage;
};

// =============================================================
//               DERIVED CLASS: KeyBoardGame
// =============================================================
// Handles interactive gameplay and recording (Save Mode)
class KeyBoardGame : public Game
{
public:
    KeyBoardGame(bool recordMode = false);
    virtual ~KeyBoardGame();

protected:
    // Virtual Implementations
    virtual char getNextChar() override;
    virtual void handleEventReport(const std::string& type, const std::string& details) override;
    virtual void initSession() override;
    virtual void endSession() override;
    virtual void resetGame() override;

    // Configuration Overrides
    virtual bool allowSaveLoad() const override { return !isRecording; }

private:
    bool isRecording;
    std::ofstream stepsFileOut;
    std::ofstream resultFileOut;
};

// =============================================================
//               DERIVED CLASS: FileGame
// =============================================================
// Handles playback and automated testing (Load/Silent Mode)
class FileGame : public Game
{
public:
    FileGame(bool silentMode = false);
    virtual ~FileGame();

protected:
    // Virtual Implementations
    virtual char getNextChar() override;
    virtual void handleEventReport(const std::string& type, const std::string& details) override;
    virtual void initSession() override;
    virtual void endSession() override;

    // Silent Mode Optimizations
    virtual void handleSleep() override;
    virtual void outputGraphics() override;

    // Configuration Overrides
    virtual bool isSilentMode() const override { return isSilent; }
    virtual bool allowSaveLoad() const override { return false; }

private:
    void loadExpectedResults();
    void verifyEvent(const std::string& type, const std::string& details);

private:
    bool isSilent;
    std::ifstream stepsFileIn;
    std::vector<std::pair<long, char>> stepsBuffer;
    size_t playbackIndex = 0;

    // Verification Data
    std::deque<GameEvent> expectedResults;
    bool testFailed = false;
    std::string failureReason;
};