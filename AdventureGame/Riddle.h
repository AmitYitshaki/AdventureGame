#pragma once

#include "GameObject.h"

class Player;
class Screen;

/*
    Riddle:
    Collectible puzzle item that opens a full-screen UI
    when picked. Removed from world upon pickup.
*/
enum class RiddleId
{
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

    // Static layouts
    static const char* RIDDLE1[HEIGHT];
    static const char* RIDDLE2[HEIGHT];
    static const char* RIDDLE3[HEIGHT];

public:
    Riddle();
    Riddle(int x, int y, ScreenId screen, RiddleId id);

    RiddleId getId() const { return id; }
    int getCorrectAnswer() const { return correctAnswer; }

    const char* getLine(int row) const;

    bool isSolved() const { return solved; }
    void markSolved() { solved = true; }

    bool handleCollision(Player& p, const Screen& screen) override;
};
