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

    virtual void SetPosition(Vector2 pos) { position = pos; }
    Vector2 GetPosition() { return position; }

protected:
    Vector2 position = {0};
    bool destroyQueued = false;

};