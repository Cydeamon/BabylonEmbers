#include "Bullet.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Enemy.h>

Bullet::Bullet(Vector2 initPosition, bool xPositive, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    Engine::SetPhysFilterCategories(
        shapeId, 
        GamePhysicsCategories::ARROW, 
        0 // GamePhysicsCategories::ENEMY | GamePhysicsCategories::TOWER_BRICK
    );

    b2Shape_SetDensity(shapeId, 0);
    b2Shape_SetUserData(shapeId, this);


    float bulletAngle = (((rand() % 15) / 10.0f) + -3.0f) * DEG2RAD;
    bulletAngle = atan2(bulletAngle, 0.1);
    this->SetRotation(b2Rot{cos(bulletAngle), sin(bulletAngle)});
    this->SetVelocity({(400 * cos(bulletAngle)) * (xPositive ? 1 : -1), 400 * sin(bulletAngle)});
}

void Bullet::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();
}

void Bullet::processCollisions()
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
                Brick* brick = dynamic_cast<Brick*>(other);
                Enemy* enemy = dynamic_cast<Enemy*>(other);
                
                if (brick)
                {
                    brick->Damage();
                    QueueDestroy();
                }

                if (enemy)
                {
                    b2Vec2 hitDirection = {position.x - enemy->GetPosition().x, position.y - enemy->GetPosition().y};
                    hitDirection = b2Normalize(hitDirection);
                    enemy->Die({hitDirection.x, hitDirection.y});
                    QueueDestroy();
                }
            }
        }        
    }
}
