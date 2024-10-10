#include "Player.h"
#include <Game/PhysicsCategory.h>
#include <Game/Arrow.h>
#include <iostream>

Player::Player() : Character()
{
    spritesheetTextureIdle = LoadTexture("Assets/PlayerIdle.png");
    spritesheetTextureRunning = LoadTexture("Assets/PlayerRun.png");
    crossbowTexture = LoadTexture("Assets/Crossbow.png");
    
    // Setup physics filters
    filterCategories = GamePhysicsCategories::PLAYER;
    filterMask = 
        GamePhysicsCategories::ENEMY | GamePhysicsCategories::GROUND | GamePhysicsCategories::TOWER_BRICK | 
        GamePhysicsCategories::ARROW | GamePhysicsCategories::TOWER_TOP | GamePhysicsCategories::PLAYER;

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

        // Apply controls if on ground
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

        if (IsMouseButtonDown(0))
        {
            if (GetTime() - lastShotTime > weaponCooldownTime)
            {
                shot();
                lastShotTime = GetTime();
            }
        }


        // Rotate weapon
        weaponAngle = atan2(Engine::GetMousePositionScaled().y - position.y, Engine::GetMousePositionScaled().x - position.x);
        weaponAngle *= RAD2DEG;

        if (state == IDLE)
        {
            if (position.x - Engine::GetMousePositionScaled().x < 0)
                lookDirection.x = 1;
            else
                lookDirection.x = -1;
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

void Player::Draw()
{
    Character::Draw();

    if (!dead)
    {
        DrawTexturePro(
            crossbowTexture,
            {0, 0, crossbowTexture.width, crossbowTexture.height},
            {position.x + 10, position.y + 9, crossbowTexture.width, crossbowTexture.height},
            {lookDirection.x > 0 ? 1 : -3, 3},
            weaponAngle,
            WHITE
        );
    }
}

void Player::shot()
{
    Vector2 arrowStartPos = {lookDirection.x > 0 ? position.x + 10 : position.x + 3, lookDirection.x > 0 ? position.y + 9 : position.y + 9};
    new Arrow(arrowStartPos);
}
