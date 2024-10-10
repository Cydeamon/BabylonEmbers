#include "Enemy.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>

Enemy::Enemy() : Character()
{
    // Setup physics filters
    filterCategories = GamePhysicsCategories::ENEMY;
    filterMask = 
        GamePhysicsCategories::ENEMY | GamePhysicsCategories::GROUND | GamePhysicsCategories::TOWER_BRICK | 
        GamePhysicsCategories::ARROW | GamePhysicsCategories::TOWER_TOP | GamePhysicsCategories::PLAYER;

    Engine::SetPhysFilterCategories(
        physShapeId,
        filterCategories,
        filterMask
    );
}