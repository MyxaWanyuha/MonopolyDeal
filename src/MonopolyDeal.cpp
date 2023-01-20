#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"
#include <fstream>
#include <controllers/AIController.h>
#include <controllers/NeuronController.h>

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
    ShowAllPlayerData(g, g.GetCurrentPlayerIndex());
    for (int i = 0; i < g.GetPlayers().size(); ++i)
    {
        if (i != g.GetCurrentPlayerIndex())
            ShowPublicPlayerData(g, i);
    }
    //std::cout << "\n";
}

int main()
{
    int NeuroAIWins = 0;
    int RandomAIWins = 0;
    int draws = 0;
    const int gamesCount = 1;
    for (int i = 0; i < gamesCount; ++i)
    {
        //std::cout << "game " << i << "   ";
        auto game = std::make_unique<Monopoly::Game>();
        game->InitOutputFunction(ShowGameData);
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

        const int playersCount = 2;
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
                    n.second += 0.0;
                break;
            }
            game->GameBody();
        }
        if (game->GetWinnderIndex() == 0)
        {
            ++NeuroAIWins;
            std::cout << "ML!\n";
            for (auto& n : neuro->m_InvolvedNeurons)
                n.second += 100.0;
        }
        else if (game->GetWinnderIndex() != -1)
        {
            ++RandomAIWins;
            std::cout << "AI!\n";
            for (auto& n : neuro->m_InvolvedNeurons)
                n.second -= 50.0;
        }
        for (auto& n : neuro->m_InvolvedNeurons)
            Monopoly::NeuroController::s_Neurons[n.first] += n.second;
        neuro->m_InvolvedNeurons.clear();
    }
    std::cout << "NeuroAI Wins: " << NeuroAIWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "RandomAI Wins: " << RandomAIWins / static_cast<double>(gamesCount) * 100.0 << "%\n";
    std::cout << "Draws: " << draws / static_cast<double>(gamesCount) * 100.0 << "%\n";

    Monopoly::NeuroController::SaveNeurons();
}
