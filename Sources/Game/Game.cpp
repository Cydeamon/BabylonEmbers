#include "Game.h"

Game::Game()
{
    Engine::Init({1600, 900}, {480, 270});
}

void Game::Run()
{
    GameObject obj1({100, 10});

    while (Engine::IsRunning())
    {   
        Engine::Draw();
    }
}
