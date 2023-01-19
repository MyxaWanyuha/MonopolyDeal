#include "Monopoly_pch.h"
#include "CSVRow.h"

#include <fstream>
#include "JsonConstants.h"
#include "nlohmann/json.hpp"
using json = ::nlohmann::json;

namespace Monopoly
{

    Game::Game()
    {
        m_CurrentPlayerTurnCounter = c_TurnCardsCount;
        m_CurrentPlayerTurnCounterForAllMoves = m_CurrentPlayerTurnCounter * 2;
        std::ifstream file("cardList.csv");
        if (!file.is_open())
        {
            std::cerr << "Couldn't find a file!\n";
            exit(1);
        }
        CSVRow row;
        file >> row;// skip first row
        const int rowsWithCardsCount = 58;
        for (int i = 0; i < rowsWithCardsCount; ++i)
        {
            if (!(file >> row))
            {
                std::cerr << "Can't read file!\n";
                exit(2);
            }
            assert(row.size() == 8);

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

            std::string name(row[0]);
            name.shrink_to_fit();
            std::string shortData(row[7]);
            shortData.shrink_to_fit();
            const auto& cardType = row[1];
            const int count = to_int(row[3]);
            const int value = to_int(row[2]);
            if (cardType == "Action")
            {
                const auto& actionType = row[6];
                if (actionType == "PassGo")
                    InitDeck<PassGoCard>(count, name, shortData, value);
                else if (actionType == "DoubleTheRent")
                    InitDeck<DoubleTheRentCard>(count, name, shortData, value);
                else if (actionType == "JustSayNo")
                    InitDeck<JustSayNoCard>(count, name, shortData, value);
                else if (actionType == "Hotel")
                    InitDeck<HotelCard>(count, name, shortData, value);
                else if (actionType == "House")
                    InitDeck<HouseCard>(count, name, shortData, value);
                else if (actionType == "DealBreaker")
                    InitDeck<DealBreakerCard>(count, name, shortData, value);
                else if (actionType == "SlyDeal")
                    InitDeck<SlyDealCard>(count, name, shortData, value);
                else if (actionType == "ForcedDeal")
                    InitDeck<ForcedDealCard>(count, name, shortData, value);
                else if (actionType == "ItsMyBirthday")
                    InitDeck<ItsMyBirthdayCard>(count, name, shortData, value);
                else if (actionType == "DebtCollector")
                    InitDeck<DebtCollectorCard>(count, name, shortData, value);
                else if (actionType == "RentWild")
                    InitDeck<RentWildCard>(count, name, shortData, value);
                else if (actionType == "RentLightBlueBrown")
                    InitDeck<RentLightBlueBrown>(count, name, shortData, value);
                else if (actionType == "RentOrangePink")
                    InitDeck<RentOrangePink>(count, name, shortData, value);
                else if (actionType == "RentYellowRed")
                    InitDeck<RentYellowRed>(count, name, shortData, value);
                else if (actionType == "RentUtilityRailroad")
                    InitDeck<RentUtilityRailroad>(count, name, shortData, value);
                else if (actionType == "RentBlueGreen")
                    InitDeck<RentBlueGreen>(count, name, shortData, value);
            }
            else if (cardType == "Money")
            {
                InitDeck<MoneyCard>(count, name, shortData, value);
            }
            else if (cardType == "Property")
            {
                auto color1 = c_ColorStrToEnum.at(row[4]);
                auto color2 = c_ColorStrToEnum.at(row[5]);
                for (int j = 0; j < count; ++j)
                    m_Deck.push_front(std::make_shared<PropertyCard>(name, shortData, value, color1, color2));
            }
        }

        assert(m_Deck.size() == 106);// all cards readed
    }

    Game::Game(const std::string& fileName)
    {
        Load(fileName);
    }

    bool Game::InitNewGame(const uint32_t playersCount, const uint32_t seed)
    {
        std::shuffle(m_Deck.begin(), m_Deck.end(), std::default_random_engine(seed));
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
        m_CurrentPlayerIndex = rand() % playersCount;

        return true;
    }

