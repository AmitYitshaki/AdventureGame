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
    loadRiddlesFromFile("Riddles.txt");
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
    player1.resetStats();
    player2.resetStats();

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
    loadRiddlesFromFile("Riddles.txt");
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

void Game::writeHudText(std::vector<std::string>& buffer, int x, int y, const std::string& text)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        writeToBuffer(buffer, x + (int)i, y, text[i]);
    }
}

void Game::drawObjectsToBuffer(std::vector<std::string>& buffer)
{
    ScreenId currentId = currentScreen->getScreenId();

    for (auto obj : gameObjects)
    {
        if (!obj || obj->getScreenId() != currentId)
            continue;
		obj->drawToBuffer(buffer);
    }
}

void Game::drawPlayersToBuffer(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS)
        return;

    writeToBuffer(buffer, player1.getX(), player1.getY(), player1.getChar());
    writeToBuffer(buffer, player2.getX(), player2.getY(), player2.getChar());
}

void Game::drawStatusMessageAt(std::vector<std::string>& buffer, int x, int y, int maxWidth)
{
    if (statusMessage.empty() || maxWidth <= 0) return;

    std::string msg = statusMessage;

    // 1. חיתוך אם ארוך מדי
    if ((int)msg.length() > maxWidth) {
        msg = msg.substr(0, maxWidth);
    }
    
    // 2. ריפוד ברווחים (כדי למחוק הודעות קודמות ארוכות יותר)
    // ניצור מחרוזת באורך המקסימלי המותר, מלאה ברווחים, ונלביש עליה את ההודעה
    std::string finalMsg = msg + std::string(maxWidth - msg.length(), ' ');

    writeHudText(buffer, x, y, finalMsg);
}

void Game::drawLegendToBuffer(std::vector<std::string>& buffer)
{
    if (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS) return;

    // --- חישובים ---
    int startX = player1.getHudX() - 1;
    int y = player1.getHudY();
    const int LEGEND_WIDTH = 75;
    std::cout << "DEBUG: Drawing Legend at Y=" << y << std::endl;
    // הגנה מקריסה
    if (y - 1 < 0 || y + 1 >= Screen::HEIGHT)
    {
        std::cout << "DEBUG: Legend Y is out of bounds! Y=" << y << std::endl;
        return;
    }
    int actualWidth = (startX + LEGEND_WIDTH > Screen::WIDTH) ? Screen::WIDTH - startX : LEGEND_WIDTH;

    // ------------------------------------
    // 1. ציור המסגרת (Border)
    // ------------------------------------
    std::string border(actualWidth, '#');
    std::string clean(actualWidth - 2, ' ');

    writeHudText(buffer, startX, y - 1, border);     // למעלה
    writeHudText(buffer, startX, y + 1, border);     // למטה
    writeToBuffer(buffer, startX, y, '#');           // צד שמאל
    writeHudText(buffer, startX + 1, y, clean);      // ניקוי אמצע
    writeToBuffer(buffer, startX + actualWidth - 1, y, '#'); // צד ימין


    // ------------------------------------
    // 2. נתוני שחקנים (Player Stats)
    // ------------------------------------
    // שחקן 1
    writeHudText(buffer, player1.getHudX(), y,
        "p1 Hp:"+ std::to_string(player1.getLive()) + " item:");
    char p1Icon = player1.hasItem() ? player1.getItemChar() : '_';
    writeToBuffer(buffer, player1.getHudX() + 13, y, p1Icon);

    // שחקן 2
    writeHudText(buffer, player2.getHudX(), y,
        "p2 Hp:" + std::to_string(player2.getLive()) + " item:");
    char p2Icon = player2.hasItem() ? player2.getItemChar() : '_';
    writeToBuffer(buffer, player2.getHudX() + 13, y, p2Icon);

    // ניקוד
    int scoreX = player2.getHudX() + 16;
    writeHudText(buffer, scoreX, y,
        "score:" + std::to_string(player1.getScore() + player2.getScore()));

    // ------------------------------------
    // 3. הודעות (Status Message)
    // ------------------------------------
    // שימוש בפונקציית העזר האלגנטית שיצרנו!
    int msgX = scoreX + 11;
    int maxMsgLen = (startX + actualWidth - 1) - msgX;

    drawStatusMessageAt(buffer, msgX, y, maxMsgLen);
}

