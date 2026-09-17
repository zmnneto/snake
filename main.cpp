#include "raylib.h"
#include <cstdlib>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SNAKE_LENGTH 256
#define CELL_SIZE 20

#define FPS 60

bool gameOver = false;

typedef struct Timer {
    float lifetime;
} Timer;

void StartTimer(Timer *timer, float lifetime) {
    if (timer != NULL) { timer->lifetime = lifetime; }
}

void UpdateTimer(Timer *timer) {
    if (timer != NULL) {
        timer->lifetime -= GetFrameTime();
    }
}

bool TimerDone(Timer *timer) {
    if (timer != NULL) { return timer->lifetime <= 0; }
    return false;
}

typedef struct Snake {
    Vector2 body[SNAKE_LENGTH]; //TEM QUE SER SEPARADO PARA CURVAS, SE NAO SERIA UMA CABEÇA GIGANTE
    int bodyLength = 0; // o body
    // Vector2 position;
    Vector2 size;
    Vector2 direction;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    Color color;
} Food;

typedef struct Score_t {
    int score = {0};
    float speed = 0.15f;
} Score_t;

void RespawnFood(Food *food) {
    int maxX = (WINDOW_WIDTH / CELL_SIZE) - 1;
    int maxY = (WINDOW_HEIGHT / CELL_SIZE) - 1;
    food->position.x = GetRandomValue(0, maxX) * CELL_SIZE;
    food->position.y = GetRandomValue(0, maxY) * CELL_SIZE;
}

void DrawFood(Food food) {
    DrawRectangleV(food.position, food.size, food.color);
}

void RespawnSnake(Snake *snake) {
    int maxX = (WINDOW_WIDTH / CELL_SIZE) - 1;
    int maxY = (WINDOW_HEIGHT / CELL_SIZE) - 1;

    snake->body[0].x = GetRandomValue(0, maxX) * CELL_SIZE;
    snake->body[0].y = GetRandomValue(0, maxY) * CELL_SIZE;

    snake->bodyLength = 1;

    snake->direction = (Vector2){1, 0}; // Começa parada
}

void DrawSnake(Snake snake) {
    for (int i = 0; i < snake.bodyLength; i++) {
        DrawRectangleV(snake.body[i], snake.size, snake.color);
    }
}

void UpdateSnakeDirection(Snake *snake) {
    if (IsKeyPressed(KEY_RIGHT) && snake->direction.x == 0) {
        snake->direction = (Vector2){1, 0};
    }
    if (IsKeyPressed(KEY_LEFT) && snake->direction.x == 0) {
        snake->direction = (Vector2){-1, 0};
    }
    // Y negativo = Para Cima
    if (IsKeyPressed(KEY_UP) && snake->direction.y == 0) {
        snake->direction = (Vector2){0, -1};
    }
    // Y positivo = Para Baixo
    if (IsKeyPressed(KEY_DOWN) && snake->direction.y == 0) {
        snake->direction = (Vector2){0, 1};
    }
}

void UpdateSnakeMoviment(Snake *snake) {
    //da calda para cabeça segue o bloco da frente
    for (int i = snake->bodyLength - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0].x += snake->direction.x * CELL_SIZE;
    snake->body[0].y += snake->direction.y * CELL_SIZE;
}

void CheckColision(Snake *snake) {
    //window
    if (snake->body[0].x < 0 || snake->body[0].x >= WINDOW_WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= WINDOW_HEIGHT) {
        gameOver = true;
        return;
    }
    //snake
    if (snake->body[0].x == snake->body[SNAKE_LENGTH].x) {
        gameOver = true;
        return;
    }
}

void ResetGame(Snake *snake, Food *food, Timer *moveTimer) {
    if (gameOver == true) {
        RespawnSnake(snake);
        RespawnFood(food);
        StartTimer(moveTimer, 0.15f);
        gameOver = false;
    }
}

void EatFood(Snake *snake, Food *food, Score_t *s) {
    //aparentemente isso seria o Vector2, basicamente .x && .y
    if (snake->body[0].x == food->position.x &&
        snake->body[0].y == food->position.y) {
        RespawnFood(food);

        if (snake->bodyLength < SNAKE_LENGTH) {
            snake->bodyLength++;
        }
        s->score += 5;
    }
}

void DrawScore(Score_t s) {
    DrawText(TextFormat("Pointing: %d", s.score), 10, 10, 20, WHITE);
}


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SNAKE");
    SetTargetFPS(FPS);

    Score_t score = {0};

    Timer moveTimer = {0};
    StartTimer(&moveTimer, 0.15f);

    Food food = {0};
    food.size = (Vector2){CELL_SIZE, CELL_SIZE};
    food.color = RED;
    RespawnFood(&food);

    Snake snake = {0};
    snake.size = (Vector2){CELL_SIZE, CELL_SIZE};
    snake.color = WHITE;
    RespawnSnake(&snake);

    while (!WindowShouldClose()) {
        if (!gameOver) {
            UpdateSnakeDirection(&snake);

            // 2. Atualização da lógica de jogo / movimentação
            UpdateTimer(&moveTimer);
            if (TimerDone(&moveTimer)) {
                UpdateSnakeMoviment(&snake);
                EatFood(&snake, &food, &score);
                CheckColision(&snake);

                StartTimer(&moveTimer, 0.15f);
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                score.score = 0;
                ResetGame(&snake, &food, &moveTimer);
            }
        }

        // 3. Desenho
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFood(food);
        DrawSnake(snake);
        DrawScore(score);

        if (gameOver) {
            DrawText("Perdeu, Enter ou Espaço pra reiniciar.",
                     WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 - 40, 40, RED);
        }
        DrawText("Pressione as setas para mover | ESPACO para resetar", 10, 550, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
