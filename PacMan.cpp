#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include "PacMan.h"
using namespace std;
const int width = 40;
const int height = 20;
struct Ghost {
    int x, y;
    int color;
    bool alive;
    Ghost(int _x, int _y, int _color) : x(_x), y(_y), color(_color), alive(true) {}
};
class PacMan{
    private:
        vector<Ghost> ghosts;
        bool gameOver, poweredUp;
        int pacmanX, pacmanY, score, highscore;
        int mouthState = 0;
        int powerTimer = 0;
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        char maze[height][width+1] = {
            "########################################",
            "#......................................#",
            "#.####.#####.##.#####.####.#####.#####.#",
            "#o####.#####.##.#####.####.#####.#####o#",
            "#.####.#####.##.#####.####.#####.#####.#",
            "#......................................#",
            "#.####.##.#######....##########.##.#####",
            "#.####.##.#######....##########.##.#####",
            "#......................................#",
            "#.####.##.##.####....##.##########.##.##",
            "#.####.##.##.####    ############.##..##",
            "#.............###GGGG##................#",
            "#...#####.##.##########.##.#####.#.....#",
            "#...#####.##.####..####.##.#####.#.....#",
            "#......................................#",
            "#.####.#####.##.#####.####.#####.#####.#",
            "#o####.#####.##.#####.####.#####.#####o#",
            "#......................................#",
            "#......................................#",
            "########################################"
        };
        void SetColor(int color) {
            SetConsoleTextAttribute(console, color);
        }

