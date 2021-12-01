#include "DoubleLinkedNodeMatrix.h"
#include <iostream>

//The problem matrix must have the first row filled with all 1s to properly create the column header row
DoubleLinkedNodeMatrix::DoubleLinkedNodeMatrix(vector<vector<bool>>& problemMatrix)
{
	nCols = problemMatrix.size();
	nRows = problemMatrix[0].size();

	meshedMatrix.resize(nCols);
	for (int col = 0; col < nCols; col++) {
		meshedMatrix[col].resize(nRows);
		for (int row = 0; row < nRows; row++) {
			meshedMatrix[col][row] = make_shared<DoubleLinkedNode>();
		}
	}

	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			//If the position has a one in it, mesh it into the matrix
			//Row and col are flipped in the problem matrix. Unfortunate, but too late to rewire all my code
			if (problemMatrix[col][row]) {
				int rowNeighbor;
				int colNeighbor;

				//Increment the count of nodes in the column header if the row is not 0.
				if (row > 0)
					meshedMatrix[col][0]->incrementNodeCount();

				//Set column header pointer
				meshedMatrix[col][row]->columnHeader = meshedMatrix[col][0];

				//Set row and column IDs
				meshedMatrix[col][row]->setRowCol(col, row);

				if (meshedMatrix[0][0]->getRow() != 0) {
					cout << "What the fruit\n";
				}

				//Left pointer
				rowNeighbor = row;
				colNeighbor = col;
				//Continuously get the neighboring position to the left until
				do {
					colNeighbor = getLeft(colNeighbor);
					//The position is confirmed as true
				} while (!problemMatrix[colNeighbor][rowNeighbor]);
				//Assign the left pointer to the position just found
				meshedMatrix[col][row]->left = meshedMatrix[colNeighbor][rowNeighbor];

				//Right pointer
				rowNeighbor = row;
				colNeighbor = col;
				//Continuously get the neighboring position to the right until
				do {
					colNeighbor = getRight(colNeighbor);
					//The position is confirmed as true
				} while (!problemMatrix[colNeighbor][rowNeighbor]);
				//Assign the right pointer to the position just found
				meshedMatrix[col][row]->right = meshedMatrix[colNeighbor][rowNeighbor];

				//Up pointer
				rowNeighbor = row;
				colNeighbor = col;
				//Continuously get the neighboring position above until
				do {
					rowNeighbor = getUp(rowNeighbor);
					//The position is confirmed as true
				} while (!problemMatrix[colNeighbor][rowNeighbor]);
				//Assign the up pointer to the position just found
				meshedMatrix[col][row]->up = meshedMatrix[colNeighbor][rowNeighbor];

				//Down pointer
				rowNeighbor = row;
				colNeighbor = col;
				//Continuously get the neighboring position below until
				do {
					rowNeighbor = getDown(rowNeighbor);
					//The position is confirmed as true
				} while (!problemMatrix[colNeighbor][rowNeighbor]);
				//Assign the down pointer to the position just found
				meshedMatrix[col][row]->down = meshedMatrix[colNeighbor][rowNeighbor];

			}
		}
	}

	//Initialize the header pointer
	header = make_shared<DoubleLinkedNode>();

	//link header right pointer to the column header of the first column 
	header->right = meshedMatrix[0][0];

	//link header left pointer to the column header of the last column 
	header->left = meshedMatrix[nCols - 1][0];

	//Link the first and last columns to the header
	meshedMatrix[0][0]->left = header;
	meshedMatrix[nCols - 1][0]->right = header;

}

bool DoubleLinkedNodeMatrix::empty()
{
	return header->right == header;
}

shared_ptr<DoubleLinkedNode> DoubleLinkedNodeMatrix::minColumn()
{
	shared_ptr<DoubleLinkedNode> column = header->right;
	shared_ptr<DoubleLinkedNode> minColumn = column;

	column = column->right;

	while (column != header) {
		if (column->getNodeCount() < minColumn->getNodeCount()) {
			minColumn = column;
		}
		column = column->right;
	}

	return minColumn;
}

int DoubleLinkedNodeMatrix::getRight(int i)
{
	return (i + 1) % nCols;
}

int DoubleLinkedNodeMatrix::getLeft(int i)
{
	return (i - 1 < 0) ? nCols - 1 : i - 1;
}

int DoubleLinkedNodeMatrix::getUp(int i)
{
	return (i - 1 < 0) ? nRows - 1 : i - 1;
}

int DoubleLinkedNodeMatrix::getDown(int i)
{
	return (i + 1) % nRows;
}
