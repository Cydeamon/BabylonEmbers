#include "EnemyGunner.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>
#include <Game/Projectiles/Bullet.h>

EnemyGunner::EnemyGunner() : Enemy()
{
    spritesheetTextureAttack = LoadTexture("Assets/EnemyGunnerAttack.png");
    spritesheetTextureAttackStart = LoadTexture("Assets/EnemyGunnerAttackStart.png");
    spritesheetTextureAttackEnd = LoadTexture("Assets/EnemyGunnerAttackEnd.png");
    spritesheetTextureRunning = LoadTexture("Assets/EnemyGunnerRun.png");
    b2Shape_SetUserData(physShapeId, this);
}

void EnemyGunner::Update()
{
    Enemy::Update();

    if (!dead)
    {
        if (state != WAITING)
        {
            if (state == ATTACK_START && currentTexture != &spritesheetTextureAttackStart)
            {
                SetAnimationTexture(&spritesheetTextureAttackStart);
                repeatAnimation = false;
            }        
            
            if (state == ATTACK_END && currentTexture != &spritesheetTextureAttackEnd)
            {
                SetAnimationTexture(&spritesheetTextureAttackEnd);
                repeatAnimation = false;
            }        

            if (state == ATTACK && currentTexture != &spritesheetTextureAttack)
            {
                SetAnimationTexture(&spritesheetTextureAttack);
                repeatAnimation = true;
            }

            if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
            {
                SetAnimationTexture(&spritesheetTextureRunning);
                repeatAnimation = true; 
            }

            if (state == ATTACK_START && animationPaused)
                state = ATTACK;
            else 
            if (state == ATTACK_END && animationPaused)
            {
                state = WAITING;
                waitTimeStart = GetTime();
            }

            if (state == RUNNING)    
            {
                // Move in direction
                b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);

                if (abs(velocity.x) < 20)
                    b2Body_SetLinearVelocity(physBodyId, {moveDirection.x * 25, moveDirection.y});

                // If close enough to center, throw bomb
                float distanceToCenter = abs((Engine::GetInternalResolution().x / 2) - (position.x + (size.x / 2)));

                if (distanceToCenter < 200)
                {
                    state = EnemyGunner::ATTACK_START;
                    attackStartTime = GetTime();
                }
            } 

            if (GetTime() - waitTimeStart > cooldownTime)
            {   
                if (curFrame == 0)
                {
                    attacked = false;
                    animationPaused = false;
                }
                
                if (GetTime() - lastShotTime > timeBetweenShots)
                {
                    if (state == ATTACK)
                    {                    
                        shot();
                        lastShotTime = GetTime();
                    }
                }

                if (GetTime() - attackStartTime > shootingTime && state == ATTACK)
                {
                    state = ATTACK_END;
                }
            }
            else
                state = WAITING;
        }
        else
        {
            if (GetTime() - waitTimeStart > cooldownTime)
            {
                state = ATTACK_START;
                attackStartTime = GetTime();
            }
        }
    }
}

void EnemyGunner::shot()
{
    Vector2 bulletStartPos = {lookDirection.x > 0 ? position.x + 20 : position.x - 20, lookDirection.x > 0 ? position.y + 9 : position.y + 9};
    new Bullet(bulletStartPos, lookDirection.x > 0);
}
