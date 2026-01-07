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

--- Architecture UPDATED FOR EX3 ---
* Game (Abstract Base Class):
  The central engine. Manages the core game loop, physics, rendering,
  screen transitions, and shared logic. It defines the virtual interface
  for input handling and event reporting.

* KeyBoardGame (Inherits from Game):
  Handles interactive gameplay.
  - inputs: From keyboard (_kbhit/_getch).
  - outputs: Visual rendering to screen.
  - recording: Writes inputs to `.steps` file and events to `.result` file (in -save mode).

* FileGame (Inherits from Game):
  Handles automated playback and testing.
  - inputs: Reads steps from `.steps` file.
  - outputs: Visual rendering (unless in -silent mode).
  - verification: Compares actual game events against expected `.result` file.

* LevelLoader (Static):
  Utility class for parsing `.screen` files and metadata.

* Main (Factory Pattern):
  Parses command line arguments and instantiates the correct Game derived class.

--- Game Modes & Usage ---
1. Regular Game:
   > adv-world.exe
   Plays normally with keyboard input.

2. Save Mode (Recording):
   > adv-world.exe -save
   Plays normally, but records every keystroke to `adv-world.steps`
   and every major event to `adv-world.result`.

3. Load Mode (Playback):
   > adv-world.exe -load
   Replays the game from the `adv-world.steps` file visually. Ignores keyboard input.

4. Silent Mode (Testing):
   > adv-world.exe -load -silent
   Runs the playback as fast as possible without rendering graphics.
   Reports "Test Passed" or "Test Failed" based on result verification.

--- Level Design ---
The game includes 3 distinct screens + 1 Victory screen:
1. Room 1: Intro to keys, doors, and springs.
2. Room 2: Dark Maze (requires Torch).
3. Room 3: Puzzle level (Switches, Obstacles, Teamwork).
4. Room 4: Victory.


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


 --- AI Assistance Declaration ---
AI tools (CoPilot/ChatGPT/Gemini) were used for development support:
- Generating initial class skeletons (Getters/Setters).
- Assisting with the double-buffer rendering logic.
- Debugging complex collision edge-cases (specifically Spring & Obstacle physics).
- Writing the README and documentation.
- Designing ASCII art for the Victory/Start screens.
- Refactoring code into the `LevelLoader` and `GameException` classes.
- Shuffle Riddles. 
- Sound syste basic function
-refactoring the 
- Refactoring the Game engine to support Polymorphism (Base Game class splitting).
- Implementing the Record/Replay logic (Catch-up mechanism for desync prevention).

All code was manually reviewed, integrated, and tested by us.