#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>

class Brick : public PhysicsRectangle
{
public:
    Brick(Vector2 position = {0}, Vector2 size = {0}, float gap = 0);
    void Update() override;
    void Destroy(Vector2 reactionDirection = {0});
    void Damage(Vector2 reactionDirection = {0});
    int GetHealth() { return health; }
    
private: 
    int health = 3;    
};