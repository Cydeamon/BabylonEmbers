#include "EnemySmasher.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>

EnemySmasher::EnemySmasher() : Enemy()
{
    spritesheetTextureAttack = LoadTexture("Assets/EnemySmasherAttack.png");
    spritesheetTextureRunning = LoadTexture("Assets/EnemySmasherRun.png");
    b2Shape_SetUserData(physShapeId, this);

    // Set initial position outside of the screen on random side
    bool left = rand() % 2 == 0;
    float x = (left ? +50 : Engine::GetInternalResolution().x - 50);
    SetPosition({x, Game::FloorY - (size.y / 2)});

    // Set initial velocity
    moveDirection = {left ? 1 : -1, 0};    
    lookDirection = {moveDirection.x, moveDirection.y};

}



void EnemySmasher::Update()
{
    Enemy::Update();

    if (!dead)
    {
        processCollisions();

        // Set animation texture
        if (state == ATTACK && currentTexture != &spritesheetTextureAttack)
            SetAnimationTexture(&spritesheetTextureAttack);

        if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
            SetAnimationTexture(&spritesheetTextureRunning);

        if (GetTime() - waitTimeStart > waitTimeAfterDestroy)
        {
            lookDirection = {moveDirection.x, moveDirection.y};
            // Move in direction
            if (state == RUNNING)    
                b2Body_SetLinearVelocity(physBodyId, {moveDirection.x * 50, moveDirection.y});

            // If attacking, apply damage to the brick
            if (state == ATTACK)
            {
                if (curFrame == 0)
                    wasAttacked = false;

                if (curFrame == 8 && !wasAttacked && targetBrick)
                {
                    targetBrick->Damage();
                    wasAttacked = true;

                    if (!targetBrick->GetHealth())
                    {
                        state = RUNNING;
                        targetBrick = nullptr;
                        waitTimeStart = GetTime();
                    }
                }
            }
        }
        else
        {
            b2Body_SetLinearVelocity(physBodyId, {-moveDirection.x * 50, moveDirection.y});
            lookDirection = {-moveDirection.x, moveDirection.y};
        }
    }
}

void EnemySmasher::processCollisions()
{
    if (GetTime() - waitTimeStart > waitTimeAfterDestroy)
    {
        int bodyContactCapacity = b2Body_GetContactCapacity(physBodyId);
        b2ContactData contactData[bodyContactCapacity];
        int bodyContactCount = b2Body_GetContactData(physBodyId, contactData, bodyContactCapacity);

        for (int i = 0; i < bodyContactCapacity && i < bodyContactCount; i++)
        {
            b2ContactData* data = contactData + i;
            void* contacts[2];
            contacts[0] = b2Shape_GetUserData(data->shapeIdA);
            contacts[1] = b2Shape_GetUserData(data->shapeIdB);

            for (int j = 0; j < 2; j++)
            {
                if (contacts[j])
                {
                    GameObject* other = (GameObject*) contacts[j];
                    Brick* otherBrick = dynamic_cast<Brick*>(other);
                    
                    if (otherBrick && state == EnemySmasherState::RUNNING)
                    {
                        state = EnemySmasherState::ATTACK;
                        targetBrick = otherBrick;
                        b2Body_SetLinearVelocity(physBodyId, {0, 0});
                    }
                }
            }        
        }
    }
}