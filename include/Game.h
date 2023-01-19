#pragma once
#include "Player.h"
#include "IControllerIO.h"

namespace Monopoly
{

class Game
{
    using ETurn = IControllerIO::ETurn;
    using EActionInput = IControllerIO::EActionInput;
    using Players = std::vector<Player>;
    using Deck = std::deque<CardContainerElem>;
    using Discard = std::vector<CardContainerElem>;
    static constexpr uint32_t c_MinPlayersCount = 2;
    static constexpr uint32_t c_MaxPlayersCount = 5;
    static constexpr int c_StartCardsCount = 5;
    static constexpr int c_TurnCardsCount = 3;
    static constexpr int c_PassGoCardsCount = 2;
    static constexpr int c_MaxCardsCountInTurnEnd = 7;
    static constexpr int c_ItsMyBirthdayAmount = 2;
    static constexpr int c_DebtCollectorAmount = 5;
public:
    using Controllers = std::vector<std::shared_ptr<IControllerIO>>;
    Game();
    Game(const std::string& fileName);
    bool InitNewGame(const uint32_t playersCount, const uint32_t seed);
    bool InitControllers(const Controllers& controllers);
    bool GetGameIsNotEnded() const { return m_WinnerIndex == -1; }
    int GetWinnderIndex() const { return m_WinnerIndex; }
    static constexpr int c_FullSetsCountForWin = 3;
    int Run();

    size_t GetDeckCardsCount() const { return m_Deck.size(); }
    const Discard& GetDrawCards() const { return m_Discard; }
    size_t GetPlayersCount() const { return m_Players.size(); }
    size_t GetCurrentPlayerIndex() const { return m_CurrentPlayerIndex; }
    const Players& GetPlayers() const { return m_Players; }

    void Save(const std::string& fileName) const;
    void Load(const std::string& fileName);

    enum class ETurnOutput
    {
        IncorrectInput,
        IncorrectIndex,
        IncorrectCard,
        CardProcessed,
        NextPlayer,
        GameOver
    };
    void GameBody();
    int GetDoubleTheRentCountMayUse(const Player& player) const;
    void FindEnhancementsAndFullSetsWithout(const Monopoly::Player& currentPlayer,
        std::vector<int>& emptyHouseSetsIndexes,
        std::vector<int>& emptyHotelSetsIndexes,
        std::vector<int>& fullSetsWithoutHouseIndexes,
        std::vector<int>& fullSetsWithoutHotelsIndexes) const;
    bool IsDraw() const;
    int CardsInGameCount() const;
private:
    void BeginTurn();
    ETurnOutput Turn(const ETurn input, const int cardIndex = -1, const int setIndex = -1);
    ETurnOutput MoveHouseOrHotelFromTableToSet(Player& currentPlayer);
    int GetExtraCardsCount() const;
    void RemoveExtraCards(const std::vector<int>& extraCardsIndices);
    void EndTurn();
    void PlayerTakeCardsFromDeck(Player& player, const int count);

    // Action cards process
    ETurnOutput ProcessActionCard(Player& currentPlayer, const CardContainerElem& card);
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
    void InitDeck(const int count, const std::string& name, const std::string& shortData, const int value)
    {
        for (int i = 0; i < count; ++i)
            m_Deck.push_front(std::make_shared<T>(name, shortData, value));
    }

    Controllers m_Controllers;
    Players m_Players;
    Deck m_Deck;
    Discard m_Discard;
    uint32_t m_CurrentPlayerIndex = 0;
    int m_CurrentPlayerTurnCounter = 0;
    int m_CurrentPlayerTurnCounterForAllMoves = 0;
    int m_WinnerIndex = -1;
};

}
