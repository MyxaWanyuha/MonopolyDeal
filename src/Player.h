#pragma once
#include <list>
#include <memory>
#include <vector>
#include "CardSet.h"

namespace Monopoly
{

    class Player
    {
    public:
        using CardSets = std::vector<CardSet>;

        const CardContainer& GetCardsInHand() const { return m_Hand; }
        const CardContainer& GetCardsInBank() const { return m_Bank; }
        const CardSets& GetCardSets() const { return m_CardSets; }

        int CountBankMoney() const;
        void AddCardsToHand(CardContainer&& cards);

    private:
        CardContainer m_Bank;
        CardContainer m_Hand;
        CardSets m_CardSets;
    };

}