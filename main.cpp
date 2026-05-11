#include <iostream>
#include <raylib.h>
#include <string.h>
#include <fstream>
#include <vector>
using namespace std;

/* TO DO LIST:

Correct word guessing: 

Using valid guess.txt and answerSet.txt 

Visual keyboard:
	Creating a submit guess and backspace key on that users can click

*/

//Wordle itle colors to match the offical game
Color wordleGray = {58, 58, 60, 255};
Color wordleYellow = {181, 159, 59, 255};
Color wordleGreen = {83, 141, 78, 255};
Color wordleKeyboard = {129, 131, 132, 255};

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
		color = wordleKeyboard;
	}

	//Draws the key to the screen
	void draw(Font font)
	{
		DrawRectangleRec(rect, color);
		DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, BLACK);

		int fontSize = 35;

		// Measure the letter's actual size
		Vector2 textSize = MeasureTextEx(font, TextFormat("%c", letter), fontSize, 1);

		// Center horizontally and vertically
		float textX = rect.x + (rect.width - textSize.x) / 2.0f;
		float textY = rect.y + (rect.height - textSize.y) / 2.0f;

		DrawTextEx(font, TextFormat("%c", letter), {textX, textY}, fontSize, 1, WHITE);
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
			color = wordleKeyboard;
		}
	}
};

//Loads words from wordList file, picks a random word,stores it to global wordList vector
vector<string> wordList; //Valid guesses the user can pick from
vector<string> answerList; //Valid solutions the game can pick from
string getTargetWord()
{
	wordList.clear();
	answerList.clear();

	//Loading valid guesses
	ifstream guessFile("wordle-allowed-guesses.txt");
	string word;
	while(getline(guessFile, word))
	{
		wordList.push_back(word);
	}
	guessFile.close();

	//Loading valid answers
	ifstream answerFile("wordle-answers-alphabetical.txt");
	while(getline(answerFile, word))
	{
		answerList.push_back(word);
		wordList.push_back(word);
	}
	answerFile.close();

	int randomIndex = GetRandomValue(0, answerList.size() - 1);
	string targetWord = answerList[randomIndex];

	//Converting the select word from wordList to all uppercase to match user input
	for(int i = 0; i < targetWord.length(); i++)
	{
		targetWord[i] = toupper(targetWord[i]);
	}

	//string targetWord = "WHIPS"; //HARDCODED FOR TESTING

	return targetWord;

}

//Function responsible for determining the hint colors used in the grid for hints
vector<int> giveFeedback(string gussedWord, string targetWord)
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
			for(int j = 0; j < gussedWord.length(); j++)
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

void resetKeyboard(vector<Key> &keyboard)
{
    for(int i = 0; i < keyboard.size(); i++)
    {
        keyboard[i].state = -1;
        keyboard[i].updateColor();
    }
}

//Add keyboards keys when created.. Reset through a forloop setting each key to -1
void resetGame(string &currentGuess, string &targetWord, vector<string> &previousGuesses, vector<vector<int>> &allHints, int &currentRow, bool &gameWon, bool &gameOver, bool &results, bool &showInvalidWord, float &invalidWordTimer, vector<Key> &keyboard)
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
	resetKeyboard(keyboard);
}

//Draws the 6x5 guess grid, submitted rows with hint colors, active row with typed letters, empty rows as outlines
void drawGrid(vector<string> &previousGuesses, vector<vector<int>> &allHints, string &currentGuess, int &currentRow, Font clearSans, int windowWidth)
{
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
}

void initKeyboard(vector<Key> &keyboard, int windowWidth)
{
    string row1 = "QWERTYUIOP";
    string row2 = "ASDFGHJKL";
    string row3 = "ZXCVBNM";

    float keyWidth = 50;
    float keyHeight = 60;
    float keySpacing = 5;
    float startY = 680;

    float startX = (windowWidth - (row1.length() * (keyWidth + keySpacing))) / 2;
    for(int i = 0; i < row1.length(); i++)
	{
		keyboard.push_back(Key(startX + i * (keyWidth + keySpacing), startY, keyWidth, keyHeight, row1[i]));
	}

    startX = (windowWidth - (row2.length() * (keyWidth + keySpacing))) / 2;
    for(int i = 0; i < row2.length(); i++)
	{
		keyboard.push_back(Key(startX + i * (keyWidth + keySpacing), startY + keyHeight + keySpacing, keyWidth, keyHeight, row2[i]));
	}

    startX = (windowWidth - (row3.length() * (keyWidth + keySpacing))) / 2;
    for(int i = 0; i < row3.length(); i++)
	{
		keyboard.push_back(Key(startX + i * (keyWidth + keySpacing), startY + (keyHeight + keySpacing) * 2, keyWidth, keyHeight, row3[i]));
	}
}

