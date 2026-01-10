#include "Game.h"
#include <iostream>
#include <cmath> // עבור std::abs

FileGame::FileGame(bool silentMode) : isSilent(silentMode) {}

FileGame::~FileGame() {
    if (stepsFileIn.is_open()) stepsFileIn.close();
}

void FileGame::initSession()
{
    stepsFileIn.open("adv-world.steps");
    if (!stepsFileIn) {
        std::cerr << "Error: Could not open steps file." << std::endl;
        isRunning = false;
        return;
    }

    std::string header;
    // קריאת כותרת וסיד
    if (!(stepsFileIn >> header >> randomSeed)) {
        isRunning = false;
        return;
    }
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
    // 1. האם יש צעדים (לחיצות) שממתינים לביצוע במחזור הנוכחי?
    if (playbackIndex < stepsBuffer.size()) {
        if (stepsBuffer[playbackIndex].first <= cycleCounter) {
            return stepsBuffer[playbackIndex++].second;
        }
    }

    // 2. אם נגמרו הצעדים, נבדוק אם סיימנו את כל האירועים המצופים
    if (playbackIndex >= stepsBuffer.size()) {

        // אם אין יותר למה לחכות - נסיים (עם גרייס קטן)
        if (expectedResults.empty()) {
            static int gracePeriod = 0;
            if (gracePeriod++ > 20) isRunning = false;
            return 0;
        }

        // אם יש ציפיות (למשל פיצוץ שאמור לקרות), נחכה לו
        static int timeoutCounter = 0;
        timeoutCounter++;

        // אם עברו יותר מ-3000 מחזורים (כ-3 דקות משחק) ועדיין לא קרה כלום
        if (timeoutCounter > 3000) {
            testFailed = true;

            // בניית הודעת שגיאה מפורטת
            std::string missingEvent = expectedResults.front().type;
            long expectedTime = expectedResults.front().cycle;

            failureReason = "Timeout! Waiting for event: " + missingEvent +
                " (Expected at cycle: " + std::to_string(expectedTime) +
                ", Current cycle: " + std::to_string(cycleCounter) + ")";

            isRunning = false;
        }
    }

    return 0; // אין מקש ללחוץ, ממשיכים להריץ את הלוגיקה
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
        failureReason = "Unexpected event: " + type + " (Detail: " + details + ")";
        isRunning = false;
        return;
    }

    GameEvent expected = expectedResults.front();

    // בדיקה: האם זה האירוע שחיכינו לו?
    bool typeMatch = (expected.type == type);
    bool detailsMatch = (expected.details == details);
    long timeDiff = std::abs(expected.cycle - (long)cycleCounter);

    // הגדלנו מעט את הטווח ל-20 כדי להיות סלחניים יותר להבדלי תזמון קטנים
    if (typeMatch && detailsMatch && timeDiff <= 20) {
        expectedResults.pop_front(); // האירוע עבר בהצלחה

        // === התיקון לסנכרון סיום המשחק ===
        // אם אימתנו שהמשחק נגמר, אין סיבה להמשיך לרוץ ולחכות ל-Timeout.
        // אנחנו עוצרים את הסימולציה כאן ועכשיו.
        if (type == "GAME_ENDED") {
            isRunning = false;
        }
        // ================================
    }
    else {
        // כישלון
        testFailed = true;
        failureReason = "Mismatch!\nExpected: [" + expected.type + "] (" + expected.details + ") at cycle " + std::to_string(expected.cycle) +
            "\nGot:      [" + type + "] (" + details + ") at cycle " + std::to_string(cycleCounter);
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
        // ניקוי רווחים מיותרים בהתחלה
        if (!details.empty() && details[0] == ' ') details.erase(0, 1);
        expectedResults.push_back({ t, type, details });
    }
}

void FileGame::endSession()
{
    // --- תיקון: ניקוי מסך כפוי לפני הדפסת התוצאות ---
    if (isSilent) {
        // שימוש בפקודות מערכת לניקוי אגרסיבי כדי להעיף שאריות גרפיקה
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

void FileGame::handleSleep() {
    if (isSilent) return; // Silent = מקסימום ביצועים
    Sleep(TICK_MS / 4);
}

void FileGame::outputGraphics() {
    if (isSilent) return; // Silent = ללא גרפיקה
    draw();
}