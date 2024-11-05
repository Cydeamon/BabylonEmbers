#include "Brick.h"
#include <iostream>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>

Brick::Brick(Vector2 position, Vector2 size, float gap) : PhysicsRectangle(position, size, DYNAMIC)
{
    b2Shape_SetDensity(shapeId, 2000);
    b2Shape_SetFriction(shapeId, 0);
    SetPadding(gap);
    Engine::SetPhysFilterCategories(
        shapeId,
        TOWER_BRICK,
        DEBRIS | TOWER_TOP | PLAYER | ENEMY | ARROW | GROUND | TOWER_BRICK | BODY | BOMB | MOLOTOV_PARTICLE
    );

    physShapes.push_back(shapeId);
    physBodies.push_back(bodyId);
}

void Brick::Update() 
{
    PhysicsRectangle::Update();
    
    if (IsMouseButtonPressed(0))
    {
        if (Engine::IsDebug())
        {
            if (IsPointWithinBody(Engine::GetMousePositionScaled()))
                Destroy();
        }
    }
}

void Brick::Destroy(Vector2 reactionDirection)
{
    int pieceSize = (rand() % 2) + 1;
    int piecesX;
    int piecesY = (size.y / pieceSize) / ((rand() % 3) + 1);
    
    for (int y = 0; y < size.y; y += (size.y / piecesY))
    {
        pieceSize = (rand() % 2) + 1;
        piecesX = (size.x / pieceSize) / ((rand() % 3) + 1);

        for (int x = 0; x < size.x; x += (size.x / piecesX))
        {
            PhysicsRectangle *piece = new PhysicsRectangle(
                {position.x + x, position.y + y}, 
                {(float) pieceSize, (float) pieceSize}, 
                PhysicsRectangle::BodyType::DYNAMIC
            );

            int maxForce = 200 * pieceSize;
            float forceX = (rand() % maxForce * 2) - maxForce;
            float forceY = (rand() % maxForce * 2) - maxForce;

            if (reactionDirection.x != 0 || reactionDirection.y != 0)
            {
                forceX = reactionDirection.x * maxForce;
                forceY = reactionDirection.y * maxForce;
            }

            b2Shape_SetDensity(piece->GetShapeId(), 100);
            piece->SetColor(BLACK);
            b2Shape_SetUserData(piece->GetShapeId(), piece);

            Engine::SetPhysFilterCategories(
                piece->GetShapeId(),
                DEBRIS,
                GROUND | TOWER_BRICK | TOWER_TOP | 
                BODY | BOMB
            );
            b2Body_ApplyLinearImpulse(piece->GetBodyId(), {forceX, forceY}, b2Body_GetWorldCenterOfMass(piece->GetBodyId()), true);
        }
    }            

    QueueDestroy();
}

void Brick::Damage(Vector2 reactionDirection)
{
    health--;

    if (health <= 0)
        Destroy(reactionDirection);

    if (reactionDirection.x != 0 || reactionDirection.y != 0)
    {        
        b2Body_ApplyLinearImpulseToCenter(GetBodyId(), {reactionDirection.x * 1000, reactionDirection.y * 1000}, true);
    }
}
