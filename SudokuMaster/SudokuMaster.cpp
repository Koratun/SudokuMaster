#include "SudokuMaster.h"
#include "ColorHelper.h"
#include "InvalidPuzzleException.h"
#include <Windows.h>
#include <map>
#include <fstream>
#include <string>
#include <iostream>

SudokuMaster::SudokuMaster(sf::Font& font, float x, float y, function<int()> numberSelected, function<bool()> markMode, function<void()> transitionStage)
{
	this->numberSelected = numberSelected;
	this->markMode = markMode;
	this->transitionStage = transitionStage;
	this->x = x;
	this->y = y;

	boardButtons.resize(9);
	completeBoardData.resize(9);
	userBoardData.resize(9);
	allPossibilities.resize(9, vector<vector<int>>(9, vector<int>(9)));

	//Create all the buttons in the sudoku board by row and column
	for (int row = 0; row < 9; row++) {
		boardButtons[row].resize(9);
		userBoardData[row].resize(9);
		completeBoardData[row].resize(9);
		for (int col = 0; col < 9; col++) {
			//Make the actual button
			boardButtons[row][col] = make_shared<SudokuNumberButton>(font, x, y, 
				//Special function to be called when this specific button is clicked.
				[&, row, col]() {
					int numSelected = this->numberSelected();
					bool markMode = this->markMode();
					boardButtons[row][col]->clicked(numSelected, markMode);
					if (markMode && boardButtons[row][col]->getSource() != SudokuNumberButton::inputSource::computer) {
						//Saves number to user board, if it's 10, then it erases the number
						userBoardData[row][col] = numSelected % 10;
					}
					//Check if all spots are filled
					bool full = true;
					//Check if we are in solve mode or play mode
					bool solveMode = true;
					for (int col = 0; col < 9; col++) {
						for (int row = 0; row < 9; row++) {
							if (userBoardData[row][col] == 0)
								full = false;
							if (boardButtons[row][col]->getSource() == SudokuNumberButton::inputSource::computer)
								solveMode = false;
						}
					}
					//If the puzzle is full and we are not in solve mode, check if it is valid
					if (full && !solveMode) {
						//If it is valid, the puzzle has been solved!
						if (checkPuzzle()) {
							//Delete the puzzle file
							remove("puzzle.txt");
							//Transition the stage with the function provided
							this->transitionStage();
						}
					}
				});
			//Increment x to move next button to correct position
			x += 50;
			//Offset x for rectangle we will be drawing later
			if (col % 3 == 2) {
				x += 15;
			}
		}
		//Reset x for next row.
		x = this->x;
		//Increment y to move next button to correct position
		y += 50;
		//Offset y for rectangle we will be drawing later
		if (row % 3 == 2) {
			y += 15;
		}
	}

	//do some math and create the separator rectangles at the correct positions!
	x = this->x + 50 * 3 + 8;
	y = this->y;
	for (int i = 0; i < 4; i++) {
		separatorRects[i] = sf::RectangleShape(sf::Vector2f(50*9 + 30, 6));
		separatorRects[i].setFillColor(ColorHelper::Gray);
		separatorRects[i].setPosition(x, y);
		if (i < 2) {
			separatorRects[i].setRotation(90);
			x += 50 * 3 + 13;
		}
		else {
			y += 50 * 3 + 13;
		}
		if (i == 1) {
			x = this->x;
			y = this->y + 50 * 3;
		}
	}

}

void SudokuMaster::createSudokuPuzzle(int difficulty)
{
	generateBoard();
	puzzlify(difficulty);
	
	updateBoardLabels();

	//Save board data
	ofstream fileWriter("puzzle.txt");

	//Save the complete puzzle
	for (vector<int>& row : completeBoardData)
	{
		for (int& i : row)
		{
			fileWriter << i;
		}
		fileWriter << '\n';
	}

	//Save the starting place for the user
	for (vector<int>& row : userBoardData)
	{
		for (int& i : row)
		{
			fileWriter << i;
		}
		fileWriter << '\n';
	}

	fileWriter.close();

}

void SudokuMaster::updateBoardLabels()
{
	//Display puzzle by updating text of sudoku buttons
	for (int col = 0; col < 9; col++) {
		for (int row = 0; row < 9; row++) {
			if (userBoardData[col][row] != 0) {
				//Lock mutex to update the text of all buttons. 
				//We don't want the program to try to draw the text in the middle of editing said text.
				mtx.lock();
				boardButtons[col][row]->giveComputerNumber(userBoardData[col][row]);
				mtx.unlock();
			}
		}
	}
}

void SudokuMaster::saveProgress(int difficulty)
{
	//Temporarily load all data into a string variable
	ifstream fileReader("puzzle.txt");
	string data;

	for (int i = 0; i < 18; i++) {
		string temp;
		getline(fileReader, temp);
		data += temp + '\n';
	}

	fileReader.close();

	//Save all board data by overwriting the file
	ofstream fileWriter("puzzle.txt");

	//Rewrite all the data held in the file previously
	fileWriter << data;

	//Save the user's current progress
	for (vector<int>& row : userBoardData)
	{
		for (int& i : row)
		{
			fileWriter << i;
		}
		fileWriter << '\n';
	}

	fileWriter << difficulty << endl;

	fileWriter.close();
}

