#include "Game.h"
#include <conio.h>
#include <string> 
#include <vector>
#include <iostream>

std::string Game::statusMessage = ""; // Initialize the static member

// --- Helper Functions Implementation ---

// 1. Initialize buffer from the current screen map
std::vector<std::string> Game::initBuffer() {
    std::vector<std::string> buffer;
    for (int i = 0; i < Screen::HEIGHT; ++i) {
        std::string line = currentScreen->getLine(i);
        // Ensure the line is exactly 80 chars wide
        if (line.length() < Screen::WIDTH) {
            line.resize(Screen::WIDTH, ' ');
        }
        buffer.push_back(line);
    }
    return buffer;
}

// 2. Write a single character to the buffer safely
void Game::writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c) {
    if (y >= 0 && y < buffer.size()) {
        if (x >= 0 && x < buffer[y].size()) {
            buffer[y][x] = c;
        }
    }
}

// 3. Write a string to the buffer
void Game::writeTextToBuffer(std::vector<std::string>& buffer, int x, int y, const std::string& text) {
    for (size_t i = 0; i < text.length(); ++i) {
        writeToBuffer(buffer, x + (int)i, y, text[i]);
    }
}

// 4. Draw game objects (Keys, Doors, etc.)
void Game::drawObjectsToBuffer(std::vector<std::string>& buffer) {
    ScreenId currentId = currentScreen->getScreenId();

    for (auto obj : gameObjects) {
        if (obj != nullptr)
            continue;
        if (obj->getScreenId() == currentId) {
            
            if (auto spring = dynamic_cast<Spring*>(obj)) {

                spring->drawToBuffer(buffer);
            }
            else {
                writeToBuffer(buffer, obj->getX(), obj->getY(), obj->getChar());
                //if obj point is -1 -1 (removed from game) we don't draw it
            }
        }
    }
}
// 5. Draw players
void Game::drawPlayersToBuffer(std::vector<std::string>& buffer) {
    if (currentScreen == &screens[(int)ScreenId::HOME] ||
        currentScreen == &screens[(int)ScreenId::INSTRUCTIONS]) {
        return;
    }
    writeToBuffer(buffer, player1.getX(), player1.getY(), player1.getChar());
    writeToBuffer(buffer, player2.getX(), player2.getY(), player2.getChar());
}

// 6. Draw status message (HUD)
void Game::drawStatusToBuffer(std::vector<std::string>& buffer) {
    if (!statusMessage.empty()) {
        writeTextToBuffer(buffer, 45, 0, statusMessage); // Adjusted X to 45 to fit nicely
    }
}

// 7. Render the final buffer to the console
void Game::renderBuffer(const std::vector<std::string>& buffer) {
    gotoxy(0, 0);
    for (const auto& line : buffer) {
        std::cout << line << '\n';
    }
    std::cout.flush(); // Single flush for the entire frame
}

// --- Main Game Functions ---

void Game::draw()
{
    // 1. Prepare Canvas
    std::vector<std::string> buffer = initBuffer();

    // 2. Draw Layers
    drawObjectsToBuffer(buffer);
    drawPlayersToBuffer(buffer);
    drawStatusToBuffer(buffer);

    // 3. Render to Screen
    renderBuffer(buffer);
}

