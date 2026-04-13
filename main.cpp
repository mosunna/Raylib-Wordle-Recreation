#include <iostream>
#include <raylib.h>
#include <string.h>
using namespace std;

/* TO DO LIST:

-User interaction handling:
    Function(isValidWord) for checking previously gusessed words (Has to check all words in wordList file)

Enter Key and Guess Submission:
    IsKeyPressed(KEY_ENTER) && currentGuess.length() == 5
    Save word uses to an string Vector to use later to check if word has previous been guessed by user
    Increment currentRow and currentGuess

-Guess checking
    Color evaluation for





Visual keyboard:
Copy the layout that the actual wordle has for it's keyboard






*/


int main () 
{

    const int windowWidth = 800;
    const int windowHeight = 900;
    InitWindow(windowWidth, windowHeight, "Wordle!");
    SetTargetFPS(60);

    //Colors for grid hints
    Color backgroundColor = {18, 18, 19, 255};
    Color incorrectGuess = {240, 2, 23, 255};
    Color closeGuess = {181, 159, 59, 255};
    Color correctGuess = {115, 29, 43 ,255};

    string currentGuess;
    int currentRow = 0;

    while(WindowShouldClose() == false) //Checks for if 'esc' key is pressed or if closed icon is pressed
    {

        char letterGuess = GetCharPressed(); //getCharPressed works with as a queue with 0 equalling empty
        while(letterGuess > 0)
        {
            if(letterGuess >= 'A' && letterGuess <= 'Z' || letterGuess >= 'a' && letterGuess <= 'z')
            {
                if(currentGuess.length() < 5)
                {
                    letterGuess = toupper(letterGuess);
                     currentGuess = currentGuess + letterGuess;
                }

            }
            letterGuess = GetCharPressed();

        }

        if(IsKeyPressed(KEY_BACKSPACE))
        {
            if(currentGuess.length() != 0)
            {
                currentGuess.pop_back(); // removing the last letter of the string
            }
        }
        
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

            
            if (row == currentRow)
            {
                if (col < currentGuess.length())
                {
                    char letter = currentGuess[col];

                    int fontSize = 35;
                    int textWidth = MeasureText(TextFormat("%c", letter), fontSize);
                    int textX = tileX + (tileSize - textWidth) / 2;
                    int textY = tileY + (tileSize - fontSize) / 2;

                    DrawText(TextFormat("%c", letter), textX, textY, fontSize, RAYWHITE);
                }
            }
                
        }
    }



    

        EndDrawing();
    }


    CloseWindow();
    return 0;
}