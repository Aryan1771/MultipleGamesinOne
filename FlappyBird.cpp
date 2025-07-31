#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "FlappyBird.h"
#define xSize 32
#define ySize 16
#define pipeCount 3
#define qKey 'Q'

using namespace std;

class GameObject {
protected:
    int x;
    int y;

public:
    GameObject(int startX = 0, int startY = 0) : x(startX), y(startY) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }

    virtual void update() = 0;
};

class Bird : public GameObject {
private:
    int velocity;

public:
    Bird(int startX = 10, int startY = 10) : GameObject(startX, startY), velocity(0) {}

    void flap() {
        y -= 2;
    }

    void update() override {
        y += 1;
    }

    void reset() {
        x = 10;
        y = 10;
        velocity = 0;
    }
};

class Pipe : public GameObject {
public:
    Pipe(int startX = 25, int startY = 5) : GameObject(startX, startY) {}

    void update() override {
        x--;
    }
};

class PipeManager {
private:
    Pipe pipes[pipeCount];

public:
    PipeManager() {
        reset();
    }

    void reset() {
        for (int i = 0; i < pipeCount; i++) {
            pipes[i].setX(25 + 15 * i);
            pipes[i].setY((rand() % 7) + 5);
        }
    }

    void update() {
        for (int i = 0; i < pipeCount; i++) {
            pipes[i].update();

            if (pipes[i].getX() == -1) {
                if (i == 0) {
                    pipes[i].setX(pipes[2].getX() + 15);
                } else {
                    pipes[i].setX(pipes[i - 1].getX() + 15);
                }
                pipes[i].setY((rand() % 7) + 5);
            }
        }
    }

    Pipe& getPipe(int index) {
        return pipes[index];
    }
};

class GameEngine {
private:
    Bird bird;
    PipeManager pipeManager;
    int frame;
    bool gameOver;

    void draw() {
        char buff[5000];
        strcpy(buff, "\e[17A");

        for (int y = 0; y <= ySize; y++) {
            for (int x = 0; x <= xSize; x++) {
                if (
                    y == 0 ||
                    y == ySize ||
                    x == 0 ||
                    x == xSize
                ) {
                    strcat(buff, "[]");
                    continue;
                }

                for (int i = 0; i < pipeCount; i++) {
                    Pipe& pipe = pipeManager.getPipe(i);
                    if (
                        pipe.getX() >= x - 1 &&
                        pipe.getX() <= x + 1 &&
                        (
                            pipe.getY() == y + 3 ||
                            pipe.getY() == y - 3
                        )
                    ) {
                        strcat(buff, "\033[32m[]");
                        goto bottom;
                    } else if (
                        pipe.getX() == x - 1 &&
                        pipe.getY() == y - 4
                    ) {
                        strcat(buff, "\033[32m]/");
                        goto bottom;
                    } else if (
                        pipe.getX() == x &&
                        (
                            pipe.getY() <= y - 4 ||
                            pipe.getY() >= y + 4
                        )
                    ) {
                        strcat(buff, "\033[32m][");
                        goto bottom;
                    } else if (
                        pipe.getX() == x + 1 &&
                        pipe.getY() == y - 4
                    ) {
                        strcat(buff, "\033[32m\\[");
                        goto bottom;
                    } else if (
                        pipe.getX() == x - 1 &&
                        pipe.getY() == y + 4
                    ) {
                        strcat(buff, "\033[32m]\\");
                        goto bottom;
                    } else if (
                        pipe.getX() == x + 1 &&
                        pipe.getY() == y + 4
                    ) {
                        strcat(buff, "\033[32m/[");
                        goto bottom;
                    } else if (
                        pipe.getX() == x + 1 &&
                        (
                            pipe.getY() <= y - 5 ||
                            pipe.getY() >= y + 5
                        )
                    ) {
                        strcat(buff, "\033[32m [");
                        goto bottom;
                    } else if (
                        pipe.getX() == x - 1 &&
                        (
                            pipe.getY() <= y - 5 ||
                            pipe.getY() >= y + 5
                        )
                    ) {
                        strcat(buff, "\033[32m] ");
                        goto bottom;
                    }
                }

                if (
                    bird.getY() == y &&
                    bird.getX() == x
                ) {
                    strcat(buff, "\033[33m)>");
                } else if (
                    bird.getY() == y &&
                    bird.getX() == x + 1
                ) {
                    strcat(buff, "\033[33m_(");
                } else if (
                    bird.getY() == y &&
                    bird.getX() == x + 2
                ) {
                    strcat(buff, "\033[33m _");
                } else if (
                    bird.getY() == y - 1 &&
                    bird.getX() == x
                ) {
                    strcat(buff, "\033[33m) ");
                } else if (
                    bird.getY() == y - 1 &&
                    bird.getX() == x + 1
                ) {
                    strcat(buff, "\033[33m__");
                } else if (
                    bird.getY() == y - 1 &&
                    bird.getX() == x + 2
                ) {
                    strcat(buff, "\033[33m \\");
                } else {
                    strcat(buff, "  ");
                }

            bottom:;
            }

            strcat(buff, "\n");
        }

        printf("%s", buff);
    }

