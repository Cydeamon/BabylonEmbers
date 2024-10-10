#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Enemy.h"

class EnemySmasher : public Enemy
{
public:
    EnemySmasher();
    void Update() override;

    enum EnemySmasherState { RUNNING, ATTACK };

private:
    Texture2D spritesheetTextureAttack;
    Texture2D spritesheetTextureRunning;
    EnemySmasherState state = RUNNING;
    Brick* targetBrick = nullptr;
    bool wasAttacked = false;
    double waitTimeAfterDestroy = 1;
    double waitTimeStart = -1000;

    void processCollisions();

};