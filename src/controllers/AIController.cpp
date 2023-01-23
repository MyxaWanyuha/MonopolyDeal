#include "Monopoly_pch.h"

#include "AIController.h"
#include "JsonConstants.h"
extern bool s_ShowGameOutput;
namespace Monopoly
{

    void AIController::SelectMove() const
    {
        const auto validMoves = GetAllValidPlayerTurns(m_Index);
        const auto index = rand() % validMoves.size();
        m_Move = validMoves[index];
    }

    void AIController::InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container)
    {
        for (int i = 0; i < extraCardsCount; ++i)
        {
            int index = 0;
            do {
                index = rand() % IControllerIO::m_Game.GetPlayers()[m_Index].GetCardsInHand().size();
            } while (container.end() != std::find(container.begin(), container.end(), index));
            container.emplace_back(index);
        }
    }

    void AIController::InputMove(ETurn& turn, int& cardIndex, int& setIndexForFlip) const
    {
        SelectMove();

        if(s_ShowGameOutput) 
            std::cout << "Player " << m_Index << " did " << m_Move << "\n\n";
        turn = m_Move[Monopoly::c_JSON_Command];
        cardIndex = m_Move.contains(Monopoly::c_JSON_CardIndex) ? m_Move[Monopoly::c_JSON_CardIndex] : -1;
        setIndexForFlip = m_Move.contains(Monopoly::c_JSON_PlayerSetIndex) ? m_Move[Monopoly::c_JSON_PlayerSetIndex] : -1;
    }

    IControllerIO::EActionInput AIController::GetActionInput() const
    {
        return m_Move[Monopoly::c_JSON_ActionCommand];
    }

    int AIController::SelectSetIndex(const std::vector<int>& indices) const
    {
        return m_Move[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputDealBreaker(int& victimIndex, int& setIndex) const
    {
        victimIndex = m_Move[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Move[Monopoly::c_JSON_VictimSetIndex];
    }

    void AIController::InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet)
    {
        victimIndex = m_Move[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Move[Monopoly::c_JSON_VictimSetIndex];
        propertyIndexInSet = m_Move[Monopoly::c_JSON_VictimPropertyIndexInSet];
    }

    void AIController::InputForcedDeal(int& victimIndex, int& victimSetIndex, 
        int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet)
    {
        victimIndex = m_Move[Monopoly::c_JSON_VictimIndex];
        victimSetIndex = m_Move[Monopoly::c_JSON_VictimSetIndex];
        victimPropertyIndexInSet = m_Move[Monopoly::c_JSON_VictimPropertyIndexInSet];
        playerSetIndex = m_Move[Monopoly::c_JSON_PlayerSetIndex];
        playerPropertyIndexInSet = m_Move[Monopoly::c_JSON_PlayerPropertyIndexInSet];
    }

    void AIController::InputDebtCollector(int& victimIndex)
    {
         victimIndex = m_Move[Monopoly::c_JSON_VictimIndex];
    }

    void AIController::InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices,
        std::unordered_map<int, std::vector<int>>& setIndices)
    {
        int payAmount = 0;
        int properties = 0;
        const auto& player = m_Game.GetPlayers()[m_Index];
        const auto& bank = player.GetCardsInBank();
        std::vector<int> availableMoneyIndices;
        for (int i = 0; i < bank.size(); ++i)
            availableMoneyIndices.emplace_back(i);
        while (payAmount < amount)
        {
            const bool useMoneyToPay = !availableMoneyIndices.empty() ? rand() % 2 : false;
            if (useMoneyToPay)
            {
                const auto moneyIndex = rand() % availableMoneyIndices.size();
                moneyIndices.emplace_back(availableMoneyIndices[moneyIndex]);
                availableMoneyIndices.erase(availableMoneyIndices.begin() + moneyIndex);
                payAmount += bank[moneyIndices.back()]->GetValue();
            }
            else
            {
                if (player.GetCardSets().size() == 0)
                    continue;
                const auto setIndex = rand() % player.GetCardSets().size();
                const auto& set = player.GetCardSets()[setIndex];
                if (set.GetCards().empty())
                {
                    if (set.IsHasHouse())
                    {
                        payAmount += set.GetHouse()->GetValue();
                        setIndices[setIndex].emplace_back(0);
                    }
                    else
                    {
                        payAmount += set.GetHotel()->GetValue();
                        setIndices[setIndex].emplace_back(1);
                    }
                }
                else
                {
                    int addIndex = 0;
                    if (set.IsHasHouse()) // index of house is set.size()
                        ++addIndex;
                    if (set.IsHasHotel()) // index of hotel is set.size() + 1
                        ++addIndex;
                    const auto propertyIndex = rand() % (set.GetCards().size() + addIndex);
                    if (propertyIndex < set.GetCards().size())
                    {
                        if (set.GetCards()[propertyIndex]->GetValue() == 0)// is wild card
                            continue;
                        payAmount += set.GetCards()[propertyIndex]->GetValue();
                    }
                    else if (propertyIndex == set.GetCards().size())
                    {
                        payAmount += set.GetHouse()->GetValue();
                    }
                    else if (propertyIndex == (set.GetCards().size() + 1))
                    {
                        payAmount += set.GetHotel()->GetValue();
                    }
                    if(std::find(setIndices[setIndex].begin(), 
                        setIndices[setIndex].end(), propertyIndex) == setIndices[setIndex].end())
                        setIndices[setIndex].emplace_back(propertyIndex);
                }
            }
        }
    }

    void AIController::InputRentWild(int& victimIndex, int& setIndex)
    {
        victimIndex = m_Move[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Move[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputRentTwoColors(int& setIndex)
    {
        setIndex = m_Move[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes,
        const std::vector<int>& emptyHotelSetsIndexes, const std::vector<int>& fullSetsWithoutHouseIndexes,
        const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex)
    {
        emptyIndex = m_Move[Monopoly::c_JSON_EmptySetIndex];
        setIndex = m_Move[Monopoly::c_JSON_PlayerSetIndex];
    }

    bool AIController::InputUseJustSayNo(const int victimIndex) const
    {
        return rand() % 2;
    }

    void AIController::InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const
    {
        howManyCardsToUse = m_Move[Monopoly::c_JSON_DoubleTheRentUseCount];
    }

    json AIController::GetAllValidPlayerTurns(int index) const
    {
        json turns;
        {
            json pass;
            pass[Monopoly::c_JSON_Command] = ETurn::Pass;
            pass[m_TurnValueStr] = 0;
            turns += pass;
        }
        const auto& player = m_Game.GetPlayers()[index];

        for (int i = 0; i < player.GetCardsInHand().size(); ++i)
        {
            const auto& card = player.GetCardsInHand()[i];
            if (card->GetType() == Monopoly::ECardType::Money)
            {
                turns += { {Monopoly::c_JSON_Command, ETurn::PlayCard}, { Monopoly::c_JSON_CardIndex, i }, { m_TurnValueStr, card->GetValue() }};
            }
            else if (card->GetType() == Monopoly::ECardType::Property)
            {
                auto value = card->GetValue();
                for (const auto& set : player.GetCardSets())
                {
                    if (set.GetColor() == card->GetCurrentColor() && !set.IsFull())
                    {
                        value += set.GetPayValue();
                    }
                }
                turns += { {Monopoly::c_JSON_Command, ETurn::PlayCard}, { Monopoly::c_JSON_CardIndex, i }, { m_TurnValueStr, value }};
            }
            else if (card->GetType() == Monopoly::ECardType::Action)
            {
                auto extra = ExtraActionInformation(card, player);
                json eCopy;
                eCopy[Monopoly::c_JSON_Command] = ETurn::PlayCard;
                eCopy[Monopoly::c_JSON_CardIndex] = i;
                eCopy[Monopoly::c_JSON_ActionCommand] = EActionInput::ToBank;
                eCopy[m_TurnValueStr] = card->GetValue();
                turns += eCopy;

                for (auto& e : extra)
                {
                    if (e.contains(m_TurnValueStr))
                        eCopy[m_TurnValueStr] = e[m_TurnValueStr];
                    eCopy[Monopoly::c_JSON_ActionCommand] = EActionInput::Play;
                    eCopy.merge_patch(e);
                    turns += eCopy;
                }
            }
        }

        for (int i = 0; i < player.GetCardSets().size(); ++i)
        {
            const auto& set = player.GetCardSets()[i];
            for (int j = 0; j < set.GetCards().size(); ++j)
            {
                const auto& card = set.GetCards()[j];
                if (card->GetColor2() != Monopoly::EColor::None)
                {
                    json flip;
                    flip[Monopoly::c_JSON_Command] = ETurn::FlipCard;
                    flip[Monopoly::c_JSON_CardIndex] = j;
                    flip[Monopoly::c_JSON_PlayerSetIndex] = i;
                    flip[m_TurnValueStr] = set.IsFull() ? -set.GetPayValue() : card->GetValue();
                    turns += flip;
                }
            }
        }

        std::vector<int> emptyHouseSetsIndexes;
        std::vector<int> emptyHotelSetsIndexes;
        std::vector<int> fullSetsWithoutHouseIndexes;
        std::vector<int> fullSetsWithoutHotelsIndexes;
        m_Game.FindEnhancementsAndFullSetsWithout(player,
            emptyHouseSetsIndexes,
            emptyHotelSetsIndexes,
            fullSetsWithoutHouseIndexes,
            fullSetsWithoutHotelsIndexes);
        ValidMoveEnhancementToFullSet(emptyHouseSetsIndexes, fullSetsWithoutHouseIndexes, turns);
        ValidMoveEnhancementToFullSet(emptyHotelSetsIndexes, fullSetsWithoutHotelsIndexes, turns);
        return turns;
    }

    void AIController::ValidMoveEnhancementToFullSet(std::vector<int>& emptyIndexes, std::vector<int>& fullSetsIndexes, json& turns) const
    {
        if (!emptyIndexes.empty() && !fullSetsIndexes.empty())
        {
            for (int i = 0; i < emptyIndexes.size(); ++i)
            {
                for (int j = 0; j < fullSetsIndexes.size(); ++j)
                {
                    json hh;
                    hh[Monopoly::c_JSON_Command] = ETurn::HouseHotelOnTable;
                    hh[Monopoly::c_JSON_EmptySetIndex] = emptyIndexes[i];
                    hh[Monopoly::c_JSON_PlayerSetIndex] = fullSetsIndexes[j];
                    hh[m_TurnValueStr] = m_Game.GetPlayers()[m_Index].GetCardSets()[j].GetPayValue();
                    turns += hh;
                }
            }
        }
    }

    json AIController::ExtraActionInformation(const Monopoly::CardContainerElem& card, const Monopoly::Player& player) const
    {
        json res;
        switch (card->GetActionType())
        {
        case Monopoly::EActionType::Hotel:
        {
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                const auto& set = player.GetCardSets()[i];
                if (set.GetColor() != Monopoly::EColor::Railroad
                    && set.GetColor() != Monopoly::EColor::Utility
                    && set.IsFull() && set.IsHasHouse() && !set.IsHasHotel())
                {
                    res += { {Monopoly::c_JSON_PlayerSetIndex, i}, { m_TurnValueStr, set.GetPayValue() + card->GetValue() } };
                }
            }
        }break;
        case Monopoly::EActionType::House:
        {
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                const auto& set = player.GetCardSets()[i];
                if (set.GetColor() != Monopoly::EColor::Railroad
                    && set.GetColor() != Monopoly::EColor::Utility
                    && set.IsFull() && !set.IsHasHouse())
                {
                    res += { {Monopoly::c_JSON_PlayerSetIndex, i}, { m_TurnValueStr, set.GetPayValue() + card->GetValue() } };
                }
            }
        }break;
        case Monopoly::EActionType::DealBreaker:
        {
            for (int p = 0; p < m_Game.GetPlayersCount(); ++p)
            {
                const auto& victim = m_Game.GetPlayers()[p];
                if (&victim == &player) continue;

                for (int i = 0; i < victim.GetCardSets().size(); ++i)
                {
                    if (victim.GetCardSets()[i].IsFull())
                    {
                        res += { {Monopoly::c_JSON_VictimIndex, p},
                            { Monopoly::c_JSON_VictimSetIndex , i},
                            { m_TurnValueStr, victim.GetCardSets()[i].GetPayValue() }};
                    }
                }
            }
        }break;
        case Monopoly::EActionType::SlyDeal:
        {
            for (int p = 0; p < m_Game.GetPlayersCount(); ++p)
            {
                const auto& victim = m_Game.GetPlayers()[p];
                if (&victim == &player) continue;
                for (int i = 0; i < victim.GetCardSets().size(); ++i)
                {
                    const auto& set = victim.GetCardSets()[i];
                    if (set.IsFull()) continue;
                    for (int j = 0; j < set.GetCards().size(); ++j)
                    {
                        auto max = 0;
                        for (const auto& playerSet : player.GetCardSets())
                        {
                            if (!playerSet.IsFull()
                                && playerSet.GetColor() == victim.GetCardSets()[i].GetColor()
                                && playerSet.GetPayValue() > max)
                            {
                                max = playerSet.GetPayValue();
                            }
                        }
                        res += {
                            { Monopoly::c_JSON_VictimIndex, p },
                            { Monopoly::c_JSON_VictimSetIndex , i },
                            { Monopoly::c_JSON_VictimPropertyIndexInSet, j },
                            { m_TurnValueStr, set.GetCards()[j]->GetValue() + max } };
                    }
                }
            }
        }break;
        case Monopoly::EActionType::ForcedDeal:
        {
            json playerProperties;
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                const auto& set = player.GetCardSets()[i];
                if (set.IsFull()) continue;
                for (int j = 0; j < set.GetCards().size(); ++j)
                {
                    json v;
                    v[Monopoly::c_JSON_PlayerSetIndex] = i;
                    v[Monopoly::c_JSON_PlayerPropertyIndexInSet] = j;
                    v[m_TurnValueStr] = -set.GetCards()[j]->GetValue();
                    playerProperties += v;
                }
            }
            if (playerProperties.empty()) break;

            for (int p = 0; p < m_Game.GetPlayersCount(); ++p)
            {
                const auto& victim = m_Game.GetPlayers()[p];
                if (&victim == &player) continue;

                for (int i = 0; i < victim.GetCardSets().size(); ++i)
                {
                    const auto& set = victim.GetCardSets()[i];
                    if (set.IsFull()) continue;
                    for (int j = 0; j < set.GetCards().size(); ++j)
                    {
                        for (const auto& e : playerProperties)
                        {
                            json turn = e;
                            json v;
                            v[Monopoly::c_JSON_VictimIndex] = p;
                            v[Monopoly::c_JSON_VictimSetIndex] = i;
                            v[Monopoly::c_JSON_VictimPropertyIndexInSet] = j;
                            turn.merge_patch(v);
                            const int value = turn[m_TurnValueStr];
                            
                            //const int setInd = turn[Monopoly::c_JSON_PlayerSetIndex];
                            //const int propInd = turn[Monopoly::c_JSON_PlayerPropertyIndexInSet];
                            
                            turn[m_TurnValueStr] = value + set.GetCards()[j]->GetValue();
                            res += turn;
                        }
                    }
                }
            }
        }break;
        case Monopoly::EActionType::ItsMyBirthday:
        {
            int value = 0;
            for (int p = 0; p < m_Game.GetPlayersCount(); ++p)
            {
                const auto& victim = m_Game.GetPlayers()[p];
                if (&victim == &player) continue;
                const auto victimValue = victim.CountBankAndPropertiesValues();
                value += victimValue >= 2 ? 2 : victimValue;
            }
            res += { {m_TurnValueStr, value} };
        }break;
        case Monopoly::EActionType::DebtCollector:
        {
            for (int i = 0; i < m_Game.GetPlayersCount(); ++i)
            {
                const auto& victim = m_Game.GetPlayers()[i];
                if (&victim == &player) continue;
                json v;
                v[Monopoly::c_JSON_VictimIndex] = i;
                const auto value = victim.CountBankAndPropertiesValues();
                v[m_TurnValueStr] = value >= 5 ? 5 : value;
                res += v;
            }
        }break;
        case Monopoly::EActionType::RentWild:
        {
            if (player.GetCardSets().empty()) break;
            const json dtr = GetDoubleTheRentMayUse(player);
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                const auto payValue = player.GetCardSets()[i].GetPayValue();
                for (int j = 0; j < m_Game.GetPlayersCount(); ++j)
                {
                    if (&m_Game.GetPlayers()[j] == &player) continue;
                    json inf;
                    inf[Monopoly::c_JSON_PlayerSetIndex] = i;
                    inf[Monopoly::c_JSON_VictimIndex] = j;

                    const auto victimValue = m_Game.GetPlayers()[j].CountBankAndPropertiesValues();
                    if (dtr.empty())
                    {
                        inf[m_TurnValueStr] = std::min(victimValue, payValue);
                        res += inf;
                    }
                    else
                    {
                        for (const auto& d : dtr)
                        {
                            json victimCopyDTR = inf;
                            const int useCount = d[Monopoly::c_JSON_DoubleTheRentUseCount];
                            const int valueToPay = payValue * std::max(1, useCount);
                            victimCopyDTR[m_TurnValueStr] = std::min(victimValue, valueToPay);

                            victimCopyDTR.merge_patch(d);
                            res += victimCopyDTR;
                        }
                    }
                }
            }
        }break;
        case Monopoly::EActionType::RentLightBlueBrown:
        {
            RentTwoColorAction(player, Monopoly::EColor::LightBlue, Monopoly::EColor::Brown, res);
        }break;
        case Monopoly::EActionType::RentOrangePink:
        {
            RentTwoColorAction(player, Monopoly::EColor::Orange, Monopoly::EColor::Pink, res);
        }break;
        case Monopoly::EActionType::RentYellowRed:
        {
            RentTwoColorAction(player, Monopoly::EColor::Yellow, Monopoly::EColor::Red, res);
        }break;
        case Monopoly::EActionType::RentUtilityRailroad:
        {
            RentTwoColorAction(player, Monopoly::EColor::Utility, Monopoly::EColor::Railroad, res);
        }break;
        case Monopoly::EActionType::RentBlueGreen:
        {
            RentTwoColorAction(player, Monopoly::EColor::Blue, Monopoly::EColor::Green, res);
        }break;
        }
        return res;
    }

    void AIController::RentTwoColorAction(const Monopoly::Player& player, Monopoly::EColor color1, Monopoly::EColor color2, json& res) const
    {
        if (player.GetCardSets().empty()) return;
        const json dtr = GetDoubleTheRentMayUse(player);
        //json sets;
        for (int i = 0; i < player.GetCardSets().size(); ++i)
        {
            const auto color = player.GetCardSets()[i].GetColor();
            if (color == color1 || color == color2)
            {
                auto payValue = player.GetCardSets()[i].GetPayValue();
                json inf;
                inf[Monopoly::c_JSON_PlayerSetIndex] = i;
                //sets += inf;
                if (dtr.empty())
                {
                    int valueToPay = 0;
                    for (int j = 0; j < m_Game.GetPlayersCount(); ++j)
                    {
                        if (&m_Game.GetPlayers()[j] == &player) continue;
                        const auto victimValue = m_Game.GetPlayers()[j].CountBankAndPropertiesValues();
                        valueToPay += std::min(payValue, victimValue);
                    }
                    inf[m_TurnValueStr] = valueToPay;
                    res += inf;
                }
                else
                {
                    for (const auto& d : dtr)
                    {
                        json infCopy = inf;
                        const int useCount = d[Monopoly::c_JSON_DoubleTheRentUseCount];
                        payValue *= std::max(1, useCount);
                        int valueToPay = 0;
                        for (int j = 0; j < m_Game.GetPlayersCount(); ++j)
                        {
                            if (&m_Game.GetPlayers()[j] == &player) continue;
                            const auto victimValue = m_Game.GetPlayers()[j].CountBankAndPropertiesValues();
                            valueToPay += std::min(payValue, victimValue);
                        }
                        infCopy[m_TurnValueStr] = valueToPay;

                        infCopy.merge_patch(d);
                        res += infCopy;
                    }
                }
            }
        }
    }

    json AIController::GetDoubleTheRentMayUse(const Monopoly::Player& player) const
    {
        int dtrCount = m_Game.GetDoubleTheRentCountMayUse(player);
        json dtr;
        for (int i = 0; i < (dtrCount + 1); ++i)
        {
            // 0 1 2
            json v;
            v[Monopoly::c_JSON_DoubleTheRentUseCount] = i;
            dtr += v;
        }
        return dtr;
    }

}