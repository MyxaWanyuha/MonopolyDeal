#include "Monopoly_pch.h"

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

    bool Player::IsWinner() const
    {
        int fullSetsCount = 0;
        for (const auto& set : m_CardSets)
        {
            fullSetsCount += static_cast<int>(set.IsFull());
        }
        return fullSetsCount >= Game::c_FullSetsCountForWin;
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

    CardContainerElem Player::RemoveCardFromSet(const int setIndex, const int cardIndex)
    {
        auto res = m_CardSets[setIndex].RemoveCard(cardIndex);
        if (m_CardSets[setIndex].IsEmpty())
        {
            m_CardSets.erase(m_CardSets.begin() + setIndex);
        }
        return res;
    }

    CardContainer Player::RemoveCardsWithValueNotZeroFromSet(const int setIndex, const std::vector<int>& cardIndices)
    {
        return m_CardSets[setIndex].RemoveCardsWithValueNotZero(cardIndices);
    }

    void Player::RemoveHousesHotelsFromIncompleteSets()
    {
        for (auto& e : m_CardSets)
        {
            if (!e.IsFull())
            {
                if (e.IsHasHouse())
                {
                    auto house = e.RemoveCard(e.GetCards().size());
                    m_CardSets.emplace_back(CardSet(house));
                }
                if (e.IsHasHotel())
                {
                    auto hotel = e.RemoveCard(e.GetCards().size() + 1);
                    m_CardSets.emplace_back(CardSet(hotel));
                }
            }
        }
    }
    
    CardContainer Player::RemoveCardsFromHand(const std::vector<int>& cardIndices)
    {
        return m_Hand.RemoveCards(cardIndices);
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

    CardContainer Player::RemoveCardsFromBank(const std::vector<int>& cardIndices)
    {
        return m_Bank.RemoveCards(cardIndices);
    }

    void Player::AddProperty(const CardContainerElem& card)
    {
        for (auto& e : m_CardSets)
        {
            if (e.GetColor() == card->GetCurrentColor() && e.IsFull() == false)
            {
                e.AddProperty(card);
                return;
            }
        }
        m_CardSets.emplace_back(CardSet(card));
    }

    void Player::RemoveSets()
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
