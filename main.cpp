#include <iostream>
#include <raylib.h>
#include <string.h>
#include <fstream>
#include <vector>
using namespace std;

/* TO DO LIST:

-Gameplay status
	Ending the game after current guess is entered: Not allowing more guesses.
    User guesses word: Display how many guesses it took the player to get the correct word

    User doesn't guess within the 6 guesses: Display a game over, display what the correct word was
                                             A "Play Again?" option. If yes, restart game with new word,
                                             If no, close window.


	Ending the game after winning with same play again options or not.  


Visual keyboard:
	Completing the Key class
	Drawing the keyboard, having drawings updated based off guess state of individual letter
	Clicking the keyboard also being an option to enter letters to the grid


*/

Color wordleGray = {58, 58, 60, 255};
Color wordleYellow = {181, 159, 59, 255};
Color wordleGreen = {83, 141, 78, 255};

class Key
{
public:

	Rectangle rect;
	char letter;
	int state; // -1 usused, 0 - gray, 1 -yellow , 2-green
	Color color;

	// Constructor
	Key(float x, float y, float width, float height, char keyLetter)
	{
		rect = {x, y, width, height};

		letter = keyLetter;

		//Default unused state
		state = -1;

		//Default keyboard color
		color = LIGHTGRAY;
	}
	
	void draw()
	{
		DrawRectangleRec(rect, color);

		DrawRectangleLines(rect.x,rect.y,rect.width,rect.height,BLACK);

		int fontSize = 30;

		int textWidth = MeasureText(TextFormat("%c", letter), fontSize);

		int textX = rect.x + (rect.width - textWidth) / 2;

		int textY = rect.y + (rect.height - fontSize) / 2;

		DrawText(TextFormat("%c", letter),textX,textY,fontSize,WHITE);
	}

	//Updating color based off key state
	void updateColor()
	{
		// Green
		if(state == 2)
		{
			color = wordleGreen;
		}

		// Yellow
		else if(state == 1)
		{
			color = wordleYellow;
		}

		// Gray
		else if(state == 0)
		{
			color = wordleGray;
		}

		// Unused
		else
		{
			color = LIGHTGRAY;
		}
	}
};


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

//Function responsible for determining the hint colors used in the grid for hints
vector<int> giveFeedback(string gussedWord,	string targetWord)
{
	vector<int> hints(5,0); // 0 for gray ,1 for yellow, 2 for green
	vector<int> usedLetters(5, false);

	//For loop responsible for correct guess in correct position
	for(int i = 0; i < gussedWord.length(); i++)
	{
		if(gussedWord[i] == targetWord[i])
		{
			hints[i] = 2;
			usedLetters[i] = true;
		}
	}

	//For loop responsible for a guess that contains the letter in the hidden word
	for(int i = 0; i < gussedWord.length(); i++)
	{
		if(hints[i] == 0)
		{
			for(int j = 0; j <gussedWord.length(); j++)
			{
				if(usedLetters[j] == false && gussedWord[i] == targetWord[j])
				{
					hints[i] = 1;
					usedLetters[j] = true;
					break;
				}
			}
		}
	}
	return hints;
}


int main ()
{

	const int windowWidth = 800;
	const int windowHeight = 900;
	InitWindow(windowWidth, windowHeight, "Wordle!");
	SetTargetFPS(60);

	Color backgroundColor = {18, 18, 19, 255};

	string currentGuess;
	string targetWord = getTargetWord();
	vector<string> previousGuesses; // vector to save valid previous guesses (used to update grid drawing)
	vector<vector<int>> allHints; //Stores the hints for all guessed words
	vector<Key> keyboard; 
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

				previousGuesses.push_back(currentGuess); //Saving previous guesss to print to grid

				//Pushing hintResults to the global hint vector
				vector<int> hintResults = giveFeedback(currentGuess,targetWord);
				allHints.push_back(hintResults);

				//Incrementing the grid grow and resetting currentGuess
				currentRow++;
				currentGuess = "";

			}

		}


		BeginDrawing();

		//Drawing the background of the game
		DrawRectangle(0, 0, windowWidth, windowHeight, backgroundColor);

		DrawText(TextFormat("Row: %d Guess: %s", currentRow, currentGuess.c_str()), 10, 10, 20, RED);

		int tileSize = 70;
		int spacing  = 10;
		int rows = 6;
		int cols = 5;

		//Size of individual guessing grid
		int gridWidth = (tileSize * cols) + (spacing * (cols - 1));
		int startX    = (windowWidth - gridWidth) / 2;
		int startY    = 100;

		// Double loop to draw empty tiles
		for(int row = 0; row < rows; row++)
		{
			for(int col = 0; col < cols; col++)
			{
				int tileX = startX + col * (tileSize + spacing);
				int tileY = startY + row * (tileSize + spacing);

				if(row < currentRow)
				{
					// Submitted row: fill tile with hint color
					Color tileColor;
					int hint = allHints[row][col];

					if(hint == 2)
					{
						tileColor = wordleGreen;
					}     
					 
					else if(hint == 1)
					{
						tileColor = wordleYellow;
					}  

					else
					{
						tileColor = wordleGray;
					}

					DrawRectangle(tileX, tileY, tileSize, tileSize, tileColor);
					DrawRectangleLines(tileX, tileY, tileSize, tileSize, tileColor); // Border matches fill

					char letter = previousGuesses[row][col];
					int fontSize   = 35;
					int textWidth  = MeasureText(TextFormat("%c", letter), fontSize);
					int textX      = tileX + (tileSize - textWidth) / 2;
					int textY      = tileY + (tileSize - fontSize) / 2;

					DrawText(TextFormat("%c", letter), textX, textY, fontSize, RAYWHITE);
				}

				else if(row == currentRow && col < (int)currentGuess.length())
				{
					// Active row: show letters being typed, no color yet
					DrawRectangleLines(tileX, tileY, tileSize, tileSize, DARKGRAY);

					char letter = currentGuess[col];
					int fontSize = 35;
					int textWidth  = MeasureText(TextFormat("%c", letter), fontSize);
					int textX = tileX + (tileSize - textWidth) / 2;
					int textY = tileY + (tileSize - fontSize) / 2;

					DrawText(TextFormat("%c", letter), textX, textY, fontSize, RAYWHITE);
				}

				else
				{
					DrawRectangleLines(tileX, tileY, tileSize, tileSize, DARKGRAY);
				}
			}
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
