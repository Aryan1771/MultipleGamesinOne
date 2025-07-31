#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include "SnakeGame.h"
// Utility functions and enums
void setCursorPosition(int x, int y) {
    COORD position = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

enum class Color { BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, MAGENTA = 5,
                  YELLOW = 6, WHITE = 7, BRIGHT_GREEN = 10, BRIGHT_RED = 12,
                  BRIGHT_YELLOW = 14, BRIGHT_WHITE = 15 };

void setTextColor(Color textColor, Color bgColor = Color::BLACK) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          static_cast<int>(textColor) + (static_cast<int>(bgColor) << 4));
}

enum class Direction { UP, DOWN, LEFT, RIGHT };

// Game object classes
class GameObject {
protected:
    int x, y;
    char symbol;
    Color color;
public:
    GameObject(int initialX, int initialY, char sym, Color col = Color::WHITE)
        : x(initialX), y(initialY), symbol(sym), color(col) {}
    virtual ~GameObject() {}

    virtual void render() const = 0;

    int getX() const { return x; }
    int getY() const { return y; }
};

class Fruit : public GameObject {
private:
    std::vector<std::pair<char, Color>> fruitTypes;

public:
    Fruit(int mapWidth, int mapHeight)
        : GameObject(0, 0, 'F', Color::RED) {

        fruitTypes = {
            {'@', Color::RED},
            {'%', Color::YELLOW},
            {'O', Color::BRIGHT_RED},
            {'*', Color::MAGENTA},
            {'#', Color::GREEN}
        };

        changeFruitType();
        relocate(mapWidth, mapHeight);
    }

    void changeFruitType() {
        int randomIndex = rand() % fruitTypes.size();
        symbol = fruitTypes[randomIndex].first;
        color = fruitTypes[randomIndex].second;
    }

    void relocate(int mapWidth, int mapHeight) {
        x = rand() % (mapWidth - 4) + 2;
        y = rand() % (mapHeight - 4) + 2;
        changeFruitType();
    }

    void render() const override {
        setCursorPosition(x, y);
        setTextColor(color);
        std::cout << symbol;
        setTextColor(Color::WHITE);
    }
};

class SnakeSegment : public GameObject {
public:
    SnakeSegment(int initialX, int initialY, char sym = 'o', Color col = Color::GREEN)
        : GameObject(initialX, initialY, sym, col) {}

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void render() const override {
        setCursorPosition(x, y);
        setTextColor(color);
        std::cout << symbol;
        setTextColor(Color::WHITE);
    }
};

class Snake {
private:
    std::vector<SnakeSegment> body;
    Direction currentDirection;
    Direction nextDirection;
    bool isAlive;
    public:
    Snake(int initialX, int initialY)
        : currentDirection(Direction::RIGHT),
          nextDirection(Direction::RIGHT),
          isAlive(true) {
        body.push_back(SnakeSegment(initialX, initialY, 'O', Color::BRIGHT_GREEN));
        body.push_back(SnakeSegment(initialX - 1, initialY));
        body.push_back(SnakeSegment(initialX - 2, initialY));
    }

    void setDirection(Direction dir) {
        nextDirection = dir;
    }

    void move() {
        // Prevent 180 degree turns
        if (!((currentDirection == Direction::UP && nextDirection == Direction::DOWN) ||
              (currentDirection == Direction::DOWN && nextDirection == Direction::UP) ||
              (currentDirection == Direction::LEFT && nextDirection == Direction::RIGHT) ||
              (currentDirection == Direction::RIGHT && nextDirection == Direction::LEFT))) {
            currentDirection = nextDirection;
        }

        // Erase the tail
        setCursorPosition(body.back().getX(), body.back().getY());
        std::cout << " ";

        // Move body segments
        for (size_t i = body.size() - 1; i > 0; --i) {
            body[i].setPosition(body[i - 1].getX(), body[i - 1].getY());
        }

        // Move head
        int newX = body[0].getX();
        int newY = body[0].getY();

        switch (currentDirection) {
            case Direction::UP: --newY; break;
            case Direction::DOWN: ++newY; break;
            case Direction::LEFT: --newX; break;
            case Direction::RIGHT: ++newX; break;
        }

        body[0].setPosition(newX, newY);
    }

    void grow() {
        body.push_back(SnakeSegment(body.back().getX(), body.back().getY()));
    }

    void render() const {
        for (const auto& segment : body) {
            segment.render();
        }
    }

    bool checkCollisionWithSelf() {
        int headX = body[0].getX(), headY = body[0].getY();
        for (size_t i = 1; i < body.size(); ++i) {
            if (headX == body[i].getX() && headY == body[i].getY()) return true;
        }
        return false;
    }

    bool checkCollisionWithWall(int mapWidth, int mapHeight) {
        int headX = body[0].getX(), headY = body[0].getY();
        return (headX <= 0 || headX >= mapWidth - 1 || headY <= 0 || headY >= mapHeight - 1);
    }

    bool checkFruitCollision(const Fruit& fruit) {
        return (body[0].getX() == fruit.getX() && body[0].getY() == fruit.getY());
    }

    bool isSnakeAlive() const { return isAlive; }
    void setAlive(bool alive) { isAlive = alive; }
    size_t getLength() const { return body.size(); }

    bool isPositionOccupied(int x, int y) const {
        for (const auto& segment : body) {
            if (segment.getX() == x && segment.getY() == y) return true;
        }
        return false;
    }
};

class SnakeGam {
private:
    const int mapWidth;
    const int mapHeight;
    Snake snake;
    Fruit fruit;
    int score;
    int level;
    int frameCount;
    int speed;
    bool paused;

