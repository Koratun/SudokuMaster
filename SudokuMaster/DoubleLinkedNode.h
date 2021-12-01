#pragma once
#include<memory>
#include<vector>

using namespace std;

class DoubleLinkedNode
{
public:
	DoubleLinkedNode();

	shared_ptr<DoubleLinkedNode> right;
	shared_ptr<DoubleLinkedNode> left;
	shared_ptr<DoubleLinkedNode> down;
	shared_ptr<DoubleLinkedNode> up;

	shared_ptr<DoubleLinkedNode> columnHeader;

	int getRow();
	void setRow(int row);
	int getCol();
	void setCol(int col);
	void setRowCol(int col, int row);

	int getNodeCount();
	void incrementNodeCount();
	void decrementNodeCount();

private:
	int rowID;
	int colID;
	int nodeCount;


};

