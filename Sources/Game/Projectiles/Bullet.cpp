#include "Bullet.h"
#include "box2d/math_functions.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Enemy.h>
#include <Game/Characters/Player.h>
#include <Game/Game.h>

Bullet::Bullet(Vector2 initPosition, bool xPositive, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    Engine::SetPhysFilterCategories(
        shapeId, 
        ARROW, 
        0
    );

    b2Shape_SetDensity(shapeId, 0);
    b2Shape_SetUserData(shapeId, this);

    float bulletAngle;
    Vector2 aimPosition = {Engine::GetInternalResolution().x / 2, Game::FloorY - 100 };

    if (Player::BelowThreshold) 
        aimPosition = {Player::Position.x, Player::Position.y + 4};

    aimPosition.y += rand() % 30 - rand() % 30;

    bulletAngle = atan2(aimPosition.y - position.y, aimPosition.x - position.x);
    
    // bulletAngle = atan2(bulletAngle, 0.1);
    this->SetRotation(b2Rot{cos(bulletAngle), sin(bulletAngle)});
    this->SetVelocity({300 * cos(bulletAngle), 300 * sin(bulletAngle)});
    this->SetColor(BLACK);

    Engine::PlayAudio("GunShot");
}

void Bullet::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();
}

void Bullet::processCollisions()
{
    int bodyContactCapacity = b2Body_GetContactCapacity(bodyId);
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
            if (contacts[j])
            {
                Brick* brick = dynamic_cast<Brick*>(contacts[j]);
                Character* character = dynamic_cast<Character*>(contacts[j]);
                
                if (brick)
                {
                    Engine::PlayAudio("BulletHitEnv");
                    brick->Damage();
                }
                else
                if (character)
                {
                    b2Vec2 hitDirection = {(character->GetPosition().x + 8) - position.x, (character->GetPosition().y) - position.y + 8};
                    hitDirection = b2Normalize(hitDirection);
                    character->Die({-hitDirection.x, hitDirection.y}, 1000, true);
                    Engine::PlayAudio("BulletHitCharacter");
                }
                else
                {
                    Engine::PlayAudio("BulletHitEnv");
                }
                
                QueueDestroy();
            }
        }        
    }
}
