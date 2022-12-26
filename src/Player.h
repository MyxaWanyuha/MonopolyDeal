#pragma once
#include <list>
#include <memory>
#include <vector>
#include "CardSet.h"

namespace Monopoly
{
    using CardIndexesContainer = std::vector<int>;
    class Player
    {
    public:
        using CardSets = std::vector<CardSet>;

        size_t GetCountCardsInHand() const { return m_Hand.size(); }
        const CardContainer& GetCardsInHand() const { return m_Hand; }
        const CardContainer& GetCardsInBank() const { return m_Bank; }
        const CardSets& GetCardSets() const { return m_CardSets; }

        void AddCardsToHand(CardContainer&& cards);
        CardContainer RemoveCardsFromHand(const CardIndexesContainer& cardIndexes);
        CardContainerElem RemoveCardFromHand(const int cardIndex);
        void AddCardToBank(const CardContainerElem& card);
        void AddProperty(const CardContainerElem& card);
    private:
        CardContainer m_Bank;
        CardContainer m_Hand;
        CardSets m_CardSets;
    };

}