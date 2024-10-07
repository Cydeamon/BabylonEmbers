#include "TowerTop.h"

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
    Engine::AttachPhysShapeToBody(physBodyId, {{0, 0}, {3, 14}, {0, 14}});
    Engine::AttachPhysShapeToBody(physBodyId, {{79, 0}, {79, 14}, {76, 14}});
    Engine::AttachPhysShapeToBody(physBodyId, {{3, 15}, {76, 15}, {69, 26}, {10, 26}});
}