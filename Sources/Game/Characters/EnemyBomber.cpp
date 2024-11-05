#include "EnemyBomber.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>
#include <Game/Projectiles/Bomb.h>
#include <Game/Characters/Player.h>

EnemyBomber::EnemyBomber() : Enemy()
{
    spritesheetTextureAttack = LoadTexture("Assets/EnemyBomberAttack.png");
    spritesheetTextureRunning = LoadTexture("Assets/EnemyBomberRun.png");
    b2Shape_SetUserData(physShapeId, this);
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

            if (distance < 150)
                state = EnemyBomber::ATTACK;
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
