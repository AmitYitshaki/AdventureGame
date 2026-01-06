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
#include <thread> // <--- ADDED for non-blocking sound
#include <deque>

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

#include "Bomb.h"
#include "Key.h"
#include "Door.h"
#include "Spring.h"
#include "Torch.h"
#include "Riddle.h"   
#include "Obstacle.h"
#include "Switch.h"
#include "Laser.h"

enum class RunMode {
    Normal,
    Save,   // Recording input
    Load,   // Replaying input visually
    Silent  // Replaying input without visuals (Testing)
};

struct GameEvent {
    long cycle;
    std::string type;    // e.g., "SCREEN_CHANGE", "LIFE_LOST"
    std::string details; // e.g., "P1 ROOM2", "CORRECT"
};

class Game
{
public:
    Game();
    Game(RunMode mode = RunMode::Normal);
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void start();

    // Made public static so other classes (Player, Door) can trigger sounds easily
    static void playSound(int frequency, int duration);

private:
    // --- Core Loop ---
    void update();
    void draw();
    void handleInput();
    void initGame();
    void resetGame();
    void end() { isRunning = false; }

    // --- Memory Management ---
    void cleanupDeadObjects(); // <--- NEW: Garbage Collector

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
    static bool isSoundEnabled(); 
    void setConsoleColor(int colorCode);
    int getColorForChar(char c);
    int resolveColor(char c, int x, int y);

    // --- Logic & Mechanics ---
    void ChangeDirection(char c);
    void handleRiddleInput(char key);
    void stopMovement();
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
    void safeDeleteObject(GameObject* objToRemove);

    // --- Recording & Playback Implementation ---
    void initFiles();              // שיניתי את השם מ-initStepsFile כי זה מטפל גם ב-results
    void closeFiles();             // פונקציה חדשה לסגירה ודיווח

    void recordInput(char key);
    char getRecordedInput();

    // ניהול תוצאות
    void loadExpectedResults();    // טעינת קובץ התוצאות לזיכרון
    void reportEvent(const std::string& type, const std::string& details); // דיווח מרכזי
    void verifyEvent(const std::string& type, const std::string& details); // אימות מול הצפי

    // --- Update Helpers (Refactoring) ---
    void processPlayerMovement(Player& p, Player* other); // מטפל בלולאת המהירות
    void handleInteractions();                            // מטפל בחידות ואינטראקציות
    void checkGameStatus();                               // בודק חיים ו-Game Over
    void handleFlowControl();                             // מטפל ב-Restart/Exit
    void checkPlaybackStatus();                           // מטפל בסיום הקלטה

    // --- Save/Load System ---
    void saveGameState(const std::string& filename);
    void loadGameState(const std::string& filename);
    // Helper to find an object at specific coords (for re-linking items)
    GameObject* findObjectAt(int x, int y);
    // Factory: Creates an object from a text line
    GameObject* createObjectFromSave(const std::string& type, std::stringstream& ss);

    const RiddleData* getRiddleDataById(int id) const;

public:
    static void setStatusMessage(const std::string& msg);

private:
    static constexpr int TICK_MS = 64;
    const std::string SAVE_FILENAME = "savegame.sav";
	static constexpr ScreenId FINAL_LEVEL = ScreenId::ROOM4; // <---------------- if adding more levels, change this

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
    static bool soundEnabled; // Changed to static

    static std::string statusMessage;

	// Recording & Playback
    RunMode currentMode;
    unsigned long cycleCounter = 0; // The game "Time"
    unsigned int randomSeed = 0;

    bool isGameSessionActive = false; // האם אנחנו בתוך משחק פעיל (לא תפריט)
    void startRecordingSession();     // התחלת הקלטה/ניגון (איפוס שעון ופתיחת קבצים)
    void stopRecordingSession();      // סיום הקלטה (סגירת קבצים)

    std::ofstream stepsFileOut;
    std::ifstream stepsFileIn;
    std::vector<std::pair<long, char>> stepsBuffer;
    size_t playbackIndex = 0;

    // === NEW: Result File Handling ===
    std::ofstream resultFileOut;           // לכתיבת תוצאות (Save Mode)
    std::deque<GameEvent> expectedResults; // לאימות תוצאות (Load/Silent Mode)

    bool testFailed = false;
    std::string failureReason;
};