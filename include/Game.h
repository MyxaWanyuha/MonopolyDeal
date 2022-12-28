#pragma once
#include <vector>
#include <deque>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "Player.h"

namespace Monopoly
{

class Game
{
    static const uint32_t c_MinPlayersCount = 2;
    static const uint32_t c_MaxPlayersCount = 5;
    static const int c_StartCardsCount = 5;
    static const int c_TurnCardsCount = 3;
    static const int c_PassGoCardsCount = 2;
    static const int c_MaxCardsCountInTurnEnd = 7;
    static const int c_ItsMyBirthdayAmount = 2;
    static const int c_DebtCollectorAmount = 5;
    using Players = std::vector<Player>;
    using Deck = std::deque<CardContainerElem>;
protected:
    Game();
    bool Init(uint32_t playersCount);
    bool IsNotEnded() const { return m_bIsNotEnded; }

    enum ETurn
    {
        Pass = 1,
        FlipCard = 2,
        PlayCard = 3
    };
    enum class ETurnOutput
    {
        IncorrectInput,
        IncorrectIndex,
        IncorrectCard,
        CardProcessed,
        NextPlayer
    };

    void BeginTurn();
    ETurnOutput Turn(const ETurn input, const int cardIndex = -1, const int setIndex = -1);
    int GetExtraCardsCount() const;
    void RemoveExtraCards(const CardIndexesContainer& extraCardsIndexes);
    void EndTurn();

    enum EActionInput
    {
        ToBank = 0,
        Play = 1
    };

    virtual EActionInput GetActionInput() const = 0;
    virtual int SelectSetIndex(const std::vector<int>& indexes) const = 0;
    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const = 0;
    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) = 0;
    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet) = 0;
    virtual void InputDebtCollector(int& victimIndex) = 0;
    virtual void InputPay(int& notUsed, std::vector<int>& moneyIndexes, std::unordered_map<int, std::vector<int>>& setIndexes) = 0;

    virtual bool InputWannaUseJustSayNo(const int victimIndex) const = 0;

    json GetAllData() const;

private:
    void PlayerTakeCardsFromDeck(Player& player, const int count);

    // Action cards process
    ETurnOutput ProcessActionCard(Player& currentPlayer, CardContainerElem& card);
    ETurnOutput PassGo(Player& player);
    ETurnOutput Enhancement(Player& player, const CardContainerElem& card);
    ETurnOutput DealBreaker(Player& player, const CardContainerElem& card);
    ETurnOutput SlyDeal(Player& player, const CardContainerElem& card);
    ETurnOutput ForcedDeal(Player& player, const CardContainerElem& card);
    ETurnOutput ItsMyBirthday(Player& player, const CardContainerElem& card);
    ETurnOutput DebtCollector(Player& player, const CardContainerElem& card);

    bool JustSayNo(const int player1Index, const int player2Index);

    void Pay(int victimIndex, int amount);


    Players m_Players;
    Deck m_Deck;
    uint32_t m_CurrentPlayerIndex = 0;
    int m_CurrentPlayerTurnCounter = c_TurnCardsCount;
    bool m_bIsNotEnded = true;
};

}
