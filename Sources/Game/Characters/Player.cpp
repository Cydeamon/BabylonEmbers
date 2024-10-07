#include "Player.h"
#include <iostream>

Player::Player() : Character()
{
    initPhysicsBody();
    SetDrawPriority(LOW);
}


void Player::Update()
{
    Character::Update();    
    
    if (!dead)
    {
        // Kill player if out of bounds
        if (position.x + size.x < 0 || position.x - size.x > Engine::GetInternalResolution().x || 
            position.y + size.y < 0 || position.y - size.y > Engine::GetInternalResolution().y)
        {
            die();
            return;
        }

        // Apply controls
        int capacity = b2Body_GetContactCapacity(physBodyId);

        if (capacity)
        {
            b2Vec2 finalVelocity = b2Body_GetLinearVelocity(physBodyId);
    
            if (IsKeyDown(KEY_LEFT) && finalVelocity.x > -speed)
                finalVelocity.x = -speed;
            
            if (IsKeyDown(KEY_RIGHT) && finalVelocity.x < speed)
                finalVelocity.x = speed;

            b2Body_SetLinearVelocity(physBodyId, finalVelocity);
        }        
    }
}