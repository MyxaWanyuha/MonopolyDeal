#pragma once
#include <list>
#include <memory>
#include <vector>
#include "CardSet.h"

namespace Monopoly
{
    using CardIndicesContainer = std::vector<int>;
    class Player
    {
    public:
        using CardSets = std::vector<CardSet>;

        std::vector<int> GetFullSetsIndices() const;
        std::vector<int> GetNotFullSetsIndices() const;
        int GetNotFullSetsCount() const;

        size_t GetCountCardsInHand() const { return m_Hand.size(); }
        const CardContainer& GetCardsInHand() const { return m_Hand; }
        const CardContainer& GetCardsInBank() const { return m_Bank; }
        const CardSets& GetCardSets() const { return m_CardSets; }
        
        int GetIndexJustSayNo() const;
        bool AddHouseToCardSet(int setIndex, const CardContainerElem& card);
        bool AddHotelToCardSet(int setIndex, const CardContainerElem& card);
        
        CardContainerElem RemoveCardFromHand(const int cardIndex);
        void AddCardsToHand(CardContainer&& cards);
        CardContainer RemoveCardsFromHand(const CardIndicesContainer& cardIndices);

        void AddCardToBank(const CardContainerElem& card);
        void AddCardsToBank(CardContainer&& cards);
        CardContainer RemoveCardsFromBank(const CardIndicesContainer& cardIndices);

        void AddSet(CardSet&& set);
        CardSet RemoveSet(const int setIndex);
        CardContainerElem RemoveCardFromSet(const int setIndex, const int propertyIndexInSet);
        CardContainer RemoveCardsFromSet(const int index, const CardIndicesContainer& cardIndices);
        void RemoveHousesHotelsFromIncompleteSets();

        void RemoveAllFromBank();
        void AddProperty(const CardContainerElem& card);
        void RemoveProperties();

        int CountBankAndPropertiesValues() const;
    private:
        CardContainer m_Bank;
        CardContainer m_Hand;
        CardSets m_CardSets;
    };

}