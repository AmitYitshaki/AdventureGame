#include "Game.h"
#include <conio.h>
#include <iostream>
#include <cstdlib>

std::string Game::statusMessage = "";
bool Game::soundEnabled = true; // Define static variable

// =============================================================
//                    CONSTRUCTOR & INIT
// =============================================================

Game::Game(RunMode mode)
    : player1(0, 0, '$', 16),
    player2(0, 0, '&', 35),
    isRunning(true),
    currentScreen(nullptr),
    currentMode(mode),   
    cycleCounter(0)       
{
    system("mode con: cols=80 lines=50");
    if (currentMode == RunMode::Load || currentMode == RunMode::Silent) {
        initFiles();
		srand(randomSeed); // Ensure consistent randomness for replay
	}
    else
    {
        randomSeed = (unsigned int)time(NULL);
        srand(randomSeed);

        if (currentMode == RunMode::Save) {
            initFiles();
		}
    }

    initGame();
}

Game::~Game()
{
    closeFiles();

    for (auto obj : gameObjects) delete obj;
    gameObjects.clear();
}

// === NEW: Non-Blocking Sound System ===
bool Game::isSoundEnabled() { return soundEnabled; }

void Game::playSound(int frequency, int duration)
{
    if (!soundEnabled) return;

    // Run Beep in a separate thread so it doesn't freeze the game loop
    std::thread([frequency, duration]() {
        Beep(frequency, duration);
        }).detach();
}

void Game::initGame()
{
    screens[(int)ScreenId::HOME] = Screen(ScreenId::HOME);
    LevelLoader::loadScreenFromFile("adv-start.screen", screens[(int)ScreenId::HOME]);

    screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
    LevelLoader::loadScreenFromFile("adv-instructions.screen", screens[(int)ScreenId::INSTRUCTIONS]);

    currentScreen = &screens[(int)ScreenId::HOME];
}

void Game::resetGame()
{
    RiddleMode = false;
    currentRiddle = nullptr;
    setStatusMessage("");

    for (auto obj : gameObjects) delete obj;
    gameObjects.clear();

    player1.resetStats();
    player2.resetStats();

    screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
    LevelLoader::loadLevelFromFile("adv-world_01.screen", screens[(int)ScreenId::ROOM1], gameObjects);

    screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
    LevelLoader::loadLevelFromFile("adv-world_02.screen", screens[(int)ScreenId::ROOM2], gameObjects);

    screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);
    LevelLoader::loadLevelFromFile("adv-world_03.screen", screens[(int)ScreenId::ROOM3], gameObjects);

    screens[(int)ScreenId::ROOM4] = Screen(ScreenId::ROOM4);
    LevelLoader::loadScreenFromFile("adv-world_04.screen", screens[(int)ScreenId::ROOM4]);

    loadRiddlesFromFile("Riddles.txt");
    goToScreen(ScreenId::ROOM1);
}

void Game::start()
{
    hideCursor();
    while (isRunning)
    {
        handleInput();
        update();
        draw();

        // ניהול זמנים לפי מצב הריצה
        if (currentMode == RunMode::Silent) {
            // לא ישנים בכלל - הרצה במהירות המקסימלית של המעבד
        }
        else if (currentMode == RunMode::Load) {
            // הרצה ויזואלית מואצת (Fast Forward)
            Sleep(TICK_MS / 3);
        }
        else {
            // משחק רגיל או הקלטה - מהירות רגילה
            Sleep(TICK_MS);
        }
    }
    cls();
}

void Game::restartCurrentLevel()
{
    ScreenId currentId = currentScreen->getScreenId();

    // --- התיקון: מחיקה סלקטיבית ---
    // אנחנו מוחקים רק אובייקטים ששייכים לחדר שאותו אנחנו מאתחלים
    auto it = gameObjects.begin();
    while (it != gameObjects.end()) {
        if ((*it)->getScreenId() == currentId) {
            delete* it;                 // שחרור זיכרון
            it = gameObjects.erase(it); // הוצאה מהרשימה
        }
        else {
            ++it; // אובייקטים מחדרים אחרים נשארים בטוחים
        }
    }
    // -----------------------------

    player1.restoreState();
    player2.restoreState();

    std::string filename;
    switch (currentId) {
    case ScreenId::ROOM1: filename = "adv-world_01.screen"; break;
    case ScreenId::ROOM2: filename = "adv-world_02.screen"; break;
    case ScreenId::ROOM3: filename = "adv-world_03.screen"; break;
    case ScreenId::ROOM4: filename = "adv-world_04.screen"; break;
    default: filename = "adv-world_01.screen"; break;
    }

    // יצירת המסך מחדש
    screens[(int)currentId] = Screen(currentId);

    // טעינת האובייקטים (זה יוסיף אותם לסוף הרשימה, אחרי האובייקטים של החדרים האחרים)
    LevelLoader::loadLevelFromFile(filename, screens[(int)currentId], gameObjects);

    RiddleMode = false;
    currentRiddle = nullptr;
    currentRiddlePlayer = nullptr;
    setStatusMessage("");
    goToScreen(currentId);
    assignRiddlesToLevel();
}

void Game::gameOverScreen(const std::string& message)
{
    // דיווח לקובץ התוצאות שהמשחק נגמר (כדי שנוכל לוודא את זה בטסט)
    reportEvent("GAME_OVER", message);

    playSound(100, 600); // Long low beep
    system("cls");

    // --- קוד הציור (נשאר זהה) ---
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 9;
    int x = (Screen::WIDTH - BOX_WIDTH) / 2;
    int y = (Screen::HEIGHT - BOX_HEIGHT) / 2;

    auto printCentered = [&](int rowOffset, const std::string& text) {
        int padding = (BOX_WIDTH - 2 - (int)text.length()) / 2;
        gotoxy(x, y + rowOffset);
        std::cout << "#" << std::string(padding, ' ') << text << std::string(BOX_WIDTH - 2 - padding - text.length(), ' ') << "#";
        };

    gotoxy(x, y);     std::cout << std::string(BOX_WIDTH, '#');
    for (int i = 1; i < BOX_HEIGHT - 1; ++i) {
        gotoxy(x, y + i); std::cout << "#" << std::string(BOX_WIDTH - 2, ' ') << "#";
    }
    gotoxy(x, y + BOX_HEIGHT - 1); std::cout << std::string(BOX_WIDTH, '#');

    printCentered(2, "GAME OVER");
    printCentered(4, message);
    printCentered(6, "[R] Restart Level    [H] Main Menu");

    // --- הלולאה המתוקנת ---
    while (true) {
        char key = 0;

        // 1. טיפול במצב LOAD / SILENT (קריאה מהקובץ)
        if (currentMode == RunMode::Load || currentMode == RunMode::Silent) {
            key = getRecordedInput();

            // הגנה: אם נגמרו הצעדים בקובץ ואין קלט, חייבים לצאת כדי לא להיתקע בלולאה אינסופית
            if (key == 0) {
                isRunning = false;
                return;
            }
        }
        // 2. טיפול במצב רגיל / SAVE (קריאה מהמקלדת)
        else {
            if (_kbhit()) {
                key = _getch();
                if (currentMode == RunMode::Save) {
                    recordInput(key); // שמירה לקובץ
                }
            }
        }

        // לוגיקה רגילה
        if (key == 'R' || key == 'r') { pendingRestart = true; return; }
        else if (key == 'H' || key == 'h') { exitToMainMenu = true; return; }
    }
}

