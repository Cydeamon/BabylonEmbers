#include "EnemyMolotov.h"
#include "Engine/Engine.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>
#include <Game/Projectiles/Molotov.h>

EnemyMolotov::EnemyMolotov() : Enemy()
{
    spritesheetTextureAttack = Engine::LoadTextureFromTexturePool("Assets/Textures/EnemyMolotovAttack.png");
    spritesheetTextureRunning = Engine::LoadTextureFromTexturePool("Assets/Textures/EnemyMolotovRun.png");
    b2Shape_SetUserData(physShapeId, this);
    stopX = 150 + (rand() % 30) - 15;
}

void EnemyMolotov::Update()
{
    Enemy::Update();

    if (!dead)
    {
        // Set animation texture
        if (state == ATTACK && currentTexture != &spritesheetTextureAttack)
        {
            SetAnimationTexture(&spritesheetTextureAttack);
            repeatAnimation = false;
        }        

        if (state == ATTACK && animationPaused)
            curFrame = 0;

        if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
            SetAnimationTexture(&spritesheetTextureRunning);

        if (state == RUNNING)    
        {
            playStepsSounds = true;

            // Move in direction
            b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);

            if (abs(velocity.x) < 40)
                b2Body_SetLinearVelocity(physBodyId, {moveDirection.x * 50, moveDirection.y});

            // If close enough to center, throw bomb
            float distanceToCenter = abs((Engine::GetInternalResolution().x / 2) - (position.x + (size.x / 2)));

            if (distanceToCenter < stopX)
                state = EnemyMolotov::ATTACK;
        }
        else
        {
            playStepsSounds = false;
        }

        if (GetTime() - waitTimeStart > waitTimeAfterThrow)
        {   
            if (curFrame == 0)
            {
                attacked = false;
                animationPaused = false;
            }

            if (state == ATTACK)
            {                
                lookDirection.x = Player::Position.x < position.x ? -1 : 1;

                if (!attacked && curFrame == 8)
                {
                    new Molotov(position);
                    waitTimeStart = GetTime();
                    attacked = true;
                }
            }
        }
    }
}
