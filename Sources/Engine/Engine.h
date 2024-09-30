#pragma once

#include <raylib.h>
#include <physac.h>
#include <Config.h>
#include "GameObjects/GameObject.h"

namespace Engine
{    
    void Init(Vector2 windowSize, Vector2 internalResolution = {0, 0});
    bool IsRunning();
    void Draw();
    void SetInternalResolution(Vector2 resolution);
    void RegisterGameObject(GameObject* obj);
}