void Game::drawRiddle(std::vector<std::string>& buffer)
{
    if (!currentRiddle) return;

    // 1. חישוב הגודל האמיתי של החידה הנוכחית
    // אנחנו עוברים על כל השורות כדי למצוא את השורה הכי ארוכה ואת מספר השורות
    int contentHeight = 0;
    int contentWidth = 0;

    // נניח שיש מקסימום 20 שורות (הגבול ששמנו ב-h), נסרוק עד שנקבל שורה ריקה
    // או שפשוט נשתמש בידיעה שיש לנו וקטור בתוך RiddleData
    // אבל מכיוון שאין לנו גישה ישירה לווקטור מבחוץ, נשתמש ב-getLine בלולאה
    for (int i = 0; i < Riddle::HEIGHT; ++i)
    {
        const char* line = currentRiddle->getLine(i);
        if (line == nullptr || line[0] == '\0') {
            // אם הגענו לשורה ריקה והיא לא חלק מהעיצוב (בסוף), אפשר לעצור?
            // למען הפשטות, נניח שהחידה מסתיימת כשנגמר המידע בנתונים
            // הדרך הכי טובה היא להוסיף פונקציה getLineCount ל-Riddle, אבל נסתדר בלי:
        }

        // בדיקת אורך
        int len = (line) ? (int)strlen(line) : 0;
        if (len > 0) {
            contentHeight = i + 1; // הגובה הוא האינדקס האחרון שיש בו תוכן + 1
            if (len > contentWidth) contentWidth = len;
        }
    }

    // אם החידה ריקה משום מה, לא מציירים
    if (contentHeight == 0) return;

    // 2. חישוב מיקום מרכזי
    int startX = (Screen::WIDTH - contentWidth) / 2;
    int startY = (Screen::HEIGHT - contentHeight) / 2;

    // 3. ציור החידה (1 ל-1 כמו בקובץ)
    for (int y = 0; y < contentHeight; ++y)
    {
        const char* lineStr = currentRiddle->getLine(y);
        int len = (lineStr) ? (int)strlen(lineStr) : 0;

        for (int x = 0; x < contentWidth; ++x)
        {
            char c = ' '; // ברירת מחדל: רווח

            // אם אנחנו בתוך הטקסט של השורה
            if (x < len) {
                c = lineStr[x];
            }

            // כתיבה לבאפר (דורס את מה שמתחת - יוצר רקע אטום)
            writeToBuffer(buffer, startX + x, startY + y, c);
        }
    }

    // 4. ציור הודעת סטטוס (מתחת לחידה)
    // ככה זה לא עולה על הטקסט המקורי של החידה
    if (!statusMessage.empty())
    {
        std::string msg = statusMessage;
        int msgLen = (int)msg.length();

        // נמקם את ההודעה שורה אחת מתחת לחידה, במרכז
        int msgX = (Screen::WIDTH - msgLen) / 2;
        int msgY = startY + contentHeight + 1;

        // נוודא שלא חורגים מגובה המסך
        if (msgY < Screen::HEIGHT) {
            writeHudText(buffer, msgX, msgY, msg);
        }
    }
}

void Game::renderBuffer(const std::vector<std::string>& buffer)
{
    gotoxy(0, 0);

    // משתנה שזוכר מה הצבע האחרון שביקשנו מהמערכת
    // מתחילים מ-1- (ערך לא הגיוני) כדי לכפות שינוי בתו הראשון
    int lastColor = -1;

    for (int y = 0; y < (int)buffer.size(); ++y)
    {
        const std::string& line = buffer[y];
        for (int x = 0; x < (int)line.length(); ++x)
        {
            char c = line[x];

            // 1. חישוב הצבע הנדרש (פעולה מתמטית מהירה)
            int requiredColor = resolveColor(c, x, y);

            // 2. רק אם הצבע הנדרש שונה ממה שיש כרגע בפועל - קוראים למערכת ההפעלה
            if (requiredColor != lastColor)
            {
                setConsoleColor(requiredColor); // הפקודה הכבדה
                lastColor = requiredColor;      // מעדכנים את הזיכרון שלנו
            }

            std::cout << c;
        }
        std::cout << '\n';
    }

    // איפוס צבע בסוף ללבן
    if (lastColor != 7) setConsoleColor(7);

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

    // כל פונקציה אחראית לבדוק אם היא רלוונטית למסך הנוכחי
    drawObjectsToBuffer(buffer);
    drawPlayersToBuffer(buffer);
    applyLighting(buffer);
    drawLegendToBuffer(buffer);

    drawHomeMessage(buffer); // <--- השורה החדשה והנקייה

    if (RiddleMode && currentRiddle)
        drawRiddle(buffer);

    renderBuffer(buffer);

    if (isDebugMode())
        drawDebugDashboard();
}

