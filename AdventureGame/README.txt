Student id: 322819483 Amit Yitshaki, 208296715 Niv Katz

=============================================================
              ADVENTURE GAME - PROJECT SUMMARY
=============================================================

--- Implemented Elements ---
The project implements ALL required game elements as defined in Exercise 2:
1. Two Players: Independent movement and controls.
2. Static Elements: Walls ('#').
3. Interactive Items: Keys, Doors (1-9), Springs, Bombs, Switches - Laser, Obstacles.
4. Collectibles: Torch (lighting) and Riddle (logic puzzle).
5. Architecture: The solution strictly avoids recursion and manages memory safely.

--- Architecture Summary ---
* Game:
  The central engine. Manages the game loop, double-buffered rendering,
  screen transitions, input handling, and global state.

* Player:
  A standalone class (does NOT inherit from GameObject) representing the users.
  Manages physics, inventory, life, score, and movement logic.

* GameObject (Base Class):
  Base class for all interactive entities on the board.
  Derived classes: Bomb, Switch, Laser, Spring, Door, Key, Obstacle, Riddle.
  Note: Static walls are managed by the Screen class, not as GameObjects.

* LevelLoader (Static):
  A dedicated utility class for parsing `.screen` files. It handles both
  ASCII map generation and complex metadata parsing.

--- Game Mechanics: Springs ---
* Independence: Springs are logical objects that function independently of walls.
* Visuals: Springs compress visually ('W' -> 'w' -> '_') when stepped on.
* Loading: When a player reaches the spring's base (the last segment), they become "Loaded".
* Launching: A loaded player CANNOT move. They must press 'STAY' to release the spring.
* Flight: Launch power depends on spring length.

--- Room Descriptions (Level Design) ---
The game includes 4 distinct screens demonstrating different mechanics:

1. Room 1 - "The Playroom":
   A relatively simple introductory level. Introduces basic mechanics like
   keys, doors, and springs in a bright, open space.

2. Room 2 - "The Dark Maze":
   A complex maze level with "Dark Mode" enabled. Players must use Torches
   to navigate and find the exit, relying on memory and limited visibility.

3. Room 3 - "Teamwork Hall":
   A puzzle-heavy level designed around cooperation. Requires players to
   coordinate switch toggles and obstacle pushing to progress.

4. Room 4 - "Victory Screen":
   A celebration screen displaying the final status and offering a restart.

* Dynamic Legend: The HUD (Legend) position changes between levels to demonstrate
  flexibility. It is intentionally compact to maximize playable area.

--- LEVEL DESIGN GUIDE: How to Add New Screens ---
To extend the game with new levels, follow these 4 steps:

STEP 1: Create the File
   - Create a new text file named `adv-world_XX.screen` (e.g., `adv-world_05.screen`).
   - Ensure the file is 25 lines high and 80 characters wide.

STEP 2: Draw the Map (ASCII Art)
   - Use '#' for Walls.
   - Use '$' for Player 1 start pos, '&' for Player 2.
   - Use ' ' (Space) for empty floor.
   - Objects: 'K'=Key, '@'=Bomb, 'W'=Spring, '?'=Riddle, '/'=Switch.
   - Doors: Use digits '1'-'9' (The char 'D' is NOT used for doors).
   - Legend Area: Place 'L' at the top-left of the status bar area.

STEP 3: Add Metadata (Below the map)
   Only complex objects need metadata. Bombs, Obstacles, and Riddles DO NOT need metadata.
   Add commands at the bottom of the file:
   - Door:    DOOR_DATA <x> <y> <id> <target_screen_id> <is_locked(0/1)>
   - Key:     KEY_DATA <x> <y> <id>
   - Spring:  SPRING_DATA <x> <y> <direction(U/D/L/R)> <screen_id> <length>
   - Connect: CONNECT <switch_x> <switch_y> <target_x> <target_y>
   - Dark:    Add the word "DARK" to make the room require a torch.

STEP 4: Update the Code (Game.cpp)
   - Add a `LevelLoader::loadLevelFromFile(...)` call in `resetGame`.
   - Update the switch-case in `restartCurrentLevel`.
   - Add the new enum value to `ScreenID.h`.
   
 --- AI Assistance Declaration ---
AI tools (CoPilot/ChatGPT/Gemini) were used for development support:
- Generating initial class skeletons (Getters/Setters).
- Assisting with the double-buffer rendering logic.
- Debugging complex collision edge-cases (specifically Spring & Obstacle physics).
- Writing the README and documentation.
- Designing ASCII art for the Victory/Start screens.
- Refactoring code into the `LevelLoader` and `GameException` classes.
- Shuffle Riddles. 
- Sound system.

All code was manually reviewed, integrated, and tested by us.