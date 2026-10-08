# MultipleGamesinOne

MultipleGamesinOne is a Windows console game hub written in C++. It brings several classic mini-games into one menu-driven application.

## Included Games

- Brick Breaker
- Pac-Man
- Snake
- Flappy Bird

## Features

- Main menu for selecting a game
- Separate source and header files for each game
- Console cursor control for smoother rendering
- Keyboard-based input through `conio.h`
- Windows console API usage for screen and cursor behavior
- Basic high-score file support
- Code::Blocks project files included

## Tech Stack

- C++
- Windows console APIs
- Code::Blocks project format

## Project Structure

```text
main.cpp                 Game hub entry point and menu
BrickBreaker.cpp/.h      Brick Breaker implementation
PacMan.cpp/.h            Pac-Man implementation
SnakeGame.cpp/.h         Snake implementation
FlappyBird.cpp/.h        Flappy Bird implementation
highscore.txt            Score storage
MultipleGamesinOne.cbp   Code::Blocks project file
```

## Requirements

Windows and a Windows-targeting C++ compiler are required. The manual command uses MinGW-w64 `g++`; Code::Blocks also needs a configured compiler toolchain. Run from a writable directory for score storage.

## Getting Started

### Option 1: Open in Code::Blocks

1. Open `MultipleGamesinOne.cbp` in Code::Blocks.
2. Build the project.
3. Run the generated executable from the IDE.

### Option 2: Compile manually on Windows

Compile all source files together with a Windows-compatible C++ compiler:

```powershell
g++ main.cpp BrickBreaker.cpp PacMan.cpp SnakeGame.cpp FlappyBird.cpp -o MultipleGamesinOne.exe
```

Run the game hub:

```powershell
.\MultipleGamesinOne.exe
```

## Platform support

This project uses Windows-specific headers such as `windows.h` and `conio.h`, so it is intended for Windows terminals.

## License

This repository is licensed under the GPL-3.0 license. See `LICENSE` for details.
