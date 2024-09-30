#include "Game.h"

Game::Game()
{
    Engine::Init({1600, 900}, {480, 270});
}

void Game::Run()
{
    while (Engine::IsRunning())
    {   
        Engine::Draw();
    }
}