    bool Game::InitControllers(const Controllers& controllers)
    {
        if (controllers.size() != m_Players.size())
        {
            std::cerr << "Invalide controllers count!\n";
            return false;
        }
        m_Controllers = controllers;
        return true;
    }

    int Game::Run()
    {
        while (GetGameIsNotEnded())
        {
            if (IsDraw())
            {
                //std::cout << "Draw!\n";
                return 1;
            }
            GameBody();
        }
        //std::cout << "AI " << m_CurrentPlayerIndex << " won!\n";
        return 0;
    }

    void Game::GameBody()
    {
        BeginTurn();
        ETurnOutput turnOutput;
        do {
            m_Controllers[m_CurrentPlayerIndex]->ShowPrivatePlayerData(m_CurrentPlayerIndex);
            for (int i = 0; i < GetPlayers().size(); ++i)
            {
                if (i != GetCurrentPlayerIndex())
                    m_Controllers[i]->ShowPublicPlayerData(i);
            }
            auto turn = ETurn::Pass;
            int cardIndex = 0, setIndex = 0;
            m_Controllers[m_CurrentPlayerIndex]->InputTurn(turn, cardIndex, setIndex);
            turnOutput = Turn(turn, cardIndex, setIndex);
            if (turnOutput == Game::ETurnOutput::IncorrectInput)
            {
                std::cerr << "Input is incorrect!\n";
            }
            else if (turnOutput == Game::ETurnOutput::IncorrectIndex)
            {
                std::cerr << "Index is incorrect!\n";
            }
        } while (turnOutput != Game::ETurnOutput::NextPlayer && turnOutput != Game::ETurnOutput::GameOver);
        if (turnOutput == Game::ETurnOutput::GameOver)
        {
            return;
        }
        if (const auto extraCardsCount = Game::GetExtraCardsCount(); extraCardsCount > 0)
        {
            std::vector<int> container;
            m_Controllers[m_CurrentPlayerIndex]->InputIndexesToRemove(extraCardsCount, container);
            assert(container.size() == extraCardsCount);
            RemoveExtraCards(container);
        }
        EndTurn();
    }

    int Game::CardsInGameCount() const
    {
        // method for debug
        int res = 0;
        res += GetDeckCardsCount();
        res += GetDrawCards().size();
        for (const auto& p : GetPlayers())
        {
            res += p.GetCardsInHand().size();
            res += p.GetCardsInBank().size();
            for (const auto& set : p.GetCardSets())
            {
                res += set.GetCards().size();
                res += set.IsHasHotel();
                res += set.IsHasHouse();
            }
        }
        return res;
    }

    void Game::BeginTurn()
    {
        if (m_Players[m_CurrentPlayerIndex].GetCardsInHand().size() == 0)
        {
            PlayerTakeCardsFromDeck(m_Players[m_CurrentPlayerIndex], c_StartCardsCount);
        }
        else
        {
            PlayerTakeCardsFromDeck(m_Players[m_CurrentPlayerIndex], c_PassGoCardsCount);
        }
    }

    Game::ETurnOutput Game::Turn(const ETurn input, const int cardIndex, const int setIndexForFlip)
    {
        --m_CurrentPlayerTurnCounterForAllMoves;
        if (m_CurrentPlayerTurnCounterForAllMoves <= 0)
        {
            return ETurnOutput::NextPlayer;
        }
        auto& currentPlayer = m_Players[m_CurrentPlayerIndex];
        switch (input)
        {
        case ETurn::Pass:
            return ETurnOutput::NextPlayer;
        case ETurn::FlipCard:
        {
            const auto card = currentPlayer.RemoveCardFromSet(setIndexForFlip, cardIndex);
            if (card != nullptr && card->GetColor2() != EColor::None)
            {
                card->SwapColor();
                currentPlayer.AddProperty(card);
                if (currentPlayer.IsWinner())
                {
                    m_WinnerIndex = m_CurrentPlayerIndex;
                    return ETurnOutput::GameOver;
                }
                return ETurnOutput::CardProcessed;
            }
            else
            {
                currentPlayer.AddProperty(card);
            }
            return ETurnOutput::IncorrectIndex;
        }
        case ETurn::PlayCard:
        {
            auto card = currentPlayer.RemoveCardFromHand(cardIndex);
            const auto type = card->GetType();
            if (type == ECardType::Money)
            {
                currentPlayer.AddCardToBank(card);
            }
            else if (type == ECardType::Property)
            {
                currentPlayer.AddProperty(card);
                if (currentPlayer.IsWinner())
                {
                    m_WinnerIndex = m_CurrentPlayerIndex;
                    return ETurnOutput::GameOver;
                }
            }
            else if (type == ECardType::Action)
            {
                if (m_Controllers[m_CurrentPlayerIndex]->GetActionInput() == EActionInput::ToBank)
                {
                    currentPlayer.AddCardToBank(card);
                }
                else
                {
                    const auto res = ProcessActionCard(currentPlayer, card);
                    if (res != ETurnOutput::CardProcessed)
                    {
                        return res;
                    }
                }
                if (currentPlayer.IsWinner())
                {
                    m_WinnerIndex = m_CurrentPlayerIndex;
                    return ETurnOutput::GameOver;
                }
                for (int i = 0; i < m_Players.size(); ++i)
                {
                    if (m_Players[i].IsWinner())
                    {
                        m_WinnerIndex = i;
                        return ETurnOutput::GameOver;
                    }
                }
            }
            break;
        }
        case ETurn::HouseHotelOnTable:
            return MoveHouseOrHotelFromTableToSet(currentPlayer);
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

    Game::ETurnOutput Game::MoveHouseOrHotelFromTableToSet(Player& currentPlayer)
    {
        std::vector<int> emptyHouseSetsIndexes;
        std::vector<int> emptyHotelSetsIndexes;
        std::vector<int> fullSetsWithoutHouseIndexes;
        std::vector<int> fullSetsWithoutHotelsIndexes;
        FindEnhancementsAndFullSetsWithout(currentPlayer, emptyHouseSetsIndexes, emptyHotelSetsIndexes, fullSetsWithoutHouseIndexes, fullSetsWithoutHotelsIndexes);

        if ((!emptyHouseSetsIndexes.empty() && !fullSetsWithoutHouseIndexes.empty()) 
            || (!emptyHotelSetsIndexes.empty() && !fullSetsWithoutHotelsIndexes.empty()))
        {
            int emptyIndex, setIndex;
            m_Controllers[m_CurrentPlayerIndex]->InputMoveHouseHotelFromTableToFullSet(
                emptyHouseSetsIndexes, emptyHotelSetsIndexes, 
                fullSetsWithoutHouseIndexes, fullSetsWithoutHotelsIndexes,
                emptyIndex, setIndex);

            auto emptySet = currentPlayer.GetCardSets()[emptyIndex]; 
            CardContainerElem card;
            if (emptySet.IsHasHouse())
            {
                card = emptySet.RemoveCard(emptySet.GetCards().size());
                currentPlayer.AddHouseToCardSet(setIndex, card);
            }
            else
            {
                card = emptySet.RemoveCard(emptySet.GetCards().size() + 1);
                currentPlayer.AddHotelToCardSet(setIndex, card);
            }
            currentPlayer.RemoveSet(emptyIndex);
        }
        else
        {
            return ETurnOutput::IncorrectInput;
        }
        return ETurnOutput::CardProcessed;
    }

    void Game::FindEnhancementsAndFullSetsWithout(const Monopoly::Player& currentPlayer, std::vector<int>& emptyHouseSetsIndexes, std::vector<int>& emptyHotelSetsIndexes, std::vector<int>& fullSetsWithoutHouseIndexes, std::vector<int>& fullSetsWithoutHotelsIndexes) const
    {
        for (int i = 0; i < currentPlayer.GetCardSets().size(); ++i)
        {
            const auto& set = currentPlayer.GetCardSets()[i];
            if (set.GetCards().empty())
            {
                if (set.IsHasHouse())
                    emptyHouseSetsIndexes.emplace_back(i);
                else if (set.IsHasHotel())
                    emptyHotelSetsIndexes.emplace_back(i);
            }
            else if (set.IsFull())
            {
                if (!set.IsHasHouse())
                    fullSetsWithoutHouseIndexes.emplace_back(i);
                else if (!set.IsHasHotel())
                    fullSetsWithoutHotelsIndexes.emplace_back(i);
            }
        }
    }

    bool Game::IsDraw()
    {
        if (m_Deck.empty() && !m_Discard.empty())
        {
            std::shuffle(m_Discard.begin(), m_Discard.end(), std::default_random_engine(1337));
            for (const auto& card : m_Discard)
                m_Deck.emplace_back(card);
            m_Discard.clear();
            return false;
        }

        for(const auto& p : m_Players)
            if(p.GetCardsInHand().size() != 0)
                return false;

        return true;
    }

    Game::ETurnOutput Game::ProcessActionCard(Player& currentPlayer, const CardContainerElem& card)
    {
        switch (card->GetActionType())
        {
        case EActionType::PassGo:
        {
            m_Discard.emplace_back(card);
            return PassGo(currentPlayer);
        }
        case EActionType::Hotel:
            return Enhancement(currentPlayer, card);
        case EActionType::House:
            return Enhancement(currentPlayer, card);
        case EActionType::DealBreaker:
            return DealBreaker(currentPlayer, card);
        case EActionType::SlyDeal:
            return SlyDeal(currentPlayer, card);
        case EActionType::ForcedDeal:
            return ForcedDeal(currentPlayer, card);
        case EActionType::ItsMyBirthday:
            return ItsMyBirthday(currentPlayer, card);
        case EActionType::DebtCollector:
            return DebtCollector(currentPlayer, card);
        case EActionType::RentWild:
            return RentWild(currentPlayer, card);

        case EActionType::RentLightBlueBrown:
        case EActionType::RentOrangePink:
        case EActionType::RentYellowRed:
        case EActionType::RentUtilityRailroad:
        case EActionType::RentBlueGreen:
            return RentTwoColors(currentPlayer, card);

        case EActionType::None:
        case EActionType::DoubleTheRent:
        case EActionType::JustSayNo:
            return ETurnOutput::IncorrectCard;
        default:
            return ETurnOutput::IncorrectCard;
        }
    }

    int Game::GetExtraCardsCount() const
    {
        const auto extraCardsCount = static_cast<int>(m_Players[m_CurrentPlayerIndex].GetCountCardsInHand() - c_MaxCardsCountInTurnEnd);
        return extraCardsCount > 0 ? extraCardsCount : 0;
    }

    void Game::RemoveExtraCards(const std::vector<int>& extraCardsIndices)
    {
        const auto extra = m_Players[m_CurrentPlayerIndex].RemoveCardsFromHand(extraCardsIndices);
        for (auto& e : extra)
        {
            m_Deck.push_front(e);
        }
    }

    void Game::EndTurn()
    {
        m_CurrentPlayerIndex = (m_CurrentPlayerIndex + 1) % m_Players.size();
        m_CurrentPlayerTurnCounter = c_TurnCardsCount;
        m_CurrentPlayerTurnCounterForAllMoves = m_CurrentPlayerTurnCounter * 2;
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

    Game::ETurnOutput Game::PassGo(Player& player)
    {
        PlayerTakeCardsFromDeck(player, c_PassGoCardsCount);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::Enhancement(Player& player, const CardContainerElem& card)
    {
        std::vector<int> setsIndices;
        const auto& sets = player.GetCardSets();
        for (int i = 0; i < sets.size(); ++i)
        {
            const auto color = sets[i].GetColor();
            const auto isHasEnhancement = card->GetActionType() == EActionType::House ? sets[i].IsHasHouse() : sets[i].IsHasHotel();
            if (color != EColor::Railroad && color != EColor::Utility && isHasEnhancement == false && sets[i].IsFull())
            {
                setsIndices.emplace_back(i);
            }
        }

        if (setsIndices.empty())
        {
            return ETurnOutput::IncorrectCard;
        }

        const auto index = m_Controllers[m_CurrentPlayerIndex]->SelectSetIndex(setsIndices);
        if (std::find(setsIndices.begin(), setsIndices.end(), index) == setsIndices.end())
        {
            return ETurnOutput::IncorrectIndex;
        }

        if(player.AddHouseToCardSet(index, card) || player.AddHotelToCardSet(index, card))
            return ETurnOutput::CardProcessed;
        return ETurnOutput::IncorrectCard;
    }

    Game::ETurnOutput Game::DealBreaker(Player& player, const CardContainerElem& card)
    {
        int victimIndex, setIndex;
        m_Controllers[m_CurrentPlayerIndex]->InputDealBreaker(victimIndex, setIndex); 

        if (!m_Players[victimIndex].GetCardSets()[setIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            player.AddSet(m_Players[victimIndex].RemoveSet(setIndex));
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::SlyDeal(Player& player, const CardContainerElem& card)
    {
        int victimIndex, setIndex, propertyIndexInSet;
        m_Controllers[m_CurrentPlayerIndex]->InputSlyDeal(victimIndex, setIndex, propertyIndexInSet); 

        if (m_Players[victimIndex].GetCardSets()[setIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            player.AddProperty(m_Players[victimIndex].RemoveCardFromSet(setIndex, propertyIndexInSet));
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::ForcedDeal(Player& player, const CardContainerElem& card)
    {
        int victimIndex, victimSetIndex, victimPropertyIndexInSet, playerSetIndex, playerPropertyIndexInSet;
        if(player.GetNotFullSetsCount() == 0)
            return ETurnOutput::IncorrectCard;
        m_Controllers[m_CurrentPlayerIndex]->InputForcedDeal(victimIndex, victimSetIndex, 
            victimPropertyIndexInSet, playerSetIndex, playerPropertyIndexInSet);

        if (m_Players[victimIndex].GetCardSets()[victimSetIndex].IsFull() 
            || m_Players[m_CurrentPlayerIndex].GetCardSets()[playerSetIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            auto victimCard = m_Players[victimIndex].RemoveCardFromSet(victimSetIndex, victimPropertyIndexInSet);
            auto instigatorCard = m_Players[m_CurrentPlayerIndex].RemoveCardFromSet(playerSetIndex, playerPropertyIndexInSet);
            player.AddProperty(victimCard);
            m_Players[victimIndex].AddProperty(instigatorCard);
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::ItsMyBirthday(Player& player, const CardContainerElem& card)
    {
        for (int i = 0; i < m_Players.size(); ++i)
        {
            if (i != m_CurrentPlayerIndex && !JustSayNo(i, m_CurrentPlayerIndex))
            {
                Pay(i, c_ItsMyBirthdayAmount);
            }
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::DebtCollector(Player& player, const CardContainerElem& card)
    {
        int victimIndex;
        m_Controllers[m_CurrentPlayerIndex]->InputDebtCollector(victimIndex);

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            Pay(victimIndex, c_DebtCollectorAmount);
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::RentWild(Player& player, const CardContainerElem& card)
    {
        int victimIndex, setIndex;
        m_Controllers[m_CurrentPlayerIndex]->InputRentWild(victimIndex, setIndex);

        auto payValue = player.GetCardSets()[setIndex].GetPayValue();
        auto howManyDoubleTheRent = DoubleTheRent(player, payValue);
        JustSayNoDoubleTheRent(howManyDoubleTheRent, victimIndex, payValue);
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::RentTwoColors(Player& player, const CardContainerElem& card)
    {
        int setIndex;
        m_Controllers[m_CurrentPlayerIndex]->InputRentTwoColors(setIndex);

        auto payValue = player.GetCardSets()[setIndex].GetPayValue();
        const auto howManyCardsToUse = DoubleTheRent(player, payValue);
        for (int i = 0; i < m_Players.size(); ++i)
        {
            if (i != m_CurrentPlayerIndex && !JustSayNo(i, m_CurrentPlayerIndex))
            {
                int pv = payValue;
                JustSayNoDoubleTheRent(howManyCardsToUse, i, pv);
                Pay(i, pv);
            }
        }
        m_Discard.emplace_back(card);
        return ETurnOutput::CardProcessed;
    }

    bool Game::JustSayNo(const int victimIndex, const int instigatorIndex)
    {
        const auto i = m_Players[victimIndex].GetIndexJustSayNo();
        if (i >= 0 && m_Controllers[victimIndex]->InputUseJustSayNo(victimIndex))
        {
            const auto card = m_Players[victimIndex].RemoveCardFromHand(i);
            m_Discard.emplace_back(card);
            return !JustSayNo(instigatorIndex, victimIndex);
        }
        return false;
    }

    int Game::DoubleTheRent(Player& player, int& payValue)
    {
        if (m_CurrentPlayerTurnCounter <= 1) return 0;

        int doubleTheRentCount = GetDoubleTheRentCountMayUse(player);
        int howManyCardsToUse = 0;
        if (doubleTheRentCount > 0)
        {
            m_Controllers[m_CurrentPlayerIndex]->InputDoubleTheRent(doubleTheRentCount, howManyCardsToUse);
            m_CurrentPlayerTurnCounter -= howManyCardsToUse;
            for (int i = 0; i < howManyCardsToUse; ++i)
            {
                payValue *= 2;
                for (int j = 0; j < player.GetCardsInHand().size(); ++j)
                {
                    if (player.GetCardsInHand()[j]->GetActionType() == EActionType::DoubleTheRent)
                    {
                        const auto card = player.RemoveCardFromHand(j);
                        m_Discard.emplace_back(card);
                    }
                }
            }
        }
        return howManyCardsToUse;
    }

    int Game::GetDoubleTheRentCountMayUse(const Player& player) const
    {
        int res = 0;
        for (const auto& e : player.GetCardsInHand())
        {
            if (e->GetActionType() == EActionType::DoubleTheRent)
            {
                ++res;
            }
        }
        return std::min(m_CurrentPlayerTurnCounter - 1, res);
    }

    void Game::JustSayNoDoubleTheRent(const int howManyCardsToUse, const int victimIndex, int& payValue)
    {
        for (int i = 0; i < howManyCardsToUse; ++i)
        {
            if (JustSayNo(victimIndex, m_CurrentPlayerIndex))
            {
                payValue /= 2;
            }
        }
    }
    
    void Game::Pay(int victimIndex, int amount)
    {
        const int allMoney = m_Players[victimIndex].CountBankAndPropertiesValues();
        if (allMoney < amount)
        {
            const auto& bank = m_Players[victimIndex].GetCardsInBank();
            for (const auto& card : bank)
            {
                m_Players[m_CurrentPlayerIndex].AddCardToBank(card);
            }
            m_Players[victimIndex].RemoveAllFromBank();

            const auto& sets = m_Players[victimIndex].GetCardSets();
            for (const auto& set : sets)
            {
                const auto& cards = set.GetCards();
                for (const auto& card : cards)
                {
                    m_Players[m_CurrentPlayerIndex].AddProperty(card);
                }
                if (set.IsHasHouse())
                    m_Players[m_CurrentPlayerIndex].AddProperty(set.GetHouse());
                if (set.IsHasHotel())
                    m_Players[m_CurrentPlayerIndex].AddProperty(set.GetHotel());
            }
            m_Players[victimIndex].RemoveSets();
        }
        else
        {
            std::vector<int> moneyIndices;
            std::unordered_map<int, std::vector<int>> setIndices;

            m_Controllers[victimIndex]->InputPay(victimIndex, amount, moneyIndices, setIndices);
            {
                auto money = m_Players[victimIndex].RemoveCardsFromBank(moneyIndices);
                m_Players[m_CurrentPlayerIndex].AddCardsToBank(std::move(money));
            }
            for (const auto& [key, value] : setIndices)
            {
                auto properties = m_Players[victimIndex].RemoveCardsWithValueNotZeroFromSet(key, value);
                for (const auto& card : properties)
                {
                    m_Players[m_CurrentPlayerIndex].AddProperty(card);
                }
            }
            m_Players[victimIndex].RemoveHousesHotelsFromIncompleteSets();
        }
    }

    void Game::Save(const std::string& fileName) const
    {
        json save;

        save[c_JSON_CurrentPlayerIndex] = m_CurrentPlayerIndex;
        save[c_JSON_CurrentPlayerTurnCounter] = m_CurrentPlayerTurnCounter;

        auto CardToJson = [](const CardContainerElem& card)
        {
            auto GetStrByColor = [](const EColor& color)
            {
                for (auto it = c_ColorStrToEnum.begin(); it != c_ColorStrToEnum.end(); ++it)
                {
                    if (it->second == color)
                    {
                        return it->first;
                    }
                }
                assert("Wrong color!");
                return std::string_view("");
            };
            json result;
            result[c_JSON_Card_Name] = card->GetName();
            result[c_JSON_Card_ShortData] = card->GetShortData();
            result[c_JSON_Card_Value] = card->GetValue();
            if (card->GetType() == ECardType::Money)
            {
                result[c_JSON_Card_Type] = c_JSON_Card_Type_Money;
            }
            else if (card->GetType() == ECardType::Action)
            {
                result[c_JSON_Card_Type] = c_JSON_Card_Type_Action;
                result[c_JSON_Card_Action] = c_ActionTypeEnumToStr.at(card->GetActionType());
            }
            else if (card->GetType() == ECardType::Property)
            {
                result[c_JSON_Card_Type] = c_JSON_Card_Type_Property;
                result[c_JSON_Card_Color] = GetStrByColor(card->GetCurrentColor());
                result[c_JSON_Card_SecondColor] = GetStrByColor(card->GetColor2());
            }
            else
            {
                assert("ECardType is None!");
            }
            return result;
        };

        {
            json deck;
            for (const auto& e : m_Deck)
                deck += CardToJson(e);
            save[c_JSON_Deck] = deck;
        }
        {
            json draw;
            for (const auto& e : m_Discard)
                draw += CardToJson(e);
            save[c_JSON_Draw] = draw;
        }

        json players;
        for (const auto& e : m_Players)
        {
            json playerData;
            auto addPlayerCards = [&](const CardContainer& cards, const char* jsonKey)
            {
                json cardsData;
                for (const auto& card : cards)
                    cardsData += CardToJson(card);
                playerData[jsonKey] = cardsData;
            };

            addPlayerCards(e.GetCardsInHand(), c_JSON_Hand);
            addPlayerCards(e.GetCardsInBank(), c_JSON_Bank);
            {
                auto& sets = e.GetCardSets();
                json setsData;
                for (int j = 0; j < sets.size(); ++j)
                {
                    json setCards;
                    for (const auto& card : sets[j].GetCards())
                        setCards += CardToJson(card);

                    json setjson;
                    setjson[c_JSON_SetCards] = setCards;
                    setjson[c_JSON_SetHouse] = sets[j].IsHasHouse() ? CardToJson(sets[j].GetHouse()) : nullptr;
                    setjson[c_JSON_SetHotel] = sets[j].IsHasHotel() ? CardToJson(sets[j].GetHotel()) : nullptr;
                    setjson[c_JSON_SetColor] = static_cast<int>(sets[j].GetColor());
                    setsData += setjson;
                }
                playerData[c_JSON_Sets] = setsData;
            }
            players += playerData;
        }
        save[c_JSON_Players] = players;
        std::ofstream file(fileName);
        file << save;
    }

    CardContainerElem MakeCard(const json& card)
    {
        std::string type = card[c_JSON_Card_Type];
        std::string name = card[c_JSON_Card_Name];
        name.shrink_to_fit();
        std::string shortData = card[c_JSON_Card_ShortData];
        shortData.shrink_to_fit();
        int value = card[c_JSON_Card_Value];
        if (type == c_JSON_Card_Type_Property)
        {
            std::string_view col = card[c_JSON_Card_Color];
            auto color1 = c_ColorStrToEnum.at(col);
            col = card[c_JSON_Card_SecondColor];
            auto color2 = c_ColorStrToEnum.at(col);
            return std::make_shared<PropertyCard>(name, shortData,
                value, color1, color2);
        }
        else if (type == c_JSON_Card_Type_Action)
        {
            std::string_view actionType = card[c_JSON_Card_Action];
            if (actionType == "PassGo")
                return  std::make_shared<PassGoCard>(name, shortData, value);
            else if (actionType == "DoubleTheRent")
                return std::make_shared<DoubleTheRentCard>(name, shortData, value);
            else if (actionType == "JustSayNo")
                return std::make_shared<JustSayNoCard>(name, shortData, value);
            else if (actionType == "Hotel")
                return std::make_shared<HotelCard>(name, shortData, value);
            else if (actionType == "House")
                return std::make_shared<HouseCard>(name, shortData, value);
            else if (actionType == "DealBreaker")
                return std::make_shared<DealBreakerCard>(name, shortData, value);
            else if (actionType == "SlyDeal")
                return std::make_shared<SlyDealCard>(name, shortData, value);
            else if (actionType == "ForcedDeal")
                return std::make_shared<ForcedDealCard>(name, shortData, value);
            else if (actionType == "ItsMyBirthday")
                return std::make_shared<ItsMyBirthdayCard>(name, shortData, value);
            else if (actionType == "DebtCollector")
                return std::make_shared<DebtCollectorCard>(name, shortData, value);
            else if (actionType == "RentWild")
                return std::make_shared<RentWildCard>(name, shortData, value);
            else if (actionType == "RentLightBlueBrown")
                return std::make_shared<RentLightBlueBrown>(name, shortData, value);
            else if (actionType == "RentOrangePink")
                return std::make_shared<RentOrangePink>(name, shortData, value);
            else if (actionType == "RentYellowRed")
                return std::make_shared<RentYellowRed>(name, shortData, value);
            else if (actionType == "RentUtilityRailroad")
                return std::make_shared<RentUtilityRailroad>(name, shortData, value);
            else if (actionType == "RentBlueGreen")
                return std::make_shared<RentBlueGreen>(name, shortData, value);
        }
        else
        {
            return std::make_shared<MoneyCard>(name, shortData, value);
        }
    }

    void Game::Load(const std::string& fileName)
    {
        std::ifstream ifs(fileName);
        if (ifs.is_open() == false)
        {
            std::cerr << "Can't load save!\n";
            exit(5);
        }
        const auto save = json::parse(ifs);
        
        m_CurrentPlayerIndex = save[c_JSON_CurrentPlayerIndex];
        m_CurrentPlayerTurnCounter = save[c_JSON_CurrentPlayerTurnCounter];

        if (const auto& deck = save[c_JSON_Deck]; deck.is_null() == false)
            for (const auto& card : deck)
                m_Deck.push_back(MakeCard(card));

        if (const auto& draw = save[c_JSON_Draw]; draw.is_null() == false)
            for (const auto& card : draw)
                m_Discard.push_back(MakeCard(card));

        const auto players = save[c_JSON_Players];
        for (const auto& player : players)
        {
            Player p;
            if (const auto& bank = player[c_JSON_Bank]; bank.is_null() == false)
                for (const auto& card : bank)
                    p.AddCardToBank(MakeCard(card));

            if (const auto& hand = player[c_JSON_Hand]; hand.is_null() == false)
                for (const auto& card : hand)
                    p.AddCardToHand(MakeCard(card));

            if (const auto& sets = player[c_JSON_Sets]; sets.is_null() == false)
            {
                for (const auto& set : sets)
                {
                    CardContainer cards;
                    CardContainerElem house = nullptr;
                    CardContainerElem hotel = nullptr;

                    if (auto cardsJson = set[c_JSON_SetCards]; cardsJson.is_null() == false)
                        for (const auto& card : cardsJson)
                            cards.push_back(MakeCard(card));

                    if (auto houseJson = set[c_JSON_SetHouse]; houseJson.is_null() == false)
                        house = MakeCard(houseJson);
                    if (auto hotelJson = set[c_JSON_SetHotel]; hotelJson.is_null() == false)
                        hotel = MakeCard(hotelJson);

                    EColor color = set[c_JSON_SetColor];
                    p.AddSet(CardSet(std::move(cards), std::move(house), std::move(hotel), color));
                }
            }
            m_Players.push_back(std::move(p));
        }
    }

}
