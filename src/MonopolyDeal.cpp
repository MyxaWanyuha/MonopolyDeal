#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"
#include <fstream>
#include <controllers/AIController.h>

class ConsoleGame : public Monopoly::Game
{
public:
    ConsoleGame(const std::string& fileName)
        : Game(fileName)
    {
    }

    ConsoleGame()
    {
//#if NDEBUG
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
//#else
//        const uint32_t seed = 1337322228u;
//#endif
        const int playersCount = 5;
        Game::Controllers controllers;
        for (uint32_t i = 0; i < playersCount; ++i)
        {
            controllers.emplace_back(std::make_shared<Monopoly::AIController>(i, *this));
        }
        Game::InitNewGame(playersCount, seed);
        Game::InitControllers(std::move(controllers));
    }

private:

};

int main()
{
    {
        while (true)
        {
            std::unique_ptr<ConsoleGame> g = std::make_unique<ConsoleGame>();
            g->Run();
        }
    }
    std::unique_ptr<ConsoleGame> g;

    int input = 2;
    while (true)
    {/*
        std::cout << "Load(1) or new game(2):";
        std::cin >> input;
        if (input == 1)
        {
            std::cout << "Enter filename:";
            std::string name;
            std::cin >> name;
            std::ifstream f(name.c_str());
            if (f.good() == false)
                continue;
            g = std::make_unique<ConsoleGame>(name);
            break;
        }
        else*/ if (input == 2)
        {
            g = std::make_unique<ConsoleGame>();
            break;
        }
        std::cerr << "Incorrect input!\n";
    }
    return g->Run();
}
