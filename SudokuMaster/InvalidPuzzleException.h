#pragma once

#include <stdexcept>

using namespace std;

class InvalidPuzzleException :
    public exception
{
public:
    InvalidPuzzleException();
};

