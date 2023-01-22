#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"
#include <fstream>
#include <controllers/AIController.h>
#include <controllers/NeuronController.h>
#include <controllers/PlayerController.h>
#include <controllers/GreedyAIController.h>

void ShowCard(const int index, const Monopoly::CardContainerElem& card)
{
    std::cout << " " << index << "." << card->GetShortData() << " ";
}

void ShowPlayerData(const Monopoly::Game& g, const int playerIndex)
{
    const auto& player = g.GetPlayers()[playerIndex];
    if (playerIndex == g.GetCurrentPlayerIndex())
    {
        std::cout << "Current player(" << playerIndex << "): \n";
    }
    else
    {
        std::cout << "Player(" << playerIndex << "): \n";
    }
    std::cout << "  Hand(" << player.GetCardsInHand().size() << "): ";
    for (int i = 0; i < player.GetCardsInHand().size(); ++i)
        ShowCard(i, player.GetCardsInHand()[i]);
    std::cout << "\n";

    std::cout << "  Bank(" << player.GetCardsInBank().size() << "): ";
    for (int i = 0; i < player.GetCardsInBank().size(); ++i)
        ShowCard(i, player.GetCardsInBank()[i]);
    std::cout << "\n";

    std::cout << "  Sets(" << player.GetCardSets().size() << "):\n";
    for (int j = 0; j < player.GetCardSets().size(); ++j)
    {
        std::cout << "    Card set(" << j << "): ";
        const auto& set = player.GetCardSets()[j];
        for (int i = 0; i < set.GetCards().size(); ++i)
        {
            ShowCard(i, set.GetCards()[i]);
        }
        if (set.GetColor() != Monopoly::EColor::Railroad && set.GetColor() != Monopoly::EColor::Utility)
        {
            std::cout << " " << set.GetCards().size() << ".House: " << set.IsHasHouse();
            std::cout << " " << set.GetCards().size() + 1 << ".Hotel: " << set.IsHasHotel();
        }
        std::cout << "\n";
    }
}

void ShowAllPlayerData(const Monopoly::Game& g, const int playerIndex)
{
    ShowPlayerData(g, playerIndex);
}

void ShowPublicPlayerData(const Monopoly::Game& g, const int playerIndex)
{
    ShowPlayerData(g, playerIndex);
}

void ShowGameData(const Monopoly::Game& g)
{
    //ShowAllPlayerData(g, g.GetCurrentPlayerIndex());
    //for (int i = 0; i < g.GetPlayers().size(); ++i)
    //{
    //    if (i != g.GetCurrentPlayerIndex())
    //        ShowPublicPlayerData(g, i);
    //}
    //std::cout << "\n";
}

struct NeuroWithStatistics
{
    NeuroWithStatistics() = default;
    NeuroWithStatistics(uint32_t wins, uint32_t loses, std::shared_ptr<Monopoly::NeuroController> controller)
        :wins(wins), loses(loses), controller(controller)
    {
    }
    uint32_t wins = 0;
    uint32_t loses = 0;
    std::shared_ptr<Monopoly::NeuroController> controller;
};

int main()
{
    int NeuroAIWins = 0;
    int RandomAIWins = 0;
    int draws = 0;
    const int gamesCount = 1000;
    const double winReward = 200.0;
    const double loseReward = -50.0;
    const double drawReward = -5.0;
    const int playersCount = 5;
    const int neuroCount = 1;

    std::vector<NeuroWithStatistics> neuroControllers;
    neuroControllers.resize(neuroCount);
    for (int j = 0; j < gamesCount; ++j)
    {
        std::cout << "game " << j << "   ";
        auto game = std::make_unique<Monopoly::Game>();
        game->InitOutputFunction(ShowGameData);
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

        Monopoly::Game::Controllers controllers;
        for (uint32_t i = 0; i < neuroCount; ++i)
        {
            auto neuro = std::make_shared<Monopoly::NeuroController>(i, *game);
            neuroControllers[i].controller = neuro;
            controllers.emplace_back(neuro);
        }

        for (uint32_t i = neuroCount; i < playersCount; ++i)
            controllers.emplace_back(std::make_shared<Monopoly::GreedyAIController>(i, *game));
        
        game->InitNewGame(playersCount, seed);
        game->InitControllers(controllers);

        while (game->GetGameIsNotEnded())
        {
            if (game->IsDraw())
            {
                ++draws;
                std::cout << "Draw!\n";
                for(auto& neuro : neuroControllers)
                    for (auto& n : neuro.controller->m_InvolvedNeurons)
                        n.second += drawReward;
                break;
            }
            game->GameBody();
        }
        const auto winnerIndex = game->GetWinnderIndex();
        if (winnerIndex != -1)
        {
            if (winnerIndex < neuroCount)
            {
                ++NeuroAIWins;
                std::cout << "NeuroAI!\n";

                for (uint32_t i = 0; i < neuroCount; ++i)
                {
                    if (i != winnerIndex)
                    {
                        for (auto& n : neuroControllers[i].controller->m_InvolvedNeurons)
                            n.second += loseReward;
                        ++neuroControllers[i].loses;
                    }
                    else
                    {
                        for (auto& n : neuroControllers[winnerIndex].controller->m_InvolvedNeurons)
                            n.second += winReward;
                        ++neuroControllers[i].wins;
                    }
                }
            }
            else
            {
                ++RandomAIWins;
                std::cout << "RandomAI!\n";
                for (auto& neuro : neuroControllers)
                {
                    for (auto& n : neuro.controller->m_InvolvedNeurons)
                        n.second += loseReward;
                    ++neuro.loses;
                }
            }
        }
        for (auto& neuro : neuroControllers)
        {
            for (auto& n : neuro.controller->m_InvolvedNeurons)
                Monopoly::NeuroController::s_Neurons[n.first] += n.second;
            neuro.controller->m_InvolvedNeurons.clear();
            neuro.controller = nullptr;
        }
    }
    for (uint32_t i = 0; i < neuroCount; ++i)
        std::cout << "Neuro " << i 
        << " wins: " << neuroControllers[i].wins / static_cast<double>(gamesCount) * 100.0 
        << " loses: " << neuroControllers[i].loses / static_cast<double>(gamesCount) * 100.0 << "\n";

    std::cout << "NeuroAI Wins: " << NeuroAIWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "RandomAI Wins: " << RandomAIWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "Draws: " << draws / static_cast<double>(gamesCount) * 100.0 << "%\n";

    Monopoly::NeuroController::SaveNeurons();
}