int SudokuMaster::loadPuzzle()
{
	ifstream fileReader("puzzle.txt");
	string data;

	completeBoardData.resize(9);
	userBoardData.resize(9);

	//We read in the first 9 lines for the complete puzzle
	for (int y = 0; y < 9; y++) {
		getline(fileReader, data);
		completeBoardData[y].resize(9);
		for (int x = 0; x < 9; x++) {
			completeBoardData[y][x] = data[x] - '0';
		}
	}

	//We read in the next 9 lines for the starting place for the user
	for (int y = 0; y < 9; y++) {
		getline(fileReader, data);
		userBoardData[y].resize(9);
		for (int x = 0; x < 9; x++) {
			userBoardData[y][x] = data[x] - '0';
		}
	}

	//Display puzzle by updating text of sudoku buttons
	for (int col = 0; col < 9; col++) {
		for (int row = 0; row < 9; row++) {
			if (userBoardData[col][row] != 0) {
				//Lock mutex to update the text of all buttons. 
				//We don't want the program to try to draw the text in the middle of editing said text.
				mtx.lock();
				boardButtons[col][row]->giveComputerNumber(userBoardData[col][row]);
				mtx.unlock();
			}
		}
	}

	//We read in the last 9 lines for the progress the user has made
	for (int y = 0; y < 9; y++) {
		getline(fileReader, data);
		userBoardData[y].resize(9);
		for (int x = 0; x < 9; x++) {
			if (userBoardData[y][x] != data[x] - '0') {
				userBoardData[y][x] = data[x] - '0';
				//Invoke the clicked action so it displays with the user color on the puzzle
				boardButtons[y][x]->clicked(data[x] - '0', true);
			}
		}
	}

	int difficulty;
	fileReader >> difficulty;
	return difficulty;
}

void SudokuMaster::fileToSolve(ifstream& puzzleStream)
{
	string data;
	int y = 0;
	while (getline(puzzleStream, data)) {
		if (data.size() != 9) {
			wipeData();
			throw InvalidPuzzleException();
		}

		for (int x = 0; x < 9; x++) {
			if (data[x] - '0' < 0 || data[x] - '0' > 9) {
				wipeData();
				throw InvalidPuzzleException();
			}

			userBoardData[y][x] = data[x] - '0';
		}

		y++;
	}

	if (y != 9) {
		wipeData();
		throw InvalidPuzzleException();
	}

	//Display puzzle by updating text of sudoku buttons
	for (int col = 0; col < 9; col++) {
		for (int row = 0; row < 9; row++) {
			if (userBoardData[col][row] != 0) {
				//Lock mutex to update the text of all buttons. 
				//We don't want the program to try to draw the text in the middle of editing said text.
				mtx.lock();
				//Invoke the click because we want it to display as user input, because technically it is
				boardButtons[col][row]->clicked(userBoardData[col][row], true);
				mtx.unlock();
			}
		}
	}
}

bool SudokuMaster::checkPuzzle()
{
	//If the board is valid return true
	bool valid = true;
	for (int col = 0; col < 9; col++) {
		for (int row = 0; row < 9; row++) {
			//If the user's board is not empty and does not equal the correct number, mark it as incorrect.
			if (userBoardData[col][row] != 0 && userBoardData[col][row] != completeBoardData[col][row]) {
				boardButtons[col][row]->setLabelColor(SudokuNumberButton::getInputColor(SudokuNumberButton::inputSource::incorrect));
				valid = false;
			}
		}
	}
	return valid;
}

void SudokuMaster::giveHint()
{
	bool full = true;
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			if (userBoardData[row][col] == 0)
				full = false;
		}
	}

	if (full)
		return;

	int x = rand() % 9;
	int y = rand() % 9;
	//Make sure the coords refer to an empty space
	while (userBoardData[y][x] != 0) {
		x = rand() % 9;
		y = rand() % 9;
	}

	//Fill the spot with the hint
	boardButtons[y][x]->updateHint(completeBoardData[y][x]);
	userBoardData[y][x] = completeBoardData[y][x];
}

void SudokuMaster::activateButtons()
{
	for (vector<shared_ptr<SudokuNumberButton>>& row : boardButtons) {
		for (shared_ptr<SudokuNumberButton> b : row) {
			b->setToDraw(true);
		}
	}
}

void SudokuMaster::wipeData()
{
	//Wipe all data so a new puzzle can be created
	failedRemovalPoints.clear();
	puzzleSolutions.clear();
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			completeBoardData[row][col] = 0;
			userBoardData[row][col] = 0;
			allPossibilities[row][col].clear();
			allPossibilities[row][col].resize(9);
			boardButtons[row][col]->wipeData();
		}
	}

}

void SudokuMaster::solvePuzzle()
{
	puzzleSolutions = algorithmX(userBoardData, true);
	//The index is set to the last solution so we can rotate forward to the first solution and print it properly
	solutionIndex = puzzleSolutions.size() - 1;
	if (solutionIndex >= 0) {
		rotateSolution(true);
	}
	//If there are no solutions, do not try to display the solution.
	else {
		//Delete all hint colored spots
		for (int row = 0; row < 9; row++) {
			for (int col = 0; col < 9; col++) {
				//If the buttons is not user input, then we can update it for the next solution
				if (boardButtons[row][col]->getSource() == SudokuNumberButton::inputSource::hint)
				{
					boardButtons[row][col]->clicked(10, true);
				}
			}
		}
	}
}

int SudokuMaster::solutionsFound()
{
	return puzzleSolutions.size();
}

void SudokuMaster::rotateSolution(bool forward)
{
	//Get proper solution index
	if (forward) {
		solutionIndex = (solutionIndex + 1) % solutionsFound();
	}
	else {
		if (solutionIndex - 1 < 0) {
			solutionIndex = solutionsFound() - 1;
		}
		else {
			solutionIndex--;
		}
	}

	//Update all computer solved positions for the next solution
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			//If the buttons is not user input, then we can update it for the next solution
			if (boardButtons[row][col]->getSource() != SudokuNumberButton::inputSource::user)
			{
				boardButtons[row][col]->updateHint(puzzleSolutions[solutionIndex][row][col]);
			}
		}
	}

}

int SudokuMaster::getSolutionIndex()
{
	return solutionIndex;
}

void SudokuMaster::updateButtonNotes()
{
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			mtx.lock();
			boardButtons[row][col]->updateNotes(allPossibilities[row][col]);
			mtx.unlock();
		}
	}
}

