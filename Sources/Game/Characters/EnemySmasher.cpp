#include "Player.h"
#include <iostream>

Player::Player() : Character()
{
    spritesheetTextureIdle = LoadTexture("Assets/PlayerIdle.png");
    spritesheetTextureRunning = LoadTexture("Assets/PlayerRun.png");
}


void Player::Update()
{
    Character::Update();    
    
    if (!dead)
    {        
        moveDirection = {0};

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

            if (IsKeyDown(KEY_LEFT))
            {
                moveDirection.x = -1;
                lookDirection.x = -1;
            }
            else if (IsKeyDown(KEY_RIGHT))
            {
                moveDirection.x += 1;
                lookDirection.x = 1;
            }
            else
                state = IDLE;

            if (finalVelocity.x > -speed && moveDirection.x < 0)
                finalVelocity.x = moveDirection.x * speed;
            else if (finalVelocity.x < speed && moveDirection.x > 0)
                finalVelocity.x = moveDirection.x * speed;

            if (moveDirection.x != 0 && (finalVelocity.x < 0 || finalVelocity.x > 0))
                state = RUNNING;

            b2Body_SetLinearVelocity(physBodyId, finalVelocity);
        }       

        // Set animation texture
        if (state == IDLE && currentTexture != &spritesheetTextureIdle)
        {
            SetAnimationTexture(&spritesheetTextureIdle);
            frameDuration = 1.0f / 12.0f;
        }

        if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
        {
            SetAnimationTexture(&spritesheetTextureRunning);
            frameDuration = 1.0f / 24.0f;
        }
    }
}