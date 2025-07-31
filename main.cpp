#include <iostream>
#include <conio.h>
#include <windows.h>
#include "BrickBreaker/BrickBreaker.h"
#include "PacMan/PacMan.h"
#include "SnakeGame/SnakeGame.h"
#include "FlappyBird/FlappyBird.h"
using namespace std;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
void ClearScreen() {
    COORD topLeft = {0, 0};
    SetConsoleCursorPosition(console, topLeft);
}
void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(console, &cursorInfo);
}
void showMenu() {
    cout << "========== GAME HUB ==========" << endl;
    cout << "1. Brick Breaker" << endl;
    cout << "2. Pac Man" << endl;
    cout << "3. Snake" << endl;
    cout << "4. Flappy Bird" << endl;
    cout << "5. Exit" << endl;
    cout << "Select a game: ";
}

int main() {
    while (true) {
        showMenu();

        if (_kbhit()) {
            int ch = _getch();
            int choice = ch - '0';
            switch (choice) {
                case 1: {
                    BrickBreakerGame game;
                    game.run();
                    break;
                }
                case 2: {
                    PacManGame game;
                    game.run();
                    break;
                }
                case 3: {
                    SnakeGame game;
                    game.run();
                    break;
                }
                case 4: {
                    FlappyBirdGame game;
                    game.run();
                    break;
                }
                case 5:
                    cout << "Goodbye!" << endl;
                    return 0;
                default:
                    cout << "Invalid choice. Try again!" << endl;
            }
        }
        ClearScreen();
        HideCursor();
    }
}
