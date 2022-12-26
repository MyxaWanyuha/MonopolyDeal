#include "CardSet.h"
#include "Card.h"

namespace Monopoly
{
    void CardSet::AddCard(const CardContainerElem& card)
    {
        // TODO
        // check is full
        m_Cards.push_back(card);
    }

    json CardSet::ToJSON() const
    {
        json result;
        //for (auto& card : m_Cards)
        //{
        //    result += card.ToJSON();
        //}
        //TODO
        return result;
    }

}
