#include "Player.h"
#include "Card.h"

namespace Monopoly
{

    int Player::GetIndexJustSayNo() const
    {
        return m_Hand.FindByActionType(EActionType::JustSayNo);
    }

    bool Player::AddHouseToCardSet(int setIndex, const CardContainerElem& card)
    {
        return m_CardSets[setIndex].AddHouse(card);
    }

    bool Player::AddHotelToCardSet(int setIndex, const CardContainerElem& card)
    {
        return m_CardSets[setIndex].AddHotel(card);
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

    void Player::AddCardsToBank(CardContainer&& cards)
    {
        m_Bank.splice(m_Bank.end(), cards);
    }

    void Player::AddSet(CardSet&& set)
    {
        m_CardSets.emplace_back(set);
    }

    CardSet Player::RemoveSet(const int setIndex)
    {
        auto res = std::move(m_CardSets[setIndex]);
        m_CardSets.erase(m_CardSets.begin() + setIndex);
        return res;
    }

    CardContainerElem Player::RemoveCardFromSet(const int setIndex, const int propertyIndexInSet)
    {
        return m_CardSets[setIndex].RemoveCard(propertyIndexInSet);
    }

    CardContainer Player::RemoveCardsFromSet(const int index, const CardIndexesContainer& cardIndexes)
    {
        // TODO
        return CardContainer();
    }

    void Player::RemoveHousesHotelsFromIncompleteSets()
    {
        // TODO
        // ≈сли платитьс€ проперти этого сета, то дом или отель кладутс€ р€дом, пока не соберетс€ новый полный сет, также их можно украсть с помощью SlyDeal ForcedDeal
    }

    CardContainer Player::RemoveCardsFromHand(const CardIndexesContainer& cardIndexes)
    {
        // TODO
        return CardContainer();
    }

    CardContainerElem Player::RemoveCardFromHand(const int cardIndex)
    {
        return m_Hand.GetAndErase(cardIndex);
    }

    void Player::RemoveAllFromBank()
    {
        m_Bank.clear();
    }

    void Player::AddCardToBank(const CardContainerElem& card)
    {
        m_Bank.emplace_back(card);
    }

    CardContainer Player::RemoveCardsFromBank(const CardIndexesContainer& cardIndexes)
    {
        // TODO
        return CardContainer();
    }


    void Player::AddProperty(const CardContainerElem& card)
    {
        //TODO
        // add to set or create new set
    }

    void Player::RemoveProperties()
    {
        m_CardSets.clear();
    }

    int Player::CountBankAndPropertiesValues() const
    {
        int res = 0;
        const auto& bank = GetCardsInBank();
        for (const auto& card : bank)
        {
            res += card->GetValue();
        }

        const auto& sets = GetCardSets();
        for (const auto& set : sets)
        {
            const auto& cards = set.GetCards();
            for (const auto& card : cards)
            {
                res += card->GetValue();
            }
        }
        return res;
    }

}
