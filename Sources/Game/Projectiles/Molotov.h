#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>
#include <Game/Characters/Enemy.h>

class Molotov : public PhysicsRectangle
{
public:
    Molotov(Vector2 initPosition, Vector2 size = {2, 3});
    void Update() override;
    void Explode();
    
private: 
    void processCollisions();
    Enemy *thrower;
    bool blownUp = false;
    float flameParticles;
    double countStartTime;
};