void Game::drawHomeMessage(std::vector<std::string>& buffer)
{
    // אם זה לא מסך הבית - אין מה לעשות פה
    if (currentScreen->getScreenId() != ScreenId::HOME) return;

    // אם אין הודעה להציג - יוצאים
    if (statusMessage.empty()) return;

    // חישוב המיקום (ממורכז, שורה 22)
    int msgLen = (int)statusMessage.length();
    int x = (Screen::WIDTH - msgLen) / 2;
    int y = 22;

    // ציור ההודעה
    drawStatusMessageAt(buffer, x, y, msgLen + 5);
}
/* ============================================================
                          UPDATE
   ============================================================ */


void Game::updateBombs()
{
    ScreenId curr_screen = currentScreen->getScreenId();

    for (auto obj : gameObjects)
    {
        // בדיקת יציאת חירום - אם המשתמש ביקש ריסטארט, עוצרים מיד!
        if (pendingRestart || exitToMainMenu) return;

        auto bomb = dynamic_cast<Bomb*>(obj);
        if (!bomb) continue;
        if (bomb->getScreenId() != curr_screen) continue;

        if (bomb->tick())
        {
            playSound(150, 150);
            // ... (קוד הפיצוץ שלך) ...
            Point c = bomb->getPoint();
            int radius = Bomb::getExplosionRadius();
            visualizeExplosion(c.getX(), c.getY(), radius);
            applyBombEffects(c.getX(), c.getY(), *currentScreen, radius);

            // בדיקה נוספת אחרי הפיצוץ (למקרה שהתפוצץ מפתח וביקשנו ריסטארט)
            if (pendingRestart || exitToMainMenu) return;

            bomb->removeFromGame();
            setStatusMessage("BOOM!");
        }
    }
}

