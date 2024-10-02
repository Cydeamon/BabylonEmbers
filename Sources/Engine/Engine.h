#pragma once

#include <raylib.h>
#include <Config.h>
#include "GameObjects/GameObject.h"
#include <box2d/box2d.h>

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

    // Physics
    b2WorldId GetPhysWorldID();
}