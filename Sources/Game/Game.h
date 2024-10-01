#pragma once

#define PHYSAC_PENETRATION_ALLOWANCE = 0.05f;

#include <Engine/Engine.h>
#include "Brick.h"
#include <vector>

class Game
{
public:
    Game();
    void Run();

private:
    std::vector<Brick *> bricks;

    void generateTower();
	float brickHeightInPX = 8;
	int towerHeightInBricks = 15;
	float gapBetweenBricksInPX = 1;
	int floorY = 250;
	int firstBricksLineWidthInPX = 112;
	int floorHeight = 40;
	int sidesOffsetPerLine = 2;
	int minBrickWidth = 10;
	int maxBrickWidth = 20;
};