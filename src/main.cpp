#include <raylib.h>
#include "game.h"

int main() 
{
    Color grey = {29, 29, 27, 255};
    int windowWidth = 750;
    int windowHeight = 700;

    InitWindow(windowWidth, windowHeight, "Space Invaders");
    SetTargetFPS(60);

    Game game;

    while(!WindowShouldClose()) {
        game.HandleInput();
        game.Update();

        BeginDrawing();
        ClearBackground(grey);

        game.Draw();

        EndDrawing(); 
    }

    CloseWindow();
    
}