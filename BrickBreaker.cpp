#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <string>
#include "BrickBreaker.h"
using namespace std;
// Position class for x,y coordinates
class Position {
public:
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

// GameObject base class
class GameObject {
protected:
    Position position;
    char symbol;
    bool active;

public:
    GameObject(int x, int y, char symbol) : position(x, y), symbol(symbol), active(true) {}

    virtual ~GameObject() {}

    Position getPosition() const { return position; }
    char getSymbol() const { return symbol; }
    bool isActive() const { return active; }
    void setActive(bool status) { active = status; }

    virtual void update() {}
    virtual void render(CHAR_INFO* buffer, int width) const {
        if (active) {
            int index = position.y * width + position.x;
            if (index >= 0 && index < width * 25) {
                buffer[index].Char.AsciiChar = symbol;
                buffer[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            }
        }
    }
};

// Ball class
class Ball : public GameObject {
private:
    int dx, dy; // Velocity
    int moveCounter; // Counter for slowing ball movement
    int moveSpeed; // How many frames to wait before moving

public:
    Ball(int x, int y) : GameObject(x, y, 'O'), dx(1), dy(-1), moveCounter(0), moveSpeed(3) {}

    void setDirection(int newDx, int newDy) {
        dx = newDx;
        dy = newDy;
    }

    void reverseX() { dx = -dx; }
    void reverseY() { dy = -dy; }

    void update() override {
        // Only move every moveSpeed frames
        moveCounter++;
        if (moveCounter >= moveSpeed) {
            position.x += dx;
            position.y += dy;
            moveCounter = 0;
        }
    }

    int getDx() const { return dx; }
    int getDy() const { return dy; }
};

// Paddle class
class Paddle : public GameObject {
private:
    int width;

public:
    Paddle(int x, int y, int width) : GameObject(x, y, '='), width(width) {}

    void moveLeft() {
        if (position.x > 1) position.x--;
    }

    void moveRight(int maxX) {
        if (position.x + width < maxX - 1) position.x++;
    }

    void render(CHAR_INFO* buffer, int bufferWidth) const override {
        if (active) {
            for (int i = 0; i < width; i++) {
                int index = position.y * bufferWidth + (position.x + i);
                if (index >= 0 && index < bufferWidth * 25) {
                    buffer[index].Char.AsciiChar = symbol;
                    buffer[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                }
            }
        }
    }

    int getWidth() const { return width; }
};

// Brick class
class Brick : public GameObject {
private:
    int width;
    int strength;
    WORD color;

public:
    Brick(int x, int y, int width, int strength) : GameObject(x, y, '#'), width(width), strength(strength) {
        // Set color based on strength
        switch (strength) {
            case 3: color = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
            case 2: color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case 1: color = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            default: color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
    }

    void hit() {
        strength--;
        if (strength <= 0) {
            active = false;
        } else {
            // Update color based on new strength
            switch (strength) {
                case 2: color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
                case 1: color = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
                default: color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            }
        }
    }

    void render(CHAR_INFO* buffer, int bufferWidth) const override {
        if (active) {
            for (int i = 0; i < width; i++) {
                int index = position.y * bufferWidth + (position.x + i);
                if (index >= 0 && index < bufferWidth * 25) {
                    buffer[index].Char.AsciiChar = symbol;
                    buffer[index].Attributes = color;
                }
            }
        }
    }

    int getWidth() const { return width; }
    int getStrength() const { return strength; }
};

// Game class to manage the game
class BrickBreaker {
private:
    const int WIDTH = 80;
    const int HEIGHT = 25;
    Ball ball;
    Paddle paddle;
    vector<Brick> bricks;
    bool gameOver;
    int score;
    int lives;
    int level;
    bool paused;

    // Console buffer
    HANDLE console;
    CHAR_INFO* screenBuffer;
    COORD bufferSize;
    COORD bufferCoord;
    SMALL_RECT writeRegion;

