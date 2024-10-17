#include "Bomb.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Player.h>

Bomb::Bomb(Enemy* thrower, Vector2 lookDirection, Vector2 initPosition, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    this->thrower = thrower;
    countStartTime = GetTime();

    int maxForce = 100 + (rand() % 150);
    b2Vec2 force = {maxForce, maxForce};
    // b2Vec2 force = {rand() % maxForce + 500, rand() % maxForce + 500};
    force = b2RotateVector({
        lookDirection.x < 0 ? (DEG2RAD * -90) : 0, 
        lookDirection.x > 0 ? (DEG2RAD * -90) : 0
    }, force);

    b2Shape_SetDensity(shapeId, 100);
    SetColor(BLACK);
    b2Shape_SetUserData(shapeId, this);

    Engine::SetPhysFilterCategories(
        shapeId,
        BOMB,
        TOWER_TOP | TOWER_BRICK | ENEMY | PLAYER | ARROW | GROUND
    );

    b2Body_ApplyLinearImpulseToCenter(bodyId, force, true);
}

void Bomb::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();

    if (GetTime() - countStartTime > blowTime)
    {
        QueueDestroy();
        explode();
    }    
}

void Bomb::processCollisions()
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
            if (contacts[j] && !blownUp)
            {
                QueueDestroy();
                explode();
            }
        }        
    }
}

void Bomb::explode()
{
    // Get objects in radius of explosion
    std::map<GameObject*, Vector2> objects = Engine::GetObjectsInRadius(explosionRadius, position);
    blownUp = true;

    for (auto& obj : objects)
    {
        Vector2 relativeToCenter = obj.second;
        Brick *brick = dynamic_cast<Brick*>(obj.first);
        Player *player = dynamic_cast<Player*>(obj.first);
        Enemy *enemy = dynamic_cast<Enemy*>(obj.first);

        if (brick)
        {
            int amount = rand() % 4;

            for (int i = 0; i < amount && brick->GetHealth(); i++)
                brick->Damage(relativeToCenter);
        }

        if (player)
            player->Die(relativeToCenter);

        if (enemy)
            enemy->Die(relativeToCenter);
    }
}