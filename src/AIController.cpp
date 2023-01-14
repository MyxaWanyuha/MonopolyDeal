#include "Monopoly_pch.h"

#include "AIController.h"
#include "IGameInput.h"
#include "JsonConstants.h"

namespace Monopoly
{

    void AIController::SelectTurn() const
    {
        const auto validTurns = GetAllValidPlayerTurns(m_Index);
        const auto index = rand() % validTurns.size();
        m_Turn = validTurns[index];
        //std::cout << "\nAI " << m_Index << " did " << m_Turn << "\n\n";
    }

    void ShowCard(const int index, const Monopoly::CardContainerElem& card)
    {
        std::cout << "\t\t\t" << index << ". " << card->GetShortData() << "\n";
    }

    void AIController::ShowPlayerData() const
    {
        const auto& player = m_Game.GetPlayers()[m_Index];
        std::cout << "\tCurrent player AI(" << m_Index << "): \n";
        std::cout << "\t\tHand count: " << player.GetCardsInHand().size() << "\n";
        for (int i = 0; i < player.GetCardsInHand().size(); ++i)
        {
            ShowCard(i, player.GetCardsInHand()[i]);
        }

        std::cout << "\t\tBank count: " << player.GetCardsInBank().size() << "\n";
        for (int i = 0; i < player.GetCardsInBank().size(); ++i)
        {
            ShowCard(i, player.GetCardsInBank()[i]);
        }
        std::cout << "\t\tSets count: " << player.GetCardSets().size() << "\n";
        for (int j = 0; j < player.GetCardSets().size(); ++j)
        {
            std::cout << "\t\tCard set " << j << "\n";
            const auto& set = player.GetCardSets()[j];
            for (int i = 0; i < set.GetCards().size(); ++i)
            {
                ShowCard(i, set.GetCards()[i]);
            }
            if (set.GetColor() != Monopoly::EColor::Railroad && set.GetColor() != Monopoly::EColor::Utility)
            {
                std::cout << "\t\t\tHouse: " << set.IsHasHouse();
                std::cout << "\t\t\tHotel: " << set.IsHasHotel();
            }
            std::cout << "\n";
        }
    }

    void AIController::ShowPublicPlayerData(const int index) const
    {
        //ShowPlayerData();
    }

    void AIController::ShowPrivatePlayerData(const int index) const
    {
        //ShowPlayerData();
    }