//TODO: Finish implementing generation and output of board (plus the actual puzzle bit)
void SudokuMaster::generateBoard()
{
	for (int y = 0; y <9; y++)
	{
		for (int x = 0; x <9; x++)
		{
			//Fill 3D vector of all possibilities with all the possibilities.
			for (int z = 0; z <9; z++)
			{
				allPossibilities[y][x][z] = z + 1;
			}
		}
	}

	//Generating...
	vector<vector<int>> backupBoard = completeBoardData, babyBoard = completeBoardData;
	vector<vector<vector<int>>> backupPossibilites = allPossibilities, babyPossibilities = allPossibilities;
	updateButtonNotes();

	int permutations = 1;
	bool rowFailed = false;

	//Create Sudoku board of the appropriate size
	for (int y = 0; y <9; y++)
	{
		int checkFail = y;
		for (int x = 0; x <9; x++)
		{
			//If this row has failed, then restore the board and its possibilities from the backup and reset y back to this row.
			if (!tryNewBoardValue(x, y, completeBoardData))
			{
				permutations++;
				//If this is the second time this row has failed, just reset everything.
				if (rowFailed)
				{
					completeBoardData = babyBoard;
					allPossibilities = babyPossibilities;
					y = -1;
					break;
				}
				completeBoardData = backupBoard;
				allPossibilities = backupPossibilites;
				y--;
				rowFailed = true;
				break;
			}
			//This value succeeded, display all possibilities as notes to the board
			else {
				updateButtonNotes();
				Sleep(50);
			}
		}
		//Backup the board and possibilities of the board after every row unless the backups were just used, in which case checkFail will not equal y.
		if (checkFail == y || y == -1)
		{
			backupBoard = completeBoardData;
			backupPossibilites = allPossibilities;
			rowFailed = false;
		}
	}

	////If this board has already been generated, throw an exception.
	//if (find(boards.begin(), boards.end(), board) != boards.end()) {
	//	throw "Board already created";
	//}

	////Save the board to the file
	////saveBoard(board);
	////Add the board to the live list of boards.
	//boards.push_back(board);

	//printBoard(boardPointer,9);

	//cout << "\nThis puzzle took " << permutations << " permutation" << (permutations == 1 ? "" : "s") << " to create.\n";
}

//Generate a new value for the board in the given x/y coordinates. Returns false if this row needs to be reset.
bool SudokuMaster::tryNewBoardValue(int x, int y, vector<vector<int>>& boardData, int numberToAdd)
{
	if (numberToAdd == 0) {
		do
		{
			//If this is not the first time in this loop, then this will run.
			if (numberToAdd != 0)
			{
				//Erase the number that just created an unviable board, we can't use it anyway.
				allPossibilities[y][x].erase(find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd));
				//If there are no more possibilities, then this permutation has failed and the row must be reset.
				if (allPossibilities[y][x].size() == 0)
				{
					return false;
				}
			}
			//Randomly pick one of the possibile values of this coordinate and save it.
			numberToAdd = allPossibilities[y][x][rand() % allPossibilities[y][x].size()];
		} while (!boardStillPossible(numberToAdd, x, y, boardData));
		//clear this unit of memory because we no longer need it.
		allPossibilities[y][x].clear();
	}
	else {
		if (!boardStillPossible(numberToAdd, x, y, boardData)) {
			return false;
		}
	}
	
	//delete this number from the possibilities of the horizontal line it's in.
	for (int x = 0; x <9; x++)
	{
		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != allPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			allPossibilities[y][x].erase(i);
		}
	}
	//delete this number from the possibilities of the vertical line it's in.
	for (int y = 0; y <9; y++)
	{
		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != allPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			allPossibilities[y][x].erase(i);
		}
	}

	//Get absolute coordinates of the inner squares within the outer squares of the board.

	//Some math to get the number of which outer square we are in.  
	//    9x9           4x4
	//000|111|222      00|11  <- Outer squares labeled, inner squares are each position within each outer square.
	//000|111|222      00|11
	//000|111|222  or  --+--
	//---+---+---      22|33
	//333|444|555      22|33
	//333|444|555
	//333|444|555
	//---+---+---
	//666|777|888
	//666|777|888
	//666|777|888
	int outerSquare = y - y % 3 + x / 3;
	//Create array to store two values of outer squares to calculate exact coordinate positions within the outer square later.
	int squareMultiplier[2];
	//Get the coordinate multipliers of the outer square by converting the square number to a two digit number in the base of the sqrt of the size of the board.
	string multiplierCoordsString = convertBase(outerSquare, 3);
	//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
	if (multiplierCoordsString.size() == 1)
	{
		squareMultiplier[0] = 0;
		squareMultiplier[1] = multiplierCoordsString[0] - '0';
	}
	else
	{
		squareMultiplier[0] = multiplierCoordsString[0] - '0';
		squareMultiplier[1] = multiplierCoordsString[1] - '0';
	}

	//Step through each inner square in order to delete the number just put in the board from each of their possibilities.
	for (int innerSquare = 0; innerSquare <9; innerSquare++)
	{
		//Create array to store two values of inner sqaures to calculate exact coordinate position later.
		int squareCoords[2];
		//Get the binary version of the iterator.
		string squareCoordsString = convertBase(innerSquare, 3);
		//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
		if (squareCoordsString.size() == 1)
		{
			squareCoords[0] = 0;
			squareCoords[1] = squareCoordsString[0] - '0';
		}
		else
		{
			squareCoords[0] = squareCoordsString[0] - '0';
			squareCoords[1] = squareCoordsString[1] - '0';
		}

		//Use the current outer sqaure and relative inner square to get the absolute coordinates.
		int x = squareMultiplier[1] * 3 + squareCoords[1];
		int y = squareMultiplier[0] * 3 + squareCoords[0];

		//Delete this number from the possibilities of the square it is in.

		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != allPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			allPossibilities[y][x].erase(i);
		}
	}

	boardData[y][x] = numberToAdd;
	return true;
}

