#include <iostream>
#include <raylib.h>

using namespace std;


int main () 
{

    const int windowWidth = 800;
    const int windowHeight = 900;
    InitWindow(windowWidth, windowHeight, "Wordle!");


    //Colors for grid hints
    Color backgroundColor = {18, 18, 19, 255};
    Color incorrectGuess = {240, 2, 23, 255};
    Color closeGuess = {181, 159, 59, 255};
    Color correctGuess = {115, 29, 43 ,255};

    while(WindowShouldClose() == false) //Checks for if 'esc' key is pressed or if closed icon is pressed
    {
        BeginDrawing();

        //Drawing the background of the game
        DrawRectangle(0,0, windowWidth, windowHeight, backgroundColor);

        //Drawing the word guessing grid
        int tileSize = 70;
        int spacing = 10;
        int rows = 6;
        int cols = 5;


    //Size of individual guessing grid
    int gridWidth = (tileSize * cols) + (spacing * (cols - 1));
    //Guessing grid orientation on screen
    int startX = (windowWidth - gridWidth) / 2;
    int startY = 100;

    // Double loop to draw empty tiles
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            int tileX = startX + col * (tileSize + spacing);
            int tileY = startY + row * (tileSize + spacing);

            DrawRectangleLines(tileX, tileY, tileSize, tileSize, DARKGRAY);
        }
    }

        EndDrawing();
    }


    CloseWindow();
    return 0;
}