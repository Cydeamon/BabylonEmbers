#pragma once

#include <Engine/Engine.h>

class PhysicsRectangle : GameObject
{
public: 
    PhysicsRectangle(Vector2 position, Vector2 size, float density = 10);
    void Draw() override;
    void Update() override;

    void Enable()  { /* TODO: !!! */ }
    void Disable() { /* TODO: !!! */ }
    void SetColor(Color color) { this->color = color; }
    void SetOutlineColor(Color color) { this->outlineColor = color; }
    void SetOutlineThickness(int thickness) { this->outlineThickness = thickness; }
    void DrawOutline(bool value) { drawOutline = value; }

protected: 
    Vector2 position = {0};
    Vector2 size = {0};
    float density = 0;
    Color color = BLACK;
    bool drawOutline = false;
    int outlineThickness = 1;
    Color outlineColor = GREEN;
};