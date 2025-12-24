#pragma once
#include "GameObject.h"
#include <vector>
#include <string>

// מבנה נתונים פשוט שמחזיק את תוכן החידה
struct RiddleData {
    int id = -1;
	int correctAnswer = 1; // ברירת מחדל: תשובה 1
    std::vector<std::string> textLines;
};

class Riddle : public GameObject
{
private:
    RiddleData data; // הנתונים שהוזרקו לחידה
    bool solved = false;

public:
    // גודל ה-Overlay (כמעט כל המסך)
    static constexpr int WIDTH = 76;
    static constexpr int HEIGHT = 22;

    Riddle(int x, int y, ScreenId screen)
        : GameObject(x, y, '?', screen) {
    }

    // פונקציה להזרקת הנתונים (נקראת ע"י Game)
    void setData(const RiddleData& newData) { data = newData; }

    int getCorrectAnswer() const { return data.correctAnswer; }

    // שליפת שורה לציור
    const char* getLine(int row) const {
        if (row < 0 || row >= (int)data.textLines.size()) return "";
        return data.textLines[row].c_str();
    }

    bool isSolved() const { return solved; }
    void markSolved() { solved = true; }

    bool handleCollision(Player& p, const Screen& screen) override;
};