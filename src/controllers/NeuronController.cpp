#include "NeuronController.h"
#include "NeuronController.h"
#include "Monopoly_pch.h"
#include "JsonConstants.h"

namespace Monopoly
{
    std::unordered_map<std::string, double> NeuroController::s_Neurons;

    NeuroController::Move NeuroController::PlayCardMove(const json& currentMove) const
    {
        const auto& player = m_Game.GetPlayers()[m_Index];
        const auto& cartToPlayIndex = currentMove[Monopoly::c_JSON_CardIndex];
        const auto& card = player.GetCardsInHand()[cartToPlayIndex];
        const auto& cardType = card->GetType();
        Move res;
        res.tags.emplace_back(card->GetShortData());
        switch (cardType)
        {
            case ECardType::Money:
            {
                int bankValue = 0;
                for (const auto& money : player.GetCardsInBank())
                    bankValue += money->GetValue();
                res.tags.emplace_back("BV" + std::to_string(bankValue));
            }break;
            case ECardType::Property:
            {
                int sameColorFullCountOnTable = 0;
                int sameColorNotFullCountOnTable = 0;
                for (const auto& set : player.GetCardSets())
                {
                    if (set.GetColor() == card->GetCurrentColor())
                    {
                        if (set.IsFull())
                            ++sameColorFullCountOnTable;
                        else
                            ++sameColorNotFullCountOnTable;
                    }
                }
                res.tags.emplace_back(std::to_string(static_cast<int>(card->GetCurrentColor())) + "CountSameColorFullCountOnTable"
                    + std::to_string(sameColorFullCountOnTable));
                res.tags.emplace_back(std::to_string(static_cast<int>(card->GetCurrentColor())) + "CountSameColorNotFullCountOnTable"
                    + std::to_string(sameColorNotFullCountOnTable));
            }break;
            case ECardType::Action:
            {
                const EActionInput actionInput = currentMove[Monopoly::c_JSON_ActionCommand];
                res.tags.emplace_back("actionInput" + std::to_string(static_cast<int>(actionInput)));
                if (actionInput == EActionInput::ToBank)
                {
                    res.tags.emplace_back("m" + std::to_string(card->GetValue()));
                }
                else
                {
                    res.tags.emplace_back("actionType" + std::to_string(static_cast<int>(card->GetActionType())));
                    switch (card->GetActionType())
                    {
                        case Monopoly::EActionType::DealBreaker:
                        {
                            const auto& victimIndex = currentMove[Monopoly::c_JSON_VictimIndex];
                            const auto& victimSetIndex = currentMove[Monopoly::c_JSON_VictimSetIndex];
                            const auto& victim = m_Game.GetPlayers()[victimIndex];

                            res.tags.emplace_back("VictimFullSetsCount"
                                + std::to_string(victim.GetFullSetsIndices().size()));

                            res.tags.emplace_back("VictimSetColor"
                                + std::to_string(static_cast<int>(victim.GetCardSets()[victimSetIndex].GetColor())));
                        }break;
                        case Monopoly::EActionType::SlyDeal:
                        {
                            const auto& victimIndex = currentMove[Monopoly::c_JSON_VictimIndex];
                            const auto& victimSetIndex = currentMove[Monopoly::c_JSON_VictimSetIndex];
                            const auto& victimPropertyIndexInSet
                                = currentMove[Monopoly::c_JSON_VictimPropertyIndexInSet];
                            const auto& victim = m_Game.GetPlayers()[victimIndex];
                            const auto& victimSet = victim.GetCardSets()[victimSetIndex];

                            res.tags.emplace_back("VictimNotFullSetsCount" + std::to_string(victim.GetNotFullSetsCount()));
                            res.tags.emplace_back("VictimSetColor" + std::to_string(static_cast<int>(victimSet.GetColor())));
                            res.tags.emplace_back( victimSet.GetCards()[victimPropertyIndexInSet]->GetShortData());
                        }break;
                        case Monopoly::EActionType::ForcedDeal:
                        {
                            const auto& playerSetIndex = currentMove[Monopoly::c_JSON_PlayerSetIndex];
                            const auto& playerPropertyIndexInSet = currentMove[Monopoly::c_JSON_PlayerPropertyIndexInSet];
                            const auto& victimIndex = currentMove[Monopoly::c_JSON_VictimIndex];
                            const auto& victimSetIndex = currentMove[Monopoly::c_JSON_VictimSetIndex];
                            const auto& victimPropertyIndexInSet = currentMove[Monopoly::c_JSON_VictimPropertyIndexInSet];
                            
                            //player
                            const auto& victim = m_Game.GetPlayers()[victimIndex];
                            const auto& victimSet = victim.GetCardSets()[victimSetIndex];

                            res.tags.emplace_back("VictimNotFullSetsCount" + std::to_string(victim.GetNotFullSetsCount()));
                            res.tags.emplace_back("VictimSetColor" + std::to_string(static_cast<int>(victimSet.GetColor())));
                            res.tags.emplace_back(victimSet.GetCards()[victimPropertyIndexInSet]->GetShortData());
                        }break;
                        case Monopoly::EActionType::DebtCollector:
                        {
                            //const auto& victimIndex = currentMove[Monopoly::c_JSON_VictimIndex];
                            // TODO victimMoney, victimPropertyValue ???
                        }break;
                        case Monopoly::EActionType::RentWild:
                        {
                            const auto& playerSetIndex = currentMove[Monopoly::c_JSON_PlayerSetIndex];
                            //const auto& victimIndex = currentMove[Monopoly::c_JSON_VictimIndex];// TODO use
                            const auto& playerSet = player.GetCardSets()[playerSetIndex];
                            const auto setValue = playerSet.GetPayValue();
                            res.tags.emplace_back("SetValue" + std::to_string(setValue));
                            res.tags.emplace_back("SetColor" + std::to_string(static_cast<int>(playerSet.GetColor())));
                            if (currentMove.contains(Monopoly::c_JSON_DoubleTheRentUseCount))
                            {
                                const int dtrCount = currentMove[Monopoly::c_JSON_DoubleTheRentUseCount];
                                res.tags.emplace_back("dtrCount" + std::to_string(dtrCount));
                            }
                            else
                            {
                                res.tags.emplace_back("dtrCount" + std::to_string(0));
                            }
                        }break;
                        case Monopoly::EActionType::RentLightBlueBrown:
                        case Monopoly::EActionType::RentOrangePink:
                        case Monopoly::EActionType::RentYellowRed:
                        case Monopoly::EActionType::RentUtilityRailroad:
                        case Monopoly::EActionType::RentBlueGreen:
                        {
                            const auto& playerSetIndex = currentMove[Monopoly::c_JSON_PlayerSetIndex];
                            const auto& playerSet = player.GetCardSets()[playerSetIndex];
                            const auto setValue = playerSet.GetPayValue();
                            res.tags.emplace_back("SetValue" + std::to_string(setValue));
                            res.tags.emplace_back("SetColor" + std::to_string(static_cast<int>(playerSet.GetColor())));
                            if (currentMove.contains(Monopoly::c_JSON_DoubleTheRentUseCount))
                            {
                                const int dtrCount = currentMove[Monopoly::c_JSON_DoubleTheRentUseCount];
                                res.tags.emplace_back("dtrCount" + std::to_string(dtrCount));
                            }
                            else
                            {
                                res.tags.emplace_back("dtrCount" + std::to_string(0));
                            }
                        }break;
                    }
                }
            }break;
        }
        return res;
    }

