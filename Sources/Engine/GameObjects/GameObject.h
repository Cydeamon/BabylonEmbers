#pragma once

#include <raylib.h>
\
class GameObject
{
public:
    GameObject(Vector2 position = {0});
    virtual ~GameObject();

    enum DrawPriority { LOW, NORMAL, HIGH };
    
    virtual void Draw();
    virtual void Update() {};
    void QueueDestroy() { destroyQueued = true; }
    bool IsDestroyQueued() { return destroyQueued; }

    virtual void SetPosition(Vector2 pos) { position = pos; }
    Vector2 GetPosition() { return position; }

    void SetDrawPriority(DrawPriority priority);
    DrawPriority GetDrawPriority() { return drawPriority; }

protected:
    Vector2 position = {0};
    bool destroyQueued = false;
    bool isInit = 1;
    DrawPriority drawPriority = NORMAL;

};