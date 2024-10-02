#pragma once

#include <raylib.h>

class GameObject
{
public:
    GameObject(Vector2 position = {0});
    virtual ~GameObject();
    
    virtual void Draw();
    virtual void Update() {};
    void QueueDestroy() { destroyQueued = true; }
    bool IsDestroyQueued() { return destroyQueued; }

private:
    Vector2 position = {0};
    bool destroyQueued = false;

};