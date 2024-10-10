#include "Arrow.h"
#include <iostream>
#include <string>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>
#include "Brick.h"

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
    this->SetVelocity({500 * cos(arrowAngle), 500 * sin(arrowAngle)});
}

void Arrow::Update() 
{
    PhysicsRectangle::Update();
    processCollisions();
}

void Arrow::processCollisions()
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
                Brick* otherBrick = dynamic_cast<Brick*>(other);
                
                if (otherBrick)
                    QueueDestroy();
            }
        }        
    }
}
