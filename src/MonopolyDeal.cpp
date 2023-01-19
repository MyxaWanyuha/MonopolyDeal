#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"
#include <fstream>
#include <controllers/AIController.h>
#include <controllers/NeuronController.h>

int main()
{
    int MLWins = 0;
    int AIWins = 0;
    int draws = 0;
    const int gamesCount = 1000;
    for (int i = 0; i < gamesCount; ++i)
    {
        std::cout << "game " << i << "   ";
        auto game = std::make_unique<Monopoly::Game>();
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

        const int playersCount = 5;
        Monopoly::Game::Controllers controllers;
        auto neuro = std::make_shared<Monopoly::NeuroController>(0, *game);
        controllers.emplace_back(neuro);//NeuroController
        for (uint32_t i = 1; i < playersCount; ++i)
        {
            controllers.emplace_back(std::make_shared<Monopoly::AIController>(i, *game));
        }
        game->InitNewGame(playersCount, seed);
        game->InitControllers(controllers);

        while (game->GetGameIsNotEnded())
        {
            if (game->IsDraw())
            {
                ++draws;
                std::cout << "Draw!\n";
                for (auto& n : neuro->m_InvolvedNeurons)
                    n.second += 5.0;
                break;
            }
            game->GameBody();
        }
        if (game->GetWinnderIndex() == 0)
        {
            ++MLWins;
            std::cout << "ML!\n";
            for (auto& n : neuro->m_InvolvedNeurons)
                n.second += 100.0;
        }
        else if (game->GetWinnderIndex() != -1)
        {
            ++AIWins;
            std::cout << "AI!\n";
            for (auto& n : neuro->m_InvolvedNeurons)
                n.second -= 50.0;
        }
        for (auto& n : neuro->m_InvolvedNeurons)
            Monopoly::NeuroController::s_Neurons[n.first] += n.second;
        neuro->m_InvolvedNeurons.clear();
    }
    std::cout << "ML Wins: " << MLWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "AI Wins: " << AIWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "Draws: " << draws / static_cast<double>(gamesCount) * 100.0 << "%\n";
}
