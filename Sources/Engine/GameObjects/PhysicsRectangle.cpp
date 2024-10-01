#include "PhysicsRectangle.h"
#include <stdio.h>

PhysicsRectangle::PhysicsRectangle(Vector2 position, Vector2 size, float density) : GameObject(position)
{
    this->position = position;
    this->size = size;
    this->density = density;
}

void PhysicsRectangle::Draw()
{
    DrawRectanglePro(
        { position.x, position.y, size.x, size.y },
        {0, 0}, 
        0,
        color
    );
}

void PhysicsRectangle::Update()
{
    
}