    void checkCollisions() {
        if (bird.getY() == 15) {
            gameOver = true;
        }

        for (int i = 0; i < pipeCount; i++) {
            Pipe& pipe = pipeManager.getPipe(i);
            if (bird.getX() >= pipe.getX() - 2 && bird.getX() <= pipe.getX() + 2) {
                if (bird.getY() <= pipe.getY() - 3 || bird.getY() >= pipe.getY() + 3) {
                    gameOver = true;
                }
            }
        }
    }

public:
    GameEngine() : bird(10, 10), frame(0), gameOver(false) {}

    void reset() {
        bird.reset();
        pipeManager.reset();
        frame = 0;
        gameOver = false;
    }

    void initialize() {
        system("cls");

        cout << "Press UP to jump and Q to quit.\n";

        for (int i = 0; i <= ySize; i++) {
            cout << "\n";
        }

        draw();

        system("pause>nul");
    }

    void run() {
        while (!gameOver) {
            if (GetAsyncKeyState(VK_UP)) {
                bird.flap();
            }

            if (GetAsyncKeyState(qKey)) {
                break;
            }

            if (frame == 2) {
                bird.update();
                pipeManager.update();
                frame = 0;
            }

            checkCollisions();

            draw();

            frame++;
            Sleep(100);
        }
    }

    bool isGameOver() const {
        return gameOver;
    }
};

class FlappyBird {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    void HideCursor() {
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(console, &cursorInfo);
            cursorInfo.bVisible = false;
            SetConsoleCursorInfo(console, &cursorInfo);
        }
public:
    void play() {
        srand(static_cast<unsigned>(time(nullptr)));
        system("title \"Not Flappy Duck - OOP Edition\"");

        GameEngine game;

        while (true) {
            system("cls");
            cout << "##\n";
            cout << "#                                            #\n";
            cout << "#     *********          GAME BOY            #\n";
            cout << "#     *       *           made by:           #\n";
            cout << "#     *       *            Vansh             #\n";
            cout << "#     *       *                              #\n";
            cout << "#     *********    ENTER ANY KEY TO START    #\n";
            cout << "#     * @ @ @ *           GAME               #\n";
            cout << "#     * @ @ @ *       for Flappy Bird        #\n";
            cout << "#      *******                               #\n";
            cout << "#                                            #\n";
            cout << "##\n";
            HideCursor();

            system("pause>nul");

            game.reset();
            game.initialize();
            game.run();

            if (game.isGameOver()) {
                cout << "\nGame Over! Press any key to play again...\n";
                system("pause>nul");
            }
        }
    }
};

void FlappyBirdGame::run() {
    FlappyBird game;
    game.play();

}