    void clearScreen() {
        // Clear screen with direct console functions instead of system("cls")
        COORD coordScreen = {0, 0};
        DWORD cCharsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dwConSize;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

        FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
        SetConsoleCursorPosition(hConsole, coordScreen);
    }

    void hideCursor() {
        CONSOLE_CURSOR_INFO cursor;
        cursor.dwSize = 100;
        cursor.bVisible = false;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
    }

    void drawBorder() {
        setTextColor(Color::CYAN);
        for (int i = 0; i < mapWidth; ++i) {
            setCursorPosition(i, 0);
            std::cout << "#";
            setCursorPosition(i, mapHeight - 1);
            std::cout << "#";
        }

        for (int i = 1; i < mapHeight - 1; ++i) {
            setCursorPosition(0, i);
            std::cout << "#";
            setCursorPosition(mapWidth - 1, i);
            std::cout << "#";
        }
        setTextColor(Color::WHITE);
    }

    void displayGameInfo() {
        // Display score and level in a simplified format
        setCursorPosition(mapWidth + 2, 2);
        setTextColor(Color::BRIGHT_WHITE);
        std::cout << "Score: " << score;

        setCursorPosition(mapWidth + 2, 3);
        std::cout << "Level: " << level;

        setCursorPosition(mapWidth + 2, 4);
        std::cout << "Length: " << snake.getLength();

        // Display controls
        setCursorPosition(mapWidth + 2, 6);
        setTextColor(Color::YELLOW);
        std::cout << "Controls: WASD, P-Pause, ESC-Exit";

        // Show paused message if game is paused
        if (paused) {
            setCursorPosition(mapWidth / 2 - 8, mapHeight / 2);
            setTextColor(Color::BRIGHT_YELLOW);
            std::cout << "*** GAME PAUSED ***";
        }

        setTextColor(Color::WHITE);
    }

    bool handleInput() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'w': case 'W': if (!paused) snake.setDirection(Direction::UP); break;
                case 's': case 'S': if (!paused) snake.setDirection(Direction::DOWN); break;
                case 'a': case 'A': if (!paused) snake.setDirection(Direction::LEFT); break;
                case 'd': case 'D': if (!paused) snake.setDirection(Direction::RIGHT); break;
                case 'p': case 'P':
                    paused = !paused;
                    // Clear pause message area when unpausing
                    if (!paused) {
                        setCursorPosition(mapWidth / 2 - 8, mapHeight / 2);
                        std::cout << "                 ";
                    }
                    break;
                case 27: return false; // ESC key
            }
        }
        return true;
    }

    int getCurrentSpeed() const {
        return std::max(30, speed - (level * 5));
    }

    void placeFruitSafely() {
        bool validPosition = false;
        while (!validPosition) {
            fruit.relocate(mapWidth, mapHeight);
            validPosition = !snake.isPositionOccupied(fruit.getX(), fruit.getY());
        }
    }

    void showStartScreen() {
        clearScreen();
        drawBorder();

        // Simple start screen with text only - no snake image
        setCursorPosition(mapWidth / 2 - 12, mapHeight / 2 - 2);
        setTextColor(Color::BRIGHT_GREEN);
        std::cout << "SNAKE GAME";

        setCursorPosition(mapWidth / 2 - 12, mapHeight / 2);
        setTextColor(Color::BRIGHT_WHITE);
        std::cout << "Press any key to start...";

        setTextColor(Color::WHITE);
        _getch(); // Wait for any key press
        clearScreen();
    }

public:
    SnakeGam(int width = 40, int height = 20)
        : mapWidth(width), mapHeight(height),
          snake(width / 2, height / 2), fruit(width, height),
          score(0), level(1), frameCount(0), speed(100), paused(false) {
        srand(static_cast<unsigned int>(time(nullptr)));
        hideCursor();
        placeFruitSafely();
    }

    void play() {
        // Show start screen and wait for key press
        SetConsoleTitle(TEXT("Snake Game"));
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SMALL_RECT windowSize = {0, 0, 79, 24};
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
        showStartScreen();

        // Start the game
        drawBorder();
        fruit.render();
        snake.render();
        displayGameInfo();

        while (snake.isSnakeAlive()) {
            if (!handleInput()) break; // Exit if ESC is pressed

            if (paused) {
                Sleep(50);
                continue;
            }

            frameCount++;
            if (frameCount >= getCurrentSpeed() / 10) {
                frameCount = 0;
                snake.move();

                if (snake.checkCollisionWithWall(mapWidth, mapHeight) ||
                    snake.checkCollisionWithSelf()) {
                    snake.setAlive(false);
                    break;
                }

                if (snake.checkFruitCollision(fruit)) {
                    score += 10;
                    snake.grow();

                    if (score % 50 == 0) {
                        level++;
                    }

                    placeFruitSafely();
                    fruit.render();

                    // Update score display immediately after getting fruit
                    displayGameInfo();
                }

                snake.render();
            }

            Sleep(10);
        }

        gameOver();
    }

    void gameOver() {
        setCursorPosition(mapWidth / 2 - 5, mapHeight / 2);
        setTextColor(Color::BRIGHT_RED);
        std::cout << "GAME OVER!";

        setCursorPosition(mapWidth / 2 - 10, mapHeight / 2 + 1);
        setTextColor(Color::BRIGHT_WHITE);
        std::cout << "Final Score: " << score;

        setCursorPosition(mapWidth / 2 - 15, mapHeight / 2 + 2);
        std::cout << "Press any key to exit...";

        setTextColor(Color::WHITE);
        _getch();
    }
};

void SnakeGame::run() {
    SnakeGam game;
    game.play();

}
