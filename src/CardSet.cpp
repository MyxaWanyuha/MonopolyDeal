#include "Monopoly_pch.h"
#include "CardSet.h"

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
        else if (card->GetActionType() == EActionType::House)
        {
            m_House = card;
        }
        else if (card->GetActionType() == EActionType::Hotel)
        {
            m_Hotel = card;
        }
    }

    void CardSet::AddProperty(const CardContainerElem& card)
    {
        if (card->GetType() == ECardType::Property)
        {
            if (m_Color == EColor::All)
            {
                m_Color = card->GetCurrentColor();
            }
            else
            {
                assert(m_Properties.size() < c_RentValues.at(m_Color).size());
            }
            m_Properties.push_back(card);
        }
    }

    CardContainerElem CardSet::RemoveCard(int index)
    {
        if (index == GetHouseIndex())
        {
            auto res = std::move(m_House);
            m_House = nullptr;
            return res;
        }
        if (index == GetHotelIndex())
        {
            auto res = std::move(m_Hotel);
            m_Hotel = nullptr;
            return res;
        }
        return m_Properties.GetAndErase(index);
    }

    CardContainer CardSet::RemoveCardsWithValueNotZero(const std::vector<int>& cardIndices)
    {
        CardContainer cards;
        for (const auto& i : cardIndices)
        {
            if (i == GetHouseIndex())
            {
                cards.emplace_back(std::move(m_House));
                m_House = nullptr;
            }
            else if (i == GetHotelIndex())
            {
                cards.emplace_back(std::move(m_Hotel));
                m_Hotel = nullptr;
            }
            else
            {
                auto it = m_Properties.begin();
                std::advance(it, i);
                if ((*it)->GetValue() != 0)
                {
                    cards.emplace_back(*it);
                    *it = nullptr;
                }
            }
        }
        m_Properties.remove(nullptr);
        if (m_Properties.size() == 1)
        {
            m_Color = m_Properties[0]->GetCurrentColor();
        }
        return cards;
    }

    int CardSet::HowManyCardsNeedToFull() const
    {
        if (m_Color == EColor::None || m_Color == EColor::All)
            return -1;
        return m_Properties.size() - c_RentValues.at(m_Color).size();
    }

    EColor CardSet::GetColor() const
    {
        return m_Color;
    }

    bool CardSet::IsFull() const
    {
        return m_Color != EColor::None && m_Color != EColor::All && c_RentValues.at(m_Color).size() == m_Properties.size();
    }

    bool CardSet::TryAddHouse(const CardContainerElem& cardHouse)
    {
        if (cardHouse->GetActionType() == EActionType::House && IsFull() && !IsHasHouse())
        {
            m_House = cardHouse;
            return true;
        }
        return false;
    }

    bool CardSet::TryAddHotel(const CardContainerElem& cardHotel)
    {
        if (cardHotel->GetActionType() == EActionType::Hotel && IsFull() && IsHasHouse() && !IsHasHotel())
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
        if (m_Properties.size() == 0 || m_Color == EColor::All)
        {
            return 0;
        }
        int res = 0;
        if (m_Color != EColor::None)
        {
            res = c_RentValues.at(m_Color)[m_Properties.size() - 1];
        }
        if (IsHasHotel())
            res += GetHotel()->GetValue();
        else if (IsHasHouse())
            res += GetHouse()->GetValue();
        return res;
    }

    const std::vector<int>& CardSet::GetRentValues() const
    {
        if (m_Color == EColor::None || m_Color == EColor::All)
            return {};
        return c_RentValues.at(m_Color);
    }

}
