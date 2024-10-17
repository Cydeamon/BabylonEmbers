#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Enemy.h"

class EnemyGunner : public Enemy
{
public:
    EnemyGunner();
    
    enum EnemyGunnerState { RUNNING, ATTACK_START, ATTACK_END, ATTACK, WAITING };

    void Update() override;
    void SetState(EnemyGunnerState state) { this->state = state; }

private:
    Texture2D spritesheetTextureAttack;
    Texture2D spritesheetTextureAttackStart;
    Texture2D spritesheetTextureAttackEnd;
    Texture2D spritesheetTextureRunning;
    EnemyGunnerState state = RUNNING;
    bool attacked = false;
    double cooldownTime = 5.0f;
    double waitTimeStart = -1000;
    double timeBetweenShots = 0.25;
    double lastShotTime = 0;
    double shootingTime = 2.5;
    double attackStartTime = 0;

    static float raycastResultCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);
    void shot();
};