// === NEW: Garbage Collector to improve stability ===
void Game::cleanupDeadObjects()
{
    auto it = gameObjects.begin();
    while (it != gameObjects.end()) {
        // Objects with position (-1, -1) are marked for deletion
        if ((*it)->getX() == -1) {
            // Safety: Ensure players aren't holding a pointer to the object we are about to delete
            if (player1.isHoldingItem(*it)) player1.forceDropItem();
            if (player2.isHoldingItem(*it)) player2.forceDropItem();

            delete* it;
            it = gameObjects.erase(it);
        }
        else {
            ++it;
        }
    }
}

// =============================================================
//               FILE HANDLING (RECORDING SYSTEM)
// =============================================================

void Game::initFiles() // החלף את initStepsFile בזה
{
    // === מצב SAVE: פתיחת שני הקבצים לכתיבה ===
    if (currentMode == RunMode::Save)
    {
        stepsFileOut.open("adv-world.steps");
        resultFileOut.open("adv-world.result"); // פתיחת קובץ התוצאות

        if (!stepsFileOut || !resultFileOut) {
            throw GameException("Could not create record files!", "Game::initFiles");
        }
        stepsFileOut << "SEED: " << randomSeed << std::endl;
    }
    // === מצב LOAD/SILENT: קריאת צעדים + תוצאות ===
    else if (currentMode == RunMode::Load || currentMode == RunMode::Silent)
    {
        stepsFileIn.open("adv-world.steps");
        if (!stepsFileIn) return; // אולי אין קובץ, לא נכשלים על זה

        std::string header;
        stepsFileIn >> header >> randomSeed;
        if (header != "SEED:") throw GameException("Invalid steps format", "Game::initFiles");

        srand(randomSeed); // שחזור הרנדומליות

        long t; char k;
        while (stepsFileIn >> t >> k) {
            stepsBuffer.push_back({ t, k });
        }
        stepsFileIn.close();

        loadExpectedResults(); // טעינת הצפי לקובץ התוצאות
    }
}

void Game::loadExpectedResults()
{
    std::ifstream resIn("adv-world.result");
    if (!resIn) return;

    long t;
    std::string type;
    while (resIn >> t >> type) {
        std::string details;
        std::getline(resIn, details);
        // ניקוי רווח בהתחלה שנוצר מ-getline
        if (!details.empty() && details[0] == ' ') details.erase(0, 1);
        expectedResults.push_back({ t, type, details });
    }
    resIn.close();
}

void Game::closeFiles()
{
    if (stepsFileOut.is_open()) stepsFileOut.close();
    if (resultFileOut.is_open()) resultFileOut.close();

    // במצב שקט, מדפיסים בסוף אם הטסט עבר
    if (currentMode == RunMode::Silent) {
        if (testFailed) {
            std::cout << "Test Failed: " << failureReason << std::endl;
        }
        else if (!expectedResults.empty()) {
            std::cout << "Test Failed: Game ended but missed expected events." << std::endl;
        }
        else {
            std::cout << "Test Passed" << std::endl;
        }
        // כדי שיראו את התוצאה לפני שהחלון נסגר
        std::cout << "Press any key to exit...";
        _getch();
    }
}

// שמירת קלט בזמן אמת
void Game::recordInput(char key)
{
    if (currentMode == RunMode::Save && stepsFileOut.is_open()) {
        // פורמט: זמן מקש
        stepsFileOut << cycleCounter << " " << key << std::endl;
    }
}

// שליפת קלט בזמן ריצה אוטומטית
char Game::getRecordedInput()
{
    // אם נגמרו הצעדים, אין קלט
    if (playbackIndex >= stepsBuffer.size()) return 0;

    // בדיקה האם הגענו לזמן (Cycle) של הפקודה הבאה
    // stepsBuffer[playbackIndex].first מחזיק את ה-Cycle
    if (stepsBuffer[playbackIndex].first == cycleCounter) {
        char key = stepsBuffer[playbackIndex].second;
        playbackIndex++;
        return key;
    }
    return 0;
}

void Game::reportEvent(const std::string& type, const std::string& details)
{
    // 1. שמירה לקובץ (בזמן הקלטה)
    if (currentMode == RunMode::Save && resultFileOut.is_open()) {
        resultFileOut << cycleCounter << " " << type << " " << details << std::endl;
    }

    // 2. בדיקה מול הצפי (בזמן הרצה אוטומטית)
    if (currentMode == RunMode::Load || currentMode == RunMode::Silent) {
        verifyEvent(type, details);
    }
}

void Game::verifyEvent(const std::string& type, const std::string& details)
{
    if (testFailed) return;

    if (expectedResults.empty()) {
        testFailed = true;
        failureReason = "Unexpected event: " + type + " (Expected nothing)";
        isRunning = false; // עצירת הריצה כי נכשלנו
        return;
    }

    GameEvent expected = expectedResults.front();
    expectedResults.pop_front();

    // השוואה: מאפשרים סטייה קלה של 1-2 טיקים בזמן (לפעמים קורה בגלל סדר פעולות)
    long timeDiff = std::abs(expected.cycle - (long)cycleCounter);

    if (timeDiff > 2 || expected.type != type || expected.details != details) {
        testFailed = true;
        failureReason = "Mismatch! Exp: [" + std::to_string(expected.cycle) + " " + expected.type + " " + expected.details + "] " +
            "Got: [" + std::to_string(cycleCounter) + " " + type + " " + details + "]";
        isRunning = false;
    }
}

