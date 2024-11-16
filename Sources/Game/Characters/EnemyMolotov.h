#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Enemy.h"

class EnemyMolotov : public Enemy
{
public:
    EnemyMolotov();
    void Update() override;
    enum EnemyMolotovState { RUNNING, ATTACK };
    void SetState(EnemyMolotovState state) { this->state = state; }

private:
    Texture2D spritesheetTextureAttack;
    Texture2D spritesheetTextureRunning;
    EnemyMolotovState state = RUNNING;
    bool attacked = false;
    double waitTimeAfterThrow = 5.0f;
    double waitTimeStart = -1000;
    int stopX;

    static float raycastResultCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);

};