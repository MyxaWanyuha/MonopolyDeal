#pragma once
#include <list>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
    };
    class CardSet
    {
    public:
        CardSet(enum class EColor color)
            //: m_MaxCardsCount(ColorToPropertyCount(color))
        {
        }
        const CardContainer& GetCards() const { return m_Cards; }
        void AddCard(const CardContainerElem& card);
        json ToJSON() const;
    private:
        CardContainer m_Cards;
        bool m_bIsHasHouse = false;
        bool m_bIsHasHotel = false;
    };

}
