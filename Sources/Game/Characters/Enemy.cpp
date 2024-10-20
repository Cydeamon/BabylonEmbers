#include "Enemy.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>

Enemy::Enemy() : Character()
{
    // Setup physics filters
    filterCategories = GamePhysicsCategories::ENEMY;
    filterMask = ENEMY | GROUND | TOWER_BRICK | ARROW | TOWER_TOP | PLAYER | MOLOTOV_PARTICLE;

    Engine::SetPhysFilterCategories(
        physShapeId,
        filterCategories,
        filterMask
    );

    // Set initial position outside of the screen on random side
    bool left = rand() % 2 == 0;
    float x = (left ? 0 : Engine::GetInternalResolution().x);
    SetPosition({x, Game::FloorY - (size.y / 2)});
    
    // Set initial velocity
    moveDirection = {left ? 1 : -1, 0};    
    lookDirection = {moveDirection.x, moveDirection.y};

}
