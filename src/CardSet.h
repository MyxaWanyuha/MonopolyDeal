#pragma once
#include <list>
#include "Card.h"


namespace Monopoly
{
    using CardContainer = std::list<Card>;

    class CardSet
    {
    public:
        CardSet(EColor color)
            : m_MaxCardsCount(ColorToPropertyCount(color))
        {
        }

        const CardContainer& GetCards() const { return m_Cards; }
        void AddCard(const Card& card);
        json ToJSON() const;

        bool IsFull() const { return false; /*TODO*/ }
        bool IsCanHasHouse() const { return IsFull(); }
        bool IsCanHasHotel() const { return IsFull() && IsHasHouse(); }
        bool IsHasHouse() const { return m_bIsHasHouse; /*TODO*/ }
        bool IsHasHotel() const{ return m_bIsHasHotel; /*TODO*/ }
    private:
        int ColorToPropertyCount(const EColor color);

        CardContainer m_Cards;
        const int m_MaxCardsCount;
        bool m_bIsHasHouse = false;
        bool m_bIsHasHotel = false;
    };

}
