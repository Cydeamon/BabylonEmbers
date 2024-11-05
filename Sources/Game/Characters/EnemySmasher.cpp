#include "EnemySmasher.h"
#include "Engine/Engine.h"
#include "Game/Characters/Player.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>

EnemySmasher::EnemySmasher() : Enemy()
{
    spritesheetTextureAttack = LoadTexture("Assets/EnemySmasherAttack.png");
    spritesheetTextureRunning = LoadTexture("Assets/EnemySmasherRun.png");
    b2Shape_SetUserData(physShapeId, this);
}

void EnemySmasher::Update()
{
    Enemy::Update();

    if (!dead)
    {       
        if (Player::IsGrounded && Player::IsAlive)
            moveDirection = { position.x < Player::Position.x ? 1.0f : -1.0f, 0 };

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

                    if (targetBrick->GetHealth() <= 0)
                    {
                        state = RUNNING;
                        targetBrick = nullptr;
                        waitTimeStart = GetTime();
                    }
                }

                if (curFrame == 8 && !wasAttacked && targetPlayer && Player::IsGrounded)
                {
                    if (abs(position.x - Player::Position.x) <= 20)
                    {
                        targetPlayer->Die(
                            {Player::Position.x - position.x, Player::Position.y - position.y},
                            100
                        );

                        targetPlayer = nullptr;
                        state = RUNNING;
                    }
                }
            }
        }
        else
        {   
            b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);

            if (abs(velocity.x) < 40)
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
        b2ContactData *contactData = new b2ContactData[bodyContactCapacity];
        int bodyContactCount = b2Body_GetContactData(physBodyId, contactData, bodyContactCapacity);

        for (int i = 0; i < bodyContactCapacity && i < bodyContactCount; i++)
        {
            b2ContactData* data = contactData + i;
            GameObject* contacts[2];
            contacts[0] = Engine::GetObjectByPhysShapeId(data->shapeIdA);
            contacts[1] = Engine::GetObjectByPhysShapeId(data->shapeIdB);

            for (int j = 0; j < 2; j++)
            {
                if (contacts[j])
                {
                    GameObject* other = contacts[j];
                    Brick* otherBrick = dynamic_cast<Brick*>(other);
                    Player *player = dynamic_cast<Player*>(other);
                    
                    if (otherBrick && state == EnemySmasherState::RUNNING)
                    {
                        state = EnemySmasherState::ATTACK;
                        targetBrick = otherBrick;
                        b2Body_SetLinearVelocity(physBodyId, {0, 0});
                    }

                    if (player)
                    {
                        state = EnemySmasherState::ATTACK;
                        targetPlayer = player;
                        b2Body_SetLinearVelocity(physBodyId, {0, 0});
                    }
                }
            }
        }
    }
}