// =============================================================
//                     CORE GAME LOOP UPDATES
// ============================================================

void Game::update()
{
    // 1. קידום זמן (קריטי להקלטות)
    cycleCounter++;

    // 2. תנועת שחקנים ופיזיקה
    processPlayerMovement(player1, &player2);
    processPlayerMovement(player2, &player1);

    // עדכון אפקטים (קפיצים)
    if (player1.isFlying()) player1.updateSpringEffect();
    if (player2.isFlying()) player2.updateSpringEffect();

    updateBombs(); // עדכון פצצות

    // 3. אינטראקציות ומעברים
    if (currentScreen->getScreenId() != ScreenId::HOME &&
        currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
    {
        checkLevelTransition();
    }

    handleInteractions(); // הפעלת חידות אם נדרש

    // 4. תחזוקה
    cleanupDeadObjects();

    // 5. ניהול מצב משחק (ניצחון/הפסד/תפריטים)
    checkGameStatus();
    handleFlowControl();
    checkPlaybackStatus();

    // --- HOOKS FOR PART 2 (TESTING) ---
    // checkResultEvents(); // כאן נכניס את בדיקת התוצאות בהמשך
}

void Game::handleInput()
{
	char key = 0;
    if (currentMode == RunMode::Load || currentMode == RunMode::Silent)
    {
        key = getRecordedInput(); // משיכה מהבאפר האוטומטי
    }
    else
    {
        if (!_kbhit()) return;
        key = _getch();
        if (currentMode == RunMode::Save) {
			if (RiddleMode && key == 27)  //to prevent recording ESC key during riddle mode
                return;
            recordInput(key);
        }
    }
	if (key == 0) return; // אין קלט זמינים

    if (RiddleMode && currentRiddle) {
        handleRiddleInput(key);
        return;
    }

    if (currentScreen == &screens[(int)ScreenId::HOME]) {
        if (key == '1') {
            playSound(600, 100); // Start Game Sound
            resetGame();
            return;
        }
        if (key == '2') { goToScreen(ScreenId::INSTRUCTIONS); return; }
        if (key == '3') {
            try {
                resetGame();
                loadGameState(SAVE_FILENAME);

                setStatusMessage("Game Loaded Successfully!");
                playSound(1000, 200);
                return;
            }
            catch (const std::exception& e) {
                setStatusMessage("Load Failed! Started New Game instead.");
                playSound(200, 300);
            }
            return;
		}
        if (key == '4') { end(); reportEvent("GAME_ENDED", "Score: " + std::to_string(player1.getScore() + player2.getScore()));  return; }
        if (key == 'c' || key == 'C') {
            toggleColor();
            setStatusMessage(isColorEnabled() ? "Color: ON" : "Color: OFF");
            draw(); return;
        }
        if (key == 's' || key == 'S') {
            toggleSound();
            setStatusMessage(isSoundEnabled() ? "Sound: ON" : "Sound: OFF");
            if (isSoundEnabled()) playSound(400, 100);
            return;
        }
    }

    if (currentScreen == &screens[(int)ScreenId::ROOM4]) {
        if (key == '1') { resetGame(); }
        else if (key == '2') { goToScreen(ScreenId::HOME); setStatusMessage(""); }
        else if (key == '3') { end(); }
        return;
    }

    if (currentScreen == &screens[(int)ScreenId::INSTRUCTIONS] && (key == 'h' || key == 'H')) {
        goToScreen(ScreenId::HOME); return;
    }

    if (key == 27) { pauseScreen(); return; }

    ChangeDirection(key);

    if (key == 'E' || key == 'e') player1.dropItemToScreen(currentScreen->getScreenId());
    else if (key == 'O' || key == 'o') player2.dropItemToScreen(currentScreen->getScreenId());
}

std::vector<std::string> Game::initBuffer()
{
    std::vector<std::string> buffer;
    for (int i = 0; i < Screen::HEIGHT; ++i) {
        std::string line = currentScreen->getLine(i);
        if (line.length() < Screen::WIDTH) line.resize(Screen::WIDTH, ' ');
        buffer.push_back(line);
    }
    return buffer;
}

void Game::writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c)
{
    if (y >= 0 && y < (int)buffer.size() && x >= 0 && x < (int)buffer[y].size())
        buffer[y][x] = c;
}

void Game::writeHudText(std::vector<std::string>& buffer, int x, int y, const std::string& text)
{
    for (size_t i = 0; i < text.size(); ++i)
        writeToBuffer(buffer, x + (int)i, y, text[i]);
}

void Game::drawObjectsToBuffer(std::vector<std::string>& buffer)
{
    ScreenId currentId = currentScreen->getScreenId();
    for (const auto& obj : gameObjects) {
        if (!obj || obj->getScreenId() != currentId) continue;
        obj->drawToBuffer(buffer);
    }
}

void Game::drawPlayersToBuffer(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS ||
        currentScreen->getScreenId() == ScreenId::ROOM4) return;
    writeToBuffer(buffer, player1.getX(), player1.getY(), player1.getChar());
    writeToBuffer(buffer, player2.getX(), player2.getY(), player2.getChar());
}

void Game::drawStatusMessageAt(std::vector<std::string>& buffer, int x, int y, int maxWidth)
{
    if (statusMessage.empty() || maxWidth <= 0) return;
    std::string msg = statusMessage;
    if ((int)msg.length() > maxWidth) msg = msg.substr(0, maxWidth);
    std::string finalMsg = msg + std::string(maxWidth - msg.length(), ' ');
    writeHudText(buffer, x, y, finalMsg);
}

