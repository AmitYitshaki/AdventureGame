#include "Game.h"
#include <iostream>
#include <cmath>

// === Construction ===
FileGame::FileGame(bool silentMode) : isSilent(silentMode) { soundEnabled = false; }

FileGame::~FileGame()
{
    if (stepsFileIn.is_open()) {
        stepsFileIn.close();
    }
}

// === Session Lifecycle ===
void FileGame::initSession()
{
    stepsFileIn.open("adv-world.steps");
    if (!stepsFileIn) {
        std::cerr << "Error: Could not open steps file." << std::endl;
        isRunning = false;
        return;
    }

    std::string header;
    // Read header and seed.
    if (!(stepsFileIn >> header >> randomSeed)) {
        isRunning = false;
        return;
    }
    srand(randomSeed);

    long t;
    char k;
    while (stepsFileIn >> t >> k) {
        stepsBuffer.push_back({ t, k });
    }
    stepsFileIn.close();

    loadExpectedResults();

    // Skip the menu and start immediately.
    isGameSessionActive = true;
    cycleCounter = 0;
    resetGame();
}

void FileGame::endSession()
{
    // Force a screen clear before printing results in silent mode.
    if (isSilent) {
        system("cls");

        std::cout << "========================================" << std::endl;
        if (testFailed) {
            std::cout << "TEST FAILED" << std::endl;
            std::cout << "Reason: " << failureReason << std::endl;
        }
        else if (!expectedResults.empty()) {
            std::cout << "TEST FAILED" << std::endl;
            std::cout << "Reason: Missing " << expectedResults.size() << " expected events." << std::endl;
        }
        else {
            std::cout << "TEST PASSED" << std::endl;
        }
        std::cout << "========================================" << std::endl;

        std::cout << "Press any key to exit...";
        _getch();
    }
}

// === Input Playback ===
char FileGame::getNextChar()
{
    // 1) Check whether there are queued inputs for this cycle.
    if (playbackIndex < stepsBuffer.size()) {
        if (stepsBuffer[playbackIndex].first <= cycleCounter) {
            return stepsBuffer[playbackIndex++].second;
        }
    }

    // 2) If input is exhausted, verify expected events.
    if (playbackIndex >= stepsBuffer.size()) {

        // If there is nothing left to wait for, end after a short grace period.
        if (expectedResults.empty()) {
            static int gracePeriod = 0;
            if (gracePeriod++ > 20) {
                isRunning = false;
            }
            return 0;
        }

        // If there are expected events (e.g., explosions), keep waiting.
        static int timeoutCounter = 0;
        timeoutCounter++;

        // Timeout after 3000 cycles (~3 minutes of gameplay).
        if (timeoutCounter > 3000) {
            testFailed = true;

            // Build a detailed failure message.
            std::string missingEvent = expectedResults.front().type;
            long expectedTime = expectedResults.front().cycle;

            failureReason = "Timeout! Waiting for event: " + missingEvent +
                " (Expected at cycle: " + std::to_string(expectedTime) +
                ", Current cycle: " + std::to_string(cycleCounter) + ")";

            isRunning = false;
        }
    }

    return 0; // No key for this cycle.
}

// === Event Verification ===
void FileGame::handleEventReport(const std::string& type, const std::string& details)
{
    verifyEvent(type, details);
}

void FileGame::verifyEvent(const std::string& type, const std::string& details)
{
    if (testFailed) {
        return;
    }

    if (expectedResults.empty()) {
        testFailed = true;
        failureReason = "Unexpected event: " + type + " (Detail: " + details + ")";
        isRunning = false;
        return;
    }

    GameEvent expected = expectedResults.front();

    // Check if this is the expected event.
    bool typeMatch = (expected.type == type);
    bool detailsMatch = (expected.details == details);
    long timeDiff = std::abs(expected.cycle - static_cast<long>(cycleCounter));

    // Allow a small tolerance to account for minor timing differences.
    if (typeMatch && detailsMatch && timeDiff <= 20) {
        expectedResults.pop_front();

        // If we validated GAME_ENDED, stop immediately to avoid timeouts.
        if (type == "GAME_ENDED") {
            isRunning = false;
        }
    }
    else {
        testFailed = true;
        failureReason = "Mismatch!\nExpected: [" + expected.type + "] (" + expected.details + ") at cycle " + std::to_string(expected.cycle) +
            "\nGot:      [" + type + "] (" + details + ") at cycle " + std::to_string(cycleCounter);
        isRunning = false;
    }
}

void FileGame::loadExpectedResults()
{
    std::ifstream resIn("adv-world.result");
    if (!resIn) {
        return;
    }
    long t;
    std::string type;
    while (resIn >> t >> type) {
        std::string details;
        std::getline(resIn, details);
        // Trim leading space from details.
        if (!details.empty() && details[0] == ' ') {
            details.erase(0, 1);
        }
        expectedResults.push_back({ t, type, details });
    }
}

// === Silent Mode Overrides ===
void FileGame::handleSleep()
{
    if (isSilent) {
        return;
    }
    Sleep(TICK_MS / 4);
}

void FileGame::outputGraphics()
{
    if (isSilent) {
        return;
    }
    draw();
}
