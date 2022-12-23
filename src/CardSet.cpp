#include "CardSet.h"

namespace Monopoly
{
    void CardSet::AddCard(const Card& card)
    {
        // TODO
        // check is full
        m_Cards.push_back(card);
    }

    json CardSet::ToJSON() const
    {
        json result;
        for (auto& card : m_Cards)
        {
            result += card.ToJSON();
        }
        return result;
    }

    int CardSet::ColorToPropertyCount(const EColor color)
    {
        switch (color)
        {
        case EColor::Yellow: return 3;
        case EColor::Utility: return 2;
        case EColor::Brown: return 2;
        case EColor::Blue: return 2;
        case EColor::Green: return 3;
        case EColor::LightBlue: return 3;
        case EColor::Orange: return 3;
        case EColor::Pink: return 3;
        case EColor::Railroad: return  4;
        case EColor::Red: return 3;
        default:
            return 2;
            break;
        }
    }
}