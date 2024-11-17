#include "TowerTop.h"
#include "Engine/Engine.h"
#include "Engine/GameObjects/GameObject.h"
#include "Engine/Vendor/Box2D/src/body.h"
#include "Game/Characters/Character.h"
#include "Game/Characters/Player.h"
#include "PhysicsCategory.h"
#include "box2d/box2d.h"
#include <iostream>

TowerTop::TowerTop() : GameObject()
{
    towerTopTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/PlatformTop.png");
    size = GetSize();
    initPhysicsBody();    
}

TowerTop::~TowerTop()
{
}

void TowerTop::Draw()
{
    DrawTextureEx(
        towerTopTexture,
        {position.x, position.y - 2},
        RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(physBodyId)),
        1, 
        WHITE
    );
}

void TowerTop::Update()
{
    b2Vec2 pos = b2Body_GetWorldPoint(physBodyId, { -extent.x, -extent.y });
    position = {pos.x, pos.y};
    processCollisions();
}

void TowerTop::SetPosition(Vector2 pos)
{
    position = pos;
    b2Body_SetTransform(physBodyId, {pos.x, pos.y}, b2Body_GetRotation(physBodyId));
}

void TowerTop::initPhysicsBody()
{
    extent = { 0.5f * size.x, 0.5f * size.y };
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {position.x + (size.x * 0.5f), position.y + (size.y * 0.5f)};
	bodyDef.type = b2_dynamicBody;

	physBodyId = b2CreateBody( Engine::GetPhysWorldID(), &bodyDef );    
    physShapes.push_back(attachPhysShapeToBody(physBodyId, {{0, 0}, {3, 14}, {0, 14}}));
    physShapes.push_back(attachPhysShapeToBody(physBodyId, {{79, 0}, {79, 14}, {76, 14}}));
    physShapes.push_back(attachPhysShapeToBody(physBodyId, {{3, 15}, {76, 15}, {69, 26}, {10, 26}}));
    physBodies.push_back(physBodyId);
}


b2ShapeId TowerTop::attachPhysShapeToBody(b2BodyId bodyId, std::vector<b2Vec2> vertices, float density, float friction)
{        
    const int vertSize = vertices.size();
    b2Vec2* verticesArr = new b2Vec2[vertSize];

    for (int i = 0; i < vertSize; i++)
    {
        vertices[i].x -= 0.5f * size.x;
        vertices[i].y -= 0.5f * size.y;
        verticesArr[i] = vertices[i];
    }

    b2Hull hull = b2ComputeHull(verticesArr, vertSize);
    b2Polygon shape = b2MakePolygon(&hull, 0.15f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    shapeDef.friction = friction;
    shapeDef.userData = this;

    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &shape );
    Engine::SetPhysFilterCategories(
        shapeId,
        TOWER_TOP,
        DEBRIS | PLAYER | ENEMY | ARROW | TOWER_BRICK | GROUND | BOMB | MOLOTOV_PARTICLE
    );

    return shapeId;
}

void TowerTop::processCollisions()
{
    const int bodyContactCapacity = b2Body_GetContactCapacity(physBodyId);
    b2ContactData *contactData = new b2ContactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(physBodyId, contactData, bodyContactCapacity);

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
                Player* player = dynamic_cast<Player*>(other);

                if (character)
                {                    
                    for (int k = 0; k < data->manifold.pointCount; k++)
                    {
                        b2ManifoldPoint point = data->manifold.points[k];
                        
                        if (point.normalImpulse > 500)
                        {
                            character->Die({point.anchorA.x, point.anchorA.y}, point.normalImpulse);

                            if (player)
                                Player::ReasonDead = "Tower top fall";
                        }
                     }
                }
            }
        }
    }
}