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

// מבנה עזר לאירועים (נשאר כמו שהיה)
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

    // מחיקת העתקות למניעת באגים
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // הפונקציה הראשית שמפעילה את המשחק
    void start();

    // סטטי כדי שיהיה נגיש לכולם
    static void playSound(int frequency, int duration);
    static void setStatusMessage(const std::string& msg);

protected:
    // === VIRTUAL INTERFACE (החלק הפולימורפי) ===
    // כל יורש חייב לממש איך הוא מביא את התו הבא (מהמקלדת או מהקובץ)
    virtual char getNextChar() = 0;

    // כל יורש מטפל בדיווחים אחרת (כתיבה לקובץ או אימות מול צפי)
    virtual void handleEventReport(const std::string& type, const std::string& details) = 0;

    // אתחול ספציפי (תפריט במקלדת, טעינת קבצים בקובץ)
    virtual void initSession() = 0;

    // סיום ספציפי (סגירת הקלטה, הדפסת דוח מבחן)
    virtual void endSession() = 0;

    // ניהול זמן (FileGame ידרוס את זה ב-Silent כדי לרוץ מהר)
    virtual void handleSleep();

    // ציור (FileGame ידרוס את זה ב-Silent כדי לא לצייר)
    virtual void outputGraphics();

protected:
    // === COMMON LOGIC (פונקציות עזר משותפות) ===
    void update();
    void draw(); // הציור הפיזי לבאפר
    void handleInput();
    void initGame();
    void resetGame();
    void end() { isRunning = false; }

    // עטיפה לדיווח שקוראת לפונקציה הוירטואלית
   virtual void reportEvent(const std::string& type, const std::string& details);

    // --- Helpers (אותן פונקציות שהיו לך קודם) ---
    void cleanupDeadObjects();
    void processPlayerMovement(Player& p, Player* other);
    void handleInteractions();
    void checkGameStatus();
    void handleFlowControl();

    // --- Specific Mechanics ---
    void updateBombs();
    void visualizeExplosion(int cx, int cy, int radius);
    void applyBombEffects(int cx, int cy, Screen& curr_screen, int R);
    void explodeCell(int x, int y, Screen& screen);
    void checkLevelTransition();
    void goToScreen(ScreenId id);
    void gameOverScreen(const std::string& message);
    void restartCurrentLevel();
    void pauseScreen(); // שים לב: זה יצטרך שינוי קטן ב-cpp כדי להשתמש ב-getNextChar

    // --- Riddles ---
    void handleRiddleInput(char key);
    void startRiddle(Riddle* riddle, Player& p);
    void loadRiddlesFromFile(const std::string& filename);
    void assignRiddlesToLevel();
    bool checkPlayerHasRiddle();
    const RiddleData* getRiddleDataById(int id) const;

    // --- Save/Load Game State ---
    void saveGameState(const std::string& filename);
    void loadGameState(const std::string& filename);
    GameObject* createObjectFromSave(const std::string& type, std::stringstream& ss);
    GameObject* findObjectAt(int x, int y);

    // --- Drawing Internals ---
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

    void toggleColor() { colorEnabled = !colorEnabled; }
    void toggleSound() { soundEnabled = !soundEnabled; }
    bool isColorEnabled() const { return colorEnabled; }
    static bool isSoundEnabled();

protected:
    // === SHARED DATA MEMBERS ===
    static constexpr int TICK_MS = 64;
    static constexpr ScreenId FINAL_LEVEL = ScreenId::ROOM4;

    Player player1;
    Player player2;
    Screen screens[(int)ScreenId::NumOfScreens];
    Screen* currentScreen = nullptr;
    std::vector<GameObject*> gameObjects;

    bool isRunning = true;
    unsigned long cycleCounter = 0;
    unsigned int randomSeed = 0;
    bool isGameSessionActive = false; // האם אנחנו בתוך משחק פעיל

    // Flags
    bool RiddleMode = false;
    Riddle* currentRiddle = nullptr;
    Player* currentRiddlePlayer = nullptr;
    std::vector<RiddleData> riddlesPool;
    bool exitToMainMenu = false;
    bool pendingRestart = false;
    bool colorEnabled = true;
    static bool soundEnabled;
    static std::string statusMessage;
};

// =============================================================
//               DERIVED CLASS: KeyBoardGame
// =============================================================
// אחראית על משחק אינטראקטיבי ועל הקלטה (Save Mode)
class KeyBoardGame : public Game
{
public:
    KeyBoardGame(bool recordMode = false);
    virtual ~KeyBoardGame();

protected:
    // מימושים וירטואליים
    virtual char getNextChar() override;
    virtual void handleEventReport(const std::string& type, const std::string& details) override;
    virtual void initSession() override;
    virtual void endSession() override;

private:
    bool isRecording;
    std::ofstream stepsFileOut;
    std::ofstream resultFileOut;
};

// =============================================================
//               DERIVED CLASS: FileGame
// =============================================================
// אחראית על ניגון הקלטות ועל בדיקות אוטומטיות (Load/Silent Mode)
class FileGame : public Game
{
public:
    FileGame(bool silentMode = false);
    virtual ~FileGame();

protected:
    // מימושים וירטואליים
    virtual char getNextChar() override;
    virtual void handleEventReport(const std::string& type, const std::string& details) override;
    virtual void initSession() override;
    virtual void endSession() override;

    // דריסות מיוחדות למצב Silent
    virtual void handleSleep() override;
    virtual void outputGraphics() override;

private:
    void loadExpectedResults();
    void verifyEvent(const std::string& type, const std::string& details);

private:
    bool isSilent;
    std::ifstream stepsFileIn;
    std::vector<std::pair<long, char>> stepsBuffer;
    size_t playbackIndex = 0;

    // אימות תוצאות
    std::deque<GameEvent> expectedResults;
    bool testFailed = false;
    std::string failureReason;
};