//This is a simulation function that runs the function that called it with a copy of the allPossibilities cube,
//if this function determines that the resulting board can still produce a viable sudoku board, then it returns true, otherwise it returns false.
bool SudokuMaster::boardStillPossible(int numberToAdd, int x, int y, vector<vector<int>>& boardData)
{
	vector<vector<vector<int>>> copyOfAllPossibilities = allPossibilities;
	//delete this number from the possibilities of the horizontal line it's in.
	for (int x = 0; x <9; x++)
	{
		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(copyOfAllPossibilities[y][x].begin(), copyOfAllPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != copyOfAllPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			copyOfAllPossibilities[y][x].erase(i);
		}
	}
	//delete this number from the possibilities of the vertical line it's in.
	for (int y = 0; y <9; y++)
	{
		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(copyOfAllPossibilities[y][x].begin(), copyOfAllPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != copyOfAllPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			copyOfAllPossibilities[y][x].erase(i);
		}
	}

	//Get absolute coordinates of the inner squares within the outer squares of the board.

	//Some math to get the number of which outer square we are in.  
	//    9x9           4x4
	//000|111|222      00|11  <- Outer squares labeled, inner squares are each position within each outer square.
	//000|111|222      00|11
	//000|111|222  or  --+--
	//---+---+---      22|33
	//333|444|555      22|33
	//333|444|555
	//333|444|555
	//---+---+---
	//666|777|888
	//666|777|888
	//666|777|888
	int outerSquare = y - y % 3 + x / 3;
	//Create array to store two values of outer squares to calculate exact coordinate positions within the outer square later.
	int squareMultiplier[2];
	//Get the coordinate multipliers of the outer square by converting the square number to a two digit number in the base of the sqrt of the9 of the board.
	string multiplierCoordsString = convertBase(outerSquare, 3);
	//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
	if (multiplierCoordsString.size() == 1)
	{
		squareMultiplier[0] = 0;
		squareMultiplier[1] = multiplierCoordsString[0] - '0';
	}
	else
	{
		squareMultiplier[0] = multiplierCoordsString[0] - '0';
		squareMultiplier[1] = multiplierCoordsString[1] - '0';
	}

	//Step through each inner square in order to delete the number just put in the board from each of their possibilities.
	for (int innerSquare = 0; innerSquare <9; innerSquare++)
	{
		//Create array to store two values of inner sqaures to calculate exact coordinate position later.
		int squareCoords[2];
		//Get the binary version of the iterator.
		string squareCoordsString = convertBase(innerSquare, 3);
		//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
		if (squareCoordsString.size() == 1)
		{
			squareCoords[0] = 0;
			squareCoords[1] = squareCoordsString[0] - '0';
		}
		else
		{
			squareCoords[0] = squareCoordsString[0] - '0';
			squareCoords[1] = squareCoordsString[1] - '0';
		}

		//Use the current outer sqaure and relative inner square to get the absolute coordinates.
		int x = squareMultiplier[1] * 3 + squareCoords[1];
		int y = squareMultiplier[0] * 3 + squareCoords[0];

		//Delete this number from the possibilities of the square it is in.

		//Determine if this position still has this number as a possibility
		vector<int>::iterator i = find(copyOfAllPossibilities[y][x].begin(), copyOfAllPossibilities[y][x].end(), numberToAdd);
		//If it does not, then we do not need to do anything.
		if (i != copyOfAllPossibilities[y][x].end())
		{
			//Erase the possible value wherever it is in the vector of this specific coordinate.
			copyOfAllPossibilities[y][x].erase(i);
		}
	}

	//Logic to determine if this new board is good or not
	//Loop through every position on the board
	for (int y1 = 0; y1 <9; y1++)
	{
		for (int x1 = 0; x1 <9; x1++)
		{
			//Ignore the position we are currently trying to fill.
			if (y1 == y && x1 == x)
			{
				continue;
			}
			//If the position has not been written to yet and the number of possibilities for this position is zero, then this is not a viable board. Try again.
			if (boardData[y1][x1] == 0 && copyOfAllPossibilities[y1][x1].size() == 0)
			{
				return false;
			}
		}
	}
	//If the program makes it to this line, then the board is viable. Congrats!
	return true;
}

//Converts n to a literal of the number in base 'base' instead of base 10.
//Can only be used to convert to a base number less than 10 such as binary.
string SudokuMaster::convertBase(int n, int base)
{
	//If n is zero, the answer is always zero.
	if (n == 0)
	{
		return "0";
	}
	//Create vector to store digits in.
	vector<char> newNumber;
	//Create a loop that continually reduces n and stores each digit from right to left. 
	while (n > 0)
	{
		//Save the remainder of n/base. This is the value of the digit in the new number. Then add '0' to the value because for some reason it doesn't like ints so I had to use chars.
		newNumber.push_back(n % base + '0');
		//Reduce n by a factor of the new base and discard the remainder, we saved it already above.
		n = n / base;
	}
	string answer = "";
	//The vector saved the digits from right to left, but a string must display this from left to right.
	//This loop goes through the vector backwards and adds each to the string that will be returned.
	for (int i = newNumber.size() - 1; i >= 0; i--)
	{
		answer += newNumber[i];
	}
	return answer;
}

void SudokuMaster::puzzlify(int difficulty)
{
	//Fill the user's board with the entire puzzle, then incrementally remove random spots until desired difficulty level is reached.
	userBoardData = completeBoardData;
	//First calculate for hard mode (maximum possible amount of numbers removed), then scale the puzzle to the given difficulty level
	int maxRemovalNumbers = 0;
	//int failedPointCount = 0;
	while (removeBoardValue()) {
		maxRemovalNumbers++;

		//debugging
		/*if (failedRemovalPoints.size() != failedPointCount) {
			failedPointCount = failedRemovalPoints.size();
			cout << "Numbers removed: " << maxRemovalNumbers << ", points failed: " << failedPointCount << endl;
		}*/

		//Update graphical information cause I want it to look cool. Mutexes are already handled
		updateButtonNotes();
	}

	//Reset allPossibilities and refresh notes so that the user doesn't have all the answers
	for (vector<vector<int>>& col : allPossibilities) {
		for (vector<int>& spot : col) {
			spot.clear();
		}
	}

	updateButtonNotes();

	//If hard mode, then just return with the completed puzzle
	if (difficulty == 3) {
		return;
	}

	//Otherwise add back random numbers until the desired difficulty is reached
	for (int emptySpaces = maxRemovalNumbers; emptySpaces > maxRemovalNumbers * (difficulty * .25f + .25f); emptySpaces--) {
		int x = rand() % 9;
		int y = rand() % 9;

		//Make sure the coords refer to an empty space
		while (userBoardData[y][x] != 0) {
			x = rand() % 9;
			y = rand() % 9;
		}

		//Fill the space
		userBoardData[y][x] = completeBoardData[y][x];
	}

}

