#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// SNAKE (WINDOW_WIDTH/CELL_SIZE)*(WINDOW_HEIGHT/CELL_SIZE) maybe need be a very good player to can be a window snake
#define SNAKE_LENGTH 1200

//THE BLOCK OF THINGS
#define CELL_SIZE 20
//there is a raylib func named void SetTargetFPS(int fps); But I dontunderstand if is the max of cpu frame, or anything else, like display. So i maked it fix
#define FPS 60

/*
    0.133f; because there is a calculate, 60 fps, each frame in 0.0166s, so 0.133/0.0166, so 8 frames. another value can make a frame "broke", like 7.3frames.
    Yes, there was a bug.
*/
const float INIT_SPEED = 0.133f;

bool gameOver = false;
bool allowMove = true; // Prevents the rapid double-keypress suicide bug

/*
    struct Timer_t, StartTimer and UpdateTimer to get the frame. In this model the Snake can move in 0s, INIT in 0.133f
*/
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

//Vector2 is a 2 components vector<>
struct Snake {
    Vector2 body[SNAKE_LENGTH];
    int bodyLength = 3;  //Gemini was told to remove of struct, because can broke 'C', i dont understand why, there is no bugs, maybe about older 256 lenght
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
    //util area to create a food, 40 blocks - 2 "blocks" to not live in maxX, one block before to limit 
    int maxX = (WINDOW_WIDTH / CELL_SIZE) - 2;
    int maxY = (WINDOW_HEIGHT / CELL_SIZE) - 2;
    //sort for (i = 1; i < maxX || maxY; i++) -> sort position
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
