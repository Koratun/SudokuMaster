#pragma once
#include <vector>
#include <mutex>
#include <fstream>
#include "SudokuNumberButton.h"
#include "DoubleLinkedNodeMatrix.h"

using namespace std;

class SudokuMaster
{
public:
	SudokuMaster(sf::Font& font, float x, float y, function<int()> numberSelected, function<bool()> markMode, function<void()> transitionStage);

	///Pushes pointers of every board button into the given vector, type T must be a SudokuNumberButton or one of its subclasses
	template <class T>
	void registerContainer(vector<shared_ptr<T>>& container) {
		for (vector<shared_ptr<SudokuNumberButton>>& row : boardButtons) {
			for (shared_ptr<SudokuNumberButton> button : row) {
				container.push_back(button);
			}
		}
	}
	
	sf::RectangleShape separatorRects[4];

	mutex mtx;

	//Create Sudoku puzzle from start to finish
	void createSudokuPuzzle(int difficulty);

	//Update the labels on all the buttons
	void updateBoardLabels();

	//Saves the progress of the puzzle
	void saveProgress(int difficulty);

	//To be called if the Continue button is pressed, returns the difficulty level loaded
	int loadPuzzle();

	//Takes a file and tries to put it into the puzzle
	//Throws InvalidPuzzleException if the file is formatted incorrectly.
	void fileToSolve(ifstream& puzzleStream);

	//Check user input against correct puzzle
	bool checkPuzzle();

	//Gives a hint to the user by solving a space
	void giveHint();

	//Activate board buttons for generation animation.
	void activateButtons();

	//Wipes all data to prep for a new puzzle
	void wipeData();

	//Solves the puzzle stored in userBoardData
	void solvePuzzle();

	//Returns the number of solutions found
	int solutionsFound();

	//Displays the next solution found for the puzzle
	//or the previous solution if forward is false
	void rotateSolution(bool forward);

	int getSolutionIndex();

private:
	int solutionIndex = 0;

	vector<vector<shared_ptr<SudokuNumberButton>>> boardButtons;

	//A three dimensional vector containing all possible values for every position on the board. This will be updated as the board is generated.
	vector<vector<vector<int>>> allPossibilities;

	//A list of solutions possible for the given puzzle in the solve portion.
	vector<vector<vector<int>>> puzzleSolutions;

	vector<vector<int>> completeBoardData;

	vector<vector<int>> userBoardData;

	function<int()> numberSelected;

	function<bool()> markMode;

	function<void()> transitionStage;

	float x, y;

	vector<shared_ptr<DoubleLinkedNode>> partialSolutionRows;

	void updateButtonNotes();

	void generateBoard();

	bool tryNewBoardValue(int x, int y, vector<vector<int>>& boardData, int numberToAdd = 0);

	bool boardStillPossible(int numberToAdd, int x, int y, vector<vector<int>>& boardData);

	string convertBase(int n, int base);

	//After generating a board, make it into a puzzle
	void puzzlify(int difficulty);

	vector<sf::Vector2i> failedRemovalPoints;

	//Checks if the row, column, and outer square does not contain the given value (thus making it possible to place the value here)
	bool checkValidValue(int val, int x, int y, vector<vector<int>>& puzzle);

	//Remove a value at a random position
	//Returns true if successful, false if no more points can be removed.
	bool removeBoardValue();

	//Remove a value at a specific position
	void removeBoardValue(int x, int y, vector<vector<int>>& puzzle);

	//Given a sudoku puzzle, algorithm X will return a list of possible solutions or an empty vector
	//I did not come up with this on my own, which is why this method is named after the algorithm I am using.
	//I learned the concept from wikipedia and impemented it in my program. The same goes for the DoubleLinkedNodes. 
	vector<vector<vector<int>>> algorithmX(vector<vector<int>>& puzzle, bool solving);

	//Returns a board of values containing the current progress of the program in solving the puzzle
	void updateSolutionProgress(int nSolutions);

	//The function that solves the exact cover of the sudoku puzzle
	void searchExactCover(DoubleLinkedNodeMatrix& matrix, vector<vector<shared_ptr<DoubleLinkedNode>>>& solutions, bool solving);

	//Cover the column/rows of the given node in the given matrix
	void cover(shared_ptr<DoubleLinkedNode> node);

	//Uncover the column/rows of the given node in the given matrix
	void uncover(shared_ptr<DoubleLinkedNode> node);


	//void printBoard(const vector<vector<int>>* const board, int size = 9);

};