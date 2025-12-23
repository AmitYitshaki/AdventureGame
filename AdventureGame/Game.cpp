#include "Game.h"
#include <conio.h>
#include <iostream>
#include <cstdlib>

// Static HUD message
std::string Game::statusMessage = "";

/* ============================================================
                        CONSTRUCTOR
   ============================================================ */

Game::Game()
    : player1(0, 0, '$', 16), // מאתחלים עם ערכים זמניים, הטעינה תדרוס אותם
    player2(0, 0, '&', 35),
    isRunning(true),
    currentScreen(nullptr),
    debugMode(false)
{
    system("mode con: cols=80 lines=50");
    initGame();
}

/* ============================================================
			        DESTRUCTOR & RESET
   ============================================================ */

Game::~Game()
{
    for (auto obj : gameObjects)
        delete obj;

    gameObjects.clear();
}

void Game::initGame()
{
    // טעינת מסך הבית
    screens[(int)ScreenId::HOME] = Screen(ScreenId::HOME);
    LevelLoader::loadScreenFromFile("StartScreen.txt", screens[(int)ScreenId::HOME]);

    // טעינת מסך הוראות (כדי שאפשר יהיה לצפות בהן לפני שמתחילים לשחק)
    screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
    LevelLoader::loadScreenFromFile("InstructionsScreen.txt", screens[(int)ScreenId::INSTRUCTIONS]);

    currentScreen = &screens[(int)ScreenId::HOME];
}

void Game::resetGame()
{
    // 1. איפוס משתנים לוגיים
    RiddleMode = false;
    currentRiddle = nullptr;
    setStatusMessage("");

    // 2. מחיקת אובייקטים ישנים (חשוב מאוד!)
    for (auto obj : gameObjects) delete obj;
    gameObjects.clear();

    // 3. איפוס נתוני שחקנים (חיים, ניקוד וכו') - אם יש לך פונקציה כזו ב-Player
    player1.resetLives(); // הנחה: הוספת פונקציה כזו
    player2.resetLives();
	player1.removeHeldItem();
	player2.removeHeldItem();

    // 4. טעינת שלבי המשחק "טריים" מהקבצים
    // אנחנו דורסים את מה שהיה בזיכרון בטעינה חדשה

    // ROOM 1
    screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
    LevelLoader::loadLevelFromFile("Room1Screen.txt", screens[(int)ScreenId::ROOM1], gameObjects);

    // ROOM 2
    screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
    LevelLoader::loadLevelFromFile("Room2Screen.txt", screens[(int)ScreenId::ROOM2], gameObjects);

    // ROOM 3
    screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);
    LevelLoader::loadLevelFromFile("Room3Screen.txt", screens[(int)ScreenId::ROOM3], gameObjects);

    // 5. התחלת המשחק בפועל!
    // מכיוון שלחצנו "Start New Game", אנחנו רוצים לעבור ישר לחדר 1
    goToScreen(ScreenId::ROOM1);
}

/* ============================================================
                        DRAW SYSTEM
   ============================================================ */

std::vector<std::string> Game::initBuffer()
{
    std::vector<std::string> buffer;

    for (int i = 0; i < Screen::HEIGHT; ++i)
    {
        std::string line = currentScreen->getLine(i);

        if (line.length() < Screen::WIDTH)
            line.resize(Screen::WIDTH, ' ');

        buffer.push_back(line);
    }
    return buffer;
}

void Game::writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c)
{
    if (y >= 0 && y < (int)buffer.size() &&
        x >= 0 && x < (int)buffer[y].size())
    {
        buffer[y][x] = c;
    }
}

void Game::writeTextToBuffer(std::vector<std::string>& buffer, int x, int y, const std::string& text)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        // רק אם התו אינו רווח, אנחנו דורסים את מה שמתחתיו
        // זה מאפשר לראות את הרצפה בין המילים
        if (text[i] != ' ') {
            writeToBuffer(buffer, x + (int)i, y, text[i]);
        }
    }
}

