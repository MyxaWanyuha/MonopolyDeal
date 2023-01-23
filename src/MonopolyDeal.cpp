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

bool s_ShowGameOutput = false;

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
    if (!s_ShowGameOutput) return;
    ShowAllPlayerData(g, g.GetCurrentPlayerIndex());
    for (int i = 0; i < g.GetPlayers().size(); ++i)
    {
        if (i != g.GetCurrentPlayerIndex())
            ShowPublicPlayerData(g, i);
    }
    std::cout << "\n";
}

template<class T>
struct ControllerWithStatistics
{
    ControllerWithStatistics() = default;
    ControllerWithStatistics(uint32_t wins, uint32_t loses, std::shared_ptr<T> controller)
        :wins(wins), loses(loses), controller(controller)
    {
    }
    uint32_t wins = 0;
    uint32_t loses = 0;
    std::shared_ptr<T> controller;
};

int main()
{
    const double winReward = 200.0;
    const double loseReward = -50.0;
    const double drawReward = -5.0;

    json settings;
    std::ifstream ifSettings("settings.json");
    if(!ifSettings.is_open())
    {
        std::cerr << "Can't find \"settings.json\"\n";
        return 1;
    }
    ifSettings >> settings;
    s_ShowGameOutput = settings["ShowGameOutput"];
    const int gamesCount = settings["GamesCount"];
    const int consolePlayersCount = settings["ConsolePlayersCount"];
    const int randomAIsCount = settings["RandomAIsCount"];
    const int greedyAIsCount = settings["GreedyAIsCount"];
    const int neuroAIsCount = settings["NeuroAIsCount"];
    const auto playersCount = consolePlayersCount + randomAIsCount + greedyAIsCount + neuroAIsCount;
    if (playersCount < Monopoly::Game::c_MinPlayersCount
        || neuroAIsCount > Monopoly::Game::c_MaxPlayersCount)
    {
        std::cerr << "Wrong controllers count! Check settings.json. Maximum controllers count is " << Monopoly::Game::c_MaxPlayersCount
            << " minimum controllers count is " << Monopoly::Game::c_MinPlayersCount << ". You have " << playersCount << ".\n";
        return 1;
    }

    std::vector<ControllerWithStatistics<Monopoly::IControllerIO>> allControllers;
    allControllers.resize(playersCount);
    std::vector<std::shared_ptr<Monopoly::NeuroController>> neuroControllers;
    neuroControllers.resize(neuroAIsCount);
    for (int j = 0; j < gamesCount; ++j)
    {
        std::cout << "game " << j << "  ";
        auto game = std::make_unique<Monopoly::Game>();
        game->InitOutputFunction(ShowGameData);
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

        int initializedControllers = 0;
        Monopoly::Game::Controllers controllers;
        for (uint32_t i = initializedControllers; i < neuroAIsCount; ++i)
        {
            auto neuro = std::make_shared<Monopoly::NeuroController>(i, *game);
            neuroControllers[i] = neuro;
            allControllers[i].controller = neuro;
            controllers.emplace_back(neuro);
            ++initializedControllers;
        }

        auto toInitCount = initializedControllers + greedyAIsCount;
        for (uint32_t i = initializedControllers; i < toInitCount; ++i)
        {
            auto contr = std::make_shared<Monopoly::GreedyAIController>(i, *game);
            controllers.emplace_back(contr);
            allControllers[i].controller = contr;
            ++initializedControllers;
        }
        toInitCount = initializedControllers + randomAIsCount;
        for (uint32_t i = initializedControllers; i < toInitCount; ++i)
        {
            auto contr = std::make_shared<Monopoly::AIController>(i, *game);
            controllers.emplace_back(contr);
            allControllers[i].controller = contr;
            ++initializedControllers;
        }
        toInitCount = initializedControllers + consolePlayersCount;
        for (uint32_t i = initializedControllers; i < toInitCount; ++i)
        {
            auto contr = std::make_shared<Monopoly::ConsolePlayerController>(i, *game);
            controllers.emplace_back(contr);
            allControllers[i].controller = contr;
            ++initializedControllers;
        }

        game->InitNewGame(playersCount, seed);
        game->InitControllers(controllers);

        while (game->GetGameIsNotEnded())
        {
            game->CheckIfTheresCardsToPlay();
            game->GameBody();
        }
        const auto winnerIndex = game->GetWinnderIndex();
        if (winnerIndex != -1)
        {
            std::cout << allControllers[winnerIndex].controller->ToString() << " " << winnerIndex << " winner\n";
            if (winnerIndex < neuroAIsCount)
            {
                for (uint32_t i = 0; i < neuroAIsCount; ++i)
                {
                    if (i != winnerIndex)
                    {
                        for (auto& n : neuroControllers[i]->m_InvolvedNeurons)
                            n.second += loseReward;
                    }
                    else
                    {
                        for (auto& n : neuroControllers[winnerIndex]->m_InvolvedNeurons)
                            n.second += winReward;
                    }
                }
            }
            else
            {
                for (auto& neuro : neuroControllers)
                {
                    for (auto& n : neuro->m_InvolvedNeurons)
                        n.second += loseReward;
                }
            }

            for (uint32_t i = 0; i < allControllers.size(); ++i)
            {
                if (i == winnerIndex)
                    ++allControllers[i].wins;
                else
                    ++allControllers[i].loses;
            }
        }
        for (auto& neuro : neuroControllers)
        {
            for (auto& n : neuro->m_InvolvedNeurons)
                Monopoly::NeuroController::s_Neurons[n.first] += n.second;
            neuro->m_InvolvedNeurons.clear();
            neuro = nullptr;
        }
    }
    for (uint32_t i = 0; i < allControllers.size(); ++i)
        std::cout << allControllers[i].controller->ToString() << " " << i
        << " wins: " << allControllers[i].wins / static_cast<double>(gamesCount) * 100.0 << "%"
        << " loses: " << allControllers[i].loses / static_cast<double>(gamesCount) * 100.0 << "%\n";
    Monopoly::NeuroController::SaveNeurons();
}