    void setupConsole() {
        console = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

        if (console == INVALID_HANDLE_VALUE) {
            cerr << "Error creating console buffer" << endl;
            exit(1);
        }

        SetConsoleActiveScreenBuffer(console);

        bufferSize.X = WIDTH;
        bufferSize.Y = HEIGHT;
        bufferCoord.X = 0;
        bufferCoord.Y = 0;

        writeRegion.Left = 0;
        writeRegion.Top = 0;
        writeRegion.Right = WIDTH - 1;
        writeRegion.Bottom = HEIGHT - 1;

        screenBuffer = new CHAR_INFO[WIDTH * HEIGHT];

        CONSOLE_CURSOR_INFO cursorInfo;
        cursorInfo.dwSize = 100;
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(console, &cursorInfo);
    }

    void setup() {
        ball = Ball(WIDTH / 2, HEIGHT - 3);
        paddle = Paddle(WIDTH / 2 - 5, HEIGHT - 2, 10);

        gameOver = false;
        score = 0;
        lives = 3;
        level = 1;
        paused = false;

        setupConsole();
        createBricks();
    }

    void createBricks() {
        bricks.clear();

        int brickWidth = 8;
        int gap = 2;
        int rowCount = 3;

        for (int row = 0; row < rowCount; row++) {
            for (int col = 0; col < (WIDTH - 4) / (brickWidth + gap); col++) {
                int x = col * (brickWidth + gap) + 2;
                int y = row + 2;
                int strength = rowCount - row;

                bricks.push_back(Brick(x, y, brickWidth, strength));
            }
        }
    }

    void clearBuffer() {
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            screenBuffer[i].Char.AsciiChar = ' ';
            screenBuffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
    }

    void drawBorder() {
        // Draw top and bottom borders
        for (int i = 0; i < WIDTH; i++) {
            screenBuffer[i].Char.AsciiChar = '-';
            screenBuffer[(HEIGHT - 1) * WIDTH + i].Char.AsciiChar = '-';
        }

        // Draw side borders
        for (int i = 0; i < HEIGHT; i++) {
            screenBuffer[i * WIDTH].Char.AsciiChar = '|';
            screenBuffer[i * WIDTH + WIDTH - 1].Char.AsciiChar = '|';
        }
    }

    void drawScore() {
        string scoreText = "Score: " + to_string(score) + " Lives: " + to_string(lives) + " Level: " + to_string(level);

        for (size_t i = 0; i < scoreText.length(); i++) {
            int index = (HEIGHT - 1) * WIDTH + 5 + i;
            if (index < WIDTH * HEIGHT) {
                screenBuffer[index].Char.AsciiChar = scoreText[i];
                screenBuffer[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            }
        }
    }

    void resetBall() {
        ball = Ball(WIDTH / 2, HEIGHT - 3);
        renderFrame();
        Sleep(1000);
    }

    void handleCollisions() {
        Position ballPos = ball.getPosition();

        // Check for collisions with walls
        if (ballPos.x <= 1 || ballPos.x >= WIDTH - 2) {
            ball.reverseX();
        }

        if (ballPos.y <= 1) {
            ball.reverseY();
        }

        if (ballPos.y >= HEIGHT - 1) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
            } else {
                resetBall();
            }
        }

        // Check for collisions with paddle
        Position paddlePos = paddle.getPosition();
        if (ballPos.y == paddlePos.y &&
            ballPos.x >= paddlePos.x &&
            ballPos.x < paddlePos.x + paddle.getWidth()) {

            ball.reverseY();
        }

        // Check for collisions with bricks
        for (auto& brick : bricks) {
            if (brick.isActive()) {
                Position brickPos = brick.getPosition();

                if (ballPos.y == brickPos.y &&
                    ballPos.x >= brickPos.x &&
                    ballPos.x < brickPos.x + brick.getWidth()) {

                    int prevStrength = brick.getStrength();
                    brick.hit();
                    ball.reverseY();
                    score += 10 * prevStrength;
                    break;
                }
            }
        }

        // Check if all bricks are destroyed
        bool allDestroyed = true;
        for (const auto& brick : bricks) {
            if (brick.isActive()) {
                allDestroyed = false;
                break;
            }
        }

        if (allDestroyed) {
            level++;
            createBricks();
            resetBall();
            score += 50;
        }
    }

