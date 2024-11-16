#include "Brick.h"
#include "Game/Characters/Character.h"
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

    processCollisions();
}

void Brick::Destroy(Vector2 reactionDirection)
{
    int pieceSize = (rand() % 2) + 1;
    int piecesX;
    int piecesY = (size.y / pieceSize) / ((rand() % 3) + 1);
    
    if (piecesY)
    {
        for (int y = 0; y < size.y; y += (size.y / piecesY))
        {
            pieceSize = (rand() % 2) + 1;
            piecesX = (size.x / pieceSize) / ((rand() % 3) + 1);

            if (piecesX)
            {
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
                    piece->SetSoundOnCollision("BrickDebrisFall");
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
        }            
    }

    QueueDestroy();
    Engine::PlayAudio("BrickBreak");
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

void Brick::processCollisions()
{
    const int bodyContactCapacity = b2Body_GetContactCapacity(bodyId);
    b2ContactData *contactData = new b2ContactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(bodyId, contactData, bodyContactCapacity);

    for (int i = 0; i < bodyContactCapacity && i < bodyContactCount; i++)
    {        
        b2ContactData* data = contactData + i;

        if (!b2Shape_IsValid(data->shapeIdA) || !b2Shape_IsValid(data->shapeIdB))
            continue;

        GameObject* contacts[2];
        contacts[0] = Engine::GetObjectByPhysShapeId(data->shapeIdA);
        contacts[1] = Engine::GetObjectByPhysShapeId(data->shapeIdB);

        for (int j = 0; j < 2; j++)
        {
            if (contacts[j])
            {
                GameObject* other = (GameObject*) contacts[j];
                Character* character = dynamic_cast<Character*>(other);

                if (character)
                {                    
                    for (int k = 0; k < data->manifold.pointCount; k++)
                    {
                        b2ManifoldPoint point = data->manifold.points[k];
                        
                        if (point.normalImpulse > 50)
                            character->Die({point.anchorA.x, point.anchorA.y}, point.normalImpulse);
                    }
                }
            }
        }
    }
}