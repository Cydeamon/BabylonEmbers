#include "TowerTop.h"
#include "PhysicsCategory.h"

TowerTop::TowerTop()
{
    towerTopTexture = LoadTexture("Assets/TowerTop.png");
    initPhysicsBody();    
}

TowerTop::~TowerTop()
{
    b2DestroyChain(physChainId);
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
    attachPhysShapeToBody(physBodyId, {{0, 0}, {3, 14}, {0, 14}});
    attachPhysShapeToBody(physBodyId, {{79, 0}, {79, 14}, {76, 14}});
    attachPhysShapeToBody(physBodyId, {{3, 15}, {76, 15}, {69, 26}, {10, 26}});
}


b2ShapeId TowerTop::attachPhysShapeToBody(b2BodyId bodyId, std::vector<b2Vec2> vertices, float density, float friction)
{        
    b2Vec2 verticesArr[vertices.size()];

    for (int i = 0; i < vertices.size(); i++)
        verticesArr[i] = vertices[i];

    b2Hull hull = b2ComputeHull( verticesArr, vertices.size() );
    b2Polygon shape = b2MakePolygon( &hull, 0.15f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    shapeDef.friction = friction;
    shapeDef.userData = this;

    b2ShapeId shapeId = b2CreatePolygonShape( bodyId, &shapeDef, &shape );
    Engine::SetPhysFilterCategories(
        shapeId,
        TOWER_TOP,
        DEBRIS | PLAYER | ENEMY | ARROW | TOWER_BRICK | GROUND | BOMB
    );

    return shapeId;
}