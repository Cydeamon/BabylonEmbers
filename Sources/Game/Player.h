#pragma once

#include <Engine/Engine.h>

class Player : public GameObject
{
public:
    Player();
    ~Player();
    void Update() override;
    void Draw() override;
    bool IsDead() { return dead; }

    Vector2 GetSize() { return {playerTexture.width, playerTexture.height}; }
    void SetPosition(Vector2 pos) override;

private:
    Texture2D playerTexture;
    Texture2D ragdollHeadTexture;
    Texture2D ragdollBodyTexture;
    Texture2D ragdollLegsTexture;
    Vector2 size;
    b2BodyId physBodyId; 
    b2ChainId physChainId;
    b2ShapeId physShapeId;
    b2Vec2 extent;
    float speed = 50;
    bool dead = false;

    // Ragdoll parts
    b2BodyId ragdollHeadId;
    b2Vec2 ragdollHeadExtent = {0};
    b2BodyId ragdollBodyId;
    b2Vec2 ragdollBodyExtent = {0};
    b2BodyId ragdollLegsId;
    b2Vec2 ragdollLegsExtent = {0};
    
    void initPhysicsBody();
    void die();
    void createRagdollBodies();
};