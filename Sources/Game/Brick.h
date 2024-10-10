#pragma once

#include <Engine/GameObjects/PhysicsRectangle.h>

class Brick : public PhysicsRectangle
{
public:
    Brick(Vector2 position = {0}, Vector2 size = {0}, float gap = 0);
    ~Brick();
    void Update() override;
    void Destroy();
    void Damage();
    int GetHealth() { return health; }
    
private: 
    int health = 3;    
};