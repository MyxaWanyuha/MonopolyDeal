#include <list>
#include <memory>
#include "Card.h"
#include <vector>
#include "CardSet.h"

namespace Monopoly
{

    class Player
    {
    public:
        using CardSets = std::vector<CardSet>;
        Player(CardContainer&& playerCards);
        const CardContainer& GetCardsInHand() const { return m_CardsInHand; }
        int CountBankMoney() const;
        void AddCardsToHand(CardContainer&& cards);

    private:
        CardContainer m_Bank;
        CardContainer m_CardsInHand;
        CardSets m_CardSets;
    };

}