void Game::update()
{
    // חישוב צעדים (תעופה/הליכה)
    int steps1 = player1.isFlying() ? player1.getSpeed() : 1;
    int steps2 = player2.isFlying() ? player2.getSpeed() : 1;

    // תזוזת שחקן 1
    for (int i = 0; i < steps1; ++i) {
        player1.move(*currentScreen, gameObjects, &player2);
        if (!player1.isFlying()) break;
    }

    // תזוזת שחקן 2
    for (int i = 0; i < steps2; ++i) {
        player2.move(*currentScreen, gameObjects, &player1);
        if (!player2.isFlying()) break;
    }

    // עדכון קפיצים
    if (player1.isFlying()) player1.updateSpringEffect();
    if (player2.isFlying()) player2.updateSpringEffect();

    // בדיקת מעבר שלב
    if (currentScreen->getScreenId() != ScreenId::HOME &&
        currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
    {
        checkLevelTransition();
    }

    checkIsPlayerLoaded();

    // חידות
    if (!RiddleMode && checkPlayerHasRiddle()) {
        if (player1.hasRiddle()) startRiddle(player1.getHeldRiddle(), player1);
        else if (player2.hasRiddle()) startRiddle(player2.getHeldRiddle(), player2);
    }

    // פצצות (זה המקום שבו explodeCell נקראת ויכולה להפעיל Game Over על מפתח/דלת)
    updateBombs();

    if (!pendingRestart && !exitToMainMenu) { // רק אם עוד לא נקבע גורלנו
        if (player1.getLive() <= 0) gameOverScreen("Player 1 ran out of lives!");
        else if (player2.getLive() <= 0) gameOverScreen("Player 2 ran out of lives!");
    }

    // === הביצוע הבטוח בסוף הסיבוב ===

    if (pendingRestart)
    {
        pendingRestart = false; // מורידים את הדגל
        restartCurrentLevel();  // מבצעים את הריסטארט בבטחה
        return;
    }

    if (exitToMainMenu)
    {
        exitToMainMenu = false;
        player1.stopMovement();
        player2.stopMovement();
        resetGame(); // או goToScreen(HOME)
        return;
    }
}



/* ============================================================
                           INPUT
   ============================================================ */

void Game::handleInput()
{
    if (!_kbhit()) return;

    char key = _getch();

    if (key == '9') {
        setDebugMode(!isDebugMode());
        return;
    }

    // If riddle mode → restrict input
    if (RiddleMode && currentRiddle)
    {
        handleRiddleInput(key);
        return;
    }

    // ============================================================
    //                  MAIN MENU INPUT (HOME)
    // ============================================================
    if (currentScreen == &screens[(int)ScreenId::HOME])
    {
        if (key == '1') { resetGame(); }
        if (key == '2') { goToScreen(ScreenId::INSTRUCTIONS); return; }
        if (key == '3') { end(); return; }

        // --- Toggle Color ---
        if (key == 'c' || key == 'C') {
            toggleColor();
            setStatusMessage(isColorEnabled() ? "Color Mode: ON" : "Color Mode: OFF");
            draw(); // Force redraw to see changes immediately
            return;
        }

        // --- Toggle Sound ---
        if (key == 's' || key == 'S') {
            toggleSound();
            setStatusMessage(isSoundEnabled() ? "Sound Mode: ON" : "Sound Mode: OFF");
            // משמיעים צפצוף קטן לאישור אם הסאונד הופעל
            if (isSoundEnabled()) playSound(400, 100);
            return;
        }
    }

    // ============================================================
    //                  END GAME MENU (ROOM3)
    // ============================================================
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

    // Spring launching
    if (player1.isLoaded() && (key == 's' || key == 'S'))
    {
        playSound(600, 50);
        player1.launch(player1.getLoadedSpringLen());
        setStatusMessage("");
        return;
    }
    if (player2.isLoaded() && (key == 'k' || key == 'K'))
    {
        playSound(600, 50);
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

void Game::checkLevelTransition()
{
    ScreenId t1 = player1.getCurrentLevel();
    ScreenId t2 = player2.getCurrentLevel();
    ScreenId real = currentScreen->getScreenId();
 
    if (t1 != real && t2 == real){
        setStatusMessage("p1 change Room!");
	    player1.updatepoint(-1, -1); //move player off-screen to avoid multiple triggers
    }
    else if (t2 != real && t1 == real){
        setStatusMessage("p2 change Room!");
        player2.updatepoint(-1, -1); //move player off-screen to avoid multiple triggers
    }

    if (t1 != real && t2 != real && t1 == t2)
    {
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

    // --- חישוב ה-Legend ---
    int legendBaseX = 0;
    int legendBaseY = 0;

    if (currentScreen->hasLegendDefined())
    {
        Point lPos = currentScreen->getLegendStart();
        legendBaseX = lPos.getX();
        legendBaseY = lPos.getY();

        // =========================================================
        // === התוספת: חסימת אזור ה-Legend בקירות ===
        // =========================================================
        const int LEGEND_WIDTH = 75; // רוחב ה-Legend הקבוע

        // אנחנו חוסמים 3 שורות: השורה של ה-L (גג), שורת הטקסט, ושורת הרצפה
        // הלולאה רצה מ-legendBaseY (איפה שה-L) ועד שתי שורות מתחתיו
        for (int y = legendBaseY; y <= legendBaseY + 2; ++y)
        {
            for (int x = legendBaseX; x < legendBaseX + LEGEND_WIDTH; ++x)
            {
                // הגנה מחריגה (שלא נכתוב מחוץ לגבולות המערך)
                if (y >= 0 && y < Screen::HEIGHT && x >= 0 && x < Screen::WIDTH)
                {
                    // צורבים '#' לתוך זיכרון המפה.
                    // עכשיו הפונקציה isWall תחזיר true והשחקן ייעצר אוטומטית!
                    currentScreen->setChar(x, y, '#');
                }
            }
        }
        // =========================================================
    }

    // עדכון מיקום ה-HUD לשחקנים (זה נשמר בזיכרון של השחקן)
    int textRowY = legendBaseY + 1;
    player1.setHudPosition(legendBaseX + 1, textRowY);
    player2.setHudPosition(legendBaseX + 17, textRowY); // שמרתי על ה-17 שלך


    // --- אתחול השחקנים לחדר החדש ---
    Point start1 = currentScreen->getStartPos1();
    Point start2 = currentScreen->getStartPos2();

    // שימוש בפונקציה החדשה שלא דורסת את ה-HUD ולא מאפסת חיים
    player1.initForLevel(start1.getX(), start1.getY(), '$', id);
    player2.initForLevel(start2.getX(), start2.getY(), '&', id);

    setStatusMessage("");
    if (currentScreen->isDark()) setStatusMessage("Dark Room,Try Torch!");
    assignRiddlesToLevel();
}

void Game::restartCurrentLevel()
{
    ScreenId currentId = currentScreen->getScreenId();

    // 1. ניקוי אובייקטים קיימים
    for (auto obj : gameObjects) delete obj;
    gameObjects.clear();

    // 2. שחזור המצב המדויק מתחילת השלב (חיים וניקוד)
    player1.restoreState();
    player2.restoreState();

    // 3. טעינת המפה מחדש (כדי להחזיר מפתחות/דלתות שנהרסו)
    std::string filename;
    switch (currentId) {
    case ScreenId::ROOM1: filename = "Room1Screen.txt"; break;
    case ScreenId::ROOM2: filename = "Room2Screen.txt"; break;
    case ScreenId::ROOM3: filename = "Room3Screen.txt"; break;
    default: filename = "Room1Screen.txt"; break;
    }

    // יצירה מחדש של אובייקט המסך בזיכרון
    screens[(int)currentId] = Screen(currentId);
    LevelLoader::loadLevelFromFile(filename, screens[(int)currentId], gameObjects);

    RiddleMode = false;
    currentRiddle = nullptr;
    currentRiddlePlayer = nullptr;
    setStatusMessage("");

    // 4. חזרה למשחק ומיקום השחקנים מחדש
    goToScreen(currentId);

    // החזרת החידות אם צריך
    assignRiddlesToLevel();
}

void Game::gameOverScreen(const std::string& message)
{

    playSound(100, 400);
    // ניקוי מסך כדי לתת פוקוס להודעה
    system("cls");

    // חישוב מרכז המסך (בהנחה שרוחב המסך הוא 80)
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 9;
    int x = (Screen::WIDTH - BOX_WIDTH) / 2;
    int y = (Screen::HEIGHT - BOX_HEIGHT) / 2;

    // פונקציית עזר קטנה לציור (אפשר גם ידנית עם gotoxy)
    auto printCentered = [&](int rowOffset, const std::string& text) {
        int padding = (BOX_WIDTH - 2 - (int)text.length()) / 2;
        gotoxy(x, y + rowOffset);
        std::cout << "#" << std::string(padding, ' ') << text << std::string(BOX_WIDTH - 2 - padding - text.length(), ' ') << "#";
        };

    // --- ציור המסגרת ---
    gotoxy(x, y);     std::cout << std::string(BOX_WIDTH, '#'); // גג
    for (int i = 1; i < BOX_HEIGHT - 1; ++i) {
        gotoxy(x, y + i); std::cout << "#" << std::string(BOX_WIDTH - 2, ' ') << "#"; // דפנות
    }
    gotoxy(x, y + BOX_HEIGHT - 1); std::cout << std::string(BOX_WIDTH, '#'); // רצפה

    // --- תוכן ההודעה ---
    printCentered(2, "GAME OVER");
    printCentered(4, message); // סיבת המוות (למשל: "Player 1 Died")

    // --- האופציות ---
    printCentered(6, "[R] Restart Level    [H] Main Menu");

    // --- לולאת המתנה לקלט ---
    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();

            // אופציה 1: נסה שוב את השלב הנוכחי
            if (key == 'R' || key == 'r')
            {
                pendingRestart = true;
                return;
            }

            // אופציה 2: חזרה לתפריט הראשי
            else if (key == 'H' || key == 'h')
            {
                exitToMainMenu = true;
                return;
            }
        }
    }
}

/* ============================================================
                         RIDDLE SYSTEM
   ============================================================ */

void Game::loadRiddlesFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) return;

    riddlesPool.clear();
    std::string line;
    RiddleData current;
    bool readingText = false;
    bool shouldShuffle = true; // ברירת מחדל

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // בדיקת הגדרות
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

    // לוגיקת סדר
    if (shouldShuffle) {
        unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(riddlesPool.begin(), riddlesPool.end(), std::default_random_engine(seed));
    }
    else {
        // מיון עולה לפי ID
        std::sort(riddlesPool.begin(), riddlesPool.end(),
            [](const RiddleData& a, const RiddleData& b) { return a.id < b.id; });
        // הופכים כדי שהראשון יהיה בסוף הווקטור (לשליפה קלה)
        std::reverse(riddlesPool.begin(), riddlesPool.end());
    }
}

