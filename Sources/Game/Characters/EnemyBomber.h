#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Enemy.h"

class EnemyBomber : public Enemy
{
public:
    EnemyBomber();
    void Update() override;
    enum EnemyBomberState { RUNNING, ATTACK };
    void SetState(EnemyBomberState state) { this->state = state; }

private:
    Texture2D spritesheetTextureAttack;
    Texture2D spritesheetTextureRunning;
    EnemyBomberState state = RUNNING;
    bool attacked = false;
    double waitTimeAfterThrow = 5.0f;
    double waitTimeStart = 0;
    int stopX;

    static float raycastResultCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);

};