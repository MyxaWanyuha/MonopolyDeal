#pragma once
#include <vector>
#include <deque>
#include "Player.h"

namespace Monopoly
{

class Game
{
    const int g_MinPlayersCount = 2;
    const int g_MaxPlayersCount = 5;
    const int g_StartCardsCount = 5;
    const int g_TurnCardsCount = 3;
    const int g_PassGoCardsCount = 2;
    using Players = std::vector<Player>;
    using Deck = std::deque<Card>;
public:
    Game();
    bool Init(int playersCount);
    
    bool CheckJustSayNo(int victimIndex);
    bool DealBreaker(int instigatorIndex, int victimIndex);// steal set from another player

    void BeginTurn();
    void PlayerTakeCardsFromDeck(Player& player, const int count);
private:
    Players m_Players;
    Deck m_Deck;
    int m_CurrentPlayerIndex = 0;
    int m_CurrentPlayerTurnCounter = g_TurnCardsCount;
};

}
