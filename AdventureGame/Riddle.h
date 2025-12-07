#pragma once
#include "GameObject.h"

class Player;
class Screen;

// Riddle identifiers
enum class RiddleId {
    RIDDLE1,
    RIDDLE2,
    RIDDLE3
};

class Riddle : public GameObject
{
public:
    static constexpr int WIDTH = 75;
    static constexpr int HEIGHT = 20;

private:
    RiddleId id;
    const char* const* layout = nullptr;
    int correctAnswer = 1;
    bool solved = false;

    // Layouts for all riddles
    static const char* RIDDLE1[HEIGHT];
    static const char* RIDDLE2[HEIGHT];
    static const char* RIDDLE3[HEIGHT];

public:
    Riddle();
    Riddle(int x, int y, ScreenId screen, RiddleId id);

    RiddleId getId() const { return id; }
    int getCorrectAnswer() const { return correctAnswer; }

    const char* getLine(int row) const {
        if (row >= 0 && row < HEIGHT && layout != nullptr)
            return layout[row];
        return "";
    }

    bool isSolved() const { return solved; }
    void markSolved() { solved = true; }

    virtual bool handleCollision(Player& p, const Screen& screen) override;
};