        void ResetColor() {
            SetConsoleTextAttribute(console, 15);
        }
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
        void Draw() {
            ClearScreen();
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (i == pacmanY && j == pacmanX) {
                        SetColor(14);
                        cout << (mouthState ? 'C' : 'O');
                        ResetColor();
                    } else {
                        bool ghostPrinted = false;
                        for (auto& g : ghosts) {
                            if (g.alive && g.x == j && g.y == i) {
                                SetColor(g.color);
                                cout << 'G';
                                ResetColor();
                                ghostPrinted = true;
                                break;
                            }
                        }
                        if (!ghostPrinted) {
                            if (maze[i][j] == '#') {
                                SetColor(9); // Blue walls
                                cout << maze[i][j];
                                ResetColor();
                            } else if (maze[i][j] == 'o') {
                                SetColor(12); // Red cherry
                                cout << 'o';
                                ResetColor();
                            } else {
                                cout << maze[i][j];
                            }
                        }
                    }
                }
                cout << endl;
            }
            cout << "SCORE: " << score << "  HIGHSCORE: " << highscore << endl;
            if (poweredUp) cout << "POWERED UP! Time left: " << powerTimer/10 << endl;
        }
        char direction = ' ';
        void Input() {
            if (_kbhit()) {
                int ch = _getch();

                if (ch == 224) {
                    // Arrow keys
                    ch = _getch();
                    switch (ch) {
                        case 72: direction = 'w'; break; // Up
                        case 80: direction = 's'; break; // Down
                        case 75: direction = 'a'; break; // Left
                        case 77: direction = 'd'; break; // Right
                    }
                } else {
                    // WASD keys
                    ch = tolower(ch);
                    if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd')
                        direction = ch;
                }
            }
        }
        void MovePacman() {
            int dx = 0, dy = 0;
            if (direction == 'a') dx = -1;
            else if (direction == 'd') dx = 1;
            else if (direction == 'w') dy = -1;
            else if (direction == 's') dy = 1;
            if (pacmanY + dy >= 0 && pacmanY + dy < height &&
                pacmanX + dx >= 0 && pacmanX + dx < width &&
                maze[pacmanY + dy][pacmanX + dx] != '#') {
                pacmanX += dx;
                pacmanY += dy;
                mouthState = !mouthState;
            }
        }
        void MoveGhosts() {
            for (auto& g : ghosts) {
                if (!g.alive) continue;
                int dx = 0, dy = 0;
                if (rand()%2) {
                    if (pacmanX < g.x) dx = -1;
                    else if (pacmanX > g.x) dx = 1;
                } else {
                    if (pacmanY < g.y) dy = -1;
                    else if (pacmanY > g.y) dy = 1;
                }
                if (maze[g.y+dy][g.x+dx] != '#' && !(g.x+dx == pacmanX && g.y+dy == pacmanY)) {
                    g.x += dx;
                    g.y += dy;
                }
            }
        }
        void Logic() {
            if (maze[pacmanY][pacmanX] == '.') {
                maze[pacmanY][pacmanX] = ' ';
                score += 10;
            }
            if (maze[pacmanY][pacmanX] == 'o') {
                maze[pacmanY][pacmanX] = ' ';
                poweredUp = true;
                powerTimer = 50;
                score += 50;
            }
            for (auto& g : ghosts) {
                if (g.x == pacmanX && g.y == pacmanY) {
                    if (poweredUp) {
                        g.alive = false;
                        score += 200;
                    } else {
                        gameOver = true;
                    }
                }
            }
            if (poweredUp) {
                powerTimer--;
                if (powerTimer <= 0) {
                    poweredUp = false;
                }
            }
            for (auto& g : ghosts) {
                if (!g.alive) {
                    g.x = 20;
                    g.y = 11;
                    g.alive = true;
                }
            }
            bool win = true;
            for (int i=0;i<height;i++) {
                for (int j=0;j<width;j++) {
                    if (maze[i][j] == '.' || maze[i][j] == 'o') {
                        win = false;
                        break;
                    }
                }
            }
            if (win) {
                system("cls");
                cout << "LEVEL CLEARED!" << endl;
                Sleep(2000);
                for (int i=0;i<height;i++) {
                    for (int j=0;j<width;j++) {
                        if (maze[i][j] == ' ') maze[i][j] = '.';
                    }
                }
            }
        }
        void LoadHighScore() {
            ifstream fin("highscore.txt");
            if (fin.is_open()) {
                fin >> highscore;
            }
            fin.close();
        }
        void SaveHighScore() {
            if (score > highscore) {
                ofstream fout("highscore.txt");
                fout << score;
                fout.close();
            }
        }
        void Menu() {
            system("cls");
            cout << "\n\n\n";
            SetColor(14);
            cout << "\t       PAC-MAN\n\n";
            SetColor(15);
            cout << "\t   Press P to Play\n";
            cout << "\t   Press E to Exit\n\n";
            SetColor(11);
            cout << "\t  Created by  Aryan\n";
            ResetColor();
            char choice;
            while (true) {
                if (_kbhit()) {
                    choice = _getch();
                    if (choice == 'p' || choice == 'P') {
                        break;
                    }
                    if (choice == 'e' || choice == 'E') {
                        exit(0);
                    }
                }
                Sleep(100);
            }
        }
        void DeathAnimation() {
            for (int i = 0; i < 3; i++) {
                system("cls");
                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        if (row == pacmanY && col == pacmanX) {
                            SetColor(14);
                            if (i == 0) cout << 'O';
                            else if (i == 1) cout << 'C';
                            else cout << '.';
                            ResetColor();
                        } else {
                            bool ghostHere = false;
                            for (auto& g : ghosts) {
                                if (g.alive && g.x == col && g.y == row) {
                                    SetColor(g.color);
                                    cout << 'G';
                                    ResetColor();
                                    ghostHere = true;
                                    break;
                                }
                            }
                            if (!ghostHere) {
                                if (maze[row][col] == '#') {
                                    SetColor(9);
                                    cout << maze[row][col];
                                    ResetColor();
                                } else {
                                    cout << maze[row][col];
                                }
                            }
                        }
                    }
                    cout << endl;
                }
                Sleep(500);
            }
        }
        void Setup() {
            gameOver = false;
            score = 0;
            poweredUp = false;
            pacmanX = 1;
            pacmanY = 1;
            ghosts.clear();
            ghosts.push_back(Ghost(20,11,12)); // Red
            ghosts.push_back(Ghost(21,11,13)); // Pink
            ghosts.push_back(Ghost(19,11,11)); // Blue
            ghosts.push_back(Ghost(18,11,14)); // Yellow
            Draw();
            SetColor(14);
            cout << "\n\n\t\tREADY!" << endl;
            ResetColor();
            Sleep(1000);
            LoadHighScore();
        }
    public:
        void play(){
            srand(time(0));
            SetConsoleTitle("Pac-Man");
            int ghostSpeed = 50;
            HideCursor();
            while (true) {
                Menu();
                Setup();
                while (!gameOver) {
                    Draw();
                    Input();
                    MovePacman();
                    MoveGhosts();
                    Logic();
                    Sleep(ghostSpeed);
                }
                DeathAnimation();
                SaveHighScore();
                system("cls");
                SetColor(12);
                cout << "\n\n\n\t\tGAME OVER!\n\n\n";
                cout << "\t      Your Score: "<<score<<endl;
                ResetColor();
                Sleep(5000);
            }
        }
};

void PacManGame::run() {
    PacMan game;
    game.play();

}
