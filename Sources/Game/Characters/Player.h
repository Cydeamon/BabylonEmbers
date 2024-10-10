#ifndef PLAYER_H
#define PLAYER_H

#include <Engine/Engine.h>
#include "Character.h"
#include <Engine/GameObjects/PhysicsRectangle.h>

class Player : public Character
{
public:
    Player();
    void Update() override;
    void Draw() override;

    enum PlayerState { IDLE, RUNNING };

private:
    Texture2D spritesheetTextureIdle;
    Texture2D spritesheetTextureRunning;
    Texture2D crossbowTexture;
    PlayerState state = IDLE;
    float weaponAngle = 0;
    Vector2 weaponPosition = {9, 5};
    float weaponCooldownTime = 0.5f;
    float lastShotTime = 0;

    void shot();
};

#endif