    void AIController::InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container)
    {
        for (int i = 0; i < extraCardsCount; ++i)
        {
            int index = 0;
            do {
                index = rand() % IController::m_Game.GetPlayers()[m_Index].GetCardsInHand().size();
            } while (container.end() != std::find(container.begin(), container.end(), index));
            container.emplace_back(index);
        }
    }

    void AIController::InputTurn(ETurn& turn, int& cardIndex, int& setIndexForFlip) const
    {
        SelectTurn();
        turn = m_Turn[Monopoly::c_JSON_Command];
        cardIndex = m_Turn.contains(Monopoly::c_JSON_CardIndex) ? m_Turn[Monopoly::c_JSON_CardIndex] : -1;
        setIndexForFlip = m_Turn.contains(Monopoly::c_JSON_PlayerSetIndex) ? m_Turn[Monopoly::c_JSON_PlayerSetIndex] : -1;
    }

    IGameInput::EActionInput AIController::GetActionInput() const
    {
        return m_Turn[Monopoly::c_JSON_ActionCommand];
    }

    int AIController::SelectSetIndex(const std::vector<int>& indices) const
    {
        return m_Turn[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputDealBreaker(int& victimIndex, int& setIndex) const
    {
        victimIndex = m_Turn[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Turn[Monopoly::c_JSON_VictimSetIndex];
    }

    void AIController::InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet)
    {
        victimIndex = m_Turn[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Turn[Monopoly::c_JSON_VictimSetIndex];
        propertyIndexInSet = m_Turn[Monopoly::c_JSON_VictimPropertyIndexInSet];
    }

    void AIController::InputForcedDeal(int& victimIndex, int& victimSetIndex, 
        int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet)
    {
        victimIndex = m_Turn[Monopoly::c_JSON_VictimIndex];
        victimSetIndex = m_Turn[Monopoly::c_JSON_VictimSetIndex];
        victimPropertyIndexInSet = m_Turn[Monopoly::c_JSON_VictimPropertyIndexInSet];
        playerSetIndex = m_Turn[Monopoly::c_JSON_PlayerSetIndex];
        playerPropertyIndexInSet = m_Turn[Monopoly::c_JSON_PlayerPropertyIndexInSet];
    }

    void AIController::InputDebtCollector(int& victimIndex)
    {
         victimIndex = m_Turn[Monopoly::c_JSON_VictimIndex];
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
                int addIndex = 0;
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
        victimIndex = m_Turn[Monopoly::c_JSON_VictimIndex];
        setIndex = m_Turn[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputRentTwoColors(int& setIndex)
    {
        setIndex = m_Turn[Monopoly::c_JSON_PlayerSetIndex];
    }

    void AIController::InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes,
        const std::vector<int>& emptyHotelSetsIndexes, const std::vector<int>& fullSetsWithoutHouseIndexes,
        const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex)
    {
        emptyIndex = m_Turn[Monopoly::c_JSON_EmptySetIndex];
        setIndex = m_Turn[Monopoly::c_JSON_PlayerSetIndex];
    }

    bool AIController::InputUseJustSayNo(const int victimIndex) const
    {
        return rand() % 2;
    }

    void AIController::InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const
    {
        howManyCardsToUse = m_Turn[Monopoly::c_JSON_DoubleTheRentUseCount];
    }

    json AIController::GetAllValidPlayerTurns(int index) const
    {
        json turns;
        {
            json pass;
            pass[Monopoly::c_JSON_Command] = ETurn::Pass;
            turns += pass;
        }
        const auto& player = m_Game.GetPlayers()[index];

        for (int i = 0; i < player.GetCardsInHand().size(); ++i)
        {
            const auto& card = player.GetCardsInHand()[i];
            if (card->GetType() == Monopoly::ECardType::Money || card->GetType() == Monopoly::ECardType::Property)
            {
                turns += { {Monopoly::c_JSON_Command, ETurn::PlayCard}, { Monopoly::c_JSON_CardIndex, i } };
            }
            else if (card->GetType() == Monopoly::ECardType::Action)
            {
                auto extra = ExtraActionInformation(card, player);

                json eCopy;
                eCopy[Monopoly::c_JSON_Command] = ETurn::PlayCard;
                eCopy[Monopoly::c_JSON_CardIndex] = i;
                eCopy[Monopoly::c_JSON_ActionCommand] = EActionInput::ToBank;
                turns += eCopy;

                for (auto& e : extra)
                {
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
        case Monopoly::EActionType::PassGo:
        {}break;
        case Monopoly::EActionType::DoubleTheRent:
        {}break;
        case Monopoly::EActionType::JustSayNo:
        {}break;
        case Monopoly::EActionType::Hotel:
        {
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                const auto& set = player.GetCardSets()[i];
                if (set.GetColor() != Monopoly::EColor::Railroad
                    && set.GetColor() != Monopoly::EColor::Utility
                    && set.IsFull() && set.IsHasHouse() && !set.IsHasHotel())
                {
                    json v;
                    v[Monopoly::c_JSON_PlayerSetIndex] = i;
                    res += v;
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
                    json v;
                    v[Monopoly::c_JSON_PlayerSetIndex] = i;
                    res += v;
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
                        json v;
                        v[Monopoly::c_JSON_VictimIndex] = p;
                        v[Monopoly::c_JSON_VictimSetIndex] = i;
                        res += v;
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
                        json v;
                        v[Monopoly::c_JSON_VictimIndex] = p;
                        v[Monopoly::c_JSON_VictimSetIndex] = i;
                        v[Monopoly::c_JSON_VictimPropertyIndexInSet] = j;
                        res += v;
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
                            res += turn;
                        }
                    }
                }
            }
        }break;
        case Monopoly::EActionType::ItsMyBirthday:
        {}break;
        case Monopoly::EActionType::DebtCollector:
        {
            for (int i = 0; i < m_Game.GetPlayersCount(); ++i)
            {
                const auto& victim = m_Game.GetPlayers()[i];
                if (&victim == &player) continue;
                json v;
                v[Monopoly::c_JSON_VictimIndex] = i;
                res += v;
            }
        }break;
        case Monopoly::EActionType::RentWild:
        {
            json dtr = GetDoubleTheRentMayUse(player);
            json sets;
            for (int i = 0; i < player.GetCardSets().size(); ++i)
            {
                json v;
                v[Monopoly::c_JSON_PlayerSetIndex] = i;
                sets += v;
            }
            if (sets.empty()) break;

            for (int i = 0; i < m_Game.GetPlayersCount(); ++i)
            {
                if (&m_Game.GetPlayers()[i] == &player) continue;
                json victim;
                victim[Monopoly::c_JSON_VictimIndex] = i;
                for (const auto& set : sets)
                {
                    json victimSet = victim;
                    victimSet.merge_patch(set);

                    if (dtr.empty())
                    {
                        res += victimSet;
                    }
                    else
                    {
                        for (const auto& d : dtr)
                        {
                            json victimSetDTR = victimSet;
                            victimSetDTR.merge_patch(d);
                            res += victimSetDTR;
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
        json dtr = GetDoubleTheRentMayUse(player);
        json sets;
        for (int i = 0; i < player.GetCardSets().size(); ++i)
        {
            const auto color = player.GetCardSets()[i].GetColor();
            if (color == color1 || color == color2)
            {
                json v;
                v[Monopoly::c_JSON_PlayerSetIndex] = i;
                sets += v;
                if (dtr.empty())
                {
                    res += sets;
                }
                else
                {
                    for (const auto& d : dtr)
                    {
                        json t = v;
                        t.merge_patch(d);
                        res += t;
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