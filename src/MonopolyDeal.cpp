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

bool g_ShowGameOutput = false;

void ShowCard(const int index, const Monopoly::CardContainerElem& card)
{
    std::cout << " " << index << "." << card->GetShortData() << " ";
}

void ShowPlayerData(const Monopoly::Game& game, const int playerIndex)
{
    const auto& player = game.GetPlayers()[playerIndex];
    if (playerIndex == game.GetCurrentPlayerIndex())
    {
        std::cout << "Current player(" << playerIndex << "): \n";
    }
    else
    {
        std::cout << "Player(" << playerIndex << "): \n";
    }
    std::cout << "  Hand(" << player.GetHandCardsCount() << "): ";
    for (int i = 0; i < player.GetHandCardsCount(); ++i)
    {
        ShowCard(i, player.GetCardsInHand()[i]);
    }
    std::cout << "\n";

    std::cout << "  Bank(" << player.GetCardsInBank().size() << "): ";
    for (int i = 0; i < player.GetCardsInBank().size(); ++i)
    {
        ShowCard(i, player.GetCardsInBank()[i]);
    }
    std::cout << "\n";

    std::cout << "  Sets(" << player.GetCardSets().size() << "):\n";
    for (int j = 0; j < player.GetCardSets().size(); ++j)
    {
        std::cout << "    Card set(" << j << "): ";
        const auto& set = player.GetCardSets()[j];
        for (int i = 0; i < set.GetProperties().size(); ++i)
        {
            ShowCard(i, set.GetProperties()[i]);
        }
        if (set.GetColor() != Monopoly::EColor::Railroad && set.GetColor() != Monopoly::EColor::Utility)
        {
            std::cout << " " << set.GetProperties().size() << ".House: " << set.IsHasHouse();
            std::cout << " " << set.GetProperties().size() + 1 << ".Hotel: " << set.IsHasHotel();
        }
        std::cout << "\n";
    }
}

void ShowGameData(const Monopoly::Game& g)
{
    if (!g_ShowGameOutput) return;
    ShowPlayerData(g, g.GetCurrentPlayerIndex());
    for (int i = 0; i < g.GetPlayers().size(); ++i)
    {
        if (i != g.GetCurrentPlayerIndex())
            ShowPlayerData(g, i);
    }
    std::cout << "\n";
}

template<class T>
struct ControllerWithStatistics
{
    ControllerWithStatistics() = default;
    int wins = 0;
    std::shared_ptr<T> controller;
};


template<typename T>
void InitControllers(int& initializedControllers, const int& controllersCount,
    std::unique_ptr<Monopoly::Game, std::default_delete<Monopoly::Game>>& game,
    Monopoly::Game::Controllers& controllers,
    std::vector<ControllerWithStatistics<Monopoly::IControllerIO>>& allControllers)
{
    const auto toInitCount = initializedControllers + controllersCount;
    for (uint32_t i = initializedControllers; i < toInitCount; ++i)
    {
        auto contr = std::make_shared<T>(i, *game);
        controllers.emplace_back(contr);
        allControllers[i].controller = contr;
        ++initializedControllers;
    }
}
int main()
{
    const double winReward = 10.0;
    const double loseReward = -5.0;

    json settings;
    std::ifstream ifSettings("settings.json");
    if(!ifSettings.is_open())
    {
        std::cerr << "Can't find \"settings.json\"\n";
        return 1;
    }
    ifSettings >> settings;
    g_ShowGameOutput = settings["ShowGameOutput"];
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
        InitControllers<Monopoly::GreedyAIController>(initializedControllers, greedyAIsCount, game, controllers, allControllers);
        InitControllers<Monopoly::AIController>(initializedControllers, randomAIsCount, game, controllers, allControllers);
        InitControllers<Monopoly::ConsolePlayerController>(initializedControllers, consolePlayersCount, game, controllers, allControllers);

        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
        game->InitNewGame(playersCount, seed);
        game->InitControllers(controllers);
        bool isDrawWin = false;
        int TurnsCount = 0;
        int winnerIndex = -1;
        const auto maxTurnsCount = 250;
        while (game->GetGameIsNotEnded())
        {
            isDrawWin = game->CheckIfTheresCardsToPlay();
            int moves = game->GameBody();
            TurnsCount += 6 - moves;
            if (TurnsCount > maxTurnsCount)
            {
                isDrawWin = true;
                break;
            }
        }
        if (!isDrawWin)
        {
            winnerIndex = game->GetWinnderIndex();
        }
        else
        {
            winnerIndex = game->GetRichestPlayer();
        }

        std::cout << " Turns count " << TurnsCount << "   ";
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
                    {
                        n.second += winReward;
                        //if(!isDrawWin)// TODO 
                        //    n.second += winReward + (1.0 - (TurnsCount / (double)maxTurnsCount));
                        //else
                        //    n.second += winReward + (1.0 - (TurnsCount / (double)maxTurnsCount));
                    }
                }
            }
        }
        else
        {
            for (auto& controller : neuroControllers)
            {
                for (auto& neuron : controller->m_InvolvedNeurons)
                    neuron.second += loseReward;
            }
        }

        ++allControllers[winnerIndex].wins;
        for (auto& controller : neuroControllers)
        {
            for (auto& neuron : controller->m_InvolvedNeurons)
                Monopoly::NeuroController::s_Neurons[neuron.first] += neuron.second;
            controller->m_InvolvedNeurons.clear();
            controller = nullptr;
        }
    }
    for (uint32_t i = 0; i < allControllers.size(); ++i)
        std::cout << allControllers[i].controller->ToString() << " " << i
        << " wins: " << allControllers[i].wins / static_cast<double>(gamesCount) * 100.0 << "%"
        << " loses: " << (gamesCount - allControllers[i].wins) / static_cast<double>(gamesCount) * 100.0 << "%\n";
    Monopoly::NeuroController::SaveNeurons();
}

