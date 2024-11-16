#include "Enemy.h"
#include "Character.h"
#include "raylib.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>

Enemy::Enemy() : Character()
{
    // Setup physics filters
    filterCategories = ENEMY;
    filterMask = GROUND | TOWER_BRICK | ARROW | TOWER_TOP | PLAYER | MOLOTOV_PARTICLE;

    Engine::SetPhysFilterCategories(
        physShapeId,
        filterCategories,
        filterMask
    );

    // Set initial position outside of the screen on random side
    bool left = rand() % 2 == 0;
    float x = (left ? -16 : Engine::GetInternalResolution().x + 16);
    SetPosition({x, Game::FloorY - (size.y / 2)});
    
    // Set initial velocity
    moveDirection = {left ? 1.0f : -1.0f, 0};    
    lookDirection = {moveDirection.x, moveDirection.y};
}

void Enemy::Die(Vector2 reactionDirection, float reactionForce, bool dismember)
{
    if (!dead)
    {
        Game::EnemiesLeft--;
        Character::Die(reactionDirection, reactionForce, dismember);
    }
}