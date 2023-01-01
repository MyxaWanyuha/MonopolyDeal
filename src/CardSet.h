#pragma once
#include "Card.h"

namespace Monopoly
{
    using CardContainerElem = std::shared_ptr<class ICard>;
    class CardContainer : public std::list<CardContainerElem>
    {
    public:
        CardContainerElem GetAndErase(int index)
        {
            auto p = begin();
            std::advance(p, index);
            auto result = *p;
            erase(p);
            return result;
        }

        int FindByActionType(const EActionType action) const
        {
            int res = 0;
            for (auto it = begin(); it != end(); ++it)
            {
                if ((*it)->GetActionType() == action)
                    return res;
                ++res;
            }
            return -1;
        }

        CardContainer RemoveCards(const std::vector<int>& cardIndices)
        {
            CardContainer cards;
            for (const auto& i : cardIndices)
            {
                auto it = begin();
                std::advance(it, i);
                cards.emplace_back(*it);
                *it = nullptr;
            }
            remove(nullptr);
            return cards;
        }
    };

    class CardSet
    {
    public:
        CardSet(const CardContainerElem& card);
        const CardContainer& GetCards() const { return m_Cards; }
        void AddProperty(const CardContainerElem& card);
        bool IsEmpty() const { return !IsHasHouse() && !IsHasHotel() && m_Cards.size() == 0; }
        // GetCards().size() is index of House, GetCards().size() + 1 is index of Hotel 
        CardContainerElem RemoveCard(int index);
        CardContainer RemoveCardsWithValueNotZero(const std::vector<int>& cardIndices);

        EColor GetColor() const;
        bool IsFull() const;
        bool AddHouse(const CardContainerElem& cardHouse);
        bool AddHotel(const CardContainerElem& cardHotel);
        bool IsHasHouse() const;
        bool IsHasHotel() const;
        int GetPayValue() const;

    private:
        CardContainer m_Cards;
        CardContainerElem m_House = nullptr;
        CardContainerElem m_Hotel = nullptr;
        EColor m_Color;
        static const std::unordered_map<EColor, std::vector<int>> c_RentValues;
    };

}
