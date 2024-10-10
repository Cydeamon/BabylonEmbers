#pragma once

#include <Engine/Engine.h>

class Character : public GameObject
{
public:
    Character();
    void Update() override;
    void Draw() override;
    bool IsDead() { return dead; }

    Vector2 GetSize() { return size; }
    void SetPosition(Vector2 pos) override;
    void SetAnimationTexture(Texture2D *texture);
    void Die();

protected:
    Texture2D ragdollHeadTexture;
    Texture2D ragdollBodyTexture;
    Texture2D ragdollLegsTexture;
    Vector2 size = {16, 16};
    b2BodyId physBodyId; 
    b2ChainId physChainId;
    b2ShapeId physShapeId;
    b2Vec2 extent;
    float speed = 50;
    bool dead = false;
    double frameDuration = 1.0f / 24.0f;
    double lastFrameTime;
    int curFrame = 0;
    int totalFrames = 0;
    bool repeatAnimation = true;
    bool animationPaused = false;
    Texture2D *currentTexture = nullptr;
    int curTextureRows;
    int curTextureCols;
    b2Vec2 moveDirection = {0};
    b2Vec2 lookDirection = {1, 0};

    // Ragdoll parts
    b2BodyId ragdollHeadId;
    b2ShapeId ragdollHeadShapeId;
    b2Vec2 ragdollHeadExtent = {0};
    b2BodyId ragdollBodyId;
    b2ShapeId ragdollBodyShapeId;
    b2Vec2 ragdollBodyExtent = {0};
    b2BodyId ragdollLegsId;
    b2ShapeId ragdollLegsShapeId;
    b2Vec2 ragdollLegsExtent = {0};
    uint64_t filterCategories = 0;
    uint64_t filterMask = 0;
    
    void initPhysicsBody();
    void createRagdollBodies();
};