void Game::drawLegendToBuffer(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS ||
        currentScreen->getScreenId() == ScreenId::ROOM4) return;

    int startX = player1.getHudX() - 1;
    int y = player1.getHudY();
    const int LEGEND_WIDTH = 75;

    if (y - 1 < 0 || y + 1 >= Screen::HEIGHT) return;

    int actualWidth = (startX + LEGEND_WIDTH > Screen::WIDTH) ? Screen::WIDTH - startX : LEGEND_WIDTH;
    std::string border(actualWidth, '#');
    std::string clean(actualWidth - 2, ' ');

    writeHudText(buffer, startX, y - 1, border);
    writeHudText(buffer, startX, y + 1, border);
    writeToBuffer(buffer, startX, y, '#');
    writeHudText(buffer, startX + 1, y, clean);
    writeToBuffer(buffer, startX + actualWidth - 1, y, '#');

    writeHudText(buffer, player1.getHudX(), y, "p1 Hp:" + std::to_string(player1.getLive()) + " item:");
    char p1Icon = player1.hasItem() ? player1.getItemChar() : '_';
    writeToBuffer(buffer, player1.getHudX() + 13, y, p1Icon);

    writeHudText(buffer, player2.getHudX(), y, "p2 Hp:" + std::to_string(player2.getLive()) + " item:");
    char p2Icon = player2.hasItem() ? player2.getItemChar() : '_';
    writeToBuffer(buffer, player2.getHudX() + 13, y, p2Icon);

    int scoreX = player2.getHudX() + 16;
    writeHudText(buffer, scoreX, y, "score:" + std::to_string(player1.getScore() + player2.getScore()));

    int msgX = scoreX + 11;
    int maxMsgLen = (startX + actualWidth - 1) - msgX;
    drawStatusMessageAt(buffer, msgX, y, maxMsgLen);
}

void Game::drawRiddle(std::vector<std::string>& buffer)
{
    if (!currentRiddle) return;
    int contentHeight = 0, contentWidth = 0;

    for (int i = 0; i < Riddle::HEIGHT; ++i) {
        const char* line = currentRiddle->getLine(i);
        if (line == nullptr || line[0] == '\0') {}
        int len = (line) ? (int)strlen(line) : 0;
        if (len > 0) {
            contentHeight = i + 1;
            if (len > contentWidth) contentWidth = len;
        }
    }
    if (contentHeight == 0) return;

    int startX = (Screen::WIDTH - contentWidth) / 2;
    int startY = (Screen::HEIGHT - contentHeight) / 2;

    for (int y = 0; y < contentHeight; ++y) {
        const char* lineStr = currentRiddle->getLine(y);
        int len = (lineStr) ? (int)strlen(lineStr) : 0;
        for (int x = 0; x < contentWidth; ++x) {
            char c = ' ';
            if (x < len) c = lineStr[x];
            writeToBuffer(buffer, startX + x, startY + y, c);
        }
    }

    if (!statusMessage.empty()) {
        std::string msg = statusMessage;
        int msgLen = (int)msg.length();
        int msgX = (Screen::WIDTH - msgLen) / 2;
        int msgY = startY + contentHeight + 1;
        if (msgY < Screen::HEIGHT) writeHudText(buffer, msgX, msgY, msg);
    }
}

void Game::renderBuffer(const std::vector<std::string>& buffer)
{
    gotoxy(0, 0);
    int lastColor = -1;

    for (int y = 0; y < (int)buffer.size(); ++y)
    {
        const std::string& line = buffer[y];
        for (int x = 0; x < (int)line.length(); ++x)
        {
            char c = line[x];
            int requiredColor = resolveColor(c, x, y);
            if (requiredColor != lastColor) {
                setConsoleColor(requiredColor);
                lastColor = requiredColor;
            }
            std::cout << c;
        }
        std::cout << '\n';
    }
    if (lastColor != 7) setConsoleColor(7);
    std::cout.flush();
}

void Game::draw()
{
    if (currentMode == RunMode::Silent) return;
    auto buffer = initBuffer();
    drawObjectsToBuffer(buffer);
    drawPlayersToBuffer(buffer);
    applyLighting(buffer);
    drawLegendToBuffer(buffer);
    drawHomeMessage(buffer);
    if (RiddleMode && currentRiddle) drawRiddle(buffer);
    renderBuffer(buffer);
}

void Game::drawHomeMessage(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() != ScreenId::HOME) return;
    if (statusMessage.empty()) return;
    int msgLen = (int)statusMessage.length();
    int x = (Screen::WIDTH - msgLen) / 2;
    int y = 22;
    drawStatusMessageAt(buffer, x, y, msgLen + 5);
}

