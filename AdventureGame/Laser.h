#pragma once
#include "GameObject.h"

class Laser : public GameObject
{
private:
    bool active;         // האם הלייזר דולק כרגע?
    char originalSymbol; // שומר את הצורה המקורית (- או |)

public:
    Laser(int x, int y, char c, ScreenId screen);

    bool isActive() const { return active; }

    // קבלת אות מהמתג: הופך את המצב (Toggle)
    void receiveSignal(bool state) override;

    // טיפול בהתנגשות: הורג/חוסם רק אם דולק
    bool handleCollision(Player& p, const Screen& screen) override;
};