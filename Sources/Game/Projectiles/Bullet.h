#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>

class Bullet : public PhysicsRectangle
{
public:
    Bullet(Vector2 initPosition, bool xPositive, Vector2 size = {1, 1});
    void Update() override;
    
private: 
    void processCollisions();
};