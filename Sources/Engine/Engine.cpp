#include "Engine.h"
#include <stdexcept>

namespace Engine
{    
    /*******************************************************************************************/
    /***************************************  Variables  ***************************************/
    /*******************************************************************************************/

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

    bool IsRunning()
    {
        return WindowShouldClose();
    }

    void Draw()
    {        
        BeginTextureMode(target);
        {
            ClearBackground(RAYWHITE);

            BeginMode2D(worldSpaceCamera);
            {
                DrawCircle(20, 20, 10, RED);
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

            DrawText(TextFormat("Screen resolution: %ix%i", windowSize.x, windowSize.y), 10, 10, 20, DARKBLUE);
            DrawText(TextFormat("World resolution: %ix%i", internalResolution.x, internalResolution.y), 10, 40, 20, DARKGREEN);
            DrawFPS(GetScreenWidth() - 95, 10);
        }
        EndDrawing();
    }

    void SetInternalResolution(Vector2 resolution)
    {        
        if (target.id)
            UnloadRenderTexture(target);

        Engine::internalResolution = internalResolution;
        renderRatio = internalResolution.x / internalResolution.y;
        target = LoadRenderTexture(internalResolution.x, internalResolution.y);
        sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        destRec = { -renderRatio, -renderRatio, windowSize.x + (renderRatio*2), windowSize.y + (renderRatio*2) };
    }
}