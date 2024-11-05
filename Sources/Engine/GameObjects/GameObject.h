#pragma once

#include "box2d/id.h"
#include <raylib.h>
#include <vector>
#include <box2d/box2d.h>

class GameObject
{
public:
    GameObject(Vector2 position = {0});
    virtual ~GameObject();

    enum DrawPriority { LOW, NORMAL, HIGH };
    
    virtual void Draw();
    virtual void Update() = 0;
    void QueueDestroy() { destroyQueued = true; }
    bool IsDestroyQueued() { return destroyQueued; }
    void DestroyPhysBody(b2BodyId physBodyId);
    void DestroyPhysShape(b2ShapeId physShapeId);

    Vector2 GetPosition() { return position; }
    DrawPriority GetDrawPriority() { return drawPriority; }
    std::vector<b2ShapeId>* GetPhysShapes() { return &physShapes; }
    std::vector<b2BodyId>* GetPhysBodies() { return &physBodies; }

    virtual void SetPosition(Vector2 pos) { position = pos; }
    void SetDrawPriority(DrawPriority priority);


protected:
    Vector2 position = {0};
    bool destroyQueued = false;
    bool isInit = 1;
    DrawPriority drawPriority = NORMAL;
    std::vector<b2ShapeId> physShapes;
    std::vector<b2BodyId> physBodies;

};