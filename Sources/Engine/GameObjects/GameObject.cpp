#include "GameObject.h"
#include <Engine/Engine.h>

GameObject::GameObject(Vector2 position)
{
    this->position = position;
    Engine::RegisterGameObject(this);
}

void GameObject::Draw()
{
    DrawCircle(position.x + 10, position.y + 10, 10, {255, 0, 255, 255});
    DrawText("!!!", position.x + 8, position.y + 6, 8, WHITE);
    
    char* message = "GameObject type is not set";
    DrawText("GameObject type is not set", position.x + 5 - (MeasureText(message, 8) / 2), position.y + 20, 8, RED);
}
