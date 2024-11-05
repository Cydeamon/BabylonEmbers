#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Character.h"

class Enemy : public Character
{
public:
    Enemy();
    void Die(Vector2 reactionDirection = {0}, float reactionForce = 25, bool dismember = false) override;
protected:
    Vector2 moveDirection;

};