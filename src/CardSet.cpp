#include "CardSet.h"
#include "Card.h"

namespace Monopoly
{
    const std::unordered_map<EColor, std::vector<int>> CardSet::c_RentValues = {
        { EColor::Yellow,       { 1, 3, 5 } },
        { EColor::Utility,      { 1, 2 } },
        { EColor::Brown,        { 1, 2 } },
        { EColor::Blue,         { 3, 8 } },
        { EColor::Green,        { 2, 4, 7 } },
        { EColor::LightBlue,    { 1, 2, 3 } },
        { EColor::Orange,       { 1, 3, 5 } },
        { EColor::Pink,         { 1, 2, 4 } },
        { EColor::Railroad,     { 1, 2, 3, 4 } },
        { EColor::Red,          { 2, 3, 6 } }
    };

    CardSet::CardSet(const CardContainerElem& card)
        : m_Color(card->GetCurrentColor())
    {
        if (card->GetType() == ECardType::Property)
        {
            AddProperty(card);
        }
        const bool isEnhancement = AddHouse(card) || AddHotel(card);
        assert(isEnhancement);
    }

    void CardSet::AddProperty(const CardContainerElem& card)
    {
        if (card->GetType() == ECardType::Property)
        {
            assert(m_Cards.size() < c_RentValues.at(m_Color).size());
            m_Cards.push_back(card);
        }
    }

    CardContainerElem CardSet::RemoveCard(int index)
    {
        if (index == m_Cards.size())
        {
            auto res = std::move(m_House);
            m_House = nullptr;
            return res;
        }
        if (index == (m_Cards.size() + 1))
        {
            auto res = std::move(m_Hotel);
            m_Hotel = nullptr;
            return res;
        }
        return m_Cards.GetAndErase(index);
    }

    CardContainer CardSet::RemoveCardsWithValueNotZero(const std::vector<int>& cardIndices)
    {
        CardContainer cards;
        for (const auto& i : cardIndices)
        {
            auto it = m_Cards.begin();
            std::advance(it, i);
            if ((*it)->GetValue() != 0)
            {
                cards.emplace_back(*it);
                *it = nullptr;
            }
        }
        m_Cards.remove(nullptr);
        return cards;
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

    EColor CardSet::GetColor() const
    {
        return m_Color;
    }

    bool CardSet::IsFull() const
    {
        return c_RentValues.at(m_Color).size() == m_Cards.size();
    }

    bool CardSet::AddHouse(const CardContainerElem& cardHouse)
    {
        if (IsHasHouse() == false && cardHouse->GetActionType() == EActionType::House)
        {
            m_House = cardHouse;
            return true;
        }
        return false;
    }

    bool CardSet::AddHotel(const CardContainerElem& cardHotel)
    {
        if (IsHasHouse() && IsHasHotel() == false && cardHotel->GetActionType() == EActionType::Hotel)
        {
            m_Hotel = cardHotel;
            return true;
        }
        return false;
    }

    bool CardSet::IsHasHouse() const
    {
        return m_House != nullptr;
    }

    bool CardSet::IsHasHotel() const
    {
        return m_Hotel != nullptr;
    }

    int CardSet::GetPayValue() const
    {
        if (m_Cards.size() == 0)
        {
            return 0;
        }

        auto res = c_RentValues.at(m_Color)[m_Cards.size() - 1];
        if (IsHasHotel())
            res += 4;
        else if (IsHasHouse())
            res += 3;
        return res;
    }

}