    void renderFrame() {
        clearBuffer();
        drawBorder();
        drawScore();

        ball.render(screenBuffer, WIDTH);
        paddle.render(screenBuffer, WIDTH);

        for (const auto& brick : bricks) {
            brick.render(screenBuffer, WIDTH);
        }

        if (paused) {
            string pauseText = "PAUSED - Press P to continue";
            int startX = (WIDTH - pauseText.length()) / 2;
            int y = HEIGHT / 2;

            for (size_t i = 0; i < pauseText.length(); i++) {
                int index = y * WIDTH + startX + i;
                if (index >= 0 && index < WIDTH * HEIGHT) {
                    screenBuffer[index].Char.AsciiChar = pauseText[i];
                    screenBuffer[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                }
            }
        }

        WriteConsoleOutput(
            console,
            screenBuffer,
            bufferSize,
            bufferCoord,
            &writeRegion
        );
    }

    void handleInput() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'a': case 'A': case 75: // Left arrow
                    paddle.moveLeft();
                    break;
                case 'd': case 'D': case 77: // Right arrow
                    paddle.moveRight(WIDTH);
                    break;
                case 'p': case 'P':
                    paused = !paused;
                    break;
                case 27: // ESC
                    gameOver = true;
                    break;
            }
        }
    }

    void update() {
        if (!paused) {
            ball.update();
            handleCollisions();
        }
    }

public:
    BrickBreaker() : ball(0, 0), paddle(0, 0, 0), gameOver(false), score(0), lives(3), level(1), paused(false), screenBuffer(nullptr) {
        setup();
    }

    ~BrickBreaker() {
        if (screenBuffer) {
            delete[] screenBuffer;
        }
        HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleActiveScreenBuffer(stdOutputHandle);
    }
    void showIntro() {
        SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));  // Make sure we're on the default buffer
        system("cls");
        cout << "=======================================" << endl;
        cout << "          BRICK BREAKER GAME           " << endl;
        cout << "=======================================" << endl;
        cout << "Controls:" << endl;
        cout << "  A or Left Arrow - Move paddle left" << endl;
        cout << "  D or Right Arrow - Move paddle right" << endl;
        cout << "  P - Pause game" << endl;
        cout << "  ESC - Quit game" << endl;
        cout << "\nPress any key to start..." << endl;
        _getch();  // Wait for user
    }

    void play() {
        srand(static_cast<unsigned int>(time(nullptr)));
        SetConsoleTitle("Brick Breaker Game");
        showIntro();
        setup();
        while (!gameOver) {
            handleInput();
            update();
            renderFrame();
            Sleep(50);  // Simple frame rate control
        }

        // Game over screen
        clearBuffer();

        string gameOverText = "GAME OVER!";
        string scoreText = "Final Score: " + to_string(score);
        string exitText = "Press any key to exit...";

        int centerX = WIDTH / 2;
        int centerY = HEIGHT / 2;

        // Draw game over message
        for (size_t i = 0; i < gameOverText.length(); i++) {
            int index = centerY * WIDTH + (centerX - gameOverText.length() / 2 + i);
            if (index >= 0 && index < WIDTH * HEIGHT) {
                screenBuffer[index].Char.AsciiChar = gameOverText[i];
                screenBuffer[index].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
            }
        }

        // Draw score
        for (size_t i = 0; i < scoreText.length(); i++) {
            int index = (centerY + 1) * WIDTH + (centerX - scoreText.length() / 2 + i);
            if (index >= 0 && index < WIDTH * HEIGHT) {
                screenBuffer[index].Char.AsciiChar = scoreText[i];
                screenBuffer[index].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
        }

        // Draw exit message
        for (size_t i = 0; i < exitText.length(); i++) {
            int index = (centerY + 3) * WIDTH + (centerX - exitText.length() / 2 + i);
            if (index >= 0 && index < WIDTH * HEIGHT) {
                screenBuffer[index].Char.AsciiChar = exitText[i];
                screenBuffer[index].Attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            }
        }

        WriteConsoleOutput(
            console,
            screenBuffer,
            bufferSize,
            bufferCoord,
            &writeRegion
        );

        _getch();
    }
};

void BrickBreakerGame::run() {
    BrickBreaker game;
    game.play();

}