void Game::setConsoleColor(int colorCode)
{
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

int Game::getColorForChar(char c)
{
    switch (c) {
    case '#': return 8;  case '@': return 12; case '+': return 14;
    case '$': return 11; case '&': return 13; case '*': return 2;
    case 'K': return 14; case '!': return 12; case '-': case '|': return 4;
    case '/': case '\\': return 3;  case '?': return 9;
    case 'W': case 'w':case '_': return 10; case 'D': return 6;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': return 6;
    default: return 7;
    }
}

int Game::resolveColor(char c, int x, int y)
{
    if (!colorEnabled) return 7;
    bool isMenu = (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS ||
        currentScreen->getScreenId() == ScreenId::ROOM4);

    if (isMenu || (RiddleMode && currentRiddle != nullptr)) return 7;

    if (currentScreen->hasLegendDefined()) {
        Point legendPos = currentScreen->getLegendStart();
        int ly = legendPos.getY();
        int lx = legendPos.getX();
        if (y >= ly && y <= ly + 2 && x >= lx && x < lx + 75) return 7;
    }
    return getColorForChar(c);
}

void Game::setStatusMessage(const std::string& msg) { statusMessage = msg; }

void Game::applyLighting(std::vector<std::string>& buffer)
{
    if (!currentScreen->isDark()) return;
    const int BASE = 3;
    const int TORCH = 7;
    int p1x = player1.getX(); int p1y = player1.getY();
    int p2x = player2.getX(); int p2y = player2.getY();
    int r1 = player1.hasTorch() ? TORCH : BASE;
    int r2 = player2.hasTorch() ? TORCH : BASE;
    int r1s = r1 * r1;
    int r2s = r2 * r2;

    for (int y = 1; y < Screen::HEIGHT; ++y) {
        for (int x = 0; x < Screen::WIDTH; ++x) {
            int dx1 = x - p1x, dy1 = y - p1y;
            int dx2 = x - p2x, dy2 = y - p2y;
            int d1 = dx1 * dx1 + dy1 * dy1;
            int d2 = dx2 * dx2 + dy2 * dy2;
            if (d1 > r1s && d2 > r2s) buffer[y][x] = ' ';
        }
    }
}

void Game::checkLevelTransition()
{
    ScreenId t1 = player1.getCurrentLevel();
    ScreenId t2 = player2.getCurrentLevel();
    ScreenId real = currentScreen->getScreenId();

    if (t1 != real && t2 == real && player1.getX() != -1) {
        setStatusMessage("p1 change Room!");
        reportEvent("SCREEN_CHANGE", "P1 to " + std::to_string((int)t1));
        player1.updatepoint(-1, -1);
    }
    else if (t2 != real && t1 == real && player2.getX() != -1) {
        setStatusMessage("p2 change Room!");
        reportEvent("SCREEN_CHANGE", "P2 to " + std::to_string((int)t2));
        player2.updatepoint(-1, -1);
    }

    if (t1 != real && t2 != real && t1 == t2) {

        if (t1 == FINAL_LEVEL) {
            reportEvent("GAME_WON", "Finished Last Level");
        }
        playSound(523, 100); // Level Complete Chime (C)
        playSound(659, 100); // (E)
        playSound(784, 200); // (G)

        setStatusMessage("Level Complete! +200");
        player1.addScore(100);
        player2.addScore(100);
        player1.saveState();
        player2.saveState();
        goToScreen(t1);
    }
}

void Game::goToScreen(ScreenId id)
{
    currentScreen = &screens[(int)id];
    int legendBaseX = 0;
    int legendBaseY = 0;

    if (currentScreen->hasLegendDefined()) {
        Point lPos = currentScreen->getLegendStart();
        legendBaseX = lPos.getX();
        legendBaseY = lPos.getY();
        const int LEGEND_WIDTH = 75;
        for (int y = legendBaseY; y <= legendBaseY + 2; ++y) {
            for (int x = legendBaseX; x < legendBaseX + LEGEND_WIDTH; ++x) {
                if (y >= 0 && y < Screen::HEIGHT && x >= 0 && x < Screen::WIDTH) {
                    currentScreen->setChar(x, y, '#');
                }
            }
        }
    }

    int textRowY = legendBaseY + 1;
    player1.setHudPosition(legendBaseX + 1, textRowY);
    player2.setHudPosition(legendBaseX + 17, textRowY);

    Point start1 = currentScreen->getStartPos1();
    Point start2 = currentScreen->getStartPos2();
    player1.initForLevel(start1.getX(), start1.getY(), '$', id);
    player2.initForLevel(start2.getX(), start2.getY(), '&', id);

    setStatusMessage("");
    if (currentScreen->isDark()) setStatusMessage("Dark Room,Try Torch!");
    assignRiddlesToLevel();
}

void Game::ChangeDirection(char c)
{
    switch (std::tolower(c))
    {
    case 'w': player1.setDirection(Direction::UP);    break;
    case 'x': player1.setDirection(Direction::DOWN);  break;
    case 'a': player1.setDirection(Direction::LEFT);  break;
    case 'd': player1.setDirection(Direction::RIGHT); break;
    case 's': player1.setDirection(Direction::STAY);  break;

    case 'i': player2.setDirection(Direction::UP);    break;
    case 'm': player2.setDirection(Direction::DOWN);  break;
    case 'j': player2.setDirection(Direction::LEFT);  break;
    case 'l': player2.setDirection(Direction::RIGHT); break;
    case 'k': player2.setDirection(Direction::STAY);  break;
    }
}


void Game::handleRiddleInput(char key)
{
    if (!currentRiddle || !currentRiddlePlayer) return;

    if (key < '1' || key > '4') { setStatusMessage("Press 1-4"); return; }

    int selected = key - '0';
    Player& p = *currentRiddlePlayer;
    std::string pName = (&p == &player1) ? "P1" : "P2";

    if (selected == currentRiddle->getCorrectAnswer()) {
        reportEvent("RIDDLE", pName + " Correct");
        playSound(1000, 100); // Correct Sound
        setStatusMessage("Correct! +100 Points");

        currentRiddlePlayer->addScore(100);
        currentRiddle->removeFromGame();

        RiddleMode = false;
        currentRiddle = nullptr;
        currentRiddlePlayer = nullptr;
    }
    else {
        reportEvent("RIDDLE", pName + " Wrong");
        playSound(200, 300); // Error Sound
        p.decreaseLife();
        setStatusMessage("Wrong! You lost a life.");
        draw();
        if (p.getLive() <= 0) {
            RiddleMode = false;
            currentRiddle = nullptr;
            currentRiddlePlayer = nullptr;
        }
    }
}

void Game::stopMovement()
{
    player1.setDirection(Direction::STAY);
    player2.setDirection(Direction::STAY);
}

bool Game::checkPlayerHasRiddle()
{
    return player1.hasRiddle() || player2.hasRiddle();
}

const RiddleData* Game::getRiddleDataById(int id) const {
    for (const auto& rData : riddlesPool) {
        if (rData.id == id) {
            return &rData;
        }
    }
    return nullptr;
}

void Game::startRiddle(Riddle* riddle, Player& p)
{
    if (!riddle) return;
    std::string pName = (&p == &player1) ? "P1" : "P2";
    int riddleId = riddle->getData().id;
    reportEvent("RIDDLE_START", pName + " ID:" + std::to_string(riddleId));
    currentRiddle = riddle;
    currentRiddlePlayer = &p;
    RiddleMode = true;
    stopMovement();
    setStatusMessage("Riddle time! Press 1-4 ");
}

void Game::loadRiddlesFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file){ throw GameException("Missing Riddles File: " + filename, "Game::loadRiddlesFromFile"); return;}
    riddlesPool.clear();
    std::string line;
    RiddleData current;
    bool readingText = false;
    bool shouldShuffle = true;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.find("SHUFFLE:") != std::string::npos) {
            if (line.find("FALSE") != std::string::npos) shouldShuffle = false;
            continue;
        }
        if (line == "[RIDDLE]") {
            current = RiddleData();
            readingText = false;
        }
        else if (line.find("ID:") == 0) current.id = std::stoi(line.substr(3));
        else if (line.find("ANSWER:") == 0) current.correctAnswer = std::stoi(line.substr(7));
        else if (line == "TEXT") readingText = true;
        else if (line == "END_TEXT") {
            readingText = false;
            riddlesPool.push_back(current);
        }
        else if (readingText) current.textLines.push_back(line);
    }
    file.close();

    if (shouldShuffle) {
        unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(riddlesPool.begin(), riddlesPool.end(), std::default_random_engine(seed));
    }
    else {
        std::sort(riddlesPool.begin(), riddlesPool.end(),
            [](const RiddleData& a, const RiddleData& b) { return a.id < b.id; });
        std::reverse(riddlesPool.begin(), riddlesPool.end());
    }
}

