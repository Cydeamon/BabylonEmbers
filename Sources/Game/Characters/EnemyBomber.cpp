#include "EnemyBomber.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>
#include <Game/Projectiles/Bomb.h>
#include <Game/Characters/Player.h>

EnemyBomber::EnemyBomber() : Enemy()
{
    spritesheetTextureAttack = Engine::LoadTextureFromTexturePool("Assets/Textures/EnemyBomberAttack.png");
    spritesheetTextureRunning = Engine::LoadTextureFromTexturePool("Assets/Textures/EnemyBomberRun.png");
    b2Shape_SetUserData(physShapeId, this);
    stopX = 150 + (rand() % 30) - 15;
}

void EnemyBomber::Update()
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

        if (animationPaused && state == ATTACK) 
        {
            curFrame = 0;
        }

        if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
            SetAnimationTexture(&spritesheetTextureRunning);

        if (state == RUNNING)    
        {
            playStepsSounds = true;

            // Move in direction
            if (Player::BelowThreshold)
                moveDirection = {Player::Position.x < 0 ? -1.0f : 1.0f, 0};

            b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);

            if (abs(velocity.x) < 40)
                b2Body_SetLinearVelocity(physBodyId, {moveDirection.x * 50, moveDirection.y});

            // If close enough to center, throw bomb
            float distance = abs((Engine::GetInternalResolution().x / 2) - (position.x + (size.x / 2)));

            if (Player::BelowThreshold)
                distance = abs(Player::Position.x - position.x);

            if (distance < stopX)
                state = EnemyBomber::ATTACK;
        }
        else
        {
            playStepsSounds = false;
        }

        if (GetTime() - waitTimeStart > waitTimeAfterThrow)
        {   
            if (animationPaused)
            {
                attacked = false;
                animationPaused = false;
            }

            if (state == ATTACK && !attacked && curFrame == 8)
            {                
                if (Player::BelowThreshold)
                    new Bomb(this, position);
                else 
                    new Bomb(this, {lookDirection.x,  lookDirection.y}, position, {2, 2});

                waitTimeStart = GetTime();
                attacked = true;
            }
        }
    }
}
