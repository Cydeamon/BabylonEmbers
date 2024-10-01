#pragma once

#include <raylib.h>

class GameObject
{
public:
    GameObject(Vector2 position = {0});
    virtual void Draw();
    virtual void Update() {};

private:
    Vector2 position = {0};

};