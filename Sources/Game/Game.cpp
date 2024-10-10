#include "Game.h"
#include <stdlib.h>
#include <iostream>
#include "PhysicsCategory.h"
#include "Characters/EnemySmasher.h"

Game::Game()
{
    Engine::Init({1600, 900}, {480, 270});
    font = LoadFont("Assets/Fonts/PressStart2P-Regular.ttf");
    Engine::SetDrawHUDCallback(std::bind(&Game::drawUI, this));
}

void Game::Run()
{
    prepareScene();
    EnemySmasher enemySmasher;

    while (Engine::IsRunning())
    {   
        // Update 
        Engine::Update();
        
        // Draw
        Engine::Draw();
        
    }

    Engine::Deinit();
}

void Game::prepareScene()
{
    PhysicsRectangle *floor = new PhysicsRectangle(
        {0, (float) FloorY}, 
        {1000, (float) FloorHeight}, 
        PhysicsRectangle::BodyType::STATIC
    );

    b2Shape_SetUserData(floor->GetShapeId(), floor);

    Engine::SetPhysFilterCategories(
        floor->GetShapeId(),
        GamePhysicsCategories::GROUND,
        GamePhysicsCategories::ENEMY | GamePhysicsCategories::TOWER_TOP | GamePhysicsCategories::TOWER_BRICK | 
        GamePhysicsCategories::PLAYER | GamePhysicsCategories::ARROW | GamePhysicsCategories::DEBRIS
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
            bricks.push_back(new Brick(Vector2{x, y}, Vector2{brickWidth, brickHeightInPX}, gapBetweenBricksInPX));
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
    std::string msg = "YOU DIED";
    int msgWidth = MeasureTextEx(font, msg.c_str(), 40, 0).x;
    DrawTextEx(font, msg.c_str(), {Engine::GetInternalResolution().x / 2 - msgWidth / 2, Engine::GetInternalResolution().y / 2 - 50}, 40, 0, BLACK);
}

void Game::drawUI()
{
    if (player && player->IsDead())
        showEndGameScreen();
}
