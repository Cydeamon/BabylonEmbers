#include "PhysicsRectangle.h"
#include <stdio.h>
#include <stdexcept>

PhysicsRectangle::PhysicsRectangle(Vector2 position, Vector2 size, BodyType type) : GameObject(position)
{
    this->position = position;
    this->size = size;

    bodyDef = b2DefaultBodyDef();
    
    switch (type)
    {
        case BodyType::DYNAMIC:   bodyDef.type = b2_dynamicBody;    break;
        case BodyType::KINEMATIC: bodyDef.type = b2_kinematicBody;  break;
        case BodyType::STATIC:    bodyDef.type = b2_staticBody;     break;
        default: throw std::runtime_error("PhysicsRectangle::PhysicsRectangle() - Unknown body type");
    }

    extent = { 0.5f * size.x, 0.5f * size.y };
    bodyDef.position = {position.x + (size.x * 0.5f), position.y + (size.y * 0.5f)};
    bodyId = b2CreateBody(Engine::GetPhysWorldID(), &bodyDef);
    bodyCube = b2MakeBox(extent.x, extent.y);
    shapeDef = b2DefaultShapeDef();
    shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &bodyCube);
    b2Shape_EnableHitEvents(shapeId, true);
}

PhysicsRectangle::~PhysicsRectangle()
{
    b2DestroyBody(bodyId);
}

void PhysicsRectangle::Draw()
{
    DrawRectanglePro(
        {position.x,position.y, size.x, size.y },
        {0}, 
        RAD2DEG * b2Rot_GetAngle(rotation),
        color
    );
}

void PhysicsRectangle::Update()
{    
    b2Vec2 pos = b2Body_GetWorldPoint(bodyId, { -extent.x, -extent.y });
    float radians = b2Rot_GetAngle(rotation);
    rotation = b2Body_GetRotation(bodyId);
    position = {pos.x, pos.y};
}

void PhysicsRectangle::SetPadding(float padding)
{
    this->padding = padding;
    b2DestroyBody(bodyId);

    extent = { 0.5f * (size.x + padding), 0.5f * (size.y + padding) };
    bodyDef.position = {position.x + extent.x, position.y + extent.y};
    bodyId = b2CreateBody(Engine::GetPhysWorldID(), &bodyDef);
    bodyCube = b2MakeBox(extent.x, extent.y);
    shapeDef = b2DefaultShapeDef();
    shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &bodyCube);
    b2Shape_EnableHitEvents(shapeId, true);
}

bool PhysicsRectangle::IsPointWithinBody(Vector2 point)
{
    return b2Shape_TestPoint(shapeId, {point.x, point.y});
}
