// =============================================================
// FileGame.cpp - Handles Playback & Verification
// =============================================================
#include "Game.h"
#include <iostream>

FileGame::FileGame(bool silentMode) : isSilent(silentMode) {}

FileGame::~FileGame() {
    if (stepsFileIn.is_open()) stepsFileIn.close();
}

void FileGame::initSession()
{
    stepsFileIn.open("adv-world.steps");
    if (!stepsFileIn) {
        isRunning = false;
        return;
    }

    std::string header;
    stepsFileIn >> header >> randomSeed;
    srand(randomSeed);

    long t; char k;
    while (stepsFileIn >> t >> k) {
        stepsBuffer.push_back({ t, k });
    }
    stepsFileIn.close();

    loadExpectedResults();

    // דילוג על תפריט והתחלה מיידית
    isGameSessionActive = true;
    cycleCounter = 0;
    resetGame();
}

char FileGame::getNextChar()
{
    // מנגנון Catch-Up
    if (playbackIndex < stepsBuffer.size()) {
        if (stepsBuffer[playbackIndex].first <= cycleCounter) {
            return stepsBuffer[playbackIndex++].second;
        }
    }
    else {
        // נגמרו הצעדים - סיום
        static int gracePeriod = 0;
        if (gracePeriod++ > 20) isRunning = false;
    }
    return 0;
}

void FileGame::handleEventReport(const std::string& type, const std::string& details)
{
    verifyEvent(type, details);
}

void FileGame::verifyEvent(const std::string& type, const std::string& details)
{
    if (testFailed) return;

    if (expectedResults.empty()) {
        testFailed = true;
        failureReason = "Unexpected event: " + type;
        isRunning = false;
        return;
    }

    GameEvent expected = expectedResults.front();
    expectedResults.pop_front();

    long timeDiff = std::abs(expected.cycle - (long)cycleCounter);
    if (timeDiff > 10 || expected.type != type || expected.details != details) {
        testFailed = true;
        failureReason = "Mismatch! Expected: " + expected.type + " Got: " + type;
        isRunning = false;
    }
}

void FileGame::loadExpectedResults()
{
    std::ifstream resIn("adv-world.result");
    if (!resIn) return;
    long t; std::string type;
    while (resIn >> t >> type) {
        std::string details;
        std::getline(resIn, details);
        if (!details.empty() && details[0] == ' ') details.erase(0, 1);
        expectedResults.push_back({ t, type, details });
    }
}

void FileGame::endSession()
{
    if (isSilent) {
        if (testFailed) std::cout << "Test Failed: " << failureReason << std::endl;
        else if (!expectedResults.empty()) std::cout << "Test Failed: Missing events." << std::endl;
        else std::cout << "Test Passed" << std::endl;

        std::cout << "Press any key...";
        _getch();
    }
}

void FileGame::handleSleep() {
    if (isSilent) return; // Silent = מקסימום ביצועים
    Sleep(TICK_MS);       // Load = מהירות רגילה
}

void FileGame::outputGraphics() {
    if (isSilent) return; // Silent = ללא גרפיקה
    draw();
}