void Game::assignRiddlesToLevel()
{
    for (GameObject* obj : gameObjects) {
        Riddle* riddle = dynamic_cast<Riddle*>(obj);
        if (riddle && riddle->getScreenId() == currentScreen->getScreenId()) {
            if (riddlesPool.empty()) loadRiddlesFromFile("Riddles.txt");
            if (!riddlesPool.empty()) {
                RiddleData data = riddlesPool.back();
                riddlesPool.pop_back();
                riddle->setData(data);
            }
        }
    }
}

void Game::pauseScreen()
{
    // חישוב המיקום למרכז המסך
    int x = (Screen::WIDTH - 37) / 2;
    int y = (Screen::HEIGHT - 6) / 2;

    // ציור המסגרת והתפריט
    gotoxy(x, y);     std::cout << "*************************************";
    gotoxy(x, y + 1); std::cout << "* GAME PAUSED                       *";
    gotoxy(x, y + 2); std::cout << "*                                   *";
    gotoxy(x, y + 3); std::cout << "* [ESC] Continue                    *";
    gotoxy(x, y + 4); std::cout << "* [S]   Save Current State          *";
    gotoxy(x, y + 5); std::cout << "* [H]   Exit to Main Menu           *";
    gotoxy(x, y + 6); std::cout << "*************************************";

    // לולאת הקלט של התפריט
    while (true) {
        char key = 0;

        // === 1. קריאת קלט (אוטומטי או ידני) ===
        if (currentMode == RunMode::Load || currentMode == RunMode::Silent) {
            key = getRecordedInput(); // שואב את המקש מההקלטה

            // הגנה: אם נגמרו הצעדים בקובץ ואין קלט, חייבים לצאת
            if (key == 0) {
                isRunning = false;
                return;
            }
        }
        else {
            if (_kbhit()) {
                key = _getch();
                // אם אנחנו במצב הקלטה, שומרים גם את הפעולות בתוך ה-Pause
                if (currentMode == RunMode::Save) {
                    recordInput(key);
                }
            }
        }

        if (key == 0) continue; // לא נלחץ כלום

        // === 2. לוגיקת מקשים ===

        // [ESC] חזרה למשחק
        if (key == 27) {
            return;
        }

        // [S] שמירת מצב
        if (key == 's' || key == 'S') {

            // הגנה: חסימת שמירה בזמן הקלטה כדי לא לשבור את רצף הצעדים
            if (currentMode == RunMode::Save) {
                gotoxy(x + 2, y + 2);
                std::cout << " Recording Mode: Save Disabled ";
                Sleep(1000);
                gotoxy(x + 2, y + 2);
                std::cout << "                               "; // מחיקת ההודעה
                continue;
            }

            try {
                saveGameState("savegame.sav");

                gotoxy(x + 2, y + 2);
                std::cout << "      Game Saved Successfully!     ";

                // בטעינה אוטומטית אין צורך להמתין
                if (currentMode != RunMode::Silent) {
                    Sleep(1000);
                }
                return; // חוזרים למשחק אחרי השמירה
            }
            catch (const std::exception& e) {
                setStatusMessage("Save Failed!");
            }
        }

        // [H] יציאה לתפריט ראשי
        if (key == 'h' || key == 'H') {

            reportEvent("GAME_ENDED", "User Quit to Menu");
            // יציאה רגילה לתפריט
            goToScreen(ScreenId::HOME);
            setStatusMessage("");
            return;
        }
    }
}

void Game::updateBombs()
{
    ScreenId curr_screen = currentScreen->getScreenId();
    for (auto obj : gameObjects) {
        if (pendingRestart || exitToMainMenu) return;
        auto bomb = dynamic_cast<Bomb*>(obj);
        if (!bomb || bomb->getScreenId() != curr_screen) continue;

        if (bomb->tick()) {
            std::string location = std::to_string(bomb->getX()) + "," + std::to_string(bomb->getY());
            reportEvent("BOMB_EXPLODE", location);
            playSound(150, 150); // Now async!
            Point c = bomb->getPoint();
            int radius = Bomb::getExplosionRadius();
            visualizeExplosion(c.getX(), c.getY(), radius);
            applyBombEffects(c.getX(), c.getY(), *currentScreen, radius);
            if (pendingRestart || exitToMainMenu) return;
            bomb->removeFromGame();
            setStatusMessage("BOOM!");
        }
    }
}

void Game::applyBombEffects(int cx, int cy, Screen& curr_screen, int R)
{
    for (int y = cy - R; y <= cy + R; ++y) {
        for (int x = cx - R; x <= cx + R; ++x) {
            if (!curr_screen.inBounds(x, y)) continue;
            explodeCell(x, y, curr_screen);
        }
    }
}

void Game::explodeCell(int x, int y, Screen& screen)
{
    if (player1.getX() == x && player1.getY() == y) player1.decreaseLife();
    if (player2.getX() == x && player2.getY() == y) player2.decreaseLife();

    bool isBorder = (x == 0 || x == Screen::WIDTH - 1 || y == 0 || y == Screen::HEIGHT - 1);
    bool isLegendArea = false;
    if (screen.hasLegendDefined()) {
        Point lPos = screen.getLegendStart();
        if (x >= lPos.getX() && x < lPos.getX() + Screen::LEGEND_WIDTH &&
            y >= lPos.getY() && y <= lPos.getY() + 2) {
            isLegendArea = true;
        }
    }

    if (!isBorder && !isLegendArea) {
        if (screen.getLine(y)[x] == '#') screen.setChar(x, y, ' ');
    }

    for (auto obj : gameObjects) {
        if (!obj || obj->getScreenId() != screen.getScreenId()) continue;
        if (obj->getX() < 0 || obj->getY() < 0) continue;
        if (obj->isCollected()) continue;

        if (obj->isAtPosition(x, y)) {
            if (auto key = dynamic_cast<Key*>(obj)) {
                gameOverScreen("Mission Failed: Key Destroyed!");
                return;
            }
            if (auto door = dynamic_cast<Door*>(obj)) {
                gameOverScreen("Mission Failed: Door Destroyed!");
                return;
            }
            if (dynamic_cast<Bomb*>(obj)) continue;
            if (obj->handleExplosionAt(x, y)) {
                obj->removeFromGame();
            }
        }
    }
}


void Game::safeDeleteObject(GameObject* objToRemove)
{
    // Deprecated in favor of cleanupDeadObjects, but kept for legacy
    if (!objToRemove) return;
    if (player1.isHoldingItem(objToRemove)) player1.forceDropItem();
    if (player2.isHoldingItem(objToRemove)) player2.forceDropItem();

    auto it = std::remove(gameObjects.begin(), gameObjects.end(), objToRemove);
    if (it == gameObjects.end()) return;

    gameObjects.erase(it, gameObjects.end());
    delete objToRemove;
}