    void NeuroController::SelectTurn() const
    {
        const auto validMoves = GetAllValidPlayerTurns(m_Index);
        std::vector<Move> moves;
        
        moves.reserve(validMoves.size());
        for (int i = 0; i < validMoves.size(); ++i)
        {
            const auto& player = m_Game.GetPlayers()[m_Index];
            const auto& currentMove = validMoves[i];
            const ETurn command = validMoves[i][c_JSON_Command];
            Move move;
            switch (command)
            {
                case ETurn::Pass:
                {
                    move = Move({ "pass" + std::to_string(player.GetCardsInHand().size()) });
                }break;
                case ETurn::FlipCard:
                {
                    const auto& setIndex = currentMove[Monopoly::c_JSON_PlayerSetIndex];
                    const auto& cardIndex = currentMove[Monopoly::c_JSON_CardIndex];
                    const auto& card = player.GetCardSets()[setIndex].GetCards()[cardIndex];
                    const auto& currentColor = card->GetCurrentColor();
                    const auto& unusedColor = card->GetColor2();
                    move = Move({ "flip" + card->GetShortData() +
                        "currentColor" + std::to_string(static_cast<int>(currentColor)) +
                        "unusedColor" + std::to_string(static_cast<int>(unusedColor))});
                    move.weight -= 1000000000000000000000000.0;
                }break;
                case ETurn::PlayCard:
                {
                    move = PlayCardMove(currentMove);
                }break;
                case ETurn::HouseHotelOnTable:
                {
                    const auto& emptySetIndex
                        = currentMove[Monopoly::c_JSON_EmptySetIndex];
                    const auto& fullSetIndex
                        = currentMove[Monopoly::c_JSON_PlayerSetIndex];
                    const bool isHotel = player.GetCardSets()[emptySetIndex].IsHasHotel();
                    const auto& fullColor = player.GetCardSets()[fullSetIndex].GetColor();
                    move = Move({ "hhot",
                        "emptySetIndex" + std::to_string(static_cast<int>(isHotel)),
                        "fullSetIndex" + std::to_string(static_cast<int>(fullColor)) });
                }break;
            }

            for (const auto& tag : move.tags)
            {
                m_InvolvedNeurons[tag];
                move.weight += s_Neurons[tag];
            }
            //move.weight /= move.tags.size();
            moves.emplace_back(move);
        }

        double e = 0;
        {
            double maxWeight = std::numeric_limits<double>().min();
            for (int i = 0; i < validMoves.size(); ++i)
            {
                if (moves[i].weight > maxWeight)
                {
                    maxWeight = moves[i].weight;
                }
            }
            if (static_cast<int>(maxWeight) != 0)
                e = (rand() % static_cast<int>(maxWeight));
        }

        int index = 1;
        if (validMoves.size() == 1)
            index = 0;
        else
        {
            double maxWeight = std::numeric_limits<double>().min() + e + 1.0;
            for (int i = 1; i < validMoves.size(); ++i)
            {
                if (moves[i].weight > (maxWeight - e))
                {
                    maxWeight = moves[i].weight;
                    index = i;
                }
            }
        }
        for (const auto& tag : moves[index].tags)
            m_InvolvedNeurons[tag];
        m_Move = validMoves[index];
    }

