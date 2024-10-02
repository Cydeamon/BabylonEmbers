#include "Engine.h"
#include <stdexcept>
#include <vector>

namespace Engine
{    
    /*******************************************************************************************/
    /***************************************  Variables  ***************************************/
    /*******************************************************************************************/

    // Window and rendering
    bool isPaused = false;
    bool isInternalResolutionSet = false;
    Vector2 internalResolution = {0, 0};
    Vector2 windowSize = {1280, 720};
    float renderRatio;
    Camera2D worldSpaceCamera = {0};
    Camera2D screenSpaceCamera = {0};
    RenderTexture2D target = {0};
    Rectangle sourceRec = {0};
    Rectangle destRec = {0};

    // Game objects
    std::vector<GameObject*> gameObjects;

    // Physics    
    b2WorldDef physWorldDef = {0};
    b2WorldId physWorldId = {0};

    /*******************************************************************************************/
    /***************************************  Functions  ***************************************/
    /*******************************************************************************************/

    void Init(Vector2 windowSize, Vector2 internalResolution)
    {
        // Init render and window
        if (internalResolution.x == 0 || internalResolution.y == 0)
            internalResolution = windowSize;
        else
            isInternalResolutionSet = true;

        Engine::windowSize = windowSize;
        std::string title = std::string(PROJECT_LABEL) + " " + std::string(PROJECT_VER);
        InitWindow(windowSize.x, windowSize.y, title.c_str());
        // SetTargetFPS(60);
        SetInternalResolution(internalResolution);

        worldSpaceCamera.zoom = 1.0f;
        screenSpaceCamera.zoom = 1.0f;        

        // Init physics
        physWorldDef = b2DefaultWorldDef();
        physWorldDef.gravity = {0, 9.8 * 8};
        physWorldId = b2CreateWorld(&physWorldDef);
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
                for(GameObject *obj : gameObjects)
                {
                    obj->Draw();
                }
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
        destRec = { -renderRatio, -renderRatio, windowSize.x + (renderRatio*2), windowSize.y + (renderRatio*2) };
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
        if (!isInternalResolutionSet)
            return windowSize;

        return internalResolution;
    }

    Vector2 GetMousePositionScaled()
    {
        Vector2 pos = GetMousePosition();

        pos.x /= windowSize.x / internalResolution.x;
        pos.y /= windowSize.y / internalResolution.y;
        pos.x = (int) pos.x;
        pos.y = (int) pos.y;

        return pos;
    }

    void Update()
    {
        if (!isPaused)
        {
            b2World_Step(physWorldId, GetFrameTime(), 4);
        
            for (int i = 0; i < gameObjects.size(); i++)
            {
                GameObject *obj = gameObjects[i];

                if (obj->IsDestroyQueued())
                {
                    delete obj;
                    i = 0;
                }
                else
                    obj->Update();
            }
        }
    }
    
    b2WorldId GetPhysWorldID()
    {
        return physWorldId;
    }
}