#include "Arrow.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include <Game/Brick.h>
#include <Game/Characters/Enemy.h>

Arrow::Arrow(Vector2 initPosition, Vector2 size) : PhysicsRectangle(initPosition, size, DYNAMIC)
{
    Engine::SetPhysFilterCategories(
        shapeId, 
        GamePhysicsCategories::ARROW, 
        GamePhysicsCategories::ENEMY | GamePhysicsCategories::TOWER_BRICK
    );

    b2Shape_SetDensity(shapeId, 0);
    b2Shape_SetUserData(shapeId, this);

    float arrowAngle = atan2(Engine::GetMousePositionScaled().y - initPosition.y, Engine::GetMousePositionScaled().x - initPosition.x);
    this->SetRotation(b2Rot{cos(arrowAngle), sin(arrowAngle)});
    this->SetVelocity({350 * cos(arrowAngle), 350 * sin(arrowAngle)});
}

void Arrow::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();
}

void Arrow::processCollisions()
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
                if (!characterHit)
                {
                    GameObject* other = (GameObject*) contacts[j];
                    Brick* brick = dynamic_cast<Brick*>(other);
                    Enemy* enemy = dynamic_cast<Enemy*>(other);

                    if (enemy)
                    {
                        b2Vec2 hitDirection = {(enemy->GetPosition().x + 8) - position.x, (enemy->GetPosition().y) - position.y + 8};
                        hitDirection = b2Normalize(hitDirection);
                        enemy->Die({hitDirection.x, hitDirection.y}, 1000);
                        characterHit = true;
                    }

                    QueueDestroy();
                }
            }
        }        
    }
}
