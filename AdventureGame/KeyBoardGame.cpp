#include "Game.h"
#include <conio.h>
#include <ctime>

KeyBoardGame::KeyBoardGame(bool recordMode) : isRecording(recordMode) {}

KeyBoardGame::~KeyBoardGame() {
    if (stepsFileOut.is_open()) stepsFileOut.close();
    if (resultFileOut.is_open()) resultFileOut.close();
}

// === השינוי 1: initSession רק מכין את המסך, לא פותח קבצים ===
void KeyBoardGame::initSession()
{
    // אנחנו לא פותחים כאן קבצים יותר!
    // רק מוודאים שהמשחק במצב התחלתי תקין
    isGameSessionActive = true;
    goToScreen(ScreenId::HOME);
}

// === השינוי 2: פתיחת הקבצים מתבצעת ברגע שהמשחק האמיתי מתחיל ===
void KeyBoardGame::resetGame()
{
    // 1. קודם כל מייצרים את ה-Seed! (לפני שטוענים חידות)
    if (isRecording) {
        randomSeed = (unsigned int)time(NULL);
    }
    else {
        randomSeed = (unsigned int)time(NULL);
    }

    // מעדכנים את מנוע האקראיות הגלובלי
    srand(randomSeed);

    // 2. עכשיו קוראים לאתחול של מחלקת האב
    // (בתוכה תיקרא הפונקציה loadRiddles שתשתמש ב-randomSeed שכבר הגדרנו)
    Game::resetGame();

    // 3. פתיחת קבצים וכתיבת ה-Seed שיצרנו בהתחלה
    if (isRecording) {
        // סגירה למקרה שהיה משהו פתוח קודם
        if (stepsFileOut.is_open()) stepsFileOut.close();
        if (resultFileOut.is_open()) resultFileOut.close();

        stepsFileOut.open("adv-world.steps", std::ios::trunc);
        resultFileOut.open("adv-world.result", std::ios::trunc);

        if (stepsFileOut.is_open()) {
            stepsFileOut << "SEED: " << randomSeed << std::endl;
        }
    }
}

char KeyBoardGame::getNextChar()
{
    if (_kbhit()) {
        char key = _getch();

        // הקלטה לקובץ
        // אנחנו מקליטים רק אם הקובץ פתוח (כלומר, אנחנו אחרי resetGame)
        if (isRecording && stepsFileOut.is_open())
        {
            if (RiddleMode && key == 27) return 0;

            // המקש נרשם עם ה-Cycle הנוכחי (שהוא עכשיו מסונכרן למשחק)
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