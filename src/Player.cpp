#include "Player.h"

namespace Monopoly
{

    int Player::CountBankMoney() const
    {
        // TODO
        return 0;
    }

    void Player::AddCardsToHand(CardContainer&& cards)
    {
        m_Hand.splice(m_Hand.end(), cards);
        // TestOnly for generate json
        /*int b = 0;
        for (auto it = m_Hand.begin(); it != m_Hand.end();)
        {
            auto& e = *it;
            if ((e.GetType() == ECardType::Money || e.GetType() == ECardType::Action) && ++b != 3)
            {
                m_Bank.emplace_back(e);
                m_Hand.erase(it++);
            }
            else if (e.GetType() == ECardType::Property)
            {
                CardSet set(e.GetColor());
                set.AddCard(e);
                m_Hand.erase(it++);
                m_CardSets.push_back(set);
            }
            else
                ++it;
        }*/
        // TestOnly
    }

    int CountBankMoney()
    {
        // TODO
        return 0;
    }
}