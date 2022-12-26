#include "Game.h"
#include <charconv>
#include <iostream>
#include <algorithm>
#include <random>
#include "JsonConstants.h"
#include "CSVRow.h"
#include "Card.h"

namespace Monopoly
{
#if NDEBUG
    uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
#else
    uint32_t seed = 1337322228u;
#endif

    Game::Game()
    {
        //std::ifstream file("cardList.csv");
        //if (!file.is_open())
        //{
        //    std::cerr << "Couldn't find a file!\n";
        //    exit(1);
        //}
        //CSVRow row;
        //file >> row;// skip first row
        //const int rowsWithCardsCount = 58;
        //for (int i = 0; i < rowsWithCardsCount; ++i)
        //{
        //    if (!(file >> row))
        //    {
        //        std::cerr << "Can't read file!\n";
        //        exit(2);
        //    }
        //    assert(row.size() == 7);

        //    auto to_int = [](const std::string_view& input)
        //    {
        //        int out;
        //        const std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), out);
        //        if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range)
        //        {
        //            std::cerr << "Can't read integer in file!\n";
        //            exit(3);
        //        }
        //        return out;
        //    };

        //    const std::string name(row[0]);
        //    const auto cardType = FindByValue(CardTypeStrings, row[1]);
        //    const int value = to_int(row[2]);
        //    const int count = to_int(row[3]);
        //    const auto color = FindByValue(ColorStrings, row[4]);
        //    const auto secondColor = FindByValue(ColorStrings, row[5]);
        //    const auto actionType = FindByValue(ActionTypeStrings, row[6]);
        //    for (int j = 0; j < count; ++j)
        //    {
        //        m_Deck.push_front(Card(name, cardType, value, color, secondColor, actionType));
        //    }
        //     TODO
        //}
        assert(m_Deck.size() == 106);// all cards readed
        std::shuffle(m_Deck.begin(), m_Deck.end(), std::default_random_engine(seed));
    }

    bool Game::Init(uint32_t playersCount)
    {
        if (playersCount < c_MinPlayersCount || playersCount > c_MaxPlayersCount)
        {
            std::cerr << "Invalid players count!\n";
            return false;
        }

        m_Players.resize(playersCount);
        for (uint32_t i = 0; i < playersCount; ++i)
        {
            PlayerTakeCardsFromDeck(m_Players[i], c_StartCardsCount);
        }

        srand(seed);
        m_CurrentPlayerIndex = rand() % (playersCount + 1);

        return true;
    }

    void Game::BeginTurn()
    {
        auto& currentPlayer = m_Players[m_CurrentPlayerIndex];
        PlayerTakeCardsFromDeck(currentPlayer, c_PassGoCardsCount);
    }

    Game::ETurnOutput Game::Turn(const ETurn input, const int cardIndex, const int setIndex)
    {
        auto& currentPlayer = m_Players[m_CurrentPlayerIndex];
        switch (input)
        {
        case ETurn::Pass:
            return ETurnOutput::NextPlayer;
        case ETurn::FlipCard:
            // TODO Check if has flip card
            // add to set or make new set
            // else
            return ETurnOutput::IncorrectIndex;
            break;
        case ETurn::PlayCard:
        {
            auto card = currentPlayer.RemoveCardFromHand(cardIndex);
            switch (card->GetType())
            {
            case ECardType::Money:
                currentPlayer.AddCardToBank(card);
                break;
            case ECardType::Property:
                currentPlayer.AddProperty(card);
                break;
            case ECardType::Action:
                if (GetActionInput() == EActionInput::ToBank)
                {
                    currentPlayer.AddCardToBank(card);
                }
                else
                {
                    auto res = ProcessActionCard(currentPlayer, card);
                    if(res != ETurnOutput::CardProcessed)
                    {
                        return res;
                    }
                }
                break;
            }
        }
            break;
        default:
            return ETurnOutput::IncorrectInput;
        }

        --m_CurrentPlayerTurnCounter;
        if (m_CurrentPlayerTurnCounter <= 0)
        {
            return ETurnOutput::NextPlayer;
        }
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::ProcessActionCard(Player& currentPlayer, CardContainerElem& card)
    {
        switch (card->GetActionType())
        {
        case EActionType::PassGo:
            PassGo(currentPlayer);
            return ETurnOutput::CardProcessed;
        case EActionType::Hotel:
            break;
        case EActionType::House:
            break;
        case EActionType::DealBreaker:
            break;
        case EActionType::SlyDeal:
            break;
        case EActionType::ForcedDeal:
            break;
        case EActionType::ItsMyBirthday:
            break;
        case EActionType::DebtCollector:
            break;
        case EActionType::RentWild:
            break;
        case EActionType::RentLightBlueBrown:
            break;
        case EActionType::RentOrangePink:
            break;
        case EActionType::RentYellowRed:
            break;
        case EActionType::RentUtilityRailroad:
            break;
        case EActionType::RentBlueGreen:
            break;
        case EActionType::None:
            return ETurnOutput::IncorrectCard;
        case EActionType::DoubleTheRent:
            return ETurnOutput::IncorrectCard;
        case EActionType::JustSayNo:
            return ETurnOutput::IncorrectCard;
        default:
            break;
        }
    }

    int Game::GetExtraCardsCount() const
    {
        auto extraCardsCount = (int)m_Players[m_CurrentPlayerIndex].GetCountCardsInHand() - c_MaxCardsCountInTurnEnd;
        return extraCardsCount > 0 ? extraCardsCount : 0;
    }

    void Game::RemoveExtraCards(const CardIndexesContainer& extraCardsIndexes)
    {
        m_Players[m_CurrentPlayerIndex].RemoveCardsFromHand(extraCardsIndexes);
    }

    void Game::EndTurn()
    {
        m_CurrentPlayerIndex = (m_CurrentPlayerIndex + 1) % m_Players.size();
        m_CurrentPlayerTurnCounter = c_TurnCardsCount;
    }

    void Game::PlayerTakeCardsFromDeck(Player& player, const int count)
    {
        CardContainer cards;
        for (int j = 0; j < count && m_Deck.size() != 0; ++j)
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

        for (size_t i = 0; i < m_Players.size(); ++i)
        {
            //result[c_JSON_Players] += m_Players[i].ToJson();
        }

        return result;
    }

    Game::ETurnOutput Game::PassGo(Player& player)
    {
        PlayerTakeCardsFromDeck(player, c_PassGoCardsCount);
        return ETurnOutput::CardProcessed;
    }

}