void Game::visualizeExplosion(int cx, int cy, int radius)
{
    // === תיקון: במצב שקט אסור לצייר ואסור לישון ===
    if (currentMode == RunMode::Silent) return;
    // ==============================================

    for (int y = cy - radius; y <= cy + radius; ++y) {
        for (int x = cx - radius; x <= cx + radius; ++x) {
            if (x >= 0 && x < Screen::WIDTH && y >= 0 && y < Screen::HEIGHT) {
                gotoxy(x, y);
                if (colorEnabled) setConsoleColor(14);
                std::cout << "+";
                if (colorEnabled) setConsoleColor(7);
            }
        }
    }
    gotoxy(0, 0);
    Sleep(2 * TICK_MS);
}

void Game::processPlayerMovement(Player& p, Player* other)
{
    // חישוב מספר הצעדים בטיק אחד (תלוי במהירות/תעופה)
    int steps = p.isFlying() ? p.getSpeed() : 1;

    for (int i = 0; i < steps; ++i) {
        p.move(*currentScreen, gameObjects, other);

        // אם השחקן נחת או נעצר באמצע התעופה, מפסיקים את הלולאה הנוכחית
        if (!p.isFlying() && steps > 1) break;
    }
}

void Game::handleInteractions()
{
    if (RiddleMode) return; // אם כבר בחידה, לא עושים כלום

    // בדיקה האם שחקן 1 דרך על חידה
    if (player1.getInteractingRiddle() != nullptr) {
        startRiddle(player1.getInteractingRiddle(), player1);
        player1.clearInteractingRiddle();
    }
    // בדיקה האם שחקן 2 דרך על חידה
    else if (player2.getInteractingRiddle() != nullptr) {
        startRiddle(player2.getInteractingRiddle(), player2);
        player2.clearInteractingRiddle();
    }
}

void Game::checkGameStatus()
{
    // מתעלמים אם אנחנו כבר בתהליך יציאה או אתחול
    if (pendingRestart || exitToMainMenu) return;

    if (player1.getLive() <= 0) {
        gameOverScreen("Player 1 ran out of lives!");
    }
    else if (player2.getLive() <= 0) {
        gameOverScreen("Player 2 ran out of lives!");
    }
}

void Game::handleFlowControl()
{
    // אתחול שלב (Restart Level)
    if (pendingRestart) {
        pendingRestart = false;
        restartCurrentLevel();
        return;
    }

    // יציאה לתפריט ראשי
    if (exitToMainMenu) {
        exitToMainMenu = false;
        player1.stopMovement();
        player2.stopMovement();
        resetGame();
        goToScreen(ScreenId::HOME);
        return;
    }
}

void Game::checkPlaybackStatus()
{
    // בדיקה האם סיימנו לנגן את קובץ ההקלטה
    if ((currentMode == RunMode::Load || currentMode == RunMode::Silent) &&
        playbackIndex >= stepsBuffer.size())
    {
        if (currentMode == RunMode::Silent) {
            isRunning = false; // בטסט שקט יוצאים מיד
        }
        // בטעינה ויזואלית משאירים את המשחק פתוח
    }
}

GameObject* Game::createObjectFromSave(const std::string& type, std::stringstream& ss)
{
    int x, y;
    ss >> x >> y; // Always read position first (saved by GameObject base class)

    if (type == "LASER") {
        char original;
        int activeInt;
        // Read original char and active state (0 or 1)
        ss >> original >> activeInt;

        Laser* l = new Laser(x, y, original, currentScreen->getScreenId());

        // Restore state: if saved as 0 (inactive), simulate a signal to turn it off
        if (activeInt == 0) l->receiveSignal(true);
        return l;
    }
    else if (type == "SWITCH") {
        int active, count;
        ss >> active >> count; // Read state and number of targets

        Switch* s = new Switch(x, y, currentScreen->getScreenId());
        if (active) s->toggle(); // Restore active state

        // Note: Target coordinates are left in the stream and handled in loadGameState
        return s;
    }
    else if (type == "DOOR") {
        int id, target, locked;
        ss >> id >> target >> locked;

        // התיקון הגדול: נרמול ה-ID לתצוגה
        // אם ה-ID הוא 100, נחלק ב-100 ונקבל 1 -> יהפוך לתו '1'
        // אם ה-ID הוא 300, נחלק ב-100 ונקבל 3 -> יהפוך לתו '3'
        int visualDigit = id;
        if (visualDigit >= 100) {
            visualDigit /= 100;
        }

        char doorSymbol = (char)('0' + visualDigit);

        return new Door(x, y, doorSymbol, currentScreen->getScreenId(), id, (ScreenId)target, locked);
    }
    else if (type == "KEY") {
        int id;
        ss >> id;
        return new Key(x, y, 'K', currentScreen->getScreenId(), id);
    }
    else if (type == "BOMB") {
        int active, ticks;
        ss >> active >> ticks;
        Bomb* b = new Bomb(x, y, currentScreen->getScreenId());
        if (active) b->activate(); // Restore bomb timer if it was ticking
        return b;
    }
    else if (type == "TORCH") {
        return new Torch(x, y, 'T', currentScreen->getScreenId());
    }
    else if (type == "SPRING") {
        char dirC; int target, len;
        ss >> dirC >> target >> len;

        // Map char to Direction enum
        Direction d = Direction::UP;
        if (dirC == 'D') d = Direction::DOWN;
        else if (dirC == 'L') d = Direction::LEFT;
        else if (dirC == 'R') d = Direction::RIGHT;

        return new Spring(Point(x, y, 'W'), d, (ScreenId)target, len);
    }
    else if (type == "OBSTACLE") {
        int count;
        ss >> count;
        std::vector<Point> tiles;
        for (int i = 0; i < count; ++i) {
            int px, py;
            ss >> px >> py;
            tiles.push_back(Point(px, py, '*'));
        }
        return new Obstacle(tiles, currentScreen->getScreenId());
    }
    else if (type == "RIDDLE") {
        int riddleId;
        ss >> riddleId; // קריאת ה-ID ששמרנו

        Riddle* newRiddle = new Riddle(x, y, currentScreen->getScreenId());

        // אם יש ID תקין, אנחנו מחפשים את השאלה במאגר ומכניסים אותה לאובייקט
        if (riddleId != -1) {
            const RiddleData* foundData = getRiddleDataById(riddleId);
            if (foundData) {
                newRiddle->setData(*foundData);
            }
        }
        // אם לא מצאנו (או שה-ID הוא -1), החידה תישאר ריקה ונטפל בה 
        // אולי בפונקציה אחרת, אבל לרוב בשמירה יש כבר חידה פעילה.

        return newRiddle;
    }

    return nullptr;
}

