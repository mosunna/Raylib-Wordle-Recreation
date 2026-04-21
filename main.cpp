#include <iostream>
#include <raylib.h>
#include <string.h>
#include <fstream>
#include <vector>
using namespace std;

/* TO DO LIST:
-Guess checking
    Color evaluation for guesses that player makes
    Deep checks for words that contain the correct letters, but correct guess has multiple of the letter


    Update grid to have colors... (Do AFTER game is actually working as intended)

-Gameplay status
    User guesses word: Display how many guesses it took the player to get the correct word

    User doesn't guess within the 6 guesses: Display a game over, display what the correct word was
                                             A "Play Again?" option. If yes, restart game with new word,
                                             If no, close window.




Visual keyboard:
Copy the layout that the actual wordle has for it's keyboard






*/
vector<string> wordList;
string getTargetWord()
{
	ifstream file("WordList.txt");
	string word;

	while(getline(file, word))
	{
		wordList.push_back(word);
	}

	file.close();

	int randomIndex = GetRandomValue(0, wordList.size() - 1);
	//string targetWord = wordList[randomIndex];

	string targetWord = "APPLE"; //HARDCODED FOR TESTING

	return targetWord;

}


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
	Color correctGuess = {115, 29, 43,255};

	string currentGuess;
	string targetWord = getTargetWord();
	vector<string> previousGuesses; // vector to save valid previous guesses (used to update grid drawing)
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

		if(IsKeyPressed(KEY_ENTER) && currentGuess.length() == 5)
		{
			bool validWord = false;
			string lowerGuess = currentGuess;

			for(int i = 0; i < currentGuess.length(); i++)
			{
				lowerGuess[i] = tolower(lowerGuess[i]);
			}

			for(int i = 0; i < wordList.size(); i++)
			{
				if(lowerGuess == wordList.at(i))
				{
					validWord = true;
					break;
				}

			}
			if(validWord == true)
			{
				previousGuesses.push_back(currentGuess);
				currentRow++;
				currentGuess = ""; //Setting currentGuess string to empty

			}

		}


		BeginDrawing();
		//Drawing the background of the game
		DrawRectangle(0,0, windowWidth, windowHeight, backgroundColor);

		DrawText(TextFormat("Row: %d Guess: %s", currentRow, currentGuess.c_str()), 10, 10, 20, RED);

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

				if (row < currentRow)
				{
					char letter = previousGuesses[row][col];

					int fontSize = 35;
					int textWidth = MeasureText(TextFormat("%c", letter), fontSize);
					int textX = tileX + (tileSize - textWidth) / 2;
					int textY = tileY + (tileSize - fontSize) / 2;

					DrawText(TextFormat("%c", letter), textX, textY, fontSize, RAYWHITE);
				}

				else if (row == currentRow && col < (int)currentGuess.length())
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

		EndDrawing();
	}


	CloseWindow();
	return 0;
}