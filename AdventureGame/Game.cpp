#include "Game.h"
#include "Riddle.h"
#include <conio.h>
#include <iostream>

// Static HUD message
std::string Game::statusMessage = "";

/* ============================================================
                        CONSTRUCTOR
   ============================================================ */

Game::Game()
    : player1(2, 12, '$', 16),
    player2(3, 12, '&', 35),
    isRunning(true)
{
    // Initialize screens
    screens[(int)ScreenId::HOME] = Screen(ScreenId::HOME);
    screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
    screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
    screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
    screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);

    screens[(int)ScreenId::ROOM2].setDark(true); // ROOM2 starts dark

    currentScreen = &screens[(int)ScreenId::HOME];

    // Initialize objects
    gameObjects = {
        new Key(15, 20, 'K', ScreenId::ROOM1, 1),
        new Riddle(30, 5, ScreenId::ROOM1, RiddleId::RIDDLE1),
        new Spring(Point(4,15,'w'), Point(3,15,'w'), Point(2,15,'W'), Direction::RIGHT, ScreenId::ROOM1),
        new Spring(Point(76,17,'w'),Point(77,17,'w'),Point(78,17,'W'),Direction::LEFT, ScreenId::ROOM1),
        new Door(79,12,'1', ScreenId::ROOM1, 1, ScreenId::ROOM2, true),
        new Key(20,15,'K', ScreenId::ROOM2, 2),
        new Torch(15,10,'!', ScreenId::ROOM2),
        new Torch(14,10,'!', ScreenId::ROOM2),
        new Riddle(77,12, ScreenId::ROOM2, RiddleId::RIDDLE2),
        new Door(79,12,'2', ScreenId::ROOM2, 2, ScreenId::ROOM3, true)
    };
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

void Game::resetGame()
{
    // 1. Reset global game state
    RiddleMode = false;
    currentRiddle = nullptr;
    currentRiddlePlayer = nullptr;
    setStatusMessage("");

    // 2. Delete old objects and rebuild them
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    // Re-init screens (same as in constructor)
    screens[(int)ScreenId::HOME] = Screen(ScreenId::HOME);
    screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
    screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
    screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
    screens[(int)ScreenId::ROOM2].setDark(true);
    screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);

    // Re-create all game objects (same list as in ctor)
    gameObjects = {
        new Key(15, 20, 'K', ScreenId::ROOM1, 1),
        new Riddle(30, 5, ScreenId::ROOM1, RiddleId::RIDDLE1),
        new Spring(Point(4, 15, 'w'), Point(3, 15, 'w'), Point(2, 15, 'W'), Direction::RIGHT, ScreenId::ROOM1),
        new Spring(Point(76, 17, 'w'), Point(77, 17, 'w'), Point(78, 17, 'W'), Direction::LEFT,  ScreenId::ROOM1),
        new Door(79, 12, '1', ScreenId::ROOM1, 1, ScreenId::ROOM2, true),
        new Key(20, 15, 'K', ScreenId::ROOM2, 2),
        new Torch(15, 10, '!', ScreenId::ROOM2),
        new Torch(14, 10, '!', ScreenId::ROOM2),
        new Riddle(77, 12, ScreenId::ROOM2, RiddleId::RIDDLE2),
        new Door(79, 12, '2', ScreenId::ROOM2, 2, ScreenId::ROOM3, true)
    };

    // 3. Reset players
    player1.resetForNewGame(2, 12, '$', 16, ScreenId::ROOM1);
    player2.resetForNewGame(3, 12, '&', 35, ScreenId::ROOM1);

    // 4. Start on ROOM1
    currentScreen = &screens[(int)ScreenId::ROOM1];
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
        writeToBuffer(buffer, x + (int)i, y, text[i]);
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

    writeToBuffer(buffer, player1.getHudX() - 7, player1.getHudY(), player1.getLive() + '0');
    writeToBuffer(buffer, player2.getHudX() - 7, player2.getHudY(), player2.getLive() + '0');
}

void Game::drawStatusToBuffer(std::vector<std::string>& buffer)
{
    if (!statusMessage.empty())
        writeTextToBuffer(buffer, 45, 0, statusMessage);
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
}

/* ============================================================
                          UPDATE
   ============================================================ */

void Game::update()
{
    int steps1 = player1.isFlying() ? 3 : 1;
    int steps2 = player2.isFlying() ? 3 : 1;

    // Player 1
    for (int i = 0; i < steps1; ++i)
    {
        player1.move(*currentScreen, gameObjects);
        if (!player1.isFlying()) break;
    }

    // Player 2
    for (int i = 0; i < steps2; ++i)
    {
        player2.move(*currentScreen, gameObjects);
        if (!player2.isFlying()) break;
    }

    if (player1.isFlying()) player1.updateSpringEffect();
    if (player2.isFlying()) player2.updateSpringEffect();

    // Only check transitions inside actual rooms
    if (currentScreen->getScreenId() != ScreenId::HOME &&
        currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
        checkLevelTransition();

    checkIsPlayerLoaded();

    // Riddle trigger
    if (!RiddleMode && checkPlayerHasRiddle())
    {
        if (player1.hasRiddle())
            startRiddle(player1.getHeldRiddle(), player1);
        else if (player2.hasRiddle())
            startRiddle(player2.getHeldRiddle(), player2);
    }
}

/* ============================================================
                           INPUT
   ============================================================ */

void Game::handleInput()
{
    if (!_kbhit()) return;

    char key = _getch();

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