void Game::assignRiddlesToLevel()
{
    for (GameObject* obj : gameObjects)
    {
        Riddle* riddle = dynamic_cast<Riddle*>(obj);
        // אם זו חידה והיא שייכת למסך הנוכחי
        if (riddle && riddle->getScreenId() == currentScreen->getScreenId())
        {
            if (riddlesPool.empty()) {
                // אופציונלי: טעינה מחדש אם נגמר
                loadRiddlesFromFile("Riddles.txt");
            }

            if (!riddlesPool.empty()) {
                RiddleData data = riddlesPool.back(); // שליפה מהסוף
                riddlesPool.pop_back();               // מחיקה
                riddle->setData(data);                // הזרקה לאובייקט
            }
        }
    }
}

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
    if (!currentRiddle || !getCurrentRiddlePlayer())
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
        setStatusMessage("Correct! +100 Points");
		getCurrentRiddlePlayer()->addScore(100);
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

void Game::applyBombEffects(int cx, int cy, Screen& curr_screen, int R)
{
    for (int y = cy - R; y <= cy + R; ++y)
    {
        for (int x = cx - R; x <= cx + R; ++x)
        {
            if (!curr_screen.inBounds(x, y)) continue;
            explodeCell(x, y, curr_screen);
        }
    }
}


