#include "Game.h"
#include <conio.h>
#include <ctime>

// === Construction ===
KeyBoardGame::KeyBoardGame(bool recordMode) : isRecording(recordMode) {}

KeyBoardGame::~KeyBoardGame()
{
    if (stepsFileOut.is_open()) {
        stepsFileOut.close();
    }
    if (resultFileOut.is_open()) {
        resultFileOut.close();
    }
}

// === Session Lifecycle ===
void KeyBoardGame::initSession()
{
    // Prepare the screen and session state; files are opened on reset.
    isGameSessionActive = true;
    goToScreen(ScreenId::HOME);
}

void KeyBoardGame::resetGame()
{
    // 1) Generate the seed before loading riddles.
    randomSeed = static_cast<unsigned int>(time(nullptr));

    // 2) Update the global random engine.
    srand(randomSeed);

    // 3) Call the base reset (loads riddles using the seed).
    Game::resetGame();

    // 4) Open output files and write the seed.
    if (isRecording) {
        if (stepsFileOut.is_open()) {
            stepsFileOut.close();
        }
        if (resultFileOut.is_open()) {
            resultFileOut.close();
        }

        stepsFileOut.open("adv-world.steps", std::ios::trunc);
        resultFileOut.open("adv-world.result", std::ios::trunc);

        if (stepsFileOut.is_open()) {
            stepsFileOut << "SEED: " << randomSeed << std::endl;
        }
    }
}

// === Input & Event Reporting ===
char KeyBoardGame::getNextChar()
{
    if (_kbhit()) {
        char key = _getch();

        // Record input only after resetGame opened the file.
        if (isRecording && stepsFileOut.is_open())
        {
            if (RiddleMode && key == 27) {
                return 0;
            }

            // The key is recorded with the current synchronized cycle.
            stepsFileOut << cycleCounter << " " << key << std::endl;
        }
        return key;
    }
    return 0;
}

void KeyBoardGame::handleEventReport(const std::string& type, const std::string& details)
{
    if (isRecording && resultFileOut.is_open()) {
        resultFileOut << cycleCounter << " " << type << " " << details << std::endl;
    }
}

void KeyBoardGame::endSession()
{
    if (stepsFileOut.is_open()) {
        stepsFileOut.close();
    }
    if (resultFileOut.is_open()) {
        resultFileOut.close();
    }
}
