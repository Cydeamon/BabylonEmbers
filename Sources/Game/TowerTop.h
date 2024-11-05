#pragma once

#include <Engine/Engine.h>
#include <Engine/GameObjects/PhysicsRectangle.h>
#include <vector>

class TowerTop : public GameObject
{
public:
    TowerTop();
    ~TowerTop();

    void Draw() override;
    void Update() override;
    Vector2 GetSize() { return {(float) towerTopTexture.width, (float) towerTopTexture.height}; }
    void SetPosition(Vector2 pos) override;

private:
    Texture2D towerTopTexture;
    Vector2 size;
    b2BodyId physBodyId; 
    b2ShapeId physShapeId;
    b2Vec2 extent;
    
    void initPhysicsBody();    
    b2ShapeId attachPhysShapeToBody(b2BodyId bodyId, std::vector<b2Vec2> vertices, float density = 1.5f, float friction = 5.0f);
};