void Game::explodeCell(int x, int y, Screen& screen)
{
    // 1. פגיעה בשחקנים
    if (player1.getX() == x && player1.getY() == y) player1.decreaseLife();
    if (player2.getX() == x && player2.getY() == y) player2.decreaseLife();

    // 2. הרס קירות (עם הגנה על מסגרת ומקרא)
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
        if (screen.getLine(y)[x] == '#') {
            screen.setChar(x, y, ' '); // הופך לאוויר
        }
    }

    // 3. פגיעה באובייקטים - ותפיסת מצבי GAME OVER
    // אנו עוברים על המערך. שימו לב: אם מופעל Game Over, אנחנו יוצאים מיד מהפונקציה.
    for (auto obj : gameObjects)
    {
        if (!obj) continue;
        if (obj->getScreenId() != screen.getScreenId()) continue;
        if (obj->getX() < 0 || obj->getY() < 0) continue; // כבר מחוק
        if (obj->isCollected()) continue; // כבר נאסף

        // האם הפיצוץ פוגע באובייקט הזה?
        if (obj->isAtPosition(x, y))
        {
            // --- בדיקה קריטית 1: מפתח ---
            if (auto key = dynamic_cast<Key*>(obj))
            {
                // אם פוצצנו מפתח שלא נאסף -> הפסדנו
                gameOverScreen("Mission Failed: Key Destroyed!");
                return; // יוצאים מיד כדי לא להמשיך בלוגיקה על משחק שנגמר
            }

            // --- בדיקה קריטית 2: דלת ---
            if (auto door = dynamic_cast<Door*>(obj))
            {
                // אם פוצצנו דלת -> הפסדנו (כי שברנו את המעבר)
                gameOverScreen("Mission Failed: Door Destroyed!");
                return;
            }

            // הגנה: לא מפוצצים פצצות אחרות כדי למנוע לולאה אינסופית כרגע
            if (dynamic_cast<Bomb*>(obj)) continue;

            // --- טיפול רגיל: פיצוץ אויבים/מכשולים ---
            if (obj->handleExplosionAt(x, y))
            {
                obj->removeFromGame();
            }
        }
    }
}


void Game::visualizeExplosion(int cx, int cy, int radius)
{
    // לולאה שמכסה את ריבוע הפיצוץ
    for (int y = cy - radius; y <= cy + radius; ++y)
    {
        for (int x = cx - radius; x <= cx + radius; ++x)
        {
            if (x >= 0 && x < Screen::WIDTH && y >= 0 && y < Screen::HEIGHT)
            {
                gotoxy(x, y);

                // === התיקון: הפעלת צבע ידנית ===
                if (colorEnabled) setConsoleColor(14); // 14 = צהוב
                std::cout << "+";
                if (colorEnabled) setConsoleColor(7);  // החזרה ללבן
                // ==============================
            }
        }
    }

    gotoxy(0, 0);
    Sleep(150);
}

