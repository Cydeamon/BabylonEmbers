#include "Molotov.h"
#include "Arrow.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Player.h>
#include <Game/Projectiles/FlameParticle.h>
#include <math.h>

Molotov::Molotov(Vector2 initPosition, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    countStartTime = GetTime();
    flameParticles = rand() % 25 + 25;

    float xDistanceToPlayer = abs(Player::Position.x - position.x);
    float throwAngleCompensation = xDistanceToPlayer > 50 ? 20 : 30;
    bool playerOnTheRight = Player::Position.x - position.x > 0;
    float angleToPlayer = atan2(Player::Position.y - position.y, Player::Position.x - position.x);
    destroyByScreen = false;

    if (!Player::IsGrounded)
        angleToPlayer -= (throwAngleCompensation * DEG2RAD) * (playerOnTheRight ? 1 : -1);

    float maxForce = (350 - (50 - (rand() % 100))) * 3;
    b2Vec2 force = {maxForce, 0};
    force = b2RotateVector({cos(angleToPlayer), sin(angleToPlayer)}, force);

    b2Shape_SetDensity(shapeId, 100);
    SetColor(BLACK);
    b2Shape_SetUserData(shapeId, this);

    Engine::SetPhysFilterCategories(
        shapeId,
        BOMB,
        TOWER_TOP | TOWER_BRICK | ENEMY | PLAYER | ARROW | GROUND
    );

    b2Body_ApplyLinearImpulseToCenter(bodyId, force, true);

    Engine::PlayAudio("MolotovThrow");
}

void Molotov::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();
}

void Molotov::processCollisions()
{
    const int bodyContactCapacity = b2Body_GetContactCapacity(bodyId);
    b2ContactData *contactData = new b2ContactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(bodyId, contactData, bodyContactCapacity);

    for (int i = 0; i < bodyContactCapacity && i < bodyContactCount; i++)
    {
        b2ContactData* data = contactData + i;
        GameObject* contacts[2];
        contacts[0] = Engine::GetObjectByPhysShapeId(data->shapeIdA);
        contacts[1] = Engine::GetObjectByPhysShapeId(data->shapeIdB);

        for (int j = 0; j < 2; j++)
        {
            if (contacts[j] && !blownUp)
            {
                GameObject* other = (GameObject*) contacts[j];
                Arrow* arrow = dynamic_cast<Arrow*>(other);
                
                if (other == this)
                    continue;

                if (!other->IsDestroyQueued())
                {
                    QueueDestroy();
                    Explode();
                }
            }
        }        
    }
}

void Molotov::Explode()
{
    float angleMin = 0;
    float angleMax = 180;
    float angleStep = (angleMax - angleMin) / flameParticles;

    for (flameParticles; flameParticles > 0; flameParticles--)
    {
        float angle = flameParticles * angleStep;
        new FlameParticle(position, angle);
    }

    QueueDestroy();
    Engine::PlayAudio("MolotovBreaking");
}
