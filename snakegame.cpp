#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <cctype>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;
const int MAX_SNAKE = WIDTH * HEIGHT;

int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];
int snakeLength;
int foodX, foodY;
int score, highScore;
int direction;
bool gameOver;
bool paused;

enum Direction {
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Load saved high score
void loadHighScore() {
    ifstream file("highscore.txt");
    highScore = 0;

    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

// Save high score to a file
void saveHighScore() {
    ofstream file("highscore.txt");

    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

// Generate food outside the snake
void spawnFood() {
    bool occupied;

    do {
        occupied = false;
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;

        for (int i = 0; i < snakeLength; i++) {
            if (snakeX[i] == foodX &&
                snakeY[i] == foodY) {
                occupied = true;
                break;
            }
        }
    } while (occupied);
}

// Initialize a new game
void setup() {
    score = 0;
    snakeLength = 3;
    direction = STOP;
    gameOver = false;
    paused = false;

    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;

    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = startX - i;
        snakeY[i] = startY;
    }

    spawnFood();
}

// Draw the game board
void draw() {
    system("cls");

    cout << "========== SNAKE GAME ==========\n";
    cout << "Score: " << score
         << "   High Score: " << highScore << "\n";
    cout << "Controls: W A S D / Arrow Keys | P: Pause | X: Exit\n";

    for (int y = -1; y <= HEIGHT; y++) {
        for (int x = -1; x <= WIDTH; x++) {

            if (x == -1 || x == WIDTH ||
                y == -1 || y == HEIGHT) {
                cout << "#";
            }
            else if (x == foodX && y == foodY) {
                cout << "*";
            }
            else if (x == snakeX[0] && y == snakeY[0]) {
                cout << "O";
            }
            else {
                bool body = false;

                for (int i = 1; i < snakeLength; i++) {
                    if (snakeX[i] == x &&
                        snakeY[i] == y) {
                        body = true;
                        break;
                    }
                }

                if (body)
                    cout << "o";
                else
                    cout << " ";
            }
        }
        cout << '\n';
    }

    if (paused)
        cout << "\nGAME PAUSED! Press P to resume.\n";
}

// Read keyboard input
void input() {
    if (!_kbhit())
        return;

    int key = _getch();

    // Arrow keys return an extended-key prefix
    if (key == 0 || key == 224) {
        key = _getch();

        switch (key) {
            case 75:
                if (direction != RIGHT)
                    direction = LEFT;
                break;
            case 77:
                if (direction != LEFT)
                    direction = RIGHT;
                break;
            case 72:
                if (direction != DOWN)
                    direction = UP;
                break;
            case 80:
                if (direction != UP)
                    direction = DOWN;
                break;
        }
        return;
    }

    key = tolower(key);

    switch (key) {
        case 'a':
            if (direction != RIGHT)
                direction = LEFT;
            break;
        case 'd':
            if (direction != LEFT)
                direction = RIGHT;
            break;
        case 'w':
            if (direction != DOWN)
                direction = UP;
            break;
        case 's':
            if (direction != UP)
                direction = DOWN;
            break;
        case 'p':
            paused = !paused;
            break;
        case 'x':
            gameOver = true;
            break;
    }
}

// Move snake and check collisions
void logic() {
    if (paused || direction == STOP)
        return;

    int oldTailX = snakeX[snakeLength - 1];
    int oldTailY = snakeY[snakeLength - 1];

    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    switch (direction) {
        case LEFT:
            snakeX[0]--;
            break;
        case RIGHT:
            snakeX[0]++;
            break;
        case UP:
            snakeY[0]--;
            break;
        case DOWN:
            snakeY[0]++;
            break;
    }

    // Wall collision
    if (snakeX[0] < 0 || snakeX[0] >= WIDTH ||
        snakeY[0] < 0 || snakeY[0] >= HEIGHT) {
        gameOver = true;
        return;
    }

    // Self collision
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] &&
            snakeY[0] == snakeY[i]) {
            gameOver = true;
            return;
        }
    }

    // Eat food
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        if (snakeLength < MAX_SNAKE) {
            snakeX[snakeLength] = oldTailX;
            snakeY[snakeLength] = oldTailY;
            snakeLength++;
        }

        score += 10;

        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }

        // Winning condition
        if (snakeLength == MAX_SNAKE) {
            gameOver = true;
            return;
        }

        spawnFood();
    }
}

// Display final score and ask to replay
bool gameOverScreen() {
    system("cls");

    cout << "========== GAME OVER ==========\n";
    cout << "Final Score : " << score << '\n';
    cout << "High Score  : " << highScore << '\n';

    if (snakeLength == MAX_SNAKE)
        cout << "Congratulations! You won!\n";

    cout << "\nPlay Again? (Y/N): ";

    char choice;
    cin >> choice;

    return tolower(choice) == 'y';
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    loadHighScore();

    cout << "================================\n";
    cout << "       SNAKE GAME IN C++\n";
    cout << "================================\n";
    cout << "Eat food (*) to increase your score.\n";
    cout << "Avoid the walls and your own body.\n";
    cout << "\nPress any key to start...";

    _getch();

    do {
        setup();

        while (!gameOver) {
            draw();
            input();
            logic();

            if (!paused)
                Sleep(120);
            else
                Sleep(50);
        }

    } while (gameOverScreen());

    cout << "\nThanks for playing!\n";
    return 0;
}
