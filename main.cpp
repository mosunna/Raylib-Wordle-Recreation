#include <iostream>
#include <raylib.h>
#include <string.h>
#include <fstream>
#include <vector>
using namespace std;

/* TO DO LIST:

Visual keyboard:
	Completing the Key class
	Drawing the keyboard, having drawings updated based off guess state of individual letter
	Clicking the keyboard also being an option to enter letters to the grid

*/

//Wordle itle colors to match the offical game
Color wordleGray = {58, 58, 60, 255};
Color wordleYellow = {181, 159, 59, 255};
Color wordleGreen = {83, 141, 78, 255};

//A class to represent each key that is draw on screen
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

	//Draws the key to the screen
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

//Loads words from wordList file, picks a random word,stores it to global wordList vector
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
	string targetWord = wordList[randomIndex];

	//Converting the select word from wordList to all uppercase to match user input 
	for(int i = 0; i < targetWord.length(); i++)
	{
		targetWord[i] = toupper(targetWord[i]);
	}

	//string targetWord = "APPLE"; //HARDCODED FOR TESTING

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

//Add keyboards keys when created.. Reset through a forloop setting each key to -1
void resetGame(string &currentGuess, string &targetWord, vector<string> &previousGuesses, vector<vector<int>> &allHints, int &currentRow, bool &gameWon,bool &gameOver, bool &results, bool &showInvalidWord, float &invalidWordTimer)
{
	currentGuess = "";
    targetWord = getTargetWord();
    previousGuesses.clear();
    allHints.clear();
    currentRow = 0;
    gameWon = false;
    gameOver = false;
    results = true;
    showInvalidWord = false;
    invalidWordTimer = 0.0f;
}


