#pragma once
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "Card.h"

namespace Monopoly
{
    using CardContainerElem = std::shared_ptr<class ICard>;
    //using CardContainer = std::list<CardContainerElem>;
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
    };

    class CardSet
    {
    public:
        CardSet(EColor color);
        const CardContainer& GetCards() const { return m_Cards; }
        void AddCard(const CardContainerElem& card);
        json ToJSON() const;
        EColor GetColor() const;
        bool IsFull() const;
        bool AddHouse(const CardContainerElem& cardHouse);
        bool AddHotel(const CardContainerElem& cardHotel);
        bool IsHasHouse() const;
        bool IsHasHotel() const;
    private:
        CardContainer m_Cards;
        CardContainerElem m_House = nullptr;
        CardContainerElem m_Hotel = nullptr;
        EColor m_Color;
        static const std::unordered_map<EColor, std::vector<int>> c_RentValues;
    };

}
