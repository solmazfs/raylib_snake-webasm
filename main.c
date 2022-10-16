/*******************************************************************************************
*
*   raylib study [main.c] - snake
*
*   Game originally created with raylib 4.2, last time updated with raylib 4.2
*
*   Game licensed under MIT.
*
*   Copyright (c) 2022 Fatih S. Solmaz (@solmazfs)
*
********************************************************************************************/

// no game over, no pause
// hi score +

#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#include "timer.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define _SCREEN_W 640
#define _SCREEN_H 360
#define _MAX_LEN 722 // 38*19

enum MOVE {UP,DOWN,LEFT,RIGHT};
RenderTexture2D target;
Rectangle source;
Rectangle dest;
Camera2D camera;

struct Board {
    Timer timer;
    float timer_life;
    int grid_row;
    int grid_col;
    int grid_step;
    int score;
    int hiscore;
}board;

struct Snake {
    Vector2 position;
    Vector2 new_direction;
    Vector2 arr[_MAX_LEN];
    Vector2 move_dir[4];
    Rectangle head;
    Color color;
    int size;
    bool move;
}snake;

struct Fruit {
    Vector2 position;
    Rectangle rec;
    Color color;
}fruit;

void UpdateDrawFrame(void);
void draw_grid(int col, int row);
void draw_walls();
void draw_score(int score);
void draw_hiscore(int score);
Vector2 fruit_spawn();
Vector2 snake_restart();

int main() {
    SetRandomSeed(1000);

    SetWindowState(FLAG_VSYNC_HINT);
    InitWindow(_SCREEN_W,_SCREEN_H,"snake");
    // texture
    int canvasWidth = _SCREEN_W;
    int canvasHeight = _SCREEN_H;

    target = (RenderTexture2D)LoadRenderTexture(canvasWidth,canvasHeight);
    source = (Rectangle){0,0,target.texture.width,-target.texture.height};
    dest = (Rectangle){0,0,_SCREEN_W,_SCREEN_H};

    camera = (Camera2D){0};
    camera.zoom = 1.0f;

    board.score = 0;
    board.hiscore = 0;

    // grid
    board.grid_step = 16;
    board.grid_row = 38;
    board.grid_col = 20;

    // timer
    board.timer_life = 0.2f; // speed
    board.timer = (Timer){0};

    // snake: movement
    snake.position = snake_restart();
    snake.new_direction = Vector2Zero();
    snake.head = (Rectangle){snake.position.x,snake.position.y,board.grid_step,board.grid_step};
    snake.arr[0] = snake.position;
    snake.color = DARKBLUE; // ?free
    snake.size = 0;
    snake.move = false;

    // fruit
    fruit.position = fruit_spawn();
    fruit.rec = (Rectangle){fruit.position.x,fruit.position.y,board.grid_step,board.grid_step};
    fruit.color = RED; // ?but why

    // move directions
    snake.move_dir[UP] = (Vector2){0,-1};
    snake.move_dir[DOWN] = (Vector2){0,1};
    snake.move_dir[LEFT] = (Vector2){-1,0};
    snake.move_dir[RIGHT] = (Vector2){1,0};

    int move_dir_size = sizeof(snake.move_dir)/sizeof(snake.move_dir[0]); // array size
    for (int i=0; i<move_dir_size; i++) {
        snake.move_dir[i] = Vector2Scale(snake.move_dir[i],board.grid_step);
    }

    start_timer(&board.timer, board.timer_life);

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetWindowPosition(0,0); // open window top-left corner
        SetTargetFPS(60);
        while(!WindowShouldClose()) {
            UpdateDrawFrame();
        }
    #endif
    // unload
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}

