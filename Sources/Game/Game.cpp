#include "Game.h"
#include <stdlib.h>
#include <iostream>

Game::Game()
{
    Engine::Init({1600, 900}, {480, 270});
    floorY = Engine::GetInternalResolution().y - floorHeight;
}

void Game::Run()
{
    PhysicsRectangle floor({0, floorY}, {480, floorHeight});
    generateTower();

    while (Engine::IsRunning())
    {   
        Engine::Draw();
    }

    Engine::Deinit();
}

void Game::generateTower()
{
    int towerTopRowY = 0;

    for (int curRow = 0; curRow < towerHeightInBricks; curRow++)
    {
        int widthLeft = firstBricksLineWidthInPX - (sidesOffsetPerLine * 2 * curRow);
        float x = Engine::GetInternalResolution().x / 2 - (widthLeft / 2);
        float y = floorY - brickHeightInPX - gapBetweenBricksInPX - (curRow * brickHeightInPX) - (gapBetweenBricksInPX * curRow);
        towerTopRowY = (int) y;

        while (widthLeft > 0)
        {
            float randomWidth = (rand() % (maxBrickWidth - minBrickWidth)) + minBrickWidth;
            float brickWidth = widthLeft > maxBrickWidth ? randomWidth : widthLeft;
            bricks.push_back(new Brick(Vector2{x, y}, Vector2{brickWidth, brickHeightInPX}));
            widthLeft -= brickWidth;
            x += brickWidth + gapBetweenBricksInPX;
        }
    }

    // TowerTop.Position = new(0, towerTopRowY - towerTopHeight - 10);
    // TowerTop.GlobalPosition = new(GetViewportRect().Size.X / 2, TowerTop.GlobalPosition.Y);
}
