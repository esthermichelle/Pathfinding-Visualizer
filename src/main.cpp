#include "raylib.h"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(
            "Pathfinding Visualizer",
            40,
            30,
            32,
            DARKGRAY
        );

        DrawText(
            "Project setup successful!",
            40,
            80,
            20,
            GRAY
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}