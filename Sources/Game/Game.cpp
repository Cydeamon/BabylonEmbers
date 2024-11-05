#include "Game.h"
#include <stdlib.h>
#include <iostream>
#include "Engine/Engine.h"
#include "PhysicsCategory.h"
#include "Characters/EnemySmasher.h"
#include "Characters/EnemyBomber.h"
#include "Characters/EnemyGunner.h"
#include "Characters/EnemyMolotov.h"
#include "Projectiles/Molotov.h"
#include "box2d/box2d.h"
#include "raylib.h"

int Game::EnemiesLeft = 0;

Game::Game()
{
    Engine::Init({1600, 900}, {480, 270});
    font = LoadFont("Assets/Fonts/PressStart2P-Regular.ttf");
    Engine::SetDrawHUDCallback(std::bind(&Game::drawUI, this));

    EnemiesLeft = level1EnemiesNum;
}

void Game::Run()
{
    prepareScene();

    while (Engine::IsRunning())
    {   
        // Update 
        Engine::Update();

        if (Player::IsAlive)
        {
            if (enemiesLeftToSpawn && GetTime() - lastEnemySpawnTime > enemySpawnInterval)
                spawnEnemy();

            if (EnemiesLeft <= 0)
                isLevelTransition = true; 

            if (isLevelTransition && IsKeyPressed(KEY_ENTER))
            {
                clearLevel();
                generateTower();

                level++;
                EnemiesLeft = level1EnemiesNum * level * enemiesNumberScale;
                enemiesLeftToSpawn = EnemiesLeft;
                enemySpawnInterval = enemySpawnIntervalInitial / (level / 1.5);
                isLevelTransition = false;
            }
        }
        else
        {
            if (!gameOver)
            {
                isLevelTransition = false;
                gameOver = true;
            }
        }
        
        if (gameOver)
        {
            if (IsKeyPressed(KEY_R))
            {
                clearLevel();
                generateTower();
                level = 1;
                EnemiesLeft = level1EnemiesNum;
                enemiesLeftToSpawn = EnemiesLeft;
                enemySpawnInterval = enemySpawnIntervalInitial;
                gameOver = false;
                Player::IsAlive = true;
            }
        }

        // Draw
        Engine::Draw();
    }

    Engine::Deinit();
}

void Game::clearLevel()
{
    player->QueueDestroy();

    std::vector<GameObject*> *gameObjects = Engine::GetGameObjects();

    for(auto object : *gameObjects)
    {
        PhysicsRectangle* physRect = dynamic_cast<PhysicsRectangle*>(object);

        if (physRect || towerTop)
        {
            if (physRect)
            {
                b2Filter filter = b2Shape_GetFilter(physRect->GetShapeId());
                
                if (filter.categoryBits & GROUND)
                    continue;
            }

            object->QueueDestroy();
        }
    }

    towerTop->QueueDestroy();
}

void Game::prepareScene()
{
    PhysicsRectangle *floor = new PhysicsRectangle(
        {-500, (float) FloorY}, 
        {1000, (float) FloorHeight}, 
        PhysicsRectangle::BodyType::STATIC
    );

    b2Shape_SetUserData(floor->GetShapeId(), floor);

    Engine::SetPhysFilterCategories(
        floor->GetShapeId(),
        GROUND,
        ENEMY | TOWER_TOP | TOWER_BRICK | BOMB | PLAYER | ARROW | DEBRIS | BODY | MOLOTOV_PARTICLE
    );

    generateTower();

    b2Shape_SetDensity(floor->GetShapeId(), 0);
    b2Shape_SetFriction(floor->GetShapeId(), 1);
}

