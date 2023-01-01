#pragma once
#include "Card.h"

namespace Monopoly
{

    class Player
    {
    public:
        using CardSets = std::vector<CardSet>;
        std::vector<int> GetFullSetsIndices() const;
        std::vector<int> GetNotFullSetsIndices() const;
        int GetNotFullSetsCount() const;
        bool IsWinner() const;

        size_t GetCountCardsInHand() const { return m_Hand.size(); }
        const CardContainer& GetCardsInHand() const { return m_Hand; }
        const CardContainer& GetCardsInBank() const { return m_Bank; }
        const CardSets& GetCardSets() const { return m_CardSets; }
        
        int GetIndexJustSayNo() const;
        bool AddHouseToCardSet(int setIndex, const CardContainerElem& card);
        bool AddHotelToCardSet(int setIndex, const CardContainerElem& card);
        
        CardContainerElem RemoveCardFromHand(const int cardIndex);
        void AddCardsToHand(CardContainer&& cards);
        CardContainer RemoveCardsFromHand(const std::vector<int>& cardIndices);

        void AddCardToBank(const CardContainerElem& card);
        void AddCardsToBank(CardContainer&& cards);
        CardContainer RemoveCardsFromBank(const std::vector<int>& cardIndices);

        void AddSet(CardSet&& set);
        CardSet RemoveSet(const int setIndex);
        CardContainerElem RemoveCardFromSet(const int setIndex, const int cardIndex);
        CardContainer RemoveCardsWithValueNotZeroFromSet(const int setIndex, const std::vector<int>& cardIndices);
        void RemoveHousesHotelsFromIncompleteSets();

        void RemoveAllFromBank();
        void AddProperty(const CardContainerElem& card);
        void RemoveSets();

        int CountBankAndPropertiesValues() const;

        int GetValueOfCards(const int amount, const std::vector<int>& moneyIndices, const std::unordered_map<int, std::vector<int>>& setIndices) const;
        const static int InvalidIndex = -1;
        const static int ValueLessThanAmount = -2;
    private:

        CardContainer m_Bank;
        CardContainer m_Hand;
        CardSets m_CardSets;
    };

}