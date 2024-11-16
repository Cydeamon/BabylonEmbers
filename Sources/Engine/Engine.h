#pragma once

#include <raylib.h>
#include <Config.h>
#include "GameObjects/GameObject.h"
#include <box2d/box2d.h>
#include <vector>
#include <map>
#include <functional>
#include <string>

namespace Engine
{    
    enum SoundPlayMode { ONCE, LOOP, LOOP_UNIQUE };

    void Init(Vector2 internalResolution = {0, 0});
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
    void SetPaused(bool value);
    Texture LoadTextureFromTexturePool(std::string path);
    GameObject* GetObjectByPhysShapeId(b2ShapeId id);
    Sound GetAudio(std::string soundName);
    void PlayAudio(std::string soundName, SoundPlayMode playMode = ONCE, float volume = 1.0f, float pitch = 1.0f);
    void StopAudio(std::string soundName);
    void LoadAudioFile(std::string path);
    std::map<GameObject*, Vector2> GetObjectsInRadius(float radius, Vector2 center);
    std::vector<GameObject*> *GetGameObjects();

    // Physics
    b2WorldId GetPhysWorldID();
    
}