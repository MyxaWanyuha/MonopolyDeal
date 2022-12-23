#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>

namespace Monopoly
{
    uint32_t seed = 1337;
    Game::Game()
    {
        // TODO Init cards

        // uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
        // deck push_front
        std::shuffle(m_Deck.begin(), m_Deck.end(), std::default_random_engine(seed));
    }

    bool Game::Init(int playersCount)
    {
        if (playersCount < g_MinPlayersCount || playersCount > g_MaxPlayersCount)
        {
            std::cerr << "Invalid players count!\n";
            return false;
        }

        m_Players.reserve(playersCount);
        for (int i = 0; i < playersCount; ++i)
        {
            PlayerTakeCardsFromDeck(m_Players[i], g_StartCardsCount);
        }

        srand(seed);
        m_CurrentPlayerIndex = rand() % (playersCount + 1);

        return true;
    }

    bool Game::CheckJustSayNo(int victimIndex)
    {
        // TODO
        return false;
    }

    bool Game::DealBreaker(int instigatorIndex, int victimIndex)
    {
        // TODO
        return false;
    }

    void Game::BeginTurn()
    {
        auto& currentPlayer = m_Players[m_CurrentPlayerIndex];
        PlayerTakeCardsFromDeck(currentPlayer, g_PassGoCardsCount);

    }

    void Game::PlayerTakeCardsFromDeck(Player& player, const int count)
    {
        CardContainer cards;
        for (int j = 0; j < count; ++j)
        {
            cards.emplace_back(m_Deck.back());
            m_Deck.pop_back();
        }
        player.AddCardsToHand(std::move(cards));
    }

}