void Game::saveGameState(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file) throw GameException("Failed to create save file", "Game::saveGameState");

    file << "SCREEN_ID " << (int)currentScreen->getScreenId() << std::endl;
    file << "SEED " << randomSeed << std::endl;
    file << "CYCLE " << cycleCounter << std::endl;

    file << "MAP_LAYOUT" << std::endl;
    for (const auto& line : currentScreen->getLayout()) {
        file << line << std::endl;
    }
    file << "END_MAP" << std::endl;

    // === תיקון בעיית המעבר בדלת ===
    // אנחנו שומרים ידנית את ה-ScreenID של כל שחקן ליד המידע שלו
    file << player1.getSaveData(1) << " " << (int)player1.getCurrentLevel() << std::endl;
    file << player2.getSaveData(2) << " " << (int)player2.getCurrentLevel() << std::endl;

    file << "OBJECTS_START" << std::endl;
    for (auto obj : gameObjects) {
        // שומרים רק אובייקטים של המסך הנוכחי (לפי השיטה שלך)
        if (obj->getScreenId() == currentScreen->getScreenId()) {
            file << obj->getTypeName() << " " << obj->getSaveData() << std::endl;
        }
    }
    file << "OBJECTS_END" << std::endl;

    file.close();
}

// מבנה עזר לחיבור מתגים (בתוך Game.cpp, לפני loadGameState)
struct PendingSwitch {
    Switch* s;
    std::vector<std::pair<int, int>> targets;
};

void Game::loadGameState(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) throw GameException("Failed to open save file", "Game::loadGameState");

    player1.forceDropItem();
    player2.forceDropItem();

    std::string line, cmd;
    int savedScreenId = -1;
    int p1ItemX = -1, p1ItemY = -1;
    int p2ItemX = -1, p2ItemY = -1;

    // רשימה לחיבור מתגים
    std::vector<PendingSwitch> switchLinks;

    while (file >> cmd)
    {
        if (cmd == "SCREEN_ID") {
            file >> savedScreenId;

            // 1. עדכון המסך הנוכחי
            currentScreen = &screens[savedScreenId];

            // === התיקון הקריטי: שחזור ה-Legend וה-HUD ===
            if (currentScreen->hasLegendDefined()) {
                Point lPos = currentScreen->getLegendStart();
                int legendBaseX = lPos.getX();
                int legendBaseY = lPos.getY();
                const int LEGEND_WIDTH = 75;

                // א. ציור מחדש של המסגרת (ליתר ביטחון, למקרה שהקובץ לא שמר אותה)
                for (int y = legendBaseY; y <= legendBaseY + 2; ++y) {
                    for (int x = legendBaseX; x < legendBaseX + LEGEND_WIDTH; ++x) {
                        if (y >= 0 && y < Screen::HEIGHT && x >= 0 && x < Screen::WIDTH) {
                            currentScreen->setChar(x, y, '#');
                        }
                    }
                }

                // ב. עדכון השחקנים איפה להדפיס את הטקסט!
                int textRowY = legendBaseY + 1;
                player1.setHudPosition(legendBaseX + 1, textRowY);
                player2.setHudPosition(legendBaseX + 17, textRowY);
            }
            // ==============================================

            // ניקוי אובייקטים מהמסך שנטען
            auto it = gameObjects.begin();
            while (it != gameObjects.end()) {
                if ((*it)->getScreenId() == (ScreenId)savedScreenId) {
                    delete* it;
                    it = gameObjects.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        else if (cmd == "SEED") { file >> randomSeed; srand(randomSeed); }
        else if (cmd == "CYCLE") { file >> cycleCounter; }
        else if (cmd == "MAP_LAYOUT") {
            std::vector<std::string> newLayout;
            std::getline(file, line);
            while (std::getline(file, line) && line != "END_MAP") {
                newLayout.push_back(line);
            }
            currentScreen->setLayout(newLayout);
        }
        // ... (המשך הקוד נשאר אותו דבר: PLAYER, OBJECTS וכו') ...
        else if (cmd == "PLAYER") {
            int id, x, y, lives, score, itemX, itemY, levelId;
            file >> id >> x >> y >> lives >> score >> itemX >> itemY >> levelId;
            Player* p = (id == 1) ? &player1 : &player2;
            p->initForLevel(x, y, (id == 1 ? '$' : '&'), (ScreenId)levelId);
            p->setLives(lives);
            p->setScore(score);
            if (id == 1) { p1ItemX = itemX; p1ItemY = itemY; }
            else { p2ItemX = itemX; p2ItemY = itemY; }
        }
        else if (cmd == "OBJECTS_START") {
            std::string type;
            while (file >> type && type != "OBJECTS_END") {
                std::string params;
                std::getline(file, params);
                std::stringstream ss(params);
                GameObject* newObj = createObjectFromSave(type, ss);
                if (newObj) {
                    gameObjects.push_back(newObj);
                    if (type == "SWITCH") {
                        Switch* s = dynamic_cast<Switch*>(newObj);
                        PendingSwitch ps; ps.s = s;
                        int tx, ty;
                        while (ss >> tx >> ty) ps.targets.push_back({ tx, ty });
                        switchLinks.push_back(ps);
                    }
                }
            }
        }
    }

    // חיבור מתגים וחפצים (כמו בקוד הקודם)
    for (auto& link : switchLinks) {
        for (auto& coord : link.targets) {
            GameObject* targetObj = findObjectAt(coord.first, coord.second);
            if (targetObj) link.s->addTarget(targetObj);
        }
    }
    if (p1ItemX != -1) { auto item = findObjectAt(p1ItemX, p1ItemY); if (item) { item->Collected(); player1.collectItem(item); } }
    if (p2ItemX != -1) { auto item = findObjectAt(p2ItemX, p2ItemY); if (item) { item->Collected(); player2.collectItem(item); } }

    file.close();
}

GameObject* Game::findObjectAt(int x, int y)
{
    for (auto obj : gameObjects) {
        if (obj->getX() == x && obj->getY() == y) {
            return obj;
        }
    }
    return nullptr;
}