bool SudokuMaster::checkValidValue(int val, int x, int y, vector<vector<int>>& puzzle)
{
	//Check row to see if one of the spots contains the given value
	for (int col = 0; col < 9; col++) {
		if (puzzle[y][col] == val) {
			return false;
		}
	}

	//Check column to see if one of the spots contains the given value
	for (int row = 0; row < 9; row++) {
		if (puzzle[row][x] == val) {
			return false;
		}
	}

	//Check outer square to see if one of the spots contains the given value

	int outerSquare = y - y % 3 + x / 3;
	//Create array to store two values of outer squares to calculate exact coordinate positions within the outer square later.
	int squareMultiplier[2];
	//Get the coordinate multipliers of the outer square by converting the square number to a two digit number in the base of the sqrt of the size of the board.
	string multiplierCoordsString = convertBase(outerSquare, 3);
	//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
	if (multiplierCoordsString.size() == 1)
	{
		squareMultiplier[0] = 0;
		squareMultiplier[1] = multiplierCoordsString[0] - '0';
	}
	else
	{
		squareMultiplier[0] = multiplierCoordsString[0] - '0';
		squareMultiplier[1] = multiplierCoordsString[1] - '0';
	}

	//Step through each inner square in order to delete the number just put in the board from each of their possibilities.
	for (int innerSquare = 0; innerSquare < 9; innerSquare++)
	{
		//Create array to store two values of inner sqaures to calculate exact coordinate position later.
		int squareCoords[2];
		//Get the binary version of the iterator.
		string squareCoordsString = convertBase(innerSquare, 3);
		//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
		if (squareCoordsString.size() == 1)
		{
			squareCoords[0] = 0;
			squareCoords[1] = squareCoordsString[0] - '0';
		}
		else
		{
			squareCoords[0] = squareCoordsString[0] - '0';
			squareCoords[1] = squareCoordsString[1] - '0';
		}

		//Use the current outer sqaure and relative inner square to get the absolute coordinates.
		int x = squareMultiplier[1] * 3 + squareCoords[1];
		int y = squareMultiplier[0] * 3 + squareCoords[0];

		//check each position
		if (puzzle[y][x] == val) {
			return false;
		}
	}

	//If none of the previously checked positions contained the given value, then return true! We can use this value here!
	return true;
}

bool SudokuMaster::removeBoardValue()
{
	//Check that there is at least one remaining point that is not empty or already failed to remove.
	bool canRemove = false;
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			//If the point is not empty and is not in the failed removal points, then we can move forward.
			if (userBoardData[y][x] != 0 && find(failedRemovalPoints.begin(), failedRemovalPoints.end(), sf::Vector2i(x, y)) == failedRemovalPoints.end()) {
				canRemove = true;
			}
		}
	}

	if (!canRemove)
		return false;

	int x = rand() % 9;
	int y = rand() % 9;
	//Make sure the coords refer to a filled space and not an already failed point
	while (userBoardData[y][x] == 0 || find(failedRemovalPoints.begin(), failedRemovalPoints.end(), sf::Vector2i(x, y)) != failedRemovalPoints.end()) {
		x = rand() % 9;
		y = rand() % 9;
	}

	int value = userBoardData[y][x];
	removeBoardValue(x, y, userBoardData);
	
	//If there are any spots with more than 1 possibility, then we can't be sure there is only 1 solution, so we need to check them now.
	bool checkSlns = false;
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			if (allPossibilities[y][x].size() > 1)
				checkSlns = true;
		}
	}

	//If there are now no solutions (Not possible if being called from puzzlify) or more than one solution, then we have to pick a different spot
	if (checkSlns) {
		int slns = 0;
		try {
			slns = algorithmX(userBoardData, false).size();
		}
		catch (int size) {
			slns = size;
		}

		if (slns != 1) {
			failedRemovalPoints.push_back(sf::Vector2i(x, y));
			//This should never error, if it does... I would lose my faith in logic. (Or I made a stupid typo somewhere, this is also possible)
			tryNewBoardValue(x, y, userBoardData, value);
			//Recursively call until a value is removed.
			removeBoardValue();
		}
	}

	return true;
}

void SudokuMaster::removeBoardValue(int x, int y, vector<vector<int>>& puzzle)
{
	int value = puzzle[y][x];
	//Remove the value from the given position
	puzzle[y][x] = 0;
	//Add possibilities to all spaces in the same row
	for (int col = 0; col < 9; col++) {
		//If this position doesn't already have this value as a possibility, then add it
		if (find(allPossibilities[y][col].begin(), allPossibilities[y][col].end(), value) == allPossibilities[y][col].end()) {
			//Check if the position we are about to write to is actually able to take this new value as an option
			if(checkValidValue(value, col, y, puzzle))
				allPossibilities[y][col].push_back(value);
		}
	}

	//Add possibilities to all spaces in the same column
	for (int row = 0; row < 9; row++) {
		//If this position doesn't already have this value as a possibility, then add it
		if (find(allPossibilities[row][x].begin(), allPossibilities[row][x].end(), value) == allPossibilities[row][x].end()) {
			//Check if the position we are about to write to is actually able to take this new value as an option
			if (checkValidValue(value, x, row, puzzle))
				allPossibilities[row][x].push_back(value);
		}
	}

	//Add possibilities to all spaces in the same outer square

	int outerSquare = y - y % 3 + x / 3;
	//Create array to store two values of outer squares to calculate exact coordinate positions within the outer square later.
	int squareMultiplier[2];
	//Get the coordinate multipliers of the outer square by converting the square number to a two digit number in the base of the sqrt of the size of the board.
	string multiplierCoordsString = convertBase(outerSquare, 3);
	//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
	if (multiplierCoordsString.size() == 1)
	{
		squareMultiplier[0] = 0;
		squareMultiplier[1] = multiplierCoordsString[0] - '0';
	}
	else
	{
		squareMultiplier[0] = multiplierCoordsString[0] - '0';
		squareMultiplier[1] = multiplierCoordsString[1] - '0';
	}

	//Step through each inner square in order to delete the number just put in the board from each of their possibilities.
	for (int innerSquare = 0; innerSquare < 9; innerSquare++)
	{
		//Create array to store two values of inner sqaures to calculate exact coordinate position later.
		int squareCoords[2];
		//Get the binary version of the iterator.
		string squareCoordsString = convertBase(innerSquare, 3);
		//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
		if (squareCoordsString.size() == 1)
		{
			squareCoords[0] = 0;
			squareCoords[1] = squareCoordsString[0] - '0';
		}
		else
		{
			squareCoords[0] = squareCoordsString[0] - '0';
			squareCoords[1] = squareCoordsString[1] - '0';
		}

		//Use the current outer sqaure and relative inner square to get the absolute coordinates.
		int x = squareMultiplier[1] * 3 + squareCoords[1];
		int y = squareMultiplier[0] * 3 + squareCoords[0];

		//Add possibilities if not already present in the spot
		if (find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), value) == allPossibilities[y][x].end()) {
			//Check if the position we are about to write to is actually able to take this new value as an option
			if (checkValidValue(value, x, y, puzzle))
				allPossibilities[y][x].push_back(value);
		}
	}
}


