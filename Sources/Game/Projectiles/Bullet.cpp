#include "Bullet.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Enemy.h>
#include <Game/Characters/Player.h>

Bullet::Bullet(Vector2 initPosition, bool xPositive, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    Engine::SetPhysFilterCategories(
        shapeId, 
        ARROW, 
        0
    );

    b2Shape_SetDensity(shapeId, 0);
    b2Shape_SetUserData(shapeId, this);

    float random = (rand() % 15) / 10.0f;
    float bulletAngle = (random + -3.0f) * DEG2RAD * (xPositive ? 1 : -1);

    if (Player::BelowThreshold)
        bulletAngle = atan2(-(random + -3.0f) + Player::Position.y + 4 - position.y, Player::Position.x - position.x);

    // bulletAngle = atan2(bulletAngle, 0.1);
    this->SetRotation(b2Rot{cos(bulletAngle), sin(bulletAngle)});
    this->SetVelocity({300 * cos(bulletAngle), 300 * sin(bulletAngle)});
    this->SetColor(RED);
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
                Character* character = dynamic_cast<Character*>(other);
                
                if (brick)
                {
                    brick->Damage();
                    QueueDestroy();
                }

                if (character)
                {
                    b2Vec2 hitDirection = {position.x - character->GetPosition().x, position.y - (character->GetPosition().y + 4)};
                    hitDirection = b2Normalize(hitDirection);
                    character->Die({hitDirection.x, hitDirection.y}, 2500, true);
                    QueueDestroy();
                }
            }
        }        
    }
}
