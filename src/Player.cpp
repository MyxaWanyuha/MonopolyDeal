#include "Player.h"
#include "Card.h"

namespace Monopoly
{
    std::vector<int> Player::GetFullSetsIndices() const
    {
        std::vector<int> res;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (m_CardSets[i].IsFull())
                res.emplace_back(i);
        }
        return res;
    }

    std::vector<int> Player::GetNotFullSetsIndices() const
    {
        std::vector<int> res;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (!m_CardSets[i].IsFull())
                res.emplace_back(i);
        }
        return res;
    }

    int Player::GetNotFullSetsCount() const
    {
        int res = 0;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            res += static_cast<int>(!m_CardSets[i].IsFull());
        }
        return res;
    }

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

    CardContainer Player::RemoveCardsFromSet(const int index, const CardIndicesContainer& cardIndices)
    {
        // TODO
        // Check Wild Property with value == 0
        return CardContainer();
    }

    void Player::RemoveHousesHotelsFromIncompleteSets()
    {
        // TODO
        // ���� ��������� �������� ����� ����, �� ��� ��� ����� �������� �����, ���� �� ��������� ����� ������ ���, ����� �� ����� ������� � ������� SlyDeal ForcedDeal
    }

    CardContainer Player::RemoveCardsFromHand(const CardIndicesContainer& cardIndices)
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

    CardContainer Player::RemoveCardsFromBank(const CardIndicesContainer& cardIndices)
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

    int Player::GetValueOfCards(const int amount, const std::vector<int>& moneyIndices, const std::unordered_map<int, std::vector<int>>& setIndices) const
    {
        int res = 0;
        for (const auto& e : moneyIndices)
        {
            if (e > 0 && e < m_Bank.size())
            {
                auto it = m_Bank.begin();
                std::advance(it, e);
                res += (*it)->GetValue();
            }
            else
                return Player::InvalidIndex;
        }
        for (const auto& v : setIndices)
        {
            if (v.first > 0 && v.first < m_CardSets.size())
            {
                for (const auto& e : v.second)
                {
                    if (e > 0 && e < m_CardSets[v.first].GetCards().size())
                    {
                        auto it = m_CardSets[v.first].GetCards().begin();
                        std::advance(it, e);
                        res += (*it)->GetValue();
                    }
                }
            }
            else
                return Player::InvalidIndex;
        }

        return res;
    }

}
