#include "Riddle.h"
#include "Player.h"

/*
    Riddle.cpp
    Implements riddle construction, collision handling,
    and holds the static ASCII layouts for all riddles.
*/

// ---------------------- Constructors ----------------------

Riddle::Riddle()
    : GameObject(2, 2, '?', ScreenId::ROOM1),
    id(RiddleId::RIDDLE1),
    layout(RIDDLE1),
    correctAnswer(1),
    solved(false)
{
}

Riddle::Riddle(int x, int y, ScreenId screen, RiddleId id)
    : GameObject(x, y, '?', screen),
    id(id),
    layout(nullptr),
    correctAnswer(1),
    solved(false)
{
    switch (id)
    {
    case RiddleId::RIDDLE1:
        layout = RIDDLE1;
        correctAnswer = 1;
        break;

    case RiddleId::RIDDLE2:
        layout = RIDDLE2;
        correctAnswer = 1; // true about references
        break;

    case RiddleId::RIDDLE3:
        layout = RIDDLE3;
        correctAnswer = 1; // destructor automatically
        break;

    default:
        layout = RIDDLE1;
        correctAnswer = 1;
        break;
    }
}

// ---------------------- Access to layout ----------------------

const char* Riddle::getLine(int row) const
{
    if (!layout)
        return "";

    if (row < 0 || row >= HEIGHT)
        return "";

    return layout[row];
}

// ---------------------- handleCollision ----------------------

bool Riddle::handleCollision(Player& p, const Screen& /*screen*/)
{
    // Already collected? do nothing
    if (isCollected())
        return true;

    // Player already holds an item → block
    if (p.hasItem())
        return false;

    // Let the player collect this riddle
    p.collectItem(this);

    // Mark as collected and remove from map
    Collected();
    removeFromGame();  // move to (-1, -1)

    return true; // allow player to step on this tile
}

// ---------------------- RIDDLE 1 ----------------------
// Topic: C++ operators, tricky behavior

const char* Riddle::RIDDLE1[Riddle::HEIGHT] =
{
    "---------------------------------------------------------------------------",
    "|                                                                         |",
    "|                           C++ RIDDLE 1                                  |",
    "|                                                                         |",
    "|  What will the following code print?                                    |",
    "|                                                                         |",
    "|     int x = 5;                                                          |",
    "|     cout << x++ * ++x;                                                  |",
    "|                                                                         |",
    "|     1) Undefined behavior                                               |",
    "|     2) 30                                                                |",
    "|     3) 36                                                                |",
    "|     4) Compilation error                                                 |",
    "|                                                                         |",
    "|   (Press the corresponding number 1-4 to answer)                        |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "---------------------------------------------------------------------------"
};

// ---------------------- RIDDLE 2 ----------------------
// Topic: C++ references vs pointers

const char* Riddle::RIDDLE2[Riddle::HEIGHT] =
{
    "---------------------------------------------------------------------------",
    "|                                                                         |",
    "|                           C++ RIDDLE 2                                  |",
    "|                                                                         |",
    "|  Which statement about references in C++ is TRUE?                       |",
    "|                                                                         |",
    "|     1) A reference must always be initialized when declared             |",
    "|     2) A reference can be reseated to another variable                  |",
    "|     3) A reference can be null                                          |",
    "|     4) References and pointers behave identically                       |",
    "|                                                                         |",
    "|   (Press the corresponding number 1-4 to answer)                        |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "---------------------------------------------------------------------------"
};

// ---------------------- RIDDLE 3 ----------------------
// Topic: C++ objects and destructors

const char* Riddle::RIDDLE3[Riddle::HEIGHT] =
{
    "---------------------------------------------------------------------------",
    "|                                                                         |",
    "|                           C++ RIDDLE 3                                  |",
    "|                                                                         |",
    "|  When is a destructor called in C++?                                    |",
    "|                                                                         |",
    "|     1) Automatically when an object goes out of scope                   |",
    "|     2) Only when `delete` is called                                     |",
    "|     3) Only when the program ends                                       |",
    "|     4) Never, unless called manually                                    |",
    "|                                                                         |",
    "|   (Press the corresponding number 1-4 to answer)                        |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "|                                                                         |",
    "---------------------------------------------------------------------------"
};
