#pragma once

#include "TubesAndBalls.h"

class Tube
{
public:
	Tube() { this->ID = -1; this->Size = -1; };
	Tube(int ID, int Size) { this->ID = ID; this->Size = Size; };
	std::unordered_map<int, std::pair<Tube*, int>> PosToTube;

	int TubeSize() { return Size; }
	int ID;
private:
	int Size;
};
