#include "GreedyAIController.h"
#include "Monopoly_pch.h"
#include "JsonConstants.h"

namespace Monopoly
{

    void GreedyAIController::SelectMove() const
    {
        const auto validMoves = GetAllValidPlayerTurns(m_Index);
        int maxValue = std::numeric_limits<int>().min();
        m_Move = validMoves[0];
        for (const auto& move : validMoves)
        {
            const int value = move[m_TurnValueStr];
            if (maxValue < value)
            {
                maxValue = value;
                m_Move = move;
            }
        }
    }

    void GreedyAIController::InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container)
    {
        auto& cards = const_cast<CardContainer&>(m_Game.GetPlayers()[m_Index].GetCardsInHand());
        cards.sort(
            [](const CardContainerElem& card1, const CardContainerElem& card2) {
                return card1->GetValue() < card2->GetValue();
            });

        for (int i = 0; i < extraCardsCount; ++i)
        {
            container.emplace_back(i);
        }
    }

    void GreedyAIController::InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices, 
        std::unordered_map<int, std::vector<int>>& setIndices)
    {
        auto& money = const_cast<CardContainer&>(m_Game.GetPlayers()[m_Index].GetCardsInBank());
        money.sort([](const CardContainerElem& card1, const CardContainerElem& card2)
            {
                return card1->GetValue() < card2->GetValue();
            });
        auto payAmount = 0;
        for (int i = 0; i < money.size(); ++i)
        {
            if (payAmount >= amount)
                return;
            moneyIndices.emplace_back(i);
        }

        auto& sets = const_cast<Player::CardSets&>(m_Game.GetPlayers()[m_Index].GetCardSets());
        std::sort(sets.begin(), sets.end(), [](const CardSet& set1, const CardSet& set2)
            {
                return set1.IsEmpty() || (set1.GetCards().size() < set2.GetCards().size() && !set1.IsFull());
            });

        for (int i = 0; i < sets.size(); ++i)
        {
            if (payAmount >= amount) return;
            if (sets[i].IsHasHotel())
            {
                setIndices[i].emplace_back(sets[i].GetCards().size() + 1);
                payAmount += sets[i].GetHotel()->GetValue();
            }

            if (payAmount >= amount) return;
            if (sets[i].IsHasHouse())
            {
                setIndices[i].emplace_back(sets[i].GetCards().size());
                payAmount += sets[i].GetHouse()->GetValue();
            }

            for (int j = 0; j < sets[i].GetCards().size(); ++j)
            {
                if (payAmount >= amount)
                    return;
                setIndices[i].emplace_back(j);
            }
        }
    }

    bool GreedyAIController::InputUseJustSayNo(const int victimIndex) const
    {
        return true;
    }

}