vector<vector<vector<int>>> SudokuMaster::algorithmX(vector<vector<int>>& puzzle, bool solving)
{
	//The vector to contain all possible sudoku solutions
	vector<vector<vector<int>>> solutions;

	//Size of the puzzle
	int size = puzzle.size();

	//If we are solving, then we need to manually input the data for allPossibilities
	if (solving) {
		//Get all possibilities of puzzle
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				//Fill 3D vector of all possibilities with all the possibilities.
				for (int z = 0; z < size; z++)
				{
					allPossibilities[y][x][z] = z + 1;
				}
			}
		}

		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				int numberToAdd = puzzle[y][x];
				if (numberToAdd == 0) {
					continue;
				}

				//clear this unit of memory because we no longer need it.
				allPossibilities[y][x].clear();
				//delete this number from the possibilities of the horizontal line it's in.
				for (int x = 0; x < size; x++)
				{
					//Determine if this position still has this number as a possibility
					vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
					//If it does not, then we do not need to do anything.
					if (i != allPossibilities[y][x].end())
					{
						//Erase the possible value wherever it is in the vector of this specific coordinate.
						allPossibilities[y][x].erase(i);
					}
				}
				//delete this number from the possibilities of the vertical line it's in.
				for (int y = 0; y < size; y++)
				{
					//Determine if this position still has this number as a possibility
					vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
					//If it does not, then we do not need to do anything.
					if (i != allPossibilities[y][x].end())
					{
						//Erase the possible value wherever it is in the vector of this specific coordinate.
						allPossibilities[y][x].erase(i);
					}
				}

				//Get absolute coordinates of the inner squares within the outer squares of the board.

				//Some math to get the number of which outer square we are in.  
				//    9x9           4x4
				//000|111|222      00|11  <- Outer squares labeled, inner squares are each position within each outer square.
				//000|111|222      00|11
				//000|111|222  or  --+--
				//---+---+---      22|33
				//333|444|555      22|33
				//333|444|555
				//333|444|555
				//---+---+---
				//666|777|888
				//666|777|888
				//666|777|888
				int outerSquare = y - y % (int)sqrt(size) + x / sqrt(size);
				//Create array to store two values of outer squares to calculate exact coordinate positions within the outer square later.
				int squareMultiplier[2];
				//Get the coordinate multipliers of the outer square by converting the square number to a two digit number in the base of the sqrt of the size of the board.
				string multiplierCoordsString = convertBase(outerSquare, sqrt(size));
				//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
				if (multiplierCoordsString.size() == 1)
				{
					squareMultiplier[0] = 0;
					squareMultiplier[1] = multiplierCoordsString[0] - '0';
				}
				else
				{
					squareMultiplier[0] = multiplierCoordsString[0] - '0';
					squareMultiplier[1] = multiplierCoordsString[1] - '0';
				}

				//Step through each inner square in order to delete the number just put in the board from each of their possibilities.
				for (int innerSquare = 0; innerSquare < size; innerSquare++)
				{
					//Create array to store two values of inner sqaures to calculate exact coordinate position later.
					int squareCoords[2];
					//Get the binary version of the iterator.
					string squareCoordsString = convertBase(innerSquare, sqrt(size));
					//Convert the binary string to integers using ascii math and chars. '0' - '0' = 0 and '1' - '0' = 1. Very useful.
					if (squareCoordsString.size() == 1)
					{
						squareCoords[0] = 0;
						squareCoords[1] = squareCoordsString[0] - '0';
					}
					else
					{
						squareCoords[0] = squareCoordsString[0] - '0';
						squareCoords[1] = squareCoordsString[1] - '0';
					}

					//Use the current outer sqaure and relative inner square to get the absolute coordinates.
					int x = squareMultiplier[1] * sqrt(size) + squareCoords[1];
					int y = squareMultiplier[0] * sqrt(size) + squareCoords[0];

					//Delete this number from the possibilities of the square it is in.

					//Determine if this position still has this number as a possibility
					vector<int>::iterator i = find(allPossibilities[y][x].begin(), allPossibilities[y][x].end(), numberToAdd);
					//If it does not, then we do not need to do anything.
					if (i != allPossibilities[y][x].end())
					{
						//Erase the possible value wherever it is in the vector of this specific coordinate.
						allPossibilities[y][x].erase(i);
					}
				}
			}
		}
		//end get all possibilities
	}


	//This is the matrix that will be used to solve the puzzle. We must first convert the sudoku puzzle into this matrix.
	//The matrix contains 4 different constraints to solve the puzzle against: Cell, Row, Column, and Box
	//The matrix will have the width of size squared (times 4 different constraints), and height of size cubed
	vector<vector<bool>> exactCoverMatrix(size * size * 4, vector<bool>(size * size * size + 1)); //Add one to offset for column header row

	//Set the first row all to true for the column header row
	for (int i = 0; i < exactCoverMatrix.size(); i++) {
		exactCoverMatrix[i][0] = true;
	}

	int sizeSquared = size * size;

	//Fill the matrix
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			for (int num = 1; num < size + 1; num++) {

				//Fill cell portion of matrix
				for (int i = 0; i < sizeSquared; i++) {
					//If this cell is referenced by i and row/col according to each cell, then see about the possibilities, otherwise we leave it as false
					if (i == row * size + col) {
						//If this position is not filled, then see if the current num is possible
						if (puzzle[row][col] == 0) {
							//If the position has this num as a possibility, then add true to this spot in the row.
							exactCoverMatrix[i][row * sizeSquared + col * size + num] = find(allPossibilities[row][col].begin(), allPossibilities[row][col].end(), num) != allPossibilities[row][col].end();
						}
						else {
							//If it is filled, add true if the spot in the puzzle is equal to the num we are populating at the moment
							if (puzzle[row][col] == num) {
								exactCoverMatrix[i][row * sizeSquared + col * size + num] = true;
							}
						}
					}
				}

				//Fill row portion of matrix
				for (int i = sizeSquared; i < sizeSquared * 2; i++) {
					//If this cell is referenced by i, the row and num, according to the row layout then see about the possibilities, otherwise add false
					if (i - sizeSquared == row * size + num - 1) {
						//If this position is not filled, then see if the current num is possible
						if (puzzle[row][col] == 0) {
							//If the position has this num as a possibility, then add true to this spot in the row.
							exactCoverMatrix[i][row * sizeSquared + col * size + num] = find(allPossibilities[row][col].begin(), allPossibilities[row][col].end(), num) != allPossibilities[row][col].end();
						}
						else {
							//If it is filled, add true if the spot in the puzzle is equal to the num we are populating at the moment
							if (puzzle[row][col] == num) {
								exactCoverMatrix[i][row * sizeSquared + col * size + num] = true;
							}
						}
					}
				}

				//Fill column portion of matrix
				for (int i = sizeSquared * 2; i < sizeSquared * 3; i++) {
					//If this cell is referenced by i, the col and num, according to the column layout then see about the possibilities, otherwise add false
					if (i - sizeSquared * 2 == col * size + num - 1) {
						//If this position is not filled, then see if the current num is possible
						if (puzzle[row][col] == 0) {
							//If the position has this num as a possibility, then add true to this spot in the row.
							exactCoverMatrix[i][row * sizeSquared + col * size + num] = find(allPossibilities[row][col].begin(), allPossibilities[row][col].end(), num) != allPossibilities[row][col].end();
						}
						else {
							//If it is filled, add true if the spot in the puzzle is equal to the num we are populating at the moment
							if (puzzle[row][col] == num) {
								exactCoverMatrix[i][row * sizeSquared + col * size + num] = true;
							}
						}
					}
				}

				//Fill box portion of matrix
				for (int i = sizeSquared * 3; i < sizeSquared * 4; i++) {
					//If this cell is referenced by i, the row, col, and num according to the box layout then see about the possibilities, otherwise add false
					if (i - sizeSquared * 3 == (col / (int)sqrt(size)) * size + (row / (int)sqrt(size)) * size * (int)sqrt(size) + num - 1) {
						//If this position is not filled, then see if the current num is possible
						if (puzzle[row][col] == 0) {
							//If the position has this num as a possibility, then add true to this spot in the row.
							exactCoverMatrix[i][row * sizeSquared + col * size + num] = find(allPossibilities[row][col].begin(), allPossibilities[row][col].end(), num) != allPossibilities[row][col].end();
						}
						else {
							//If it is filled, add true if the spot in the puzzle is equal to the num we are populating at the moment
							if (puzzle[row][col] == num) {
								exactCoverMatrix[i][row * sizeSquared + col * size + num] = true;
							}
						}
					}
				}

			}
		}
	}

	//Create vector to hold the raw solution data
	vector<vector<shared_ptr<DoubleLinkedNode>>> solutionsByRow;


	//This was used for debugging purposes. Also it looks cool
	/*cout << "Printing exact cover matrix...\n\n";

	for (int row = 0; row < size*size*size; row++) {
		cout << row << " ";
		if (row <= 9) {
			cout << "  ";
		}
		else if (row <= 99) {
			cout << " ";
		}
		for (int col = 0; col < exactCoverMatrix.size(); col++) {
			if (exactCoverMatrix[col][row]) {
				cout << 1;
			}
			else {
				cout << " ";
			}
		}
		cout << endl;
	}*/

	//Created a DLX dancing link matrix from the binary matrix
	DoubleLinkedNodeMatrix nodeMatrix(exactCoverMatrix);

	//Solve puzzle with the newly created meshed node matrix
	searchExactCover(nodeMatrix, solutionsByRow, solving);

	//Translate solutions by row into readable sudoku solutions
	for (vector<shared_ptr<DoubleLinkedNode>>& rowSolution : solutionsByRow) {

		//Create a temp puzzle to hold the solution and initialize it to the size of the puzzle
		vector<vector<int>> sudokuSolution(size, vector<int>(size));

		//Add solution parts to appropriate position with the correct number
		for (shared_ptr<DoubleLinkedNode> solutionNode : rowSolution) {
			//Offset rowID by one for the column header row
			int rowID = solutionNode->getRow() - 1;
			int row = rowID / sizeSquared;
			int col = (rowID % sizeSquared) / size;
			int num = (rowID % sizeSquared) % size + 1;
			sudokuSolution[row][col] = num;
		}

		//Add completed solution to the list of solutions
		solutions.push_back(sudokuSolution);
	}

	return solutions;
}

