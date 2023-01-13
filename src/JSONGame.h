#pragma once
#include "Game.h"

using namespace Monopoly;

class JSONGame : public Game
{
public:
    JSONGame()
    {
#if NDEBUG
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
#else
        const uint32_t seed = 1337322228u;
#endif
        int playersCount = 2;
        Game::Init(playersCount, seed);
    }
};