void Game::playSound(int frequency, int duration)
{
    if (!soundEnabled) return; // מכבד את הבקשה לשקט

    // Beep היא פונקציה חוסמת (עוצרת את המשחק). 
    // לכן נשתמש במשכים קצרים מאוד (למשל 50ms) כדי לא לתקוע את המשחקיות.
    // אופציה למתקדמים: להריץ בת'רד נפרד, אבל לקורס הזה זה לרוב Overkill.
    Beep(frequency, duration);
}

void Game::setConsoleColor(int colorCode)
{
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

int Game::getColorForChar(char c)
{
    switch (c)
    {
    case '#': return 8;  // קירות - אפור
    case '@': return 12; // פצצה - אדום בוהק
    case '+': return 14; // פיצוץ ויזואלי - צהוב
    case '$': return 11; // שחקן 1 - תכלת
    case '&': return 13; // שחקן 2 - ורוד/מגנטה
    case '*': return 2;  // מכשול - ירוק
    case 'K': return 14; // מפתח - צהוב
    case '!': return 12; // לפיד - אדום
    case '-': case '|': return 4; // לייזר - אדום כהה
    case '/': case '\\':return 3;  // מתג - כחול ירקרק
    case '?': return 9;  // חידה - כחול

        // === התיקונים החדשים ===
    case 'W': case 'w': return 10; // קפיץ (ראש וזנב) - ירוק בהיר
    case 'D': return 6;  // דלת (אם מצוירת כ-D)
    case '0': case '1': case '2': case '3': case '4': // דלתות וספירה לאחור
    case '5': case '6': case '7': case '8': case '9':
        return 6; // חום/כתום (או צבע אחר שתבחר לדלתות)

    default: return 7;   // שאר הדברים - לבן
    }
}

void Game::applyColor(char c, int y)
{
    // אם המשתמש כיבה צבעים - לא עושים כלום
    if (!colorEnabled) return;

    // 1. בדיקת מסכי תפריט (גישה ישירה למשתני המחלקה)
    bool isMenu = (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS);

    // 2. בדיקת חידה פעילה
    bool isRiddleActive = (RiddleMode && currentRiddle != nullptr);

    if (isMenu || isRiddleActive)
    {
        setConsoleColor(7); // לבן נקי
        return;
    }

    // 3. בדיקת Legend (דינאמית לפי השורה y)
    if (currentScreen->hasLegendDefined())
    {
        int legendStart = currentScreen->getLegendStart().getY();
        // אם השורה הנוכחית נמצאת בטווח ה-3 שורות של הלג'נד
        if (y >= legendStart && y < legendStart + 3)
        {
            setConsoleColor(7); // לבן נקי
            return;
        }
    }

    // 4. ברירת מחדל: צבעי משחק
    setConsoleColor(getColorForChar(c));
}

int Game::resolveColor(char c, int x, int y)
{
    // אם צבעים כבויים -> החזר לבן
    if (!colorEnabled) return 7;

    // 1. בדיקת מסכי תפריט
    bool isMenu = (currentScreen->getScreenId() == ScreenId::HOME ||
        currentScreen->getScreenId() == ScreenId::INSTRUCTIONS);

    // 2. בדיקת חידה פעילה
    bool isRiddleActive = (RiddleMode && currentRiddle != nullptr);

    if (isMenu || isRiddleActive)
    {
        return 7; // לבן נקי
    }

    // 3. בדיקת Legend (התיקון: בדיקת X ו-Y)
    if (currentScreen->hasLegendDefined())
    {
        Point legendPos = currentScreen->getLegendStart();
        int ly = legendPos.getY();
        int lx = legendPos.getX();

        // אנו מניחים רוחב מקסימלי של 75 ללג'נד.
        // אם ה-X וה-Y נופלים בתוך הריבוע הזה -> זה לג'נד -> לבן.
        if (y >= ly && y <= ly + 2 && x >= lx && x < lx + 75)
        {
            return 7; // לבן נקי
        }
    }

    // 4. אחרת: צבעי משחק רגילים
    return getColorForChar(c);
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
