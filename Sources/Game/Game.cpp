#include "Game.h"
#include <iostream>
#include <stdlib.h>
#include "Engine/Engine.h"
#include "PhysicsCategory.h"
#include "Characters/EnemySmasher.h"
#include "Characters/EnemyBomber.h"
#include "Characters/EnemyGunner.h"
#include "Characters/EnemyMolotov.h"
#include "box2d/box2d.h"
#include "raylib.h"
#include <filesystem>
#include <algorithm>

int Game::EnemiesLeft = 0;

Game::Game()
{
    Engine::Init({480, 270});
    font = LoadFont("Assets/Fonts/PressStart2P-Regular.ttf");
    Engine::SetDrawHUDCallback(std::bind(&Game::drawUI, this));
    startGameTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/StartScreen.png");
    EnemiesLeft = level1EnemiesNum;

    loadAllTextures();
    loadAllSounds();

    Engine::PlayAudio("Menu", Engine::LOOP, musicVolume);
}

void Game::Run()
{
    while (Engine::IsRunning())
    {   
        if (!isLevelTransition && !gameOver)
            Engine::Update();

        Engine::Draw();
        
        if (gameIsStarted)
        {            
            Engine::SetPaused(false);
            
            if (Player::IsAlive)
            {
                if (enemiesLeftToSpawn && GetTime() - lastEnemySpawnTime > enemySpawnInterval)
                    spawnEnemy();

                if (EnemiesLeft <= 0 && !isLevelTransition) 
                {
                    Engine::StopAudio("Gameplay");
                    Engine::PlayAudio("WinFanfare", Engine::ONCE, musicVolume);
                    isLevelTransition = true; 
                }

                if (isLevelTransition && IsKeyPressed(KEY_ENTER))
                {
                    
                    clearLevel();
                    generateTower();

                    level++;
                    EnemiesLeft = level1EnemiesNum * level * enemiesNumberScale;
                    enemiesLeftToSpawn = EnemiesLeft;
                    enemySpawnInterval = enemySpawnIntervalInitial / ((level + 1) / 1.995);
                    isLevelTransition = false;

                    Engine::PlayAudio("Gameplay", Engine::LOOP, musicVolume);
                    Engine::StopAudio("WinFanfare");
                    Engine::StopAudio("LoseFanfare");
                }
            }
            else
            {
                if (!gameOver)
                {
                    isLevelTransition = false;
                    gameOver = true;

                    Engine::StopAudio("Gameplay");
                    Engine::PlayAudio("LoseFanfare", Engine::ONCE, musicVolume);
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

                    Engine::StopAudio("LoseFanfare");
                    Engine::PlayAudio("Gameplay", Engine::LOOP, musicVolume);
                }
            }
        }
        else
        {
            Engine::SetPaused(true);
            if (IsKeyPressed(KEY_ENTER))
            {
                prepareScene();
                gameIsStarted = true;
                Engine::StopAudio("Menu");
                Engine::PlayAudio("Gameplay", Engine::LOOP, musicVolume);
            }
        }
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
    Vector2 towerTopPos = {
        1 + (Engine::GetInternalResolution().x / 2), 
        towerTopRowY - (towerTop->GetSize().y / 2)
    };

    towerTop->SetPosition(towerTopPos);

    player = new Player();
    player->SetPosition({
        (Engine::GetInternalResolution().x / 2) - (player->GetSize().x / 2), 
        towerTopRowY - player->GetSize().y - 3.0f
    });
}

void Game::showEndGameScreen()
{
    float centerX = Engine::GetInternalResolution().x / 2;
    int bgHeight = 5;
    int bgY = Engine::GetInternalResolution().y / 2 - 50;
        
    std::string msg = "YOU DIED";
    Vector2 line1size = MeasureTextEx(font, msg.c_str(), 40, 0);
    bgHeight += line1size.y + messagesPadding;

    std::string reasonString = "CAUSE OF DEATH: " + Player::ReasonDead;
    std::transform(reasonString.begin(), reasonString.end(), reasonString.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    Vector2 reasonDeadMsgSize = MeasureTextEx(font, reasonString.c_str(), 8, 0);
    bgHeight += reasonDeadMsgSize.y + messagesPadding;

    char restartMsg[64];
    sprintf(restartMsg, "PRESS [R] TO RESTART");
    Vector2 line2size = MeasureTextEx(font, restartMsg, 16, 0);
    bgHeight += line2size.y + messagesPadding;

    drawUIBackground(bgY - messagesPadding, bgHeight);
    DrawTextEx(font, msg.c_str(), {centerX - (line1size.x / 2), Engine::GetInternalResolution().y / 2 - 50}, 40, 0, BLACK);
    DrawTextEx(font, reasonString.c_str(), {centerX - (reasonDeadMsgSize.x / 2), Engine::GetInternalResolution().y / 2 - 6}, 8, 0, BLACK);
    DrawTextEx(font, restartMsg, {centerX - (line2size.x / 2), Engine::GetInternalResolution().y / 2 + 10}, 16, 0, BLACK);
}

void Game::drawUI()
{
    if (gameIsStarted)
    {
        if (isLevelTransition)
        {
            float centerX = Engine::GetInternalResolution().x / 2;
            int bgHeight = 5;
            int bgY = 100;
            
            char levelMsg[64];
            sprintf(levelMsg, "LEVEL %d COMPLETE", level);
            Vector2 line1size = MeasureTextEx(font, levelMsg, 16, 0);
            bgHeight += line1size.y + messagesPadding;

            char continueMsg[64];
            sprintf(continueMsg, "PRESS [ENTER] TO CONTINUE...", 1);
            Vector2 line2size = MeasureTextEx(font, continueMsg, 16, 0);
            bgHeight += line2size.y + messagesPadding;
            
            drawUIBackground(bgY - messagesPadding, bgHeight);
            DrawTextEx(font, levelMsg, {centerX - (line1size.x / 2), 100}, 16, 0, BLACK);
            DrawTextEx(font, continueMsg, {centerX - (line2size.x / 2), 125}, 16, 0, BLACK);
        }
        else
        if (gameOver)
            showEndGameScreen();
        else
        {
            char levelMsg[64];
            sprintf(levelMsg, "LEVEL: %d", level);

            char EnemiesLeftMsg[64];
            sprintf(EnemiesLeftMsg, "ENEMIES: %d", EnemiesLeft);

            char rateMsg[64];
            sprintf(rateMsg, "SPAWN RATE: %fs", enemySpawnInterval);

            int bgHeight = 5 + 15 + 25 + (messagesPadding * 2);
            int bgY = 5;

            DrawTextEx(font, levelMsg, {5, 5}, 8, 0, BLACK);
            DrawTextEx(font, EnemiesLeftMsg, {5, 15}, 8, 0, BLACK);
            DrawTextEx(font, rateMsg, {5, 25}, 8, 0, BLACK);
        }
    }
    else
    {
        DrawTexture(startGameTexture, 0, 0, WHITE);
    }
}

void Game::drawUIBackground(int bgY, int bgHeight)
{
    // Draw white border box
    DrawLine(0, bgY - 2, Engine::GetInternalResolution().x, bgY - 2, WHITE);
    DrawLine(0, bgY + bgHeight + 1, Engine::GetInternalResolution().x, bgY + bgHeight + 1, WHITE);

    // Draw black border box
    DrawLine(0, bgY - 1, Engine::GetInternalResolution().x, bgY - 1, BLACK);
    DrawLine(0, bgY + bgHeight, Engine::GetInternalResolution().x, bgY + bgHeight, BLACK);

    // Draw white background box
    DrawRectangle(0, bgY, Engine::GetInternalResolution().x, bgHeight, WHITE);
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

void Game::loadAllTextures()
{
    std::string texturesPath = "Assets/Textures/";
    
    for (const auto & entry : std::filesystem::directory_iterator(texturesPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".png")
            Engine::LoadTextureFromTexturePool(entry.path().string());
    }
}

void Game::loadAllSounds()
{
    std::vector<std::string> soundsPaths = {"Assets/Sounds/", "Assets/Music/"};
    
    for (const auto & soundsPath : soundsPaths)
    {
        for (const auto & entry : std::filesystem::directory_iterator(soundsPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".wav")
                Engine::LoadAudioFile(entry.path().string());
        }
    }
}