#pragma once

#define PHYSAC_PENETRATION_ALLOWANCE = 0.05f;

#include <Engine/Engine.h>
#include "Brick.h"
#include "TowerTop.h"
#include "Characters/Player.h"
#include "Characters/Enemy.h"
#include <vector>


class Game
{
public:
    Game();
    void Run();

	const static int FloorHeight = 40;
	const static int FloorY = 230;
	static int EnemiesLeft;

private:
	Texture startGameTexture = {0};
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
	double enemySpawnIntervalInitial = 2;
	double enemySpawnInterval = enemySpawnIntervalInitial;
	double lastEnemySpawnTime = -1000;
	bool gameOver = false;
	int level = 1;
	bool isLevelTransition = false;
	bool gameIsStarted = false;
	float enemiesNumberScale = 1.1;
	int level1EnemiesNum = 10;
	int enemiesLeftToSpawn = level1EnemiesNum;
	int messagesPadding = 5;

	void prepareScene();
    void generateTower();
	void showEndGameScreen();
	void drawUI();
	void drawUIBackground(int bgY, int bgHeight);
	void spawnEnemy();
	void clearLevel();
	void loadAllTextures();
};