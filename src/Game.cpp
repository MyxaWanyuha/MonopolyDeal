#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include "JsonConstants.h"

namespace Monopoly
{
    uint32_t seed = 1337322228;
    Game::Game()
    {
        // TODO Init cards

        // uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

        for(int i = 0; i < 10; ++i)
            m_Deck.push_front(Card("PassGo", ECardType::Action, 1, EColor::None, EColor::None, EActionType::PassGo));
        
        for (int i = 0; i < 2; ++i)
            m_Deck.push_front(Card("Double the rent", ECardType::Action, 1, EColor::None, EColor::None, EActionType::DoubleTheRent));

        for (int i = 0; i < 5; ++i)
            m_Deck.push_front(Card("Two million", ECardType::Money, 2, EColor::None, EColor::None, EActionType::None));

        for (int i = 0; i < 3; ++i)
            m_Deck.push_front(Card("Atlantic avenue", ECardType::Property, 3, EColor::Yellow, EColor::None, EActionType::None));
        
        m_Deck.push_front(Card("Wild Blue-Green", ECardType::Property, 4, EColor::Blue, EColor::Green, EActionType::None));

        std::shuffle(m_Deck.begin(), m_Deck.end(), std::default_random_engine(seed));
    }

    bool Game::Init(int playersCount)
    {
        if (playersCount < c_MinPlayersCount || playersCount > c_MaxPlayersCount)
        {
            std::cerr << "Invalid players count!\n";
            return false;
        }

        m_Players.resize(playersCount);
        for (int i = 0; i < playersCount; ++i)
        {
            PlayerTakeCardsFromDeck(m_Players[i], c_StartCardsCount);
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
        PlayerTakeCardsFromDeck(currentPlayer, c_PassGoCardsCount);
    }

    void Game::Turn(const int input)
    {
        // input 0 is skip
        
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

    json Game::GetAllData() const
    {
        json result;
        result[c_JSON_CurrentPlayerIndex] = m_CurrentPlayerIndex;

        for (int i = 0; i < m_Players.size(); ++i)
        {
            const auto& player = m_Players[i];
            json playerData;
            auto addPlayerCards = [&](const CardContainer& cards, const char* jsonKey)
            {
                json cardsData;
                for (const auto& card : cards)
                    cardsData += card.ToJSON();
                playerData[jsonKey] = cardsData;
            };

            addPlayerCards(player.GetCardsInHand(), c_JSON_Hand);
            addPlayerCards(player.GetCardsInBank(), c_JSON_Bank);
            {
                auto& sets = player.GetCardSets();
                json setsData;
                for(int j = 0; j < sets.size(); ++j)
                    setsData += sets[j].ToJSON();
                playerData[c_JSON_Sets] = setsData;
            }

            result[c_JSON_Players] += playerData;
        }

        return result;
    }

}