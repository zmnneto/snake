#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SNAKE_LENGTH 256
#define CELL_SIZE 20
#define FPS 60

const float INIT_SPEED = 0.133f;
bool gameOver = false;
bool allowMove = true; // Prevents the rapid double-keypress suicide bug

struct Timer {
    float lifetime;
};

void StartTimer(Timer *timer, float lifetime) {
    if (timer != nullptr) { timer->lifetime = lifetime; }
}

void UpdateTimer(Timer *timer) {
    if (timer != nullptr) {
        timer->lifetime -= GetFrameTime();
    }
}

bool TimerDone(Timer *timer) {
    if (timer != nullptr) { return timer->lifetime <= 0; }
    return false;
}

struct Snake {
    Vector2 body[SNAKE_LENGTH];
    int bodyLength = 3;
    Vector2 size;
    Vector2 direction;
    Color color;
};

struct Food {
    Vector2 position;
    Vector2 size;
    Color color;
};

struct Score_t {
    int score = 0;
    float speed = INIT_SPEED;
};

void RespawnFood(Food *food) {
    int maxX = (WINDOW_WIDTH / CELL_SIZE) - 2;
    int maxY = (WINDOW_HEIGHT / CELL_SIZE) - 2;
    food->position.x = GetRandomValue(1, maxX) * CELL_SIZE;
    food->position.y = GetRandomValue(1, maxY) * CELL_SIZE;
}

void DrawFood(Food food) {
    DrawRectangleV(food.position, food.size, food.color);
}

void RespawnSnake(Snake *snake) {
    int maxX = (WINDOW_WIDTH / CELL_SIZE) - 1;
    int maxY = (WINDOW_HEIGHT / CELL_SIZE) - 1;

    snake->bodyLength = 3;
    snake->direction = Vector2{1, 0}; // Starts moving Right

    // Ensure the snake spawns far enough from the left wall so the tail fits
    int startX = GetRandomValue(snake->bodyLength, maxX);
    int startY = GetRandomValue(0, maxY);

    // Initialize the head and the rest of the body correctly behind it
    for (int i = 0; i < snake->bodyLength; i++) {
        snake->body[i].x = (startX - i) * CELL_SIZE;
        snake->body[i].y = startY * CELL_SIZE;
    }
}

void DrawSnake(Snake snake) {
    for (int i = 0; i < snake.bodyLength; i++) {
        DrawRectangleV(snake.body[i], snake.size, snake.color);
    }
}

void UpdateSnakeDirection(Snake *snake) {
    // allowMove ensures we only register one direction change per grid step
    if (allowMove) {
        if (IsKeyPressed(KEY_RIGHT) && snake->direction.x == 0) {
            snake->direction = Vector2{1, 0};
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && snake->direction.x == 0) {
            snake->direction = Vector2{-1, 0};
            allowMove = false;
        }
        if (IsKeyPressed(KEY_UP) && snake->direction.y == 0) {
            snake->direction = Vector2{0, -1};
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && snake->direction.y == 0) {
            snake->direction = Vector2{0, 1};
            allowMove = false;
        }
    }
}

void UpdateSnakeMovement(Snake *snake) {
    // Tail follows the head
    for (int i = snake->bodyLength - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0].x += snake->direction.x * CELL_SIZE;
    snake->body[0].y += snake->direction.y * CELL_SIZE;
}

void CheckCollision(Snake *snake) {
    // Window boundaries collision
    if (snake->body[0].x < 0 || snake->body[0].x >= WINDOW_WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= WINDOW_HEIGHT) {
        gameOver = true;
        return;
    }

    // Self-collision check
    for (int i = snake->bodyLength - 1; i > 0; i--) {
        if (snake->body[0].x == snake->body[i].x &&
            snake->body[0].y == snake->body[i].y) {
            gameOver = true;
            return;
        }
    }
}

void ResetGame(Snake *snake, Food *food, Timer *moveTimer, Score_t *score) {
    if (gameOver) {
        RespawnSnake(snake);
        RespawnFood(food);
        score->score = 0;
        score->speed = INIT_SPEED;
        StartTimer(moveTimer, score->speed);
        gameOver = false;
        allowMove = true;
    }
}

void EatFood(Snake *snake, Food *food, Score_t *s) {
    if (snake->body[0].x == food->position.x &&
        snake->body[0].y == food->position.y) {

        RespawnFood(food);

        if (snake->bodyLength < SNAKE_LENGTH) {
            snake->bodyLength++;
        }

        s->score += 5;

        // Cap maximum speed so it doesn't become impossibly fast or negative
        if (s->speed > 0.05f) {
            s->speed -= 0.005f; // Slowed down the speed ramp up slightly
        }
    }
}

void DrawScore(Score_t s) {
    DrawText(TextFormat("Pontos: %d", s.score), 10, 10, 20, WHITE);
}


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SNAKE");
    SetTargetFPS(FPS);

    Score_t Score;

    Timer moveTimer = {0};
    StartTimer(&moveTimer, Score.speed);

    Food food = {0};
    food.size = Vector2{CELL_SIZE, CELL_SIZE};
    food.color = RED;
    RespawnFood(&food);

    Snake snake = {0};
    snake.size = Vector2{CELL_SIZE, CELL_SIZE};
    snake.color = WHITE;
    RespawnSnake(&snake);

    while (!WindowShouldClose()) {
        if (!gameOver) {
            UpdateSnakeDirection(&snake);

            // Update game logic / movement
            UpdateTimer(&moveTimer);
            if (TimerDone(&moveTimer)) {
                UpdateSnakeMovement(&snake);
                allowMove = true; // Snake has moved, allow a new direction input

                EatFood(&snake, &food, &Score);
                CheckCollision(&snake);

                StartTimer(&moveTimer, Score.speed);
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                ResetGame(&snake, &food, &moveTimer, &Score);
            }
        }

        // Drawing Phase
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFood(food);
        DrawSnake(snake);
        DrawScore(Score);

        if (gameOver) {
            const char* gameOverText = "Perdeu! Enter ou Espaço para reiniciar.";
            int textWidth = MeasureText(gameOverText, 30);
            DrawText(gameOverText, (WINDOW_WIDTH / 2) - (textWidth / 2), (WINDOW_HEIGHT / 2) - 15, 30, RED);
        }

        DrawText("Setas para mover | ESPACO para resetar", 10, WINDOW_HEIGHT - 30, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
