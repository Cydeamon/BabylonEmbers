#pragma once

#include <raylib.h>
#include <Config.h>
#include "GameObjects/GameObject.h"
#include <box2d/box2d.h>
#include <vector>
#include <functional>

namespace Engine
{    
    void Init(Vector2 windowSize, Vector2 internalResolution = {0, 0});
    void Deinit();
    bool IsRunning();
    bool IsDebug();
    void Draw();
    void SetInternalResolution(Vector2 resolution);
    void RegisterGameObject(GameObject* obj);
    void UnregisterGameObject(GameObject* obj);
    Vector2 GetInternalResolution();
    Vector2 GetMousePositionScaled();
    void Update();
    void ResortObjects();
    void SetDrawHUDCallback(std::function<void(void)> callback);
    void SetPhysFilterCategories(b2ShapeId shapeId, uint64_t filterCategories, uint64_t filterMask = 0);

    // Physics
    b2WorldId GetPhysWorldID();
}