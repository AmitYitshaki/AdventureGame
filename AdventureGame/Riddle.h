#pragma once
#include "GameObject.h"
#include <vector>
#include <string>

/*
 * ===================================================================================
 * Class: Riddle
 * -----------------------------------------------------------------------------------
 * Purpose:
 * An interactive object that pauses gameplay to present a trivia question.
 *
 * Behavior:
 * - Triggered on collision.
 * - Switches Game mode to "RiddleMode".
 * - Validates player answer input (1-4).
 * - Rewards points for correct answers, penalizes life for wrong ones.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Stores the question text and correct answer index.
 * ===================================================================================
 */

struct RiddleData {
    int id = -1;
    int correctAnswer = 1;
    std::vector<std::string> textLines;
};

class Riddle : public GameObject
{
private:
    RiddleData data;
    bool solved = false;

public:
    static constexpr int WIDTH = 76;
    static constexpr int HEIGHT = 22;

    Riddle(int x, int y, ScreenId screen) : GameObject(x, y, '?', screen) {}

    void setData(const RiddleData& newData) { data = newData; }
	const RiddleData& getData() const { return data; }
    int getCorrectAnswer() const { return data.correctAnswer; }

    const char* getLine(int row) const {
        if (row < 0 || row >= (int)data.textLines.size()) return "";
        return data.textLines[row].c_str();
    }

    bool handleCollision(Player& p, const Screen& screen) override;
    std::string getTypeName() const override { return "RIDDLE"; }
    // Riddle typically handles its own internal data via ID, 
    // assuming we don't need to save 'solved' state if it disappears when solved.
    std::string getSaveData() const override;
};