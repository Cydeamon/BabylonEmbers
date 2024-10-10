#pragma once

#define PHYSAC_PENETRATION_ALLOWANCE = 0.05f;

#include <Engine/Engine.h>
#include "Brick.h"
#include "TowerTop.h"
#include "Characters/Player.h"
#include <vector>

class Game
{
public:
    Game();
    void Run();

	const static int FloorHeight = 40;
	const static int FloorY = 230;
private:
    std::vector<Brick *> bricks;
	float brickHeightInPX = 8;
	int towerHeightInBricks = 15;
	float gapBetweenBricksInPX = 1;
	int firstBricksLineWidthInPX = 112;
	int sidesOffsetPerLine = 2;
	int minBrickWidth = 10;
	int maxBrickWidth = 20;
	Font font;
	TowerTop* towerTop = nullptr;
	Player* player = nullptr;

	void prepareScene();
    void generateTower();
	void showEndGameScreen();
	void drawUI();
};