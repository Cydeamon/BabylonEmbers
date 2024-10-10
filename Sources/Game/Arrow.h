#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>

class Arrow : public PhysicsRectangle
{
public:
    Arrow(Vector2 initPosition, Vector2 size = {5, 1});
    void Update() override;
    
private: 
    void processCollisions();
};