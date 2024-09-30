#include <Config.h>
#include <iostream>
#include <filesystem>
#include <Game/Game.h>

int main()
{
    // CD up if we're in Bin folder
    std::string cwd = std::filesystem::current_path().string();

    if (cwd.find("Bin") != std::string::npos)
        std::filesystem::current_path(std::filesystem::current_path().parent_path());

    try
    {
        Game game;
        game.Run();
    }
    catch (const std::exception &e)
    {
        std::string title = std::string(PROJECT_LABEL) + ". Fatal error";
        MessageBox(NULL, e.what(), title.c_str(), MB_OK | MB_ICONERROR | MB_TOPMOST);
    }
}