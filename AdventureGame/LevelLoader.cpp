#include "LevelLoader.h"
#include <fstream>
#include <iostream>
#include <string>

// Include your specific game objects
#include "Key.h"
#include "Riddle.h"
#include "Door.h"
#include "Torch.h"
// #include "Spring.h" // בהמשך תוסיף את זה

// --- פונקציית עזר 1: טיפול במיקום התחלתי של שחקנים ---
void LevelLoader::handlePlayerStartPosition(char c, int x, int y, Screen& screen)
{
    if (c == '$') {
        screen.setStartPos1(x, y);
    }
    else if (c == '&') {
        screen.setStartPos2(x, y);
    }
}

// --- פונקציית עזר 2: יצירת אובייקט חכם ---
// שים לב: אנחנו צריכים רק את ה-ScreenId כדי ליצור את האובייקט
void LevelLoader::createObject(char c, int x, int y, ScreenId screenId, std::vector<GameObject*>& gameObjects)
{
    // חישוב מזהה מבוסס על המסך הנוכחי
    // המרה ל-int כדי שיוכל להיכנס לשדות של Key ו-Door
    int currentLevelID = (int)screenId;

    // חישוב "החדר הבא" באופן אוטומטי
    // (הנחה: החדרים ב-Enum מסודרים עוקב: ROOM1, ROOM2, ROOM3...)
    ScreenId nextLevelID = (ScreenId)(currentLevelID + 1);

    switch (c)
    {
    case 'K':
        // המפתח מקבל את ה-ID של החדר הנוכחי
        gameObjects.push_back(new Key(x, y, 'K', screenId, currentLevelID));
        break;

    case 'D':
        // הדלת ננעלת עם ה-ID של החדר הנוכחי
        // והיא מובילה לחדר הבא (nextLevelID)
        gameObjects.push_back(new Door(x, y, 'D', screenId, currentLevelID, nextLevelID, true));
        break;

    case '?':
        gameObjects.push_back(new Riddle(x, y, screenId, RiddleId::RIDDLE1));
        break;

    case '!':
        gameObjects.push_back(new Torch(x, y, '!', screenId));
        break;
    }
}

void LevelLoader::loadScreenFromFile(const std::string& fileName, Screen& screenToFill)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open screen file: " << fileName << std::endl;
        return;
    }

    std::vector<std::string> newLayout;
    std::string line;

    // קריאה פשוטה: שורה אחר שורה
    while (std::getline(file, line))
    {
        // אופציונלי: חיתוך או הרחבה לרוחב המסך כדי למנוע בעיות תצוגה
        if (line.size() < Screen::WIDTH) {
            line.resize(Screen::WIDTH, ' ');
        }

        newLayout.push_back(line);
    }

    file.close();
    screenToFill.setLayout(newLayout);
}
// --- הפונקציה הראשית לטעינת השלב ---
void LevelLoader::loadLevelFromFile(
    const std::string& fileName,
    Screen& screenToFill,
    std::vector<GameObject*>& gameObjects)
{
    std::ifstream file(fileName);

    // בדיקת שגיאה בפתיחה
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return;
    }

    std::vector<std::string> newLayout;
    std::string line;
    int row = 0;

    // ברירת מחדל: החדר מואר
    screenToFill.setDark(false);

	//reading the first line without processing(objects or players)
	std::getline(file, line);
    newLayout.push_back(line);
    row++;

    while (std::getline(file, line))
    {
        // --- חלק א: קריאת המפה (שורות 0-24) ---
        if (row < Screen::HEIGHT)
        {
            // מוודאים שהשורה ברוחב המתאים
            if (line.size() < Screen::WIDTH) {
                line.resize(Screen::WIDTH, ' ');
            }

            // סריקה תו-תו
            for (size_t col = 0; col < line.size(); ++col)
            {
                char c = line[col];

                if (c != ' ' && c != '#')
                {
                    // 1. טיפול בשחקנים
                    if (c == '$' || c == '&')
                    {
                        handlePlayerStartPosition(c, (int)col, row, screenToFill);
                        line[col] = ' '; // מנקים מהקיר
                    }
                    else {
                        createObject(c, (int)col, row, screenToFill.getScreenId(), gameObjects);
                        line[col] = ' '; // מנקים מהקיר
                    }
                }
            }
            newLayout.push_back(line);
        }
        // --- חלק ב: קריאת נתונים נוספים (Metadata) ---
        else
        {
            // בדיקה האם החדר חשוך
            if (line.find("DARK") != std::string::npos) {
                screenToFill.setDark(true);
            }
        }
      row++;
    }

    file.close();

    // עדכון המסך בציור הסופי
    screenToFill.setLayout(newLayout);
}