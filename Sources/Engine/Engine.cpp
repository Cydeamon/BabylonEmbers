#include "Engine.h"
#include "GameObjects/GameObject.h"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include "raylib.h"
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
        return false;//PROJECT_BUILD_TYPE == "Debug";
    }

    void Draw()
    {        
        BeginTextureMode(target);
        {
            ClearBackground(RAYWHITE);

            BeginMode2D(worldSpaceCamera);
            {
                // Draw physics shapes if debug
                // if (IsDebug())
                // {
                //     b2World_Draw(physWorldId, box2dDebugDraw);
                // }

                // Draw game objects
                for (int i = 0; i < gameObjects.size(); i++)
                    gameObjects[i]->Draw();

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

        internalResolution = resolution;
        renderRatio = internalResolution.x / internalResolution.y;
        target = LoadRenderTexture(internalResolution.x, internalResolution.y);
        sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        destRec = { -renderRatio, -renderRatio, GetScreenWidth() + (renderRatio*2), GetScreenHeight() + (renderRatio*2) };
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
                SetWindowPosition(0, 50);
                ToggleBorderlessWindowed();
            }

            fullscreen = !fullscreen;
            SetInternalResolution(internalResolution);
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
}