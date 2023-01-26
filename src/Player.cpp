#include "Monopoly_pch.h"

namespace Monopoly
{

    std::vector<int> Player::GetFullSetsIndices() const
    {
        std::vector<int> res;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (m_CardSets[i].IsFull())
            {
                res.emplace_back(i);
            }
        }
        return res;
    }

    std::vector<int> Player::GetNotFullSetsIndices() const
    {
        std::vector<int> res;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (!m_CardSets[i].IsFull())
            {
                res.emplace_back(i);
            }
        }
        return res;
    }

    int Player::GetNotFullSetsCount() const
    {
        int res = 0;
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (!m_CardSets[i].IsFull())
            {
                ++res;
            }
        }
        return res;
    }

    bool Player::IsWinner() const
    {
        int fullSetsCount = 0;
        std::vector<EColor> fullSetsColors;
        for (const auto& set : m_CardSets)
        {
            if (set.IsFull() 
                && std::find(fullSetsColors.begin(), fullSetsColors.end(), set.GetColor()) 
                   == fullSetsColors.end())
            {
                fullSetsColors.emplace_back(set.GetColor());
                ++fullSetsCount;
            }
        }
        return fullSetsCount >= Game::c_FullSetsCountForWin;
    }

    int Player::GetIndexJustSayNo() const
    {
        return m_Hand.FindByActionType(EActionType::JustSayNo);
    }

    bool Player::TryAddHouseToCardSet(int setIndex, const CardContainerElem& card)
    {
        return m_CardSets[setIndex].TryAddHouse(card);
    }

    bool Player::TryAddHotelToCardSet(int setIndex, const CardContainerElem& card)
    {
        return m_CardSets[setIndex].TryAddHotel(card);
    }

    void Player::AddCardsToHand(CardContainer&& cards)
    {
        m_Hand.splice(m_Hand.end(), cards);
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
        auto set = std::move(m_CardSets[setIndex]);
        m_CardSets.erase(m_CardSets.begin() + setIndex);
        return set;
    }

    CardContainerElem Player::RemoveCardFromSet(const int setIndex, const int cardIndex)
    {
        auto card = m_CardSets[setIndex].RemoveCard(cardIndex);
        if (m_CardSets[setIndex].IsEmpty())
        {
            m_CardSets.erase(m_CardSets.begin() + setIndex);
        }
        return card;
    }

    CardContainer Player::RemoveCardsWithValueNotZeroFromSet(const int setIndex, const std::vector<int>& cardIndices)
    {
        CardContainer res;
        res.splice(res.end(), m_CardSets[setIndex].RemoveCardsWithValueNotZero(cardIndices));
        return res;
    }

    void Player::RemoveHousesHotelsFromIncompleteSets()
    {
        CardSets hhSets;
        for (auto& set : m_CardSets)
        {
            if (!set.IsFull())
            {
                if (set.IsHasHouse())
                {
                    auto house = set.RemoveCard(set.GetHouseIndex());
                    hhSets.emplace_back(CardSet(house));
                }
            }
            if (!set.IsHasHouse() && set.IsHasHotel())
            {
                auto hotel = set.RemoveCard(set.GetHotelIndex());
                hhSets.emplace_back(CardSet(hotel));
            }
        }
        for (const auto& set : hhSets)
        {
            m_CardSets.emplace_back(set);
        }
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (m_CardSets[i].IsEmpty())
            {
                m_CardSets.erase(m_CardSets.begin() + i);
                i = -1;
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

    void Player::AddCardToHand(const CardContainerElem& card)
    {
        m_Hand.emplace_back(card);
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

    void Player::AddCard(const CardContainerElem& card)
    {
        for (int i = 0; i < m_CardSets.size(); ++i)
        {
            if (TryAddHouseToCardSet(i, card) || TryAddHotelToCardSet(i, card))
            {
                return;
            }

            auto& set = m_CardSets[i];
            if ((card->GetCurrentColor() == EColor::All || set.GetColor() == card->GetCurrentColor()) 
                && !set.IsFull()
                && set.GetColor() != EColor::None)
            {
                set.AddProperty(card);
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
        for (const auto& card : GetCardsInBank())
        {
            res += card->GetValue();
        }

        for (const auto& set : GetCardSets())
        {
            for (const auto& card : set.GetProperties())
            {
                res += card->GetValue();
            }
            if (set.IsHasHouse())
                res += set.GetHouse()->GetValue();
            if (set.IsHasHotel())
                res += set.GetHotel()->GetValue();
        }
        return res;
    }

    int Player::GetValueOfCards(const int amount, const std::vector<int>& moneyIndices, const std::unordered_map<int, std::vector<int>>& setIndices) const
    {
        int res = 0;
        for (const auto& moneyIndex : moneyIndices)
        {
            if (moneyIndex >= 0 && moneyIndex < m_Bank.size())
            {
                res += m_Bank[moneyIndex]->GetValue();
            }
            else
            {
                return Player::InvalidIndex;
            }
        }
        for (const auto& setIndexWithCards : setIndices)
        {
            const auto setIndex = setIndexWithCards.first;
            if (setIndex >= 0 && setIndex < m_CardSets.size())
            {
                for (const auto& cardIndex : setIndexWithCards.second)
                {
                    if (cardIndex >= 0 && cardIndex < m_CardSets[setIndex].GetProperties().size())
                    {
                        res += m_CardSets[setIndex].GetProperties()[cardIndex]->GetValue();
                    }
                }
            }
            else
            {
                return Player::InvalidIndex;
            }
        }

        return res;
    }

}
