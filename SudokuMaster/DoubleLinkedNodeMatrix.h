#pragma once
#include "DoubleLinkedNode.h"
#include <vector>

using namespace std;

class DoubleLinkedNodeMatrix
{
public:
	DoubleLinkedNodeMatrix(vector<vector<bool>>& problemMatrix);

	//Returns if the matrix is empty
	bool empty();

	//Returns the column with the lowest number of ones
	shared_ptr<DoubleLinkedNode> minColumn();

private:

	int getRight(int i);
	int getLeft(int i);
	int getUp(int i);
	int getDown(int i);

	shared_ptr<DoubleLinkedNode> header;

	int nRows;
	int nCols;

	vector<vector<shared_ptr<DoubleLinkedNode>>> meshedMatrix;

};

