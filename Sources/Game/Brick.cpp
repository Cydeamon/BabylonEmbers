#include "Brick.h"
#include <iostream>
#include <Engine/Engine.h>
#include <Game/PhysicsCategory.h>

Brick::Brick(Vector2 position, Vector2 size, float gap) : PhysicsRectangle(position, size, DYNAMIC)
{
    SetDensity(2);
    SetFriction(0);
    SetPadding(gap);
    Engine::SetPhysFilterCategories(
        shapeId,
        GamePhysicsCategories::TOWER_BRICK,
        GamePhysicsCategories::DEBRIS | GamePhysicsCategories::TOWER_TOP | GamePhysicsCategories::PLAYER | GamePhysicsCategories::ENEMY | GamePhysicsCategories::ARROW |
        GamePhysicsCategories::GROUND | GamePhysicsCategories::TOWER_BRICK 
        
    );
}

Brick::~Brick()
{
}

void Brick::Update() 
{
    PhysicsRectangle::Update();

    if (IsMouseButtonPressed(0))
    {
        if (Engine::IsDebug())
        {
            if (IsPointWithinBody(Engine::GetMousePositionScaled()))
                destroy();
        }
    }
}

void Brick::destroy()
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

            piece->SetDensity(100);
            piece->SetColor(BLACK);

            Engine::SetPhysFilterCategories(
                piece->GetShapeId(),
                GamePhysicsCategories::DEBRIS,
                GamePhysicsCategories::DEBRIS | GamePhysicsCategories::TOWER_TOP | GamePhysicsCategories::TOWER_BRICK | GamePhysicsCategories::GROUND
            );
            b2Body_ApplyLinearImpulse(piece->GetBodyId(), {forceX, forceY}, b2Body_GetWorldCenterOfMass(piece->GetBodyId()), true);
        }
    }            

    QueueDestroy();
}