void UpdateDrawFrame(void) {
        // input
        if (IsKeyPressed(KEY_LEFT)
                && snake.new_direction.x != snake.move_dir[RIGHT].x
                && snake.move)
        {
            snake.new_direction = snake.move_dir[LEFT];
            snake.move = false;
        }
        if (IsKeyPressed(KEY_RIGHT)
                && snake.new_direction.x != snake.move_dir[LEFT].x
                && snake.move)
        {
            snake.new_direction = snake.move_dir[RIGHT];
            snake.move = false;
        }
        if (IsKeyPressed(KEY_UP)
                && snake.new_direction.y != snake.move_dir[DOWN].y
                && snake.move)
        {
            snake.new_direction = snake.move_dir[UP];
            snake.move = false;
        }
        if (IsKeyPressed(KEY_DOWN)
                && snake.new_direction.y != snake.move_dir[UP].y
                && snake.move)
        {
            snake.new_direction = snake.move_dir[DOWN];
            snake.move = false;
        }

        update_timer(&board.timer);
        if (timer_done(&board.timer)) {
            // snake move
            snake.position.x += snake.new_direction.x;
            snake.position.y += snake.new_direction.y;

            // tail logic
            for (int i=snake.size; i>0; i--) {
                snake.arr[i].x = snake.arr[i-1].x;
                snake.arr[i].y = snake.arr[i-1].y;
            }
            // must be after for loop
            snake.arr[0].x = snake.position.x;
            snake.arr[0].y = snake.position.y;

            // collision -> fruit
            snake.head = (Rectangle){snake.position.x,snake.position.y,board.grid_step,board.grid_step};
            fruit.rec = (Rectangle){fruit.position.x,fruit.position.y,fruit.rec.width,fruit.rec.height};
            if (CheckCollisionRecs(snake.head,fruit.rec)) {
                board.score+=2;
                snake.size++;
                fruit.position = fruit_spawn();
                // correcting instance tail start position
                snake.arr[snake.size].x = snake.arr[0].x;
                snake.arr[snake.size].y = snake.arr[0].y;
            }
            // collision -> body !starts from 1++ tail, 0 is head
            for (int i=1; i<snake.size; i++) {
                if (CheckCollisionRecs(snake.head,(Rectangle){snake.arr[i].x,snake.arr[i].y,board.grid_step,board.grid_step})) {
                    snake.position = snake_restart();
                    snake.new_direction = Vector2Zero();
                    fruit.position = fruit_spawn();
                    snake.size=0;
                    if (board.score > board.hiscore) {board.hiscore = board.score;}
                    board.score=0;
                }
            }
            // collision --> wall
            if (snake.position.x > board.grid_row*board.grid_step
                    || snake.position.x < board.grid_step
                    || snake.position.y > board.grid_col*board.grid_step
                    || snake.position.y < board.grid_step*2)
            {
                snake.new_direction = Vector2Zero();
                snake.position = snake_restart();
                fruit.position = fruit_spawn();
                snake.size = 0;
                if (board.score > board.hiscore) {board.hiscore = board.score;}
                board.score = 0;
            }

            snake.move = true;
            start_timer(&board.timer, board.timer_life);
        }

        BeginTextureMode(target);
            ClearBackground(BLACK);
            draw_grid(_SCREEN_W,_SCREEN_H);
            draw_walls();
            draw_score(board.score);
            draw_hiscore(board.hiscore);
            // draw apple
            DrawRectangleRounded(fruit.rec,1,1,RED);
            // draw head
            DrawRectangleV(snake.position,(Vector2){snake.head.width,snake.head.height},snake.color);
            // draw tail
            for (int i=snake.size; i>0; i--) {
                DrawRectangleV(snake.arr[i],(Vector2){snake.head.width,snake.head.height},DARKBLUE);
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(RED);
            BeginMode2D(camera);
                DrawTexturePro(target.texture,source,dest,Vector2Zero(),0,WHITE);
            EndMode2D();
            DrawFPS(4,4);
        EndDrawing();
}

// the functions are so messy but I feel too lazy to fix it.

void draw_grid(int col, int row) {
    Color rect_col = {248, 251, 243,10};
    // 32
    for (int i=0;i<col;i+=board.grid_step) {
        for (int j=0;j<row;j+=board.grid_step) {
            DrawRectangleLines(i,j,board.grid_step,board.grid_step,rect_col);
            DrawRectangleLines(i+board.grid_step,j+board.grid_step,board.grid_step,board.grid_step,rect_col);
        }
    }
}

void draw_walls() {
    Color wall_color = GRAY;
    DrawLineEx((Vector2){0,15},(Vector2){640,15},31,wall_color); // top
    DrawLineEx((Vector2){0,360-12},(Vector2){640,360-12},23,wall_color); // bottom
    DrawLineEx((Vector2){7,0},(Vector2){7,360},16,wall_color);
    DrawLineEx((Vector2){640-7,0},(Vector2){640-7,360},16,wall_color);
    DrawLine(16,31,640-16,31,WHITE); // top
    DrawLine(16,360-24,640-16,360-24,WHITE); // bottom
    DrawLine(16,31,16,360-23,WHITE); // left
    DrawLine(640-15,31,640-15,360-23,WHITE); // right
}

void draw_score(int score) {
    char *text = "SCORE: %i";
    double x = (float)640/2 - (float)MeasureText(TextFormat(text,score),24)/2;
    double w = (float)MeasureText(TextFormat(text,score),24)+9.0f;
    DrawRectangle(x-4,2,w,25,BLACK);
    DrawText(TextFormat(text,score), x, 5,24,RAYWHITE);
}

void draw_hiscore(int score) {
    char *text = "HI-SCORE: %i";
    double x = (float)640/1.1f - (float)MeasureText(TextFormat(text,score),24)/2;
    double w = (float)MeasureText(TextFormat(text,score),16)+9.0f;
    DrawRectangle(x-4,360-22,w,18,BLACK);
    DrawText(TextFormat(text,score), x, 360-22,board.grid_step,RAYWHITE);
}

Vector2 fruit_spawn() {
    Vector2 fruit = {GetRandomValue(1,board.grid_row)*board.grid_step,GetRandomValue(2,board.grid_col)*board.grid_step};
    for (int i=0; i<snake.size; i++) {
        while (fruit.x == snake.arr[i].x && fruit.y == snake.arr[i].y) {
            fruit = (Vector2){GetRandomValue(1,board.grid_row)*board.grid_step,GetRandomValue(2,board.grid_col)*board.grid_step};
            i=0;
        }
    }
    return fruit;
}

Vector2 snake_restart() {
    float x = ((float)board.grid_row/2+1)*board.grid_step;
    float y = ((float)board.grid_col/2+1)*board.grid_step;
    return (Vector2){x,y};
}
