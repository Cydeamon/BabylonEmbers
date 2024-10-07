#pragma once

#include <Engine/Engine.h>
#include "Character.h"

class Player : public Character
{
public:
    Player();
    void Update() override;

    enum PlayerState { IDLE, RUNNING };

private:
    Texture2D spritesheetTextureIdle;
    Texture2D spritesheetTextureRunning;
    PlayerState state = IDLE;

};