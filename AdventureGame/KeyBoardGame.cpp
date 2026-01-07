// =============================================================
// KeyBoardGame.cpp - Handles Input & Recording
// =============================================================
#include "Game.h"
#include <conio.h>
#include <ctime>

KeyBoardGame::KeyBoardGame(bool recordMode) : isRecording(recordMode) {}

KeyBoardGame::~KeyBoardGame() {
    if (stepsFileOut.is_open()) stepsFileOut.close();
    if (resultFileOut.is_open()) resultFileOut.close();
}

void KeyBoardGame::initSession()
{
    // פתיחת קבצים לכתיבה רק אם נתבקשנו להקליט
    if (isRecording) {
        stepsFileOut.open("adv-world.steps", std::ios::trunc);
        resultFileOut.open("adv-world.result", std::ios::trunc);

        randomSeed = (unsigned int)time(NULL);
        srand(randomSeed);

        if (stepsFileOut.is_open()) {
            stepsFileOut << "SEED: " << randomSeed << std::endl;
        }
    }
    else {
        randomSeed = (unsigned int)time(NULL);
        srand(randomSeed);
    }

    isGameSessionActive = true;
    goToScreen(ScreenId::HOME);
}

char KeyBoardGame::getNextChar()
{
    if (_kbhit()) {
        char key = _getch();

        // הקלטה לקובץ
        if (isRecording && stepsFileOut.is_open() &&
            currentScreen->getScreenId() != ScreenId::HOME &&
            currentScreen->getScreenId() != ScreenId::INSTRUCTIONS)
        {
            if (RiddleMode && key == 27) return 0; // חסימת ESC בחידות

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
    if (stepsFileOut.is_open()) stepsFileOut.close();
    if (resultFileOut.is_open()) resultFileOut.close();
}