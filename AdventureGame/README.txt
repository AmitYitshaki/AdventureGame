Student id: 322819483 Amit Yitshaki, 208296715 Niv Katz

Implemented Elements (Assignment Requirements)

The project includes all mandatory components:

Two players/Walls/Keys/Doors

Three screens: two playable rooms and a final victory screen.

Springs with full compression, loading, and launch behavior according to specification

Two optional items implemented: Torch and Riddle.

The project follows all structural requirements and avoids recursion.

Architecture Summary

Main modules:

Game:
Handles the update loop, rendering (double-buffered), input, screen transitions,
pause menu, and full "New Game" reset functionality.

Player:
Controls movement, spring-affected flight, lives, inventory, HUD position, and states
related to spring loading and flying.

GameObject (base class):
Provides polymorphic behavior for objects such as Door, Key, Torch, Riddle, Spring.
Each derived object implements its own collision logic.

Screen:
Stores the layout of each room, identifies walls, and supports dark-room mode.

Lighting System:
Distance-based visibility; Torch increases the light radius.

Point / Utils:
Responsible for console cursor control, coordinate handling, and drawing characters.

Spring Mechanics (Implemented Behavior)

The spring consists of three aligned segments next to a wall.

Compression occurs only when the player enters from the opposite direction of release.

During compression, segments visually collapse by changing their character to '_'.

When the player reaches the final segment, the spring becomes "loaded":
power = spring length
duration = power * power

Pressing STAY launches the player in the release direction.

Vertical drift (up/down) is allowed during flight.

Any collision with a wall or blocking object stops the launch immediately.

Momentum transfer to the second player upon collision was not implemented yet!

Riddle Behavior

The riddle is implemented as a collectible item.

Stepping on it adds the riddle to the player's inventory.

The game detects that a player carries a riddle and enters "Riddle Mode".

Only answers 1–4 are accepted.

A wrong answer reduces one life; a correct answer removes the riddle.

Character Choices (Documented Deviation)
Wall character used: '#'
Spring characters used: 'W' and 'w'

New Game Reset

Selecting "New Game" performs a full reset:

Use of AI Assistance

AI tools(coPilot/ChatGpt/Gemini) were used only for development support.
The following contributions were made:
Get and Set functions (just because its basic)

Assisted with designing and wiritng the double-buffer drawing mechanism.

Helped derive the formulas and logic used in the Torch lighting system.

made HomeScreen and final Screen artwork

Helped organize the code by grouping and reordering functions into a logical structure.

Ensured consistent English comments throughout the project.

Helped resolve a complex collision-handling issue in the Spring object.

Helped writing README file :)

All code was reviewed manually and integrated responsibly. 