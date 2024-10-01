#include "Engine.h"
#include <stdexcept>
#include <vector>

namespace Engine
{    
    /*******************************************************************************************/
    /***************************************  Variables  ***************************************/
    /*******************************************************************************************/

    std::vector<GameObject*> gameObjects;
    bool isInternalResolutionSet = false;
    Vector2 internalResolution = {0, 0};
    Vector2 windowSize = {1280, 720};
    float renderRatio;
    Camera2D worldSpaceCamera = {0};
    Camera2D screenSpaceCamera = {0};
    RenderTexture2D target = {0};
    Rectangle sourceRec = {0};
    Rectangle destRec = {0};

    /*******************************************************************************************/
    /***************************************  Functions  ***************************************/
    /*******************************************************************************************/

    void Init(Vector2 windowSize, Vector2 internalResolution)
    {
        if (internalResolution.x == 0 || internalResolution.y == 0)
            internalResolution = windowSize;
        else
            isInternalResolutionSet = true;

        Engine::windowSize = windowSize;
        std::string title = std::string(PROJECT_LABEL) + " " + std::string(PROJECT_VER);
        InitWindow(windowSize.x, windowSize.y, title.c_str());
        SetInternalResolution(internalResolution);

        worldSpaceCamera.zoom = 1.0f;
        screenSpaceCamera.zoom = 1.0f;
    }

    void Deinit()
    {
        CloseWindow();
    }

    bool IsRunning()
    {
        return !WindowShouldClose();
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
    
    void RegisterGameObject(GameObject *obj)
    {
        if (obj == nullptr)
            throw std::runtime_error("Engine::RegisterGameObject - Object expected, null given");

        gameObjects.push_back(obj);
    }
    Vector2 GetInternalResolution()
    {
        if (!isInternalResolutionSet)
            return windowSize;

        return internalResolution;
    }
}