    void NeuroController::InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container)
    {
        std::vector<std::pair<int, double>> cards;
        const auto& cardsInHand = m_Game.GetPlayers()[m_Index].GetCardsInHand();
        for (int i = 0; i < cardsInHand.size(); ++i)
        {
            cards.emplace_back(std::make_pair(i, s_Neurons[cardsInHand[i]->GetShortData()]));
            m_InvolvedNeurons[cardsInHand[i]->GetShortData()];
        }
        std::sort(cards.begin(), cards.end(), [&](const std::pair<int, double>& d1, const std::pair<int, double>& d2) -> bool
            {
                return d1.second < d2.second;
            });

        for (int i = 0; i < extraCardsCount; ++i)
        {
            container.emplace_back(cards[i].first);
        }
    }

    void NeuroController::InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices,
        std::unordered_map<int, std::vector<int>>& setIndices)
    {
        std::vector<std::pair<std::pair<int, int>, double>> cardsToPay; // setIndex(-1 is bank) cardIndexInSet weight
        const auto& player = m_Game.GetPlayers()[m_Index];
        const auto& cardsInBank = player.GetCardsInBank();
        for (int i = 0; i < cardsInBank.size(); ++i)
        {
            const auto weight = s_Neurons[cardsInBank[i]->GetShortData()];
            m_InvolvedNeurons[cardsInBank[i]->GetShortData()];
            cardsToPay.emplace_back(std::make_pair(std::make_pair(-1, i), weight));
        }

        const auto& sets = player.GetCardSets();
        for (int setIndex = 0; setIndex < sets.size(); ++setIndex)
        {
            const auto& set = player.GetCardSets()[setIndex];
            for (int cardIndex = 0; cardIndex < set.GetCards().size(); ++cardIndex)
            {
                const auto weight = s_Neurons[set.GetCards()[cardIndex]->GetShortData()];
                m_InvolvedNeurons[set.GetCards()[cardIndex]->GetShortData()];
                cardsToPay.emplace_back(std::make_pair(std::make_pair(setIndex, cardIndex), weight));
            }
            if (set.IsHasHouse())
            {
                const auto weight = s_Neurons[set.GetHouse()->GetShortData()];
                m_InvolvedNeurons[set.GetHouse()->GetShortData()];
                cardsToPay.emplace_back(std::make_pair(std::make_pair(setIndex, set.GetCards().size()), weight));
            }
            else if (set.IsHasHotel())
            {
                const auto weight = s_Neurons[set.GetHotel()->GetShortData()];
                m_InvolvedNeurons[set.GetHotel()->GetShortData()];
                cardsToPay.emplace_back(std::make_pair(std::make_pair(setIndex, set.GetCards().size() + 1), weight));
            }
        }

        std::sort(cardsToPay.begin(), cardsToPay.end(), 
            [&](const std::pair<std::pair<int, int>, double>& d1, const std::pair<std::pair<int, int>, double>& d2) -> bool
            {
                return d1.second < d2.second;
            });

        int currentAmount = 0;
        for (int i = 0; currentAmount < amount; ++i)
        {
            if (cardsToPay[i].first.first == -1)// is money
            {
                moneyIndices.emplace_back(cardsToPay[i].first.second);
                currentAmount += player.GetCardsInBank()[cardsToPay[i].first.second]->GetValue();
            }
            else
            {
                setIndices[cardsToPay[i].first.first].emplace_back(cardsToPay[i].first.second);
                const auto& set = player.GetCardSets()[cardsToPay[i].first.first];
                if (cardsToPay[i].first.second == set.GetCards().size())
                {
                    currentAmount += set.GetHouse()->GetValue();
                }
                else if (cardsToPay[i].first.second == (set.GetCards().size() + 1))
                {
                    currentAmount += set.GetHotel()->GetValue();
                }
                else
                {
                    currentAmount += set.GetCards()[cardsToPay[i].first.second]->GetValue();
                }
            }
        }
    }

    bool NeuroController::InputUseJustSayNo(const int victimIndex) const
    {
        if (s_Neurons["UseJustSayNo"] > s_Neurons["NotUseJustSayNo"])
        {
            m_InvolvedNeurons["UseJustSayNo"];
            return true;
        }
        else
        {
            m_InvolvedNeurons["NotUseJustSayNo"];
            return false;
        }
    }

}