void Game::update()
{
    int stepsP1 = player1.isFlying() ? 3 : 1; // כמה צעדים לעשות לשחקן 1
    int stepsP2 = player2.isFlying() ? 3 : 1; // כמה צעדים לשחקן 2

    for (int i = 0; i < stepsP1; ++i) {
        player1.move(*currentScreen, gameObjects);
    }

    for (int i = 0; i < stepsP2; ++i) {
        player2.move(*currentScreen, gameObjects);
    }

    if (currentScreen->getScreenId() != ScreenId::HOME &&
        currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
    {
        checkLevelTransition();
    }

    checkIsPlayerLoaded();
}

void Game::handleInput()
{
    if (_kbhit())
    {
        char key = _getch();
        if (currentScreen == &screens[(int)ScreenId::HOME] && key == '1') // Start new game
        {
            goToScreen(ScreenId::ROOM1);
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::HOME] && key == '2')
        {
            goToScreen(ScreenId::INSTRUCTIONS);
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::HOME] && key == '3')
        {
            end();
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::INSTRUCTIONS] && (key == 'H' || key == 'h'))
        {
            goToScreen(ScreenId::HOME);
            return;
        }
        else if (key == 27) // ESC key
        {
            pauseScreen();
            return;
        }
        else if (player1.isLoaded() && (key == 's' || key == 'S')) {
            player1.launch();
            return;
		}
        else if (player2.isLoaded() && (key == 'k' || key == 'K')) {
			player2.launch();
            return;
		}
        else
        {
            ChangeDirection(key);

            // Handle Dropping Items
            if (key == 'e' || key == 'E') {
                GameObject* obj = player1.dropItemToScreen(currentScreen->getScreenId());
            }
            else if (key == 'o' || key == 'O') {
                GameObject* obj = player2.dropItemToScreen(currentScreen->getScreenId());
            }
        }
        return;
    }
}

void Game::ChangeDirection(char c)
{
    // ----- Player 1 -----
    if (c == 'w' || c == 'W') { player1.setDirection(Direction::UP); }
    else if (c == 'x' || c == 'X') { player1.setDirection(Direction::DOWN); }
    else if (c == 'a' || c == 'A') { player1.setDirection(Direction::LEFT); }
    else if (c == 'd' || c == 'D') { player1.setDirection(Direction::RIGHT); }
    else if (c == 's' || c == 'S') { player1.setDirection(Direction::STAY); }

    // ----- Player 2 -----
    else if (c == 'i' || c == 'I') { player2.setDirection(Direction::UP); }
    else if (c == 'm' || c == 'M') { player2.setDirection(Direction::DOWN); }
    else if (c == 'j' || c == 'J') { player2.setDirection(Direction::LEFT); }
    else if (c == 'l' || c == 'L') { player2.setDirection(Direction::RIGHT); }
    else if (c == 'k' || c == 'K') { player2.setDirection(Direction::STAY); }
}

void Game::start() {
    hideCursor();
    while (isRunning) {
        handleInput();
        update();
        draw();
        Sleep(240);
    }
    cls();
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
            if (key == 27) // ESC key
            {
                return;
            }
            else if (key == 'H' || key == 'h') // H or h key
            {
                goToScreen(ScreenId::HOME);
                return;
            }
        }
    }
}

void Game::stopMovement()
{
    player1.setDirection(Direction::STAY);
    player2.setDirection(Direction::STAY);
}

void Game::setStatusMessage(const std::string& msg)
{
    statusMessage = msg;
}


void Game::resetPlayersForNewLevel()
{
    player1.updatepoint(2, 12);
    player2.updatepoint(3, 12); // Moved slightly right so they don't overlap
    player1.setDirection(Direction::STAY);
    player2.setDirection(Direction::STAY);

    // Sync logical level (allready done before but good to make sure
    ScreenId currentId = currentScreen->getScreenId();
    player1.setCurrentLevel(currentId);
    player2.setCurrentLevel(currentId);
}

void Game::checkLevelTransition()
{
    ScreenId target1 = player1.getCurrentLevel();
    ScreenId target2 = player2.getCurrentLevel();
    ScreenId currentRealLevel = currentScreen->getScreenId();

    // Update Status Message
    if (target1 != currentRealLevel && target2 == currentRealLevel) {
        setStatusMessage("Player 1 is waiting...");
    }
    else if (target2 != currentRealLevel && target1 == currentRealLevel) {
        setStatusMessage("Player 2 is waiting...");
    }
    else {
        setStatusMessage("");
    }

    // Check if both are ready to transition
    if (target1 != currentRealLevel &&
        target2 != currentRealLevel &&
        target1 == target2)
    {
        // Transition!
        goToScreen(target1);
        resetPlayersForNewLevel();
        setStatusMessage("");
    }
}
void Game::checkIsPlayerLoaded()
{
    if (player1.isLoaded() || player2.isLoaded()) {
		setStatusMessage("Spring ready!Press STAY key to launch");  
    }
}


Game::~Game()
{
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}