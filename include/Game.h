#pragma once
#include "Player.h"

namespace Monopoly
{

class Game
{
    using Players = std::vector<Player>;
    using Deck = std::deque<CardContainerElem>;
    static const uint32_t c_MinPlayersCount = 2;
    static const uint32_t c_MaxPlayersCount = 5;
    static const int c_StartCardsCount = 5;
    static const int c_TurnCardsCount = 3;
    static const int c_PassGoCardsCount = 2;
    static const int c_MaxCardsCountInTurnEnd = 7;
    static const int c_ItsMyBirthdayAmount = 2;
    static const int c_DebtCollectorAmount = 5;
public:
    static const int c_FullSetsCountForWin = 3;
    int Run();

protected:
    Game();
    bool Init(uint32_t playersCount);
    size_t GetPlayersCount() const { return m_Players.size(); }
    size_t GetCurrentPlayerIndex() const { return m_CurrentPlayerIndex; }
    const Players& GetPlayers() const { return m_Players; }
    enum class ETurn : int
    {
        Pass = 1,
        FlipCard = 2,
        PlayCard = 3,
        HouseHotelOnTable = 4
    };
    enum class ETurnOutput
    {
        IncorrectInput,
        IncorrectIndex,
        IncorrectCard,
        CardProcessed,
        NextPlayer,
        GameOver
    };
    enum class EActionInput
    {
        ToBank = 0,
        Play = 1
    };

    virtual void ShowPublicPlayerData(const int index) const = 0;
    virtual void ShowPrivatePlayerData(const int index) const = 0;
    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) = 0;
    virtual void InputTurn(ETurn& turn, int& cardIndex, int& setIndex) const = 0;
    virtual EActionInput GetActionInput() const = 0;
    virtual int SelectSetIndex(const std::vector<int>& indices) const = 0;
    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const = 0;
    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) = 0;
    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet) = 0;
    virtual void InputDebtCollector(int& victimIndex) = 0;
    virtual void InputPay(const int notUsed, const int amount, std::vector<int>& moneyIndices, std::unordered_map<int, std::vector<int>>& setIndices) = 0;
    virtual void InputRentWild(int& victimIndex, int& setIndex) = 0;
    virtual void InputRentTwoColors(int& setIndex) = 0;
    virtual void InputMoveHouseHotelFromTableToFullSet(
        const std::vector<int>& emptyHouseSetsIndexes,
        const std::vector<int>& emptyHotelSetsIndexes,
        const std::vector<int>& fullSetsWithoutHouseIndexes,
        const std::vector<int>& fullSetsWithoutHotelsIndexes,
        int& emptyIndex, int& setIndex) = 0;
    virtual bool InputUseJustSayNo(const int victimIndex) const = 0;
    virtual void InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const = 0;
private:
    void BeginTurn();
    ETurnOutput Turn(const ETurn input, const int cardIndex = -1, const int setIndex = -1);
    ETurnOutput MoveHouseOrHotelFromTableToSet(Player& currentPlayer);
    int GetExtraCardsCount() const;
    void RemoveExtraCards(const std::vector<int>& extraCardsIndices);
    void EndTurn();
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
    ETurnOutput RentWild(Player& player, const CardContainerElem& card);
    ETurnOutput RentTwoColors(Player& player, const CardContainerElem& card);
    bool JustSayNo(const int player1Index, const int player2Index);
    int DoubleTheRent(Player& player, int& payValue);
    void JustSayNoDoubleTheRent(const int howManyCardsToUse, const int victimIndex, int& payValue);
    void Pay(int victimIndex, int amount);

    template<typename T>
    void InitDeck(const int count, const std::string& name, const int value)
    {
        for (int i = 0; i < count; ++i)
            m_Deck.push_front(std::make_shared<T>(name, value));
    }

    Players m_Players;
    Deck m_Deck;
    uint32_t m_CurrentPlayerIndex = 0;
    int m_CurrentPlayerTurnCounter = 0;
    bool m_bGameIsNotEnded = true;
};

}
