#include "spring.h"
#include "Screen.h"
#include "Game.h"

// עוזר קטן לכתיבה לבאפר
static void putPointToBuffer(std::vector<std::string>& buffer, const Point& pt)
{
    int x = pt.getX();
    int y = pt.getY();
    if (y >= 0 && y < (int)buffer.size() &&
        x >= 0 && x < (int)buffer[y].size())
    {
        buffer[y][x] = pt.getChar();
    }
}

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    putPointToBuffer(buffer, start);
    putPointToBuffer(buffer, middle);
    putPointToBuffer(buffer, end);
}

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    int px = p.getX();
    int py = p.getY();

    // אם השחקן על אחד משני החלקים הראשונים – רק "נכנס" לקפיץ
    if ((px == start.getX() && py == start.getY()) ||
        (px == middle.getX() && py == middle.getY()))
    {
        return true; // לא חוסם תנועה
    }
    // אם הגיע לקצה (end) – מפעילים את הקפיץ
    if (px == end.getX() && py == end.getY())
    {
        loadSpring(p);
        return true;
    }
    return true;
}

void Spring::loadSpring(Player& p)
{
    p.stopMovement();           // עוצר תנועה רגילה
    p.setLoaded(true); // מצב "טיסה" – אתה כבר מטפל בזה בלוגיקה של Player
	p.setLaunchDirection(direction); // שומר לאיזו כיוון להטיס
}
