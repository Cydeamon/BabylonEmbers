#include "FlameParticle.h"
#include <math.h>
#include <Game/PhysicsCategory.h>
#include <Game/Characters/Player.h>
#include <Game/Characters/Enemy.h>

FlameParticle::FlameParticle(Vector2 pos, float angle) : PhysicsRectangle(pos, {1, 1})
{    
    int maxForce = 10 + (rand() % 40);
    b2Vec2 force = {-maxForce, 0};
    force = b2RotateVector({cos(angle * DEG2RAD), sin(angle * DEG2RAD)}, force);

    b2Shape_SetDensity(shapeId, 0.00001);
    SetColor(BLACK);
    b2Shape_SetUserData(shapeId, this);

    Engine::SetPhysFilterCategories(
        shapeId,
        MOLOTOV_PARTICLE,
        TOWER_TOP | TOWER_BRICK | ENEMY | PLAYER | GROUND | MOLOTOV_PARTICLE
    );

    b2Body_ApplyLinearImpulseToCenter(bodyId, force, true);
    nextSmokeTime = GetTime() + (((rand() % (int) smokeIntervalMax * 100) / 100.0f) + smokeIntervalMin);

    SetLifeTime(rand() % 5 + rand() % 5);
}

void FlameParticle::Update()
{    
    PhysicsRectangle::Update();
    processCollisions();

    if (GetTime() >= nextSmokeTime)
    {
        PhysicsRectangle *smoke = new PhysicsRectangle(
            {position.x, position.y - 1}, 
            {1, 1}, 
            PhysicsRectangle::BodyType::DYNAMIC
        );

        b2Shape_SetDensity(smoke->GetShapeId(), 0.000001);
        smoke->SetColor(BLACK);
        Engine::SetPhysFilterCategories(smoke->GetShapeId(), 0, 0);
        b2Body_SetGravityScale(smoke->GetBodyId(), -1);
        smoke->SetLifeTime((rand() % 100) / 100.0f);
        
        nextSmokeTime = GetTime() + (((rand() % (int) smokeIntervalMax * 100) / 100.0f) + smokeIntervalMin);
    }
}

void FlameParticle::processCollisions()
{
    int bodyContactCapacity = b2Body_GetContactCapacity(bodyId);
    b2ContactData contactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(bodyId, contactData, bodyContactCapacity);

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
                Enemy* enemy = dynamic_cast<Enemy*>(other);
                Player* player = dynamic_cast<Player*>(other);
                
                if (player)
                    player->Die();
                else 
                if (enemy)
                    enemy->Die();
            }
        }        
    }
}
