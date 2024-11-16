#include "Player.h"
#include "Game/Brick.h"
#include "Game/TowerTop.h"
#include "box2d/box2d.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"
#include "raylib.h"
#include <Game/PhysicsCategory.h>
#include <Game/Projectiles/Arrow.h>
#include <iostream>

Vector2 Player::Position = {0};
bool Player::IsGrounded = false;
bool Player::IsAlive = true;
bool Player::BelowThreshold = false;

Player::Player() : Character()
{
    spritesheetTextureIdle = Engine::LoadTextureFromTexturePool("Assets/Textures/PlayerIdle.png");
    spritesheetTextureRunning = Engine::LoadTextureFromTexturePool("Assets/Textures/PlayerRun.png");
    crossbowTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/Crossbow.png");
    isPlayer = true;
    
    // Setup physics filters
    filterCategories = GamePhysicsCategories::PLAYER;
    filterMask = ENEMY | GROUND | TOWER_BRICK | ARROW | TOWER_TOP | PLAYER | MOLOTOV_PARTICLE;

    b2Shape_SetUserData(physShapeId, this);
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
            Die();
            return;
        }

        // Apply controls if on ground
        int capacity = b2Body_GetContactCapacity(physBodyId);

        if (capacity)
        {
            b2Vec2 finalVelocity = b2Body_GetLinearVelocity(physBodyId);

            if (IsKeyDown(KEY_A))
            {
                moveDirection.x = -1;
                lookDirection.x = -1;
            }
            else if (IsKeyDown(KEY_D))
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

        // Handle audio
        if (state == RUNNING)
        {
            playStepsSounds = true;
        }
        else
        {
            playStepsSounds = false;
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

        processCollisions();
    }

    Position = position;
    BelowThreshold = position.y > thresholdHeight;
    IsAlive = !dead;
}

void Player::Draw() 
{
    Character::Draw();

    if (!dead)
    {
        DrawTexturePro(
            crossbowTexture,
            {0, 0, (float) crossbowTexture.width, (float) crossbowTexture.height},
            {position.x + 10, position.y + 9, (float) crossbowTexture.width, (float) crossbowTexture.height},
            {lookDirection.x > 0 ? 1.0f : -3.0f, 3.0f},
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

void Player::processCollisions()
{
    const int bodyContactCapacity = b2Body_GetContactCapacity(physBodyId);
    b2ContactData *contactData = new b2ContactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(physBodyId, contactData, bodyContactCapacity);

    for (int i = 0; i < bodyContactCapacity && i < bodyContactCount; i++)
    {
        b2ContactData* data = contactData + i;

        if (!b2Shape_IsValid(data->shapeIdA) || !b2Shape_IsValid(data->shapeIdB))
            continue;

        b2Filter filter1 = b2Shape_GetFilter(data->shapeIdA);
        b2Filter filter2 = b2Shape_GetFilter(data->shapeIdB);
        
        IsGrounded = (filter1.categoryBits & GROUND) || (filter2.categoryBits & GROUND);
    }
}