void Game::drawObjectsToBuffer(std::vector<std::string>& buffer)
{
    ScreenId currentId = currentScreen->getScreenId();

    for (auto obj : gameObjects)
    {
        if (!obj || obj->getScreenId() != currentId)
            continue;

        if (auto spring = dynamic_cast<Spring*>(obj))
            spring->drawToBuffer(buffer);
        else if (auto obstacle = dynamic_cast<Obstacle*>(obj))
            obstacle->drawToBuffer(buffer);
        else
            writeToBuffer(buffer, obj->getX(), obj->getY(), obj->getChar());
    }
}

void Game::drawPlayersToBuffer(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS)
        return;

    writeToBuffer(buffer, player1.getX(), player1.getY(), player1.getChar());
    writeToBuffer(buffer, player2.getX(), player2.getY(), player2.getChar());

    // --- עיצוב חדש וקומפקטי ל-HUD ---
    // P1: HP:3 Key
    std::string p1Text = "P1: HP:" + std::to_string(player1.getLive());
    if (player1.hasItem()) {
        p1Text += " Itm:";
        p1Text += player1.getItemChar();
    }

    std::string p2Text = "P2: HP:" + std::to_string(player2.getLive());
    if (player2.hasItem()) {
        p2Text += " Itm:";
        p2Text += player2.getItemChar();
    }

    writeTextToBuffer(buffer, player1.getHudX(), player1.getHudY(), p1Text);
    writeTextToBuffer(buffer, player2.getHudX(), player2.getHudY(), p2Text);
}

void Game::drawStatusToBuffer(std::vector<std::string>& buffer)
{
    if (!statusMessage.empty())
        writeTextToBuffer(buffer, 20, 0, statusMessage); // מיקום מרכזי יותר
}

void Game::drawRiddle(std::vector<std::string>& buffer)
{
    if (!currentRiddle)
        return;

    const int w = Riddle::WIDTH;
    const int h = Riddle::HEIGHT;

    int startX = (Screen::WIDTH - w) / 2;
    int startY = (Screen::HEIGHT - h) / 2;

    for (int y = 0; y < h; ++y)
    {
        const char* line = currentRiddle->getLine(y);
        if (!line) continue;

        for (int x = 0; x < w && line[x] != '\0'; ++x)
        {
            int bx = startX + x;
            int by = startY + y;

            if (bx >= 0 && bx < Screen::WIDTH &&
                by >= 0 && by < Screen::HEIGHT)
            {
                buffer[by][bx] = line[x];
            }
        }
    }
}

void Game::renderBuffer(const std::vector<std::string>& buffer)
{
    gotoxy(0, 0);
    for (const auto& line : buffer)
        std::cout << line << '\n';

    std::cout.flush();
}

void Game::drawDebugDashboard()
{
    // מיקום: שורה 26 (מתחת למסך המשחק)
    int y = Screen::HEIGHT + 1;

    // שורה 1: נתוני שחקנים + חדר
    gotoxy(0, y);
    std::cout << "P1:" << Logger::getPlayerInfo(player1) << " | "
        << "P2:" << Logger::getPlayerInfo(player2) << " | "
        << Logger::getScreenInfo(*currentScreen) << std::string(20, ' '); // רווחים לניקוי שאריות

    // שורה 2: אובייקטים בחדר (בשורה אחת ארוכה)
    gotoxy(0, y + 1);
    std::cout << "Objs: ";

    std::vector<std::string> objs = Logger::getScreenObjectsList(gameObjects, currentScreen->getScreenId());

    if (objs.empty()) {
        std::cout << "None";
    }
    else {
        for (const auto& s : objs) {
            std::cout << s << " "; // מדפיס אובייקטים ברצף: Dr#1(L)(10,10) Ky#2(5,5) ...
        }
    }

    // מנקה את שאר השורה למקרה שנשארו שאריות מאובייקטים קודמים
    std::cout << std::string(40, ' ');

    // החזרת סמן להתחלה למניעת בעיות
    gotoxy(0, 0);
}

