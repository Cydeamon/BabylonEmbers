#include "Brick.h"

Brick::Brick(Vector2 position, Vector2 size, float gap) : PhysicsRectangle(position, size, DYNAMIC)
{
    SetDensity(1);
    SetFriction(0.3);
    SetPadding(gap);
}