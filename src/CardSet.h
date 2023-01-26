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

        const CardContainerElem& operator[](const int index) const
        {
            auto it = begin();
            std::advance(it, index);
            return *it;
        }
    };

    class CardSet
    {
    public:
        CardSet(const CardContainerElem& card);
        CardSet(CardContainer&& cards, CardContainerElem&& house, CardContainerElem&& hotel, const EColor color)
            : m_Properties(cards), m_House(house), m_Hotel(hotel), m_Color(color)
        {
        }
        const CardContainer& GetProperties() const { return m_Properties; }
        void AddProperty(const CardContainerElem& card);
        bool IsEmpty() const { return !IsHasHouse() && !IsHasHotel() && m_Properties.size() == 0; }
        size_t GetHouseIndex() const { return GetProperties().size(); }
        size_t GetHotelIndex() const { return GetProperties().size() + 1; }
        CardContainerElem RemoveCard(int index);
        CardContainer RemoveCardsWithValueNotZero(const std::vector<int>& cardIndices);
        
        int HowManyCardsNeedToFull() const;
        EColor GetColor() const;
        bool IsFull() const;
        bool TryAddHouse(const CardContainerElem& cardHouse);
        bool TryAddHotel(const CardContainerElem& cardHotel);
        bool IsHasHouse() const;
        bool IsHasHotel() const;
        int GetPayValue() const;

        const CardContainerElem& GetHouse() const { return m_House; }
        const CardContainerElem& GetHotel() const { return m_Hotel; }

        const std::vector<int>& GetRentValues() const;
    private:
        CardContainer m_Properties;
        CardContainerElem m_House = nullptr;
        CardContainerElem m_Hotel = nullptr;
        EColor m_Color;
        static const std::unordered_map<EColor, std::vector<int>> c_RentValues;
    };

}
