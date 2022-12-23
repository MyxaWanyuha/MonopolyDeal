#include "Player.h"

namespace Monopoly
{

    Player::Player(CardContainer&& playerCards)
        : m_CardsInHand(playerCards)
    {
    }

    int Player::CountBankMoney() const
    {
        // TODO
        return 0;
    }

    void Player::AddCardsToHand(CardContainer&& cards)
    {
        m_CardsInHand.splice(m_CardsInHand.end(), cards);
    }

    int CountBankMoney()
    {
        // TODO
        return 0;
    }
}