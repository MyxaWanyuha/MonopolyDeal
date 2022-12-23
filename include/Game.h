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
    const int c_MinPlayersCount = 2;
    const int c_MaxPlayersCount = 5;
    const int c_StartCardsCount = 5;
    const int c_TurnCardsCount = 3;
    const int c_PassGoCardsCount = 2;
    using Players = std::vector<Player>;
    using Deck = std::deque<Card>;
public:
    Game();
    bool Init(int playersCount);
    
    bool CheckJustSayNo(int victimIndex);
    bool DealBreaker(int instigatorIndex, int victimIndex);// steal set from another player

    void BeginTurn();
    void Turn(const int input);

    void PlayerTakeCardsFromDeck(Player& player, const int count);

    json GetAllData() const;
private:
    Players m_Players;
    Deck m_Deck;
    int m_CurrentPlayerIndex = 0;
    int m_CurrentPlayerTurnCounter = c_TurnCardsCount;
};

}
