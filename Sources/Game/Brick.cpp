#include "Brick.h"
#include <iostream>
#include <Engine/Engine.h>

Brick::Brick(Vector2 position, Vector2 size, float gap) : PhysicsRectangle(position, size, DYNAMIC)
{
    SetDensity(1);
    SetFriction(0.3);
    SetPadding(gap);
}

void Brick::Update() 
{
    PhysicsRectangle::Update();

    if (IsMouseButtonPressed(0))
    {
        if (Engine::IsDebug())
        {
            if (IsPointWithinBody(Engine::GetMousePositionScaled()))
                delete this;
        }
    }
}
