#include "Logger.h"
#include "Player.h"
#include "Screen.h"
#include "GameObject.h"
#include "Key.h"
#include "Door.h"
#include "Switch.h"
#include "Laser.h"
#include "Torch.h"
#include "Spring.h"
#include <ctime>
#include <sstream>

// =========================================================
//                   STATIC MEMBERS
// =========================================================
std::ofstream Logger::logFile;
std::vector<std::string> Logger::recentLogs;

// =========================================================
//                   HELPER FUNCTIONS
// =========================================================

// המרת כיוון לטקסט קצר (3 אותיות לחיסכון במקום)
static std::string dirToShortStr(Direction d) {
    switch (d) {
    case Direction::UP:    return "UP";
    case Direction::DOWN:  return "DWN";
    case Direction::LEFT:  return "LFT";
    case Direction::RIGHT: return "RGT";
    default:               return "STY";
    }
}

// =========================================================
//                   LOGGER CORE
// =========================================================

void Logger::Init(const std::string& fileName)
{
    if (logFile.is_open()) logFile.close();
    logFile.open(fileName, std::ios::trunc);
}

void Logger::Log(const std::string& message)
{
    if (logFile.is_open()) {
        logFile << "[INFO] " << message << std::endl;
    }
}

void Logger::Error(const std::string& message)
{
    if (logFile.is_open()) {
        logFile << "[ERROR] " << message << std::endl;
        logFile.flush();
    }
}

// =========================================================
//                 INFO GENERATORS (DASHBOARD)
// =========================================================

// פורמט עשיר: (10,5) HP:3 Dir:LFT [FLY] [Item:K #100]
std::string Logger::getPlayerInfo(const Player& p)
{
    std::stringstream ss;

    // מיקום וחיים
    ss << "(" << p.getX() << "," << p.getY() << ") ";
    ss << "HP:" << p.getLive() << " ";

    // כיוון (חדש!)
    ss << "Dir:" << dirToShortStr(p.getDirection()) << " ";

    // סטטוסים מיוחדים (חדש!)
    if (p.isFlying()) ss << "[FLY] ";
    if (p.isLoaded()) ss << "[LOAD] ";

    // חפצים
    if (p.hasItem()) {
        ss << "[Item:" << p.getItemChar();
        if (p.getHeldKeyID() != -1) {
            ss << "#" << p.getHeldKeyID();
        }
        ss << "]";
    }
    else {
        ss << "[-]";
    }

    return ss.str();
}

std::string Logger::getScreenInfo(const Screen& s)
{
    std::string info = "Rm " + std::to_string((int)s.getScreenId());
    if (s.isDark()) info += "(Drk)";
    return info;
}

std::string Logger::getGameObjectInfo(const GameObject* obj)
{
    if (!obj) return "";

    std::stringstream ss;

    // --- טיפול באובייקטים שנאספו (חדש!) ---
    if (obj->isCollected()) {
        ss << "[COLLECTED] ";
        // לאובייקט שנאסף לרוב יש רק סוג ומזהה
        if (auto key = dynamic_cast<const Key*>(obj)) {
            ss << "Key #" << key->getKeyID();
        }
        else {
            ss << "Obj '" << obj->getChar() << "'";
        }
        return ss.str(); // מסיימים כאן, לא צריך מיקום
    }

    // --- טיפול באובייקטים רגילים על המסך ---

    // דלת
    if (auto door = dynamic_cast<const Door*>(obj)) {
        ss << "Door #" << door->getId() << " ";
        ss << (door->isLocked() ? "(L)" : "(O)") << " ";
        ss << "->Rm" << (int)door->getLeadsTo();
    }
    // מפתח
    else if (auto key = dynamic_cast<const Key*>(obj)) {
        ss << "Key #" << key->getKeyID();
    }
    // מתג
    else if (auto sw = dynamic_cast<const Switch*>(obj)) {
        ss << "Sw(" << (sw->isActive() ? "ON" : "OFF") << ")";
    }
    // קפיץ
    else if (auto spring = dynamic_cast<const Spring*>(obj)) {
        ss << "Sp(" << dirToShortStr(spring->getDirection()) << ")";
        // אפשר להוסיף את הכניסה אם רוצים, אבל שומרים על קצר
        ss << ">In:" << dirToShortStr(spring->getOppositeDirection());
    }
    // לפיד
    else if (dynamic_cast<const Torch*>(obj)) {
        ss << "Torch";
    }
    else {
        ss << "'" << obj->getChar() << "'";
    }

    // הוספת מיקום רק אם לא נאסף
    ss << "@(" << obj->getX() << "," << obj->getY() << ")";

    return ss.str();
}

std::vector<std::string> Logger::getScreenObjectsList(const std::vector<GameObject*>& allObjects, ScreenId currentScreenId)
{
    std::vector<std::string> list;

    for (const auto& obj : allObjects)
    {
        if (!obj) continue;
        if (obj->getScreenId() != currentScreenId) continue;

        // --- שינוי: לא מסננים אובייקטים שנאספו ---
        // if (obj->isCollected()) continue; // שורה זו נמחקה

        // סינון רעש רגיל (לייזרים וקירות)
        if (dynamic_cast<const Laser*>(obj)) continue;
        if (obj->getChar() == '#') continue;

        list.push_back(getGameObjectInfo(obj));
    }
    return list;
}

// מימושים ריקים נדרשים
const std::vector<std::string>& Logger::GetRecentLogs() { return recentLogs; }
std::string Logger::GetTimestamp() { return ""; }