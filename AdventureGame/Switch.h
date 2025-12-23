#pragma once
#include "GameObject.h"
#include <vector>

class Switch : public GameObject
{
private:
    bool active;                      // האם המתג לחוץ?
    std::vector<GameObject*> targets; // רשימת לייזרים שמחוברים למתג

public:
    Switch(int x, int y, ScreenId screen);

    bool isActive() const { return active; }

    // חיבור לייזר למתג (נקרא ע"י CONNECT)
    void addTarget(GameObject* obj);

    // פעולה בעת דריכה
    bool handleCollision(Player& p, const Screen& screen) override;

    // פונקציית עזר לשינוי מצב
    void toggle();
};