void SudokuMaster::updateSolutionProgress(int nSolutions)
{
	int size = 9, sizeSquared = 9 * 9;

	//Create a temp puzzle to hold the solution and initialize it to the size of the puzzle
	vector<vector<int>> sudokuSolution(size, vector<int>(size));

	//Add solution parts to appropriate position with the correct number
	for (shared_ptr<DoubleLinkedNode> solutionNode : partialSolutionRows) {
		//Offset rowID by one for the column header row
		int rowID = solutionNode->getRow() - 1;
		int row = rowID / sizeSquared;
		int col = (rowID % sizeSquared) / size;
		int num = (rowID % sizeSquared) % size + 1;
		sudokuSolution[row][col] = num;
	}

	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			//Lock the mutex so the drawing thread doesn't try to read data at the same time we write data
			mtx.lock();
			//If the space is empty and we have a partial solution for it, fill it
			if (boardButtons[row][col]->getLabel() == " " && sudokuSolution[row][col] != 0) {
				boardButtons[row][col]->updateHint(sudokuSolution[row][col]);
			}
			//Else if the label is not blank and the user did not put it there, fill it
			else if (boardButtons[row][col]->getLabel() != " " && boardButtons[row][col]->getSource() != SudokuNumberButton::inputSource::user) {
				//if it's a zero, put blank
				if (sudokuSolution[row][col] == 0) {
					boardButtons[row][col]->clicked(10, true);
				}
				else {
					//otherwise put what the program has solved
					boardButtons[row][col]->updateHint(sudokuSolution[row][col]);
				}
			}
			//Unlock the mutex so the drawing thread can access the data
			mtx.unlock();
		}
	}

	//Small pause to allow drawing
	//However, if we are finding many, many solutions, then disable the wait
	if (nSolutions < 1) {
		Sleep(50);
	}
	//Speed up to make it look cooler!
	else if (nSolutions < 10) {
		Sleep(10);
	}
}

