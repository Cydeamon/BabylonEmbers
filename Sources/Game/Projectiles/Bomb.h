#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>
#include <Game/Characters/Enemy.h>

class Bomb : public PhysicsRectangle
{
public:
    Bomb(Enemy *thrower, Vector2 lookDirection, Vector2 initPosition, Vector2 size = {5, 1});
    void Update() override;
    
private: 
    void processCollisions();
    Enemy *thrower;
    bool blownUp = false;
    float explosionRadius = 15;
    float blowTime = 5;
    double countStartTime;
    void explode();
};