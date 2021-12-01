#include "DoubleLinkedNode.h"


DoubleLinkedNode::DoubleLinkedNode()
{
    rowID = 0;
    colID = 0;
    nodeCount = 0;
}

int DoubleLinkedNode::getRow()
{
    return rowID;
}

void DoubleLinkedNode::setRow(int row)
{
    rowID = row;
}

int DoubleLinkedNode::getCol()
{
    return colID;
}

void DoubleLinkedNode::setCol(int col)
{
    colID = col;
}

void DoubleLinkedNode::setRowCol(int col, int row)
{
    rowID = row;
    colID = col;
}

int DoubleLinkedNode::getNodeCount()
{
    return nodeCount;
}

void DoubleLinkedNode::incrementNodeCount()
{
    nodeCount++;
}

void DoubleLinkedNode::decrementNodeCount()
{
    nodeCount--;
}
