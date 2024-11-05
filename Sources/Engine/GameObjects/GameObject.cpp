#include "GameObject.h"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include <Engine/Engine.h>
#include <iostream>

GameObject::GameObject(Vector2 position)
{
    this->position = position;
    Engine::RegisterGameObject(this);
}

GameObject::~GameObject()
{
    Engine::UnregisterGameObject(this);

    while (physShapes.size() > 0)
        DestroyPhysShape(physShapes[0]);

    while (physBodies.size() > 0)
        DestroyPhysBody(physBodies[0]);
}

void GameObject::Draw()
{
    DrawCircle(position.x + 10, position.y + 10, 10, {255, 0, 255, 255});
    DrawText("!!!", position.x + 8, position.y + 6, 8, WHITE);
    
    char message[32] = "GameObject type is not set";
    DrawText(message, position.x + 5 - (MeasureText(message, 8) / 2), position.y + 20, 8, RED);
}

void GameObject::SetDrawPriority(DrawPriority priority)
{
    drawPriority = priority; 
    Engine::ResortObjects();
}

void GameObject::DestroyPhysBody(b2BodyId physBodyId)
{    
    for (int i = 0; i < physBodies.size(); i++)
    {
        b2BodyId id = physBodies[i];

        if (id.index1 == physBodyId.index1 && id.revision == physBodyId.revision && id.world0 == physBodyId.world0)
        {
            physBodies.erase(physBodies.begin() + i);
            break;
        }
    }

    if (b2Body_IsValid(physBodyId))
    {
        b2DestroyBody(physBodyId);
    }
}

void GameObject::DestroyPhysShape(b2ShapeId physShapeId)
{    
    for (int i = 0; i < physShapes.size(); i++)
    {
        b2ShapeId id = physShapes[i];

        if (id.index1 == physShapeId.index1 && id.revision == physShapeId.revision && id.world0 == physShapeId.world0)
        {
            physShapes.erase(physShapes.begin() + i);
            break;
        }
    }

    if (b2Shape_IsValid(physShapeId))
    {
        b2DestroyShape(physShapeId);
    }
}