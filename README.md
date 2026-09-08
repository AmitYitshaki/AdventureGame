# AdventureGame

A two-player, C++ console adventure game for Windows, built around a data-driven level format and a record/replay engine used for automated regression testing.

Originally built as a three-part C++ course project (MTA), then extended with CI and cleanup for portfolio use.

## Features

- **Two-player co-op** on one keyboard (WASD / IJKM), moving continuously until a wall, obstacle, or explicit "stay" input.
- **Game elements**: walls, keys, doors, pressure-triggered springs (compress-then-launch physics), bombs with radius damage, switches, obstacles, torches (dynamic lighting/fog of war), and logic riddles.
- **Data-driven levels**: rooms are loaded from `.screen` text files parsed by `LevelLoader`, not hardcoded — new rooms can be authored without touching engine code.
- **Save / load**: game state (position, inventory, score, lives) can be serialized to and restored from a save file.
- **Record & replay engine**: every run can be recorded to `adv-world.steps` (inputs) and `adv-world.result` (expected events). A separate playback mode replays those inputs and verifies the actual events against the recorded ones — this is what CI uses as an automated regression test (see below).

## Architecture

```
main.cpp
  └─ Game (abstract)          — game loop, physics, rendering, screen transitions
       ├─ KeyBoardGame        — live keyboard input; optionally records .steps/.result
       └─ FileGame            — replays a .steps file; verifies events against .result

GameObject (abstract)         — base for interactive entities
  ├─ Door, Key, Spring, Bomb, Switch, Laser, Obstacle, Torch, Riddle

LevelLoader (static)          — parses .screen level files into a Screen + GameObjects
Player                        — movement, physics, inventory, score/lives (not a GameObject)
```

`Game` defines the game loop and rendering once; `KeyBoardGame` and `FileGame` each plug in a different input source and a different way of handling events (live keyboard vs. scripted playback + verification). That split is what makes the replay-based tests possible without duplicating engine logic.

## Building

Requirements: Visual Studio 2022+ with the "Desktop development with C++" workload.

1. Open `AdventureGame/AdventureGame.sln` in Visual Studio.
2. Build `Debug|x64` (or `Release|x64`).
3. Run — level (`.screen`) and text data files are copied next to the executable by a post-build step.

## Running

```
adv-world.exe                 # normal two-player game
adv-world.exe -save           # normal game, records inputs/events to adv-world.steps/.result
adv-world.exe -load           # replays adv-world.steps visually
adv-world.exe -load -silent   # replays as fast as possible, no rendering, prints PASS/FAIL
```

`-load -silent` exits with code `0` on a pass and `1` on a mismatch or timeout, so it can be used as a CI gate.

## Automated testing

`ExampleRuns/` contains recorded scenarios (a `.steps` input log + a `.result` expected-event log each). The GitHub Actions workflow (`.github/workflows/build-and-test.yml`) builds the solution with MSBuild, then replays each scenario through `-load -silent` and fails the build if any scenario's actual events diverge from its recorded expectations.

## Known limitations / next steps

- Game entities are owned via raw `new`/`delete` (`std::vector<GameObject*>`) rather than smart pointers.
- Console I/O (`windows.h`/`conio.h`) is used directly in the engine rather than behind an abstraction, so the game is Windows-only.
- No unit tests below the full-scenario replay level yet.

## Credits

Built by Amit Yitshaki and Niv Katz.
