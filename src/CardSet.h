#pragma once
#include <list>
#include "Card.h"


namespace Monopoly
{
    using CardContainer = std::list<Card>;

    class CardSet
    {
    public:
        CardSet(int cardsCount)
            : m_MaxCardsCount(cardsCount)
        {
        }

        const CardContainer& GetCards() const { return m_Cards; }

        bool IsFull() const { return false; /*TODO*/ }
        bool IsCanHasHouse() const { return IsFull(); }
        bool IsCanHasHotel() const { return IsFull() && IsHasHouse(); }
        bool IsHasHouse() const { return m_bIsHasHouse; /*TODO*/ }
        bool IsHasHotel() const{ return m_bIsHasHotel; /*TODO*/ }
    private:
        CardContainer m_Cards;
        const int m_MaxCardsCount;
        bool m_bIsHasHouse = false;
        bool m_bIsHasHotel = false;
    };

}
