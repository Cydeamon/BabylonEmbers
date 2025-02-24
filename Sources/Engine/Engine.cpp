#include "Engine.h"
#include "GameObjects/GameObject.h"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include "raylib.h"
// #include "raudio.c"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

namespace Engine
{    
    /*******************************************************************************************/
    /***************************************  Variables  ***************************************/
    /*******************************************************************************************/

    // Window and rendering
    bool isPaused = false;
    bool isInternalResolutionSet = false;
    Vector2 internalResolution = {0, 0};
    Vector2 mousePositionScaled = {0};
    float renderRatio;
    Camera2D worldSpaceCamera = {0};
    Camera2D screenSpaceCamera = {0};
    RenderTexture2D target = {0};
    Rectangle sourceRec = {0};
    Rectangle destRec = {0};
    bool fullscreen = true;
    std::map<std::string, Texture> textures;
    std::map<std::string, Sound> sounds;
    std::map<std::string, int> uniqueLoopSoundsCounter;
    std::vector<std::string> soundsOnLoop;

    // Game objects
    std::vector<GameObject*> gameObjects;

    // Physics    
    b2WorldDef physWorldDef = {0};
    b2WorldId physWorldId = {0};

    // Callbacks    
    std::function<void(void)> DrawHUDCallback;
    b2DebugDraw box2dDebugDraw = {0};

    /*******************************************************************************************/
    /***************************************  Functions  ***************************************/
    /*******************************************************************************************/

    void Init(Vector2 internalResolution)
    {        
        if (!IsDebug())
            SetTraceLogLevel(TraceLogLevel::LOG_NONE);

        // Init render and window
        isInternalResolutionSet = true;
        std::string title = std::string(PROJECT_LABEL) + " " + std::string(PROJECT_VER);

        
        int curMonitor = GetCurrentMonitor();
        InitWindow(GetMonitorWidth(curMonitor), GetMonitorHeight(curMonitor), title.c_str());
        SetTargetFPS(60);
        SetInternalResolution(internalResolution);

        worldSpaceCamera.zoom = 1.0f;
        screenSpaceCamera.zoom = 1.0f;        

        // Set window icon
        Image icon = LoadImage("Assets/icon-window.png");
        SetWindowIcon(icon);
        UnloadImage(icon);

        // Init audio
        InitAudioDevice();
        SetMasterVolume(0.8f);

        // Init physics
        physWorldDef = b2DefaultWorldDef();
        physWorldDef.gravity = {0, 9.8 * 8};
        physWorldId = b2CreateWorld(&physWorldDef);

        // Set fullscreen as default
        ToggleBorderlessWindowed();
    }

    void Deinit()
    {
        b2DestroyWorld(physWorldId);
        CloseWindow();
    }

    bool IsRunning()
    {
        return !WindowShouldClose();
    }

    bool IsDebug()
    {
        return PROJECT_BUILD_TYPE == "Debug";
    }

    void Draw()
    {        
        BeginTextureMode(target);
        {
            ClearBackground(RAYWHITE);

            BeginMode2D(worldSpaceCamera);
            {
                // Draw game objects
                for (int i = 0; i < gameObjects.size(); i++)
                {
                    if (!gameObjects[i]->IsDestroyQueued())
                        gameObjects[i]->Draw();
                }

                // Draw UI
                if (DrawHUDCallback)
                    DrawHUDCallback();
                
            }
            EndMode2D();
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(RED);

            BeginMode2D(screenSpaceCamera);
                DrawTexturePro(target.texture, sourceRec, destRec, {0}, 0.0f, WHITE);
            EndMode2D();

            if (IsDebug())
                DrawFPS(GetScreenWidth() - 95, 10);
        }
        EndDrawing();
    }

    void SetInternalResolution(Vector2 resolution)
    {        
        if (target.id)
            UnloadRenderTexture(target);

        Vector2 targetWinSize =
            fullscreen
            ? Vector2{(float) GetMonitorWidth(GetCurrentMonitor()), (float) GetMonitorHeight(GetCurrentMonitor())}
            : Vector2{(float) GetMonitorWidth(GetCurrentMonitor()) / 2, (float) GetMonitorHeight(GetCurrentMonitor()) / 2};

        internalResolution = resolution;
        renderRatio = internalResolution.x / internalResolution.y;
        target = LoadRenderTexture(internalResolution.x, internalResolution.y);
        sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        destRec = { -renderRatio, -renderRatio, targetWinSize.x + (renderRatio*2), targetWinSize.y + (renderRatio*2) };
    }
    