void Game::draw()
{
    auto buffer = initBuffer();

    drawObjectsToBuffer(buffer);
    drawPlayersToBuffer(buffer);
    drawStatusToBuffer(buffer);
    applyLighting(buffer);

    if (RiddleMode && currentRiddle)
        drawRiddle(buffer);

    renderBuffer(buffer);

    if (isDebugMode())
        drawDebugDashboard();
}

/* ============================================================
                          UPDATE
   ============================================================ */

void Game::update()
{
    // --- תיקון: שימוש במהירות האמיתית של השחקן (תומך בקפיץ) ---
    int steps1 = player1.isFlying() ? player1.getSpeed() : 1;
    int steps2 = player2.isFlying() ? player2.getSpeed() : 1;

    for (int i = 0; i < steps1; ++i) {
        player1.move(*currentScreen, gameObjects);
        if (!player1.isFlying()) break;
    }
    for (int i = 0; i < steps2; ++i) {
        player2.move(*currentScreen, gameObjects);
        if (!player2.isFlying()) break;
    }

    if (player1.isFlying()) player1.updateSpringEffect();
    if (player2.isFlying()) player2.updateSpringEffect();

    if (currentScreen->getScreenId() != ScreenId::HOME &&
        currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
        checkLevelTransition();

    checkIsPlayerLoaded();

    if (!RiddleMode && checkPlayerHasRiddle()) {
        if (player1.hasRiddle()) startRiddle(player1.getHeldRiddle(), player1);
        else if (player2.hasRiddle()) startRiddle(player2.getHeldRiddle(), player2);
    }
}

/* ============================================================
                           INPUT
   ============================================================ */

void Game::handleInput()
{
    if (!_kbhit()) return;

    char key = _getch();

    if (key == '9'){
        setDebugMode(!isDebugMode());
        return;
    }

    // If riddle mode → restrict input
    if (RiddleMode && currentRiddle)
    {
        handleRiddleInput(key);
        return;
    }

    // Main Menu input
    if (currentScreen == &screens[(int)ScreenId::HOME])
    {
        if (key == '1') { resetGame(); }
        if (key == '2') { goToScreen(ScreenId::INSTRUCTIONS); return; }
        if (key == '3') { end(); return; }
    }
    //end screen (ROOM3) menu handlig
    if (currentScreen == &screens[(int)ScreenId::ROOM3]) {
        if (key == '1') {           // Play Again (New Game)
            resetGame();
        }
        else if (key == '2') {      // Return to Main Menu
            goToScreen(ScreenId::HOME);
            setStatusMessage("");
        }
        else if (key == '3') {      // Quit Game
            end();
        }
        // Ignore any other keys in ROOM3
        return;
    }

    // Instructions → return to home
    if (currentScreen == &screens[(int)ScreenId::INSTRUCTIONS] &&
        (key == 'h' || key == 'H'))
    {
        goToScreen(ScreenId::HOME);
        return;
    }

    // ESC → pause
    if (key == 27)
    {
        pauseScreen();
        return;
    }

    // Spring launching (STAY)
    if (player1.isLoaded() && (key == 's' || key == 'S'))
    {
        player1.launch(player1.getLoadedSpringLen());
        setStatusMessage("");
        return;
    }
    if (player2.isLoaded() && (key == 'k' || key == 'K'))
    {
        player2.launch(player2.getLoadedSpringLen());
        setStatusMessage("");
        return;
    }

    // Movement
    ChangeDirection(key);

    // Item drop
    if (key == 'E' || key == 'e')
        player1.dropItemToScreen(currentScreen->getScreenId());
    else if (key == 'O' || key == 'o')
        player2.dropItemToScreen(currentScreen->getScreenId());
}

void Game::ChangeDirection(char c)
{
    // Player 1
    if (c == 'w' || c == 'W') player1.setDirection(Direction::UP);
    else if (c == 'x' || c == 'X') player1.setDirection(Direction::DOWN);
    else if (c == 'a' || c == 'A') player1.setDirection(Direction::LEFT);
    else if (c == 'd' || c == 'D') player1.setDirection(Direction::RIGHT);
    else if (c == 's' || c == 'S') player1.setDirection(Direction::STAY);

    // Player 2
    else if (c == 'i' || c == 'I') player2.setDirection(Direction::UP);
    else if (c == 'm' || c == 'M') player2.setDirection(Direction::DOWN);
    else if (c == 'j' || c == 'J') player2.setDirection(Direction::LEFT);
    else if (c == 'l' || c == 'L') player2.setDirection(Direction::RIGHT);
    else if (c == 'k' || c == 'K') player2.setDirection(Direction::STAY);
}

void Game::setStatusMessage(const std::string& msg)
{
    statusMessage = msg;
}

void Game::setDebugMode(bool adminUse)
{
    debugMode = adminUse;

    // If debug turned off, clear the debug dashboard lines we previously drew
    if (!debugMode)
    {
        int y = Screen::HEIGHT + 1; // dashboard top row
        for (int i = 0; i < 3; ++i) // clear 3 rows (same area used by drawDebugDashboard)
        {
            gotoxy(0, y + i);
            std::cout << std::string(Screen::WIDTH, ' ');
        }
        // restore cursor to top-left to avoid drawing artifacts
        gotoxy(0, 0);
        std::cout.flush();
    }
}

/* ============================================================
                         LIGHTING SYSTEM
   ============================================================ */

void Game::applyLighting(std::vector<std::string>& buffer)
{
    if (!currentScreen->isDark())
        return;

    const int BASE = 3;
    const int TORCH = 7;

    int p1x = player1.getX();
    int p1y = player1.getY();
    int p2x = player2.getX();
    int p2y = player2.getY();

    int r1 = player1.hasTorch() ? TORCH : BASE;
    int r2 = player2.hasTorch() ? TORCH : BASE;

    int r1s = r1 * r1;
    int r2s = r2 * r2;

    for (int y = 1; y < Screen::HEIGHT; ++y)
    {
        for (int x = 0; x < Screen::WIDTH; ++x)
        {
            int dx1 = x - p1x, dy1 = y - p1y;
            int dx2 = x - p2x, dy2 = y - p2y;

            int d1 = dx1 * dx1 + dy1 * dy1;
            int d2 = dx2 * dx2 + dy2 * dy2;

            if (d1 > r1s && d2 > r2s)
                buffer[y][x] = ' ';
        }
    }
}

/* ============================================================
                    LEVEL TRANSITION SYSTEM
   ============================================================ */

void Game::resetPlayersForNewLevel()
{
    player1.updatepoint(2, 12);
    player2.updatepoint(3, 12);

    player1.setDirection(Direction::STAY);
    player2.setDirection(Direction::STAY);

    ScreenId currentId = currentScreen->getScreenId();
    player1.setCurrentLevel(currentId);
    player2.setCurrentLevel(currentId);

     //show message for dark rooms
    if (currentScreen->isDark())
    {
        setStatusMessage("This room is dark... Try a torch!");
    }
    else
    {
        setStatusMessage("");
    }
}


void Game::checkLevelTransition()
{
    ScreenId t1 = player1.getCurrentLevel();
    ScreenId t2 = player2.getCurrentLevel();
    ScreenId real = currentScreen->getScreenId();
 
    if (t1 != real && t2 == real){
        setStatusMessage("Player 1 witting in next Room!");
	    player1.updatepoint(-1, -1); //move player off-screen to avoid multiple triggers
    }
    else if (t2 != real && t1 == real){
        setStatusMessage("Player 2 witting in next Room!");
        player2.updatepoint(-1, -1); //move player off-screen to avoid multiple triggers
    }

    if (t1 != real && t2 != real && t1 == t2)
    {
        goToScreen(t1);
        resetPlayersForNewLevel();
    }
}

void Game::goToScreen(ScreenId id)
{
    // 1. עדכון המסך הנוכחי
    currentScreen = &screens[(int)id];

    if (currentScreen->hasLegendDefined())
    {
        Point legendPos = currentScreen->getLegendStart();

        player1.setHudPosition(legendPos.getX(), legendPos.getY());

        player2.setHudPosition(legendPos.getX() + 20, legendPos.getY());
    }
    else
    {
        // ברירת מחדל למסכים ללא L (כמו תפריט ראשי)
        player1.setHudPosition(0, 0);
        player2.setHudPosition(0, 1);
    }

    // 2. שליפת נקודות ההתחלה שה-LevelLoader שמר בתוך המסך
    Point start1 = currentScreen->getStartPos1();
    Point start2 = currentScreen->getStartPos2();

    // 3. איפוס השחקנים למיקום החדש (כולל ה-HUD שלהם)
    // שים לב: אנחנו שולחים את id בתור המסך הנוכחי שלהם
    player1.resetForNewGame(start1.getX(), start1.getY(), '$', player1.getHudX(), id);
    player2.resetForNewGame(start2.getX(), start2.getY(), '&', player2.getHudX(), id);

    // 4. ניקוי הודעות
    setStatusMessage("");

    // בונוס: אם המסך חשוך, נעדכן הודעה
    if (currentScreen->isDark()) {
        setStatusMessage("It's dark here... you need a Torch!");
    }
}

/* ============================================================
                         RIDDLE SYSTEM
   ============================================================ */

bool Game::checkPlayerHasRiddle()
{
    return player1.hasRiddle() || player2.hasRiddle();
}

void Game::startRiddle(Riddle* riddle, Player& p)
{
    if (!riddle) return;

    currentRiddle = riddle;
    currentRiddlePlayer = &p;
    RiddleMode = true;

    stopMovement();
    setStatusMessage("Riddle time! Press 1-4 to answer.");
}

void Game::handleRiddleInput(char key)
{
    if (!currentRiddle || !currentRiddlePlayer)
        return;

    if (key < '1' || key > '4')
    {
        setStatusMessage("Press 1-4");
        return;
    }

    int selected = key - '0';
    Player& p = *currentRiddlePlayer;

    if (selected == currentRiddle->getCorrectAnswer())
    {
        setStatusMessage("Correct!");
        p.removeHeldItem();

        RiddleMode = false;
        currentRiddle = nullptr;
        currentRiddlePlayer = nullptr;
    }
    else
    {
        p.decreaseLife();
        setStatusMessage("Wrong! You lost a life.");
        draw();

        if (p.getLive() <= 0)
        {
            RiddleMode = false;
            currentRiddle = nullptr;
            currentRiddlePlayer = nullptr;
        }
    }
}

/* ============================================================
                         UTILITY
   ============================================================ */

void Game::stopMovement()
{
    player1.setDirection(Direction::STAY);
    player2.setDirection(Direction::STAY);
}

void Game::checkIsPlayerLoaded()
{
    if (player1.isLoaded() || player2.isLoaded())
        setStatusMessage("Spring ready! Press STAY.");
}

void Game::pauseScreen()
{
    int x = (Screen::WIDTH - 37) / 2;
    int y = (Screen::HEIGHT - 5) / 2;

    gotoxy(x, y);     std::cout << "*************************************";
    gotoxy(x, y + 1); std::cout << "* Game Paused                       *";
    gotoxy(x, y + 2); std::cout << "* Press ESC to continue             *";
    gotoxy(x, y + 3); std::cout << "* Press H to open Home Screen       *";
    gotoxy(x, y + 4); std::cout << "*************************************";

    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();
            if (key == 27) return;
            if (key == 'h' || key == 'H')
            {
                goToScreen(ScreenId::HOME);
                setStatusMessage("");
                return;
            }
        }
    }
}

/* ============================================================
                         MAIN LOOP
   ============================================================ */

void Game::start()
{
    hideCursor();

    while (isRunning)
    {
        handleInput();
        update();
        draw();
        Sleep(TICK_MS);
    }

    cls();
}
