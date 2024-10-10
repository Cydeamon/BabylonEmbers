#pragma once

#include <Engine/Engine.h>
#include "Game/Brick.h"
#include "Character.h"

class Enemy : public Character
{
public:
    Enemy();

protected:
    Vector2 moveDirection;

};