void SudokuMaster::searchExactCover(DoubleLinkedNodeMatrix& matrix, vector<vector<shared_ptr<DoubleLinkedNode>>>& solutions, bool solving)
{
	//Step 1 -- If matrix is not empty, proceed
	if (!matrix.empty()) {
		//Step 2 -- Determine the first column with the least number of 1s in it.
		shared_ptr<DoubleLinkedNode> column = matrix.minColumn();

		//Cover the column selected
		cover(column);

		//Step 3 -- Branch the algorithm for each row in this column with a one in it
		for (shared_ptr<DoubleLinkedNode> rowNode = column->down; rowNode != column; rowNode = rowNode->down) {
			//Step 4 -- Include this row in the partial solution
			partialSolutionRows.push_back(rowNode);

			//Update graphical component if we are solving
			if (solving)
				updateSolutionProgress(solutions.size());

			//Step 5 -- cover all rows with 1s in them that have a 1 in the column we are covering
			for (shared_ptr<DoubleLinkedNode> rightNode = rowNode->right; rightNode != rowNode; rightNode = rightNode->right)
				cover(rightNode);

			//Recursively step down a level and travel down this branch of the rowNode, start again with the more covered up matrix
			searchExactCover(matrix, solutions, solving);

			//Pop off the last solution since it is not possible
			partialSolutionRows.pop_back();

			//if solution is not possible, uncover the rows
			for (shared_ptr<DoubleLinkedNode> leftNode = rowNode->left; leftNode != rowNode; leftNode = leftNode->left)
				uncover(leftNode);
		}

		//Uncover the column we started with
		uncover(column);

	}
	else {
		//If the matrix is empty, then this branch has terminated successfully!
		//Add this to the list of solutions as the partial solution is guaranteed to be a whole solution now
		solutions.push_back(partialSolutionRows);
		//Throw the integer 2 if we are not solving a puzzle because we only need to know if more than 1 solution exists
		if (solutions.size() > 1 && !solving) {
			partialSolutionRows.clear();
			throw 2;
		}
	}

}

void SudokuMaster::cover(shared_ptr<DoubleLinkedNode> node)
{
	shared_ptr<DoubleLinkedNode> colNode = node->columnHeader;

	//Unlink column header from its neighbors
	colNode->left->right = colNode->right;
	colNode->right->left = colNode->left;

	for (shared_ptr<DoubleLinkedNode> row = colNode->down; row != colNode; row = row->down) {
		for (shared_ptr<DoubleLinkedNode> rightNode = row->right; rightNode != row; rightNode = rightNode->right) {

			//unlink row node from its north and south neighbors
			rightNode->up->down = rightNode->down;
			rightNode->down->up = rightNode->up;

			//Decrement the column count for the column of the node we are unlinking
			rightNode->columnHeader->decrementNodeCount();

		}
	}

}

void SudokuMaster::uncover(shared_ptr<DoubleLinkedNode> node)
{
	shared_ptr<DoubleLinkedNode> colNode = node->columnHeader;

	//Relink nodes in reverse order
	for (shared_ptr<DoubleLinkedNode> row = colNode->up; row != colNode; row = row->up) {
		for (shared_ptr<DoubleLinkedNode> leftNode = row->left; leftNode != row; leftNode = leftNode->left) {

			//link row node back to its north and south neighbors
			leftNode->up->down = leftNode;
			leftNode->down->up = leftNode;

			//increment the column count for the column of the node we are relinking
			leftNode->columnHeader->incrementNodeCount();

		}
	}

	//link column header to its neighbors
	colNode->left->right = colNode;
	colNode->right->left = colNode;

}


//Function to output the board to the screen. It takes the board as a const pointer to a const so that it is read-only.
//void SudokuMaster::printBoard(const vector<vector<int>>* const board, int size)
//{
//	//This ticks up one every time the console outputs a row of - symbols.
//	int colGridCount = 0;
//	for (int y = 0; y < size + sqrt(size) - 1; y++)
//	{
//		//This ticks up one every time the console outputs a | or + symbol.
//		int rowGridCount = 0;
//		for (int x = 0; x < size + sqrt(size) - 1; x++)
//		{
//			if (x == sqrt(size) + rowGridCount * (sqrt(size) + 1))
//			{
//				if (y == sqrt(size) + colGridCount * (sqrt(size) + 1))
//				{
//					cout << '+';
//				}
//				else
//				{
//					cout << '|';
//				}
//				rowGridCount++;
//			}
//			else if (y == sqrt(size) + colGridCount * (sqrt(size) + 1))
//			{
//				cout << '-';
//			}
//			else
//			{
//				//These calculations offset the y's and x's during output.
//				cout << (*board)[y - colGridCount][x - rowGridCount];
//			}
//		}
//		//If a row of -'s was just printed then now is the time to update the colGridCount.
//		if (y == sqrt(size) + colGridCount * (sqrt(size) + 1))
//		{
//			colGridCount++;
//		}
//		cout << endl;
//	}
//}