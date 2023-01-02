#include "Monopoly_pch.h"
#include "CSVRow.h"

namespace Monopoly
{
#if NDEBUG
    uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
#else
    uint32_t seed = 1337322228u;
#endif

    Game::Game()
    {
        m_CurrentPlayerTurnCounter = c_TurnCardsCount;
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
            const auto& cardType = row[1];
            const int count = to_int(row[3]);
            const int value = to_int(row[2]);
            if (cardType == "Action")
            {
                const auto& actionType = row[6];
                if (actionType == "PassGo")
                    InitDeck<PassGoCard>(count, name, value);
                else if (actionType == "DoubleTheRent")
                    InitDeck<DoubleTheRentCard>(count, name, value);
                else if (actionType == "JustSayNo")
                    InitDeck<JustSayNoCard>(count, name, value);
                else if (actionType == "Hotel")
                    InitDeck<HotelCard>(count, name, value);
                else if (actionType == "House")
                    InitDeck<HouseCard>(count, name, value);
                else if (actionType == "DealBreaker")
                    InitDeck<DealBreakerCard>(count, name, value);
                else if (actionType == "SlyDeal")
                    InitDeck<SlyDealCard>(count, name, value);
                else if (actionType == "ForcedDeal")
                    InitDeck<ForcedDealCard>(count, name, value);
                else if (actionType == "ItsMyBirthday")
                    InitDeck<ItsMyBirthdayCard>(count, name, value);
                else if (actionType == "DebtCollector")
                    InitDeck<DebtCollectorCard>(count, name, value);
                else if (actionType == "RentWild")
                    InitDeck<RentWildCard>(count, name, value);
                else if (actionType == "RentLightBlueBrown")
                    InitDeck<RentLightBlueBrown>(count, name, value);
                else if (actionType == "RentOrangePink")
                    InitDeck<RentWildCard>(count, name, value);
                else if (actionType == "RentYellowRed")
                    InitDeck<RentWildCard>(count, name, value);
                else if (actionType == "RentUtilityRailroad")
                    InitDeck<RentWildCard>(count, name, value);
                else if (actionType == "RentBlueGreen")
                    InitDeck<RentWildCard>(count, name, value);
            }
            else if (cardType == "Money")
            {
                InitDeck<MoneyCard>(count, name, value);
            }
            else if (cardType == "Property")
            {
                auto color1 = c_ColorStrToEnum.at(row[4]);
                auto color2 = c_ColorStrToEnum.at(row[5]);
                for (int i = 0; i < count; ++i)
                    m_Deck.push_front(std::make_shared<PropertyCard>(name, value, color1, color2));
            }
        }

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

    int Game::Run()
    {
        while (m_bGameIsNotEnded)
        {
            BeginTurn();
            ETurnOutput turnOutput;
            do {
                ShowPrivatePlayerData(m_CurrentPlayerIndex);
                for (int i = 0; i < GetPlayers().size(); ++i)
                {
                    if (i != GetCurrentPlayerIndex())
                        ShowPublicPlayerData(i);
                }
                ETurn turn = ETurn::Pass;
                int cardIndex = 0, setIndex = 0;
                InputTurn(turn, cardIndex, setIndex);
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

            if (auto extraCardsCount = Game::GetExtraCardsCount(); extraCardsCount > 0)
            {
                std::cout << "Player has extra cards: " << extraCardsCount << '\n'
                    << "Enter the indices of the cards you want to remove: ";
                std::vector<int> container;
                InputIndexesToRemove(extraCardsCount, container);
                assert(container.size() == extraCardsCount);
                RemoveExtraCards(container);
            }
            EndTurn();
        }
        return 0;
    }

    void Game::BeginTurn()
    {
        PlayerTakeCardsFromDeck(m_Players[m_CurrentPlayerIndex], c_PassGoCardsCount);
    }

    Game::ETurnOutput Game::Turn(const ETurn input, const int cardIndex, const int setIndex)
    {
        auto& currentPlayer = m_Players[m_CurrentPlayerIndex];
        switch (input)
        {
        case ETurn::Pass:
            return ETurnOutput::NextPlayer;
        case ETurn::FlipCard:
        {
            auto card = currentPlayer.RemoveCardFromSet(setIndex, cardIndex);
            if (!card || card->SwapColor() != EColor::None)
            {
                currentPlayer.AddProperty(card);
                if (currentPlayer.IsWinner())
                {
                    m_bGameIsNotEnded = false;
                    return ETurnOutput::GameOver;
                }
                return ETurnOutput::CardProcessed;
            }
            return ETurnOutput::IncorrectIndex;
        }
        case ETurn::PlayCard:
        {
            auto card = currentPlayer.RemoveCardFromHand(cardIndex);
            auto type = card->GetType();
            if (type == ECardType::Money)
            {
                currentPlayer.AddCardToBank(card);
            }
            else if (type == ECardType::Property)
            {
                currentPlayer.AddProperty(card);
                if (currentPlayer.IsWinner())
                {
                    m_bGameIsNotEnded = false;
                    return ETurnOutput::GameOver;
                }
            }
            else if (type == ECardType::Action)
            {
                if (GetActionInput() == EActionInput::ToBank)
                {
                    currentPlayer.AddCardToBank(card);
                }
                else
                {
                    auto res = ProcessActionCard(currentPlayer, card);
                    if (res != ETurnOutput::CardProcessed)
                    {
                        return res;
                    }
                }
                if (currentPlayer.IsWinner())
                {
                    m_bGameIsNotEnded = false;
                    return ETurnOutput::GameOver;
                }
                for (const auto& player : m_Players)
                {
                    if (player.IsWinner())
                    {
                        m_bGameIsNotEnded = false;
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
        for (int i = 0; i < currentPlayer.GetCardSets().size(); ++i)
        {
            const auto& set = currentPlayer.GetCardSets()[i];
            if (set.GetCards().size() == 0)
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

        if ((emptyHouseSetsIndexes.size() != 0 && fullSetsWithoutHouseIndexes.size() != 0) || (emptyHotelSetsIndexes.size() != 0 && fullSetsWithoutHotelsIndexes.size() != 0))
        {
            int emptyIndex, setIndex;
            InputMoveHouseHotelFromTableToFullSet(emptyHouseSetsIndexes, emptyHotelSetsIndexes,
                fullSetsWithoutHouseIndexes, fullSetsWithoutHotelsIndexes, emptyIndex, setIndex);

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

    Game::ETurnOutput Game::ProcessActionCard(Player& currentPlayer, CardContainerElem& card)
    {
        switch (card->GetActionType())
        {
        case EActionType::PassGo:
            return PassGo(currentPlayer);
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
            break;
        }
    }

    int Game::GetExtraCardsCount() const
    {
        auto extraCardsCount = (int)m_Players[m_CurrentPlayerIndex].GetCountCardsInHand() - c_MaxCardsCountInTurnEnd;
        return extraCardsCount > 0 ? extraCardsCount : 0;
    }

    void Game::RemoveExtraCards(const std::vector<int>& extraCardsIndices)
    {
        auto extra = m_Players[m_CurrentPlayerIndex].RemoveCardsFromHand(extraCardsIndices);
        for (auto& e : extra)
        {
            m_Deck.push_front(e);
        }
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
            if (color != EColor::Railroad && color != EColor::Utility && isHasEnhancement == false)
            {
                setsIndices.emplace_back(i);
            }
        }

        if (setsIndices.size() == 0)
        {
            return ETurnOutput::IncorrectCard;
        }

        auto index = SelectSetIndex(setsIndices);
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
        InputDealBreaker(victimIndex, setIndex);

        if (!m_Players[victimIndex].GetCardSets()[setIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            player.AddSet(m_Players[victimIndex].RemoveSet(setIndex));
        }
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::SlyDeal(Player& player, const CardContainerElem& card)
    {
        int victimIndex, setIndex, propertyIndexInSet;
        InputSlyDeal(victimIndex, setIndex, propertyIndexInSet);

        if (m_Players[victimIndex].GetCardSets()[setIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            player.AddProperty(m_Players[victimIndex].RemoveCardFromSet(setIndex, propertyIndexInSet));
        }
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::ForcedDeal(Player& player, const CardContainerElem& card)
    {
        int victimIndex, victimSetIndex, victimPropertyIndexInSet, playerSetIndex, playerPropertyIndexInSet;
        if(player.GetNotFullSetsCount() == 0)
            return ETurnOutput::IncorrectCard;
        InputForcedDeal(victimIndex, victimSetIndex, victimPropertyIndexInSet, playerSetIndex, playerPropertyIndexInSet);

        if (m_Players[victimIndex].GetCardSets()[victimIndex].IsFull() || m_Players[victimIndex].GetCardSets()[playerSetIndex].IsFull())
            return ETurnOutput::IncorrectIndex;

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            auto victimCard = m_Players[victimIndex].RemoveCardFromSet(victimSetIndex, victimPropertyIndexInSet);
            auto instigatorCard = m_Players[m_CurrentPlayerIndex].RemoveCardFromSet(playerSetIndex, playerPropertyIndexInSet);
            player.AddProperty(victimCard);
            m_Players[victimIndex].AddProperty(instigatorCard);
        }
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
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::DebtCollector(Player& player, const CardContainerElem& card)
    {
        int victimIndex;
        InputDebtCollector(victimIndex);

        if (!JustSayNo(victimIndex, m_CurrentPlayerIndex))
        {
            Pay(victimIndex, c_DebtCollectorAmount);
        }
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::RentWild(Player& player, const CardContainerElem& card)
    {
        int victimIndex, setIndex;
        InputRentWild(victimIndex, setIndex);
        auto payValue = player.GetCardSets()[setIndex].GetPayValue();
        auto howManyDoubleTheRent = DoubleTheRent(player, payValue);
        JustSayNoDoubleTheRent(howManyDoubleTheRent, victimIndex, payValue);
        return ETurnOutput::CardProcessed;
    }

    Game::ETurnOutput Game::RentTwoColors(Player& player, const CardContainerElem& card)
    {
        int setIndex;
        InputRentTwoColors(setIndex);

        auto payValue = player.GetCardSets()[setIndex].GetPayValue();
        auto howManyCardsToUse = DoubleTheRent(player, payValue);
        for (int i = 0; i < m_Players.size(); ++i)
        {
            if (i != m_CurrentPlayerIndex && !JustSayNo(i, m_CurrentPlayerIndex))
            {
                int pv = payValue;
                JustSayNoDoubleTheRent(howManyCardsToUse, i, pv);
                Pay(i, pv);
            }
        }
        return ETurnOutput::CardProcessed;
    }

    bool Game::JustSayNo(const int victimIndex, const int instigatorIndex)
    {
        auto i = m_Players[victimIndex].GetIndexJustSayNo();
        if (i >= 0 && InputUseJustSayNo(victimIndex))
        {
            m_Players[victimIndex].RemoveCardFromHand(i);
            return !JustSayNo(instigatorIndex, victimIndex);
        }
        return false;
    }

    int Game::DoubleTheRent(Player& player, int& payValue)
    {
        int doubleTheRentCount = 0;
        for (const auto& e : player.GetCardsInHand())
        {
            if (e->GetActionType() == EActionType::DoubleTheRent)
            {
                ++doubleTheRentCount;
            }
        }
        int howManyCardsToUse = 0;
        if (doubleTheRentCount > 0)
        {
            InputDoubleTheRent(doubleTheRentCount, howManyCardsToUse);
            for (int i = 0; i < howManyCardsToUse; ++i)
            {
                payValue *= 2;
                for (int j = 0; j < player.GetCardsInHand().size(); ++j)
                {
                    const auto& e = player.GetCardsInHand().begin();
                    std::advance(e, j);
                    if ((*e)->GetActionType() == EActionType::DoubleTheRent)
                    {
                        player.RemoveCardFromHand(j);
                    }
                }
            }
        }
        return howManyCardsToUse;
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
        int allMoney = m_Players[victimIndex].CountBankAndPropertiesValues();
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
            }
            m_Players[victimIndex].RemoveSets();
        }
        else
        {
            std::vector<int> moneyIndices;
            std::unordered_map<int, std::vector<int>> setIndices;
            InputPay(victimIndex, amount, moneyIndices, setIndices);
            {
                auto money = m_Players[victimIndex].RemoveCardsFromBank(moneyIndices);
                m_Players[m_CurrentPlayerIndex].AddCardsToBank(std::move(money));
            }
            for (const auto& set : setIndices)
            {
                auto properties = m_Players[victimIndex].RemoveCardsWithValueNotZeroFromSet(set.first, set.second);
                for (const auto& card : properties)
                {
                    m_Players[m_CurrentPlayerIndex].AddProperty(card);
                }
            }
            m_Players[victimIndex].RemoveHousesHotelsFromIncompleteSets();
        }
    }

}