int main ()
{
	const int windowWidth = 800;
	const int windowHeight = 900;
	InitWindow(windowWidth, windowHeight, "Wordle!");
	SetTargetFPS(60);

	Color backgroundColor = {18, 18, 19, 255};
	//Font for game, matches offical wordle
	Font clearSans = LoadFontEx("ClearSans-Regular.ttf", 64, 0, 0);

	string currentGuess;
	string targetWord = getTargetWord();
	vector<string> previousGuesses; // vector to save valid previous guesses (used to update grid drawing)
	vector<vector<int>> allHints; //Stores the hints for all guessed words
	vector<Key> keyboard;
	int currentRow = 0;
	bool gameWon = false;
	bool gameOver = false;
	bool results = true;
	bool showInvalidWord = false;
	float invalidWordTimer = 0.0f;

	while(WindowShouldClose() == false) //Checks for if 'esc' key is pressed or if closed icon is pressed
	{

		if(gameOver == false) //Check that allows user input only while game hasn't been lost
		{
			char letterGuess = GetCharPressed(); //getCharPressed works with as a queue with 0 equalling empty
			while(letterGuess > 0)
			{
				if(letterGuess >= 'A' && letterGuess <= 'Z' || letterGuess >= 'a' && letterGuess <= 'z') //Allows users to only enter letters
				{
					if(currentGuess.length() < 5)
					{
						letterGuess = toupper(letterGuess);
						currentGuess = currentGuess + letterGuess;
					}

				}
				letterGuess = GetCharPressed();

			}
			//Allows users to erase letters
			if(IsKeyPressed(KEY_BACKSPACE))
			{
				if(currentGuess.length() != 0)
				{
					currentGuess.pop_back(); // removing the last letter of the string
				}
			}
			
			//Check if the current 5 letter word is a valid word or not
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

					if(currentGuess == targetWord)
					{
						gameWon = true;
						gameOver = true;
					}

					//Incrementing the grid grow and resetting currentGuess
					currentRow++;
					if(currentRow == 6 && gameWon == false)
					{
						gameOver = true;
					}
					currentGuess = "";

				}
				
				else
				{
					showInvalidWord = true;
					invalidWordTimer = 2.0f;
				}
			}

			if(showInvalidWord == true)
			{
				invalidWordTimer -= GetFrameTime();

				if(invalidWordTimer <= 0)
				{
					showInvalidWord = false;
				}

			}

		}

		BeginDrawing();

		//Drawing the background of the game
		DrawRectangle(0, 0, windowWidth, windowHeight, backgroundColor);

		DrawText(TextFormat("Word is %s", targetWord.c_str()), 10, 10, 20, RED);

		int tileSize = 70;
		int spacing  = 10;
		int rows = 6;
		int cols = 5;

		//Size of individual guessing grid
		int gridWidth = (tileSize * cols) + (spacing * (cols - 1));
		int startX    = (windowWidth - gridWidth) / 2;
		int startY    = 100;

		//Double loop to draw empty tiles
		for(int row = 0; row < rows; row++)
		{
			for(int col = 0; col < cols; col++)
			{
				int tileX = startX + col * (tileSize + spacing);
				int tileY = startY + row * (tileSize + spacing);

				if(row < currentRow)
				{
					//Submitted row: fill tile with hint color
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
					int fontSize = 42;
					
					int textWidth = MeasureTextEx(clearSans, TextFormat("%c", letter), fontSize, 1).x;
					int textX = tileX + (tileSize - textWidth) / 2;
					int textY = tileY + (tileSize - fontSize) / 2;
					DrawTextEx(clearSans, TextFormat("%c", letter), {(float)textX, (float)textY}, fontSize, 1, RAYWHITE);

					
				}

				else if(row == currentRow && col < (int)currentGuess.length())
				{
					// Active row: show letters being typed, no color yet
					DrawRectangleLines(tileX, tileY, tileSize, tileSize, DARKGRAY);

					char letter = currentGuess[col];
					int fontSize = 42;
					
					int textWidth = MeasureTextEx(clearSans, TextFormat("%c", letter), fontSize, 1).x;
					int textX = tileX + (tileSize - textWidth) / 2;
					int textY = tileY + (tileSize - fontSize) / 2;
					DrawTextEx(clearSans, TextFormat("%c", letter), {(float)textX, (float)textY}, fontSize, 1, RAYWHITE);

				}

				else
				{
					DrawRectangleLines(tileX, tileY, tileSize, tileSize, DARKGRAY);
				}
			}
		}
		
		//Showing an invalid word message to the user
		if(showInvalidWord == true)
		{
			const char* msg = "Not in word list";
            int fontSize = 26;
            Vector2 msgSize = MeasureTextEx(clearSans, msg, fontSize, 1);
            int padX = 16;
            int padY = 10;
            int msgBoxW = msgSize.x + padX * 2;
            int msgBoxH = msgSize.y + padY * 2;
            int msgBoxX = (windowWidth - msgBoxW) / 2;
            int msgBoxY = 55;

            DrawRectangleRounded({(float)msgBoxX, (float)msgBoxY, (float)msgBoxW, (float)msgBoxH}, 0.3f, 8, WHITE);
            DrawTextEx(clearSans, msg, {(float)(msgBoxX + padX), (float)(msgBoxY + padY)}, fontSize, 1, BLACK);
		}

		//Showing the game over message to user: displays a win or the correctWord if user fails to guess it
		if(gameOver == true && results == true)
		{
			//Dim overlay behind popup
			DrawRectangle(0, 0, windowWidth, windowHeight, {0, 0, 0, 120});

			//Popup box
			int boxW = 340;
			int boxH = 160;
			int boxX = (windowWidth - boxW) / 2;
			int boxY = (windowHeight - boxH) / 2;

			DrawRectangleRounded({(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 0.2f, 8, {30, 30, 30, 255});
			DrawRectangleRoundedLines({(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 0.2f, 8, DARKGRAY);

			//Game over messages 
			int fontSize = 32;
			const char* message;
			if(gameWon == true && currentRow == 1)
			{
				message = TextFormat("You got Wordle in %d guess!", currentRow);
			}
			else if(gameWon == true)
			{
				message = TextFormat("You got Wordle in %d guesses!", currentRow);
			}
			else
			{
				message = TextFormat("The word was: %s", targetWord.c_str());
			}

			//Measuring and centering the text inside the box
			Vector2 textSize = MeasureTextEx(clearSans, message, fontSize, 1);
			float textX = boxX + (boxW - textSize.x) / 2;
			float textY = boxY + (boxH - textSize.y) / 2;

			DrawTextEx(clearSans, message, {textX, textY}, fontSize, 1, WHITE);

			//X button to close text box
			int closeX = boxX + boxW - 30;
			int closeY = boxY + 10;
			DrawTextEx(clearSans, "X", {(float)closeX, (float)closeY}, 24, 1, RED);

			//If user clicks on the X button
			if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				Vector2 mouse = GetMousePosition();
				if(mouse.x >= closeX && mouse.x <= closeX + 20 && mouse.y >= closeY && mouse.y <= closeY + 24)
				{
					results = false;
				}
			}
		
			//Play again button
			const char* replayMsg = "Play Again?";
			int btnW = 160;
			int btnH = 45;
			int btnX = boxX + (boxW - btnW) / 2;
			int btnY = boxY + boxH - btnH - 15;

			DrawRectangleRounded({(float)btnX, (float)btnY, (float)btnW, (float)btnH}, 0.3f, 8, wordleGreen);

			Vector2 replaySize = MeasureTextEx(clearSans, replayMsg, 22, 1);
			DrawTextEx(clearSans, replayMsg, {btnX + (btnW - replaySize.x) / 2, btnY + (btnH - replaySize.y) / 2}, 22, 1, WHITE);

			//If user clicks on play again button
			if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				Vector2 mouse = GetMousePosition();
				if(mouse.x >= btnX && mouse.x <= btnX + btnW && mouse.y >= btnY && mouse.y <= btnY + btnH)
				{
					resetGame(currentGuess, targetWord, previousGuesses, allHints,currentRow, gameWon, gameOver, results,showInvalidWord, invalidWordTimer);
				}
			}
		}
		EndDrawing();
	}

	UnloadFont(clearSans);
	CloseWindow();
	return 0;
}