void drawKeyboard(vector<Key> &keyboard, Font font)
{
    for(int i = 0; i < keyboard.size(); i++)
    {
        keyboard[i].draw(font);
    }
}

void updateKeyboard(vector<Key> &keyboard, string &currentGuess, vector<int> &hintResults)
{
    //Update keyboard key states based on guess feedback
    for(int i = 0; i < 5; i++)
    {
        char guessedLetter = toupper(currentGuess[i]);

        for(int j = 0; j < keyboard.size(); j++)
        {
            if(keyboard[j].letter == guessedLetter)
            {
                //Hint color switching
                if(hintResults[i] == 2)
                {
                    keyboard[j].state = 2;
                }

                else if(hintResults[i] == 1 && keyboard[j].state != 2)
                {
                    keyboard[j].state = 1;
                }

                else if(hintResults[i] == 0 && keyboard[j].state != 2 && keyboard[j].state != 1)
                {
                    keyboard[j].state = 0;
                }

                keyboard[j].updateColor();
            }
        }
    }
}


//Showing an invalid word message to the user
void drawInvalidWord(Font clearSans, int windowWidth)
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
void drawGameOverScreen(Font clearSans, string &targetWord, int &currentRow, bool gameWon, bool &results, int windowWidth, int windowHeight, string &currentGuess, vector<string> &previousGuesses, vector<vector<int>> &allHints, bool &gameOver, bool &showInvalidWord, float &invalidWordTimer, vector<Key> &keyboard)
{
	//Dim overlay behind popup
	DrawRectangle(0, 0, windowWidth, windowHeight, {0, 0, 0, 120});

	//Popup box
	int boxW = 340;
	int boxH = 220;
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
			resetGame(currentGuess, targetWord, previousGuesses, allHints, currentRow, gameWon, gameOver, results, showInvalidWord, invalidWordTimer, keyboard);
		}
	}
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
	vector<string> previousGuesses; //Vector to save valid previous guesses (used to update grid drawing)
	vector<vector<int>> allHints; //Stores the hints for all guessed words
	vector<Key> keyboard;
	initKeyboard(keyboard, windowWidth);
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

			//Allows users to click keyboard to enter in letter as a guess 
			if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				Vector2 mousePos = GetMousePosition();

				for(int i = 0; i < keyboard.size(); i++)
				{
					if(CheckCollisionPointRec(mousePos, keyboard[i].rect))
					{
						if(currentGuess.length() < 5)
						{
							currentGuess += keyboard[i].letter;
						}
					}
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
					updateKeyboard(keyboard, currentGuess, hintResults);

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

		/* 
		UNCOMMENT FOR ANSWER REVEALED 

		DrawText(TextFormat("Word is %s", targetWord.c_str()), 10, 10, 20, RED);
		*/

		drawGrid(previousGuesses, allHints, currentGuess, currentRow, clearSans, windowWidth);
		drawKeyboard(keyboard, clearSans);

		//Showing an invalid word message to the user
		if(showInvalidWord == true)
		{
			drawInvalidWord(clearSans, windowWidth);
		}

		//Showing the game over message to user: displays a win or the correctWord if user fails to guess it
		if(gameOver == true && results == true)
		{
			drawGameOverScreen(clearSans, targetWord, currentRow, gameWon, results, windowWidth, windowHeight, currentGuess, previousGuesses, allHints, gameOver, showInvalidWord, invalidWordTimer, keyboard);
		}

		EndDrawing();
	}

	UnloadFont(clearSans);
	CloseWindow();
	return 0;
}