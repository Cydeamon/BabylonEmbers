#include "EnemyBomber.h"
#include <iostream>
#include <Game/PhysicsCategory.h>
#include <Game/Game.h>
#include <Game/Projectiles/Bomb.h>

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

        if (state == RUNNING && currentTexture != &spritesheetTextureRunning)
            SetAnimationTexture(&spritesheetTextureRunning);

        if (state == RUNNING)    
        {
            // Move in direction
            b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);

            if (abs(velocity.x) < 40)
                b2Body_SetLinearVelocity(physBodyId, {moveDirection.x * 50, moveDirection.y});

            // If close enough to center, throw bomb
            float distanceToCenter = abs((Engine::GetInternalResolution().x / 2) - (position.x + (size.x / 2)));

            if (distanceToCenter < 150)
                state = EnemyBomber::ATTACK;
        }

        if (GetTime() - waitTimeStart > waitTimeAfterThrow)
        {   
            if (curFrame == 0)
            {
                attacked = false;
                animationPaused = false;
            }

            if (state == ATTACK && !attacked && curFrame == 8)
            {
                new Bomb(this, {lookDirection.x, lookDirection.y}, position, {2, 2});
                waitTimeStart = GetTime();
                attacked = true;
            }
        }
    }
}