void Game::generateTower()
{
    int towerTopRowY = 0;

    for (int curRow = 0; curRow < towerHeightInBricks; curRow++)
    {
        int widthLeft = firstBricksLineWidthInPX - (sidesOffsetPerLine * 2 * curRow);
        float x = Engine::GetInternalResolution().x / 2 - (widthLeft / 2);
        float y = FloorY - brickHeightInPX - gapBetweenBricksInPX - (curRow * brickHeightInPX) - (gapBetweenBricksInPX * curRow);
        towerTopRowY = (int) y;

        while (widthLeft > 0)
        {
            float randomWidth = (rand() % (maxBrickWidth - minBrickWidth)) + minBrickWidth;
            float brickWidth = widthLeft > maxBrickWidth ? randomWidth : widthLeft;
            new Brick(Vector2{x, y}, Vector2{brickWidth, brickHeightInPX}, gapBetweenBricksInPX);
            widthLeft -= brickWidth;
            x += brickWidth + gapBetweenBricksInPX;
        }
    }

    towerTop = new TowerTop();
    towerTop->SetPosition({
        1 + (Engine::GetInternalResolution().x / 2) - (towerTop->GetSize().x / 2), 
        towerTopRowY - towerTop->GetSize().y - gapBetweenBricksInPX
    });

    player = new Player();
    player->SetPosition({
        (Engine::GetInternalResolution().x / 2) - (player->GetSize().x / 2), 
        towerTopRowY - player->GetSize().y - 10.0f
    });
}

void Game::showEndGameScreen()
{
    float centerX = Engine::GetInternalResolution().x / 2;
        
    std::string msg = "YOU DIED";
    int msgWidth = MeasureTextEx(font, msg.c_str(), 40, 0).x;
    DrawTextEx(font, msg.c_str(), {centerX - (msgWidth / 2), Engine::GetInternalResolution().y / 2 - 50}, 40, 0, BLACK);

    char restartMsg[64];
    sprintf(restartMsg, "PRESS [R] TO RESTART");
    msgWidth = MeasureTextEx(font, restartMsg, 16, 0).x;
    DrawTextEx(font, restartMsg, {centerX - (msgWidth / 2), Engine::GetInternalResolution().y / 2}, 16, 0, BLACK);
}

void Game::drawUI()
{
    if (isLevelTransition)
    {
        float centerX = Engine::GetInternalResolution().x / 2;
        
        char levelMsg[64];
        sprintf(levelMsg, "LEVEL %d COMPLETE", 1);
        int msgWidth = MeasureTextEx(font, levelMsg, 16, 0).x;
        DrawTextEx(font, levelMsg, {centerX - (msgWidth / 2), 5}, 16, 0, BLACK);
        
        char continueMsg[64];
        sprintf(continueMsg, "PRESS [ENTER] TO CONTINUE...", 1);
        msgWidth = MeasureTextEx(font, continueMsg, 16, 0).x;
        DrawTextEx(font, continueMsg, {centerX - (msgWidth / 2), 25}, 16, 0, BLACK);
    }
    else
    if (gameOver)
        showEndGameScreen();
    else
    {
        char levelMsg[64];
        sprintf(levelMsg, "LEVEL: %d", level);
        DrawTextEx(font, levelMsg, {5, 5}, 8, 0, BLACK);

        char EnemiesLeftMsg[64];
        sprintf(EnemiesLeftMsg, "ENEMIES: %d", EnemiesLeft);
        DrawTextEx(font, EnemiesLeftMsg, {5, 15}, 8, 0, BLACK);

        char rateMsg[64];
        sprintf(rateMsg, "SPAWN RATE: %fs", enemySpawnInterval);
        DrawTextEx(font, rateMsg, {5, 25}, 8, 0, BLACK);
    }
}

void Game::spawnEnemy()
{
    int type = rand() % 4;
    
    switch (type)
    {
    case 0: new EnemySmasher(); break;
    case 1: new EnemyBomber(); break;
    case 2: new EnemyGunner(); break;
    case 3: new EnemyMolotov(); break;
    }

    lastEnemySpawnTime = GetTime();
    enemiesLeftToSpawn--;
}
