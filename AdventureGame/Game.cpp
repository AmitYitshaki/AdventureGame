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
    drawLegendToBuffer(buffer);
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
    if (currentScreen->isDark()) setStatusMessage("Dark room... Try a Torch!");
    assignRiddlesToLevel();
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
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
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