    void RegisterGameObject(GameObject* obj)
    {
        if (!obj)
            throw std::runtime_error("Engine::RegisterGameObject - Object expected, null given");

        gameObjects.push_back(obj);
    }

    void UnregisterGameObject(GameObject *obj)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i] == obj)
            {
                gameObjects.erase(gameObjects.begin() + i);
                break;
            }
        }
    }

    Vector2 GetInternalResolution()
    {
        return internalResolution;
    }

    Vector2 GetMousePositionScaled()
    {
        return mousePositionScaled;
    }

    void Update()
    {
        // Update mouse position scaled        
        mousePositionScaled = GetMousePosition();
        mousePositionScaled.x /= GetScreenWidth() / internalResolution.x;
        mousePositionScaled.y /= GetScreenHeight() / internalResolution.y;
        mousePositionScaled.x = (int) mousePositionScaled.x;
        mousePositionScaled.y = (int) mousePositionScaled.y;

        // Update game objects if game is not paused
        if (!isPaused)
        {
            b2World_Step(physWorldId, 1.0f / 60.0f, 4);
        
            for (int i = 0; i < gameObjects.size(); i++)
            {
                GameObject *obj = gameObjects[i];

                if (obj->IsDestroyQueued())
                {
                    delete obj;
                    i = -1;
                }
                else
                    obj->Update();
            }
        }

        // Toggle fullscreen
        if (IsKeyPressed(KEY_F11))
        {
            int curMonitor = GetCurrentMonitor();

            if (fullscreen)
            {
                Vector2 winSize = {(float) GetMonitorWidth(curMonitor) / 2, (float) GetMonitorHeight(curMonitor) / 2};
                ToggleBorderlessWindowed();
                SetWindowSize(winSize.x, winSize.y);
                SetWindowPosition(winSize.x / 2, winSize.y / 2);
            }
            else
            { 
                SetWindowSize(GetMonitorWidth(curMonitor), GetMonitorHeight(curMonitor));
                SetWindowPosition(0, 0);
                ToggleBorderlessWindowed();
            }

            fullscreen = !fullscreen;
            SetInternalResolution(internalResolution);
        }

        // Update looping sounds playback
        for (int i = 0; i < soundsOnLoop.size(); i++)
        {
            Sound sound = GetAudio(soundsOnLoop[i]);

            if (!IsSoundPlaying(sound))
                PlaySound(sound);
        }
    }

    bool compareGameObjects(GameObject* obj1, GameObject* obj2)
    {
        return obj1->GetDrawPriority() < obj2->GetDrawPriority();
    }

    void ResortObjects()
    {
        std::sort(gameObjects.begin(), gameObjects.end(), compareGameObjects); 
    }

    void SetDrawHUDCallback(std::function<void(void)> callback)
    {
        Engine::DrawHUDCallback = callback;
    }

    void SetPhysFilterCategories(b2ShapeId shapeId, uint64_t filterCategories, uint64_t filterMask)
    {                
        b2Filter filter = b2Shape_GetFilter(shapeId);
        filter.categoryBits = filterCategories;

        if (filterMask != 0)
            filter.maskBits = filterMask;
        
        b2Shape_SetFilter(shapeId, filter);
    }

    std::map<GameObject *, Vector2> GetObjectsInRadius(float radius, Vector2 center)
    {
        std::map<GameObject *, Vector2> objects;

        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->GetPosition().x >= center.x - radius && gameObjects[i]->GetPosition().x <= center.x + radius
                && gameObjects[i]->GetPosition().y >= center.y - radius && gameObjects[i]->GetPosition().y <= center.y + radius)
            {
                objects[gameObjects[i]] = gameObjects[i]->GetPosition();
                objects[gameObjects[i]].x -= center.x;
                objects[gameObjects[i]].y -= center.y;
            }
        }

        return objects;
    }

    b2WorldId GetPhysWorldID()
    {
        return physWorldId;
    }
    
    std::vector<GameObject*> *GetGameObjects()
    {
        return &gameObjects;
    }

    GameObject* GetObjectByPhysShapeId(b2ShapeId id)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            for (b2ShapeId shapeId : *gameObjects[i]->GetPhysShapes())
            {
                
                if (id.index1 == shapeId.index1 && id.world0 == shapeId.world0 && id.revision == shapeId.revision)
                    return gameObjects[i];
            }
        }

        return nullptr;
    }
    
    void SetPaused(bool value)
    {
        isPaused = value;
    }

    Texture LoadTextureFromTexturePool(std::string path)
    {
        for (auto it : textures)
        {
            if (it.first == path)
                return it.second;
        }

        Texture texture = LoadTexture(path.c_str());
        textures[path] = texture;
        
        return texture;
    }

    void UnloadTexture(int id)
    {
        for (auto it : textures)
        {
            if (it.second.id == id)
            {
                UnloadTexture(it.second);
                textures.erase(it.first);
                break;
            }
        }
    }

    void increaseUniqueLoopSoundCounter(std::string soundName)
    {
        // + 1 if value is set
        for (auto& counter : uniqueLoopSoundsCounter)
        {
            if (counter.first == soundName)
            {
                counter.second++;
                return;
            }
        }

        // Set to 1 if value is not set
        uniqueLoopSoundsCounter[soundName] = 1;
    }

    void decreaseUniqueLoopSoundCounter(std::string soundName)
    {
        int i = 0; 

        for (auto& counter : uniqueLoopSoundsCounter)
        {
            i++;

            if (counter.first == soundName)
            {
                counter.second--;

                if (counter.second == 0)
                    uniqueLoopSoundsCounter.erase(soundName);

                return;
            }
        }
    }
    
    void PlayAudio(std::string soundName, SoundPlayMode playMode, float volume, float pitch)
    {
        Sound sound = GetAudio(soundName);
        SetSoundVolume(sound, volume);
        SetSoundPitch(sound, pitch);

        if (playMode == LOOP)
        {
            soundsOnLoop.push_back(soundName);
            PlaySound(sound);
        }
        else if (playMode == LOOP_UNIQUE)
        {
            // If LOOP_UNIQUE push it to queue if it is not already there
            bool isFound = false;

            for (int i = 0; i < soundsOnLoop.size(); i++)
            {
                if (soundsOnLoop[i] == soundName)
                {
                    isFound = true;
                    break;
                }
            }

            if (!isFound)
            {
                soundsOnLoop.push_back(soundName);
                PlaySound(sound);
            }
            else
            {
                increaseUniqueLoopSoundCounter(soundName);
            }
        }
        else
        {
            PlaySound(sound);
        }
    }

    void StopAudio(std::string soundName)
    {
        Sound sound = GetAudio(soundName);

        // If is not in loop - just stop it
        if (std::find(soundsOnLoop.begin(), soundsOnLoop.end(), soundName) == soundsOnLoop.end())
        {
            StopSound(sound);
            return;
        }

        // Remove from sounds on loop
        for (int i = 0; i < soundsOnLoop.size(); i++)
        {
            // Decrease unique loop sound counter
            if (uniqueLoopSoundsCounter.find(soundName) != uniqueLoopSoundsCounter.end())
                decreaseUniqueLoopSoundCounter(soundName);

            // Try to find again, because it might have been removed by previous call
            // If value exists - it means that we still have requests for this unique loop, so we don't remove
            if (uniqueLoopSoundsCounter.find(soundName) != uniqueLoopSoundsCounter.end())
            {
                 continue;
            }
            else
            {
                // Remove from queue and stop
                if (soundsOnLoop[i] == soundName)
                {
                    soundsOnLoop.erase(soundsOnLoop.begin() + i);
                    StopSound(sound);
                    break;
                }
            }
        }
    }

    Sound GetAudio(std::string soundName)
    {
        for (auto it : sounds)
        {
            if (it.first == soundName)
                return it.second;
        }

        throw std::runtime_error("Engine::GetAudio - Sound \"" + soundName + "\" not found!");
    }
    
    void LoadAudioFile(std::string path)
    {
        if (!FileExists(path.c_str()))
            throw std::runtime_error("Engine::LoadAudioFile - File \"" + path + "\" not found!");

        std::string soundName = GetFileNameWithoutExt(path.c_str());
        Sound sound = LoadSound(path.c_str());
        sounds[soundName] = sound;
    }
}