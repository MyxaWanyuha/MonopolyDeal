#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <unordered_map>
#include <deque>
#include <random>
#include <charconv>
#include <algorithm>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "JsonConstants.h"

namespace Monopoly
{
	static const uint32_t c_MinPlayersCount = 2;
	static const uint32_t c_MaxPlayersCount = 5;
	static const int c_FullSetsCountForWin = 3;
	static const int c_StartCardsCount = 5;
	static const int c_TurnCardsCount = 3;
	static const int c_PassGoCardsCount = 2;
	static const int c_MaxCardsCountInTurnEnd = 7;
	static const int c_ItsMyBirthdayAmount = 2;
	static const int c_DebtCollectorAmount = 5;
}
