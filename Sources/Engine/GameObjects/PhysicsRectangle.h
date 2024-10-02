#pragma once

#include <Engine/Engine.h>

class PhysicsRectangle : GameObject
{
public: 
    enum BodyType { STATIC, DYNAMIC, KINEMATIC };

    PhysicsRectangle(Vector2 position, Vector2 size, BodyType type = BodyType::DYNAMIC);
    ~PhysicsRectangle();
    void Draw() override;
    void Update() override;

    void Enable()  { /* TODO: !!! */ }
    void Disable() { /* TODO: !!! */ }
    void SetColor(Color color) { this->color = color; }
    void SetOutlineColor(Color color) { this->outlineColor = color; }
    void SetOutlineThickness(int thickness) { this->outlineThickness = thickness; }
    void DrawOutline(bool value) { drawOutline = value; }   
    void SetDensity(float density) { shapeDef.density = density; }
    void SetFriction(float friction) { shapeDef.friction = friction; }
    void SetPadding(float padding);
    bool IsPointWithinBody(Vector2 point);

protected: 
    Vector2 position = {0};
    Vector2 size = {0};
    b2Rot rotation;
    Color color = BLACK;
    bool drawOutline = false;
    int outlineThickness = 1;
    Color outlineColor = GREEN;
    float padding = 0;

    BodyType bodyType = BodyType::DYNAMIC;
    b2BodyDef bodyDef;
    b2BodyId bodyId;
    b2Polygon bodyCube;
    b2ShapeDef shapeDef;
    b2ShapeId shapeId;
    b2Vec2 extent;
    
};