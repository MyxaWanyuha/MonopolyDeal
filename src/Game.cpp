#include "Game.h"
#include <charconv>
#include <iostream>
#include <algorithm>
#include <random>
#include "JsonConstants.h"
#include "CSVRow.h"

namespace Monopoly
{
    uint32_t seed = 1337322228;
    Game::Game()
    {
        std::ifstream file("cardList.csv");// TODO
        if (!file.is_open())
        {
            std::cerr << "Couldn't find a file!\n";
            exit(1);
        }
        CSVRow row;
        file >> row;// skip first row
        const int rowsCount = 58;
        for (int i = 0; i < rowsCount; ++i)
        {
            if (!(file >> row))
            {
                std::cerr << "Can't read file!\n";
                exit(2);
            }
            assert(row.size() == 7);

            auto to_int = [](const std::string_view& input)
            {
                int out;
                const std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), out);
                if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range)
                {
                    std::cerr << "Can't read integer in file!\n";
                    exit(3);
                }
                return out;
            };

            const std::string name(row[0]);
            const auto cardType = FindByValue(CardTypeStrings, row[1]);
            const int value = to_int(row[2]);
            const int count = to_int(row[3]);
            const auto color = FindByValue(ColorStrings, row[4]);
            const auto secondColor = FindByValue(ColorStrings, row[5]);
            const auto actionType = FindByValue(ActionTypeStrings, row[6]);
            for (int j = 0; j < count; ++j)
            {
                m_Deck.push_front(Card(name, cardType, value, color, secondColor, actionType));
            }
        }
        assert(m_Deck.size() == 106);// all cards readed
        // uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
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