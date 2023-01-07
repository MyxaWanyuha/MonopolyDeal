#include "Monopoly_pch.h"
#include "JSONGame.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "JsonConstants.h"

std::string_view GetStrByColor(const EColor& color)
{
    for (auto it = c_ColorStrToEnum.begin(); it != c_ColorStrToEnum.end(); ++it)
    {
        if (it->second == color)
        {
            return it->first;
        }
    }
    assert("Wrong color!");
    return "";
}

json CardToJson(const CardContainerElem& card)
{
    json result;
    result[c_JSON_Card_Name] = card->GetName();
    result[c_JSON_Card_Value] = card->GetValue();
    if (card->GetType() == ECardType::Money)
    {
        result[c_JSON_Card_Type] = c_JSON_Card_Type_Money;
    }
    else if (card->GetType() == ECardType::Action)
    {
        result[c_JSON_Card_Type] = c_JSON_Card_Type_Action;
        result[c_JSON_Card_Action] = c_ActionTypeEnumToStr.at(card->GetActionType());
    }
    else if (card->GetType() == ECardType::Property)
    {
        result[c_JSON_Card_Type] = c_JSON_Card_Type_Property;
        result[c_JSON_Card_Color] = GetStrByColor(card->GetCurrentColor());
        result[c_JSON_Card_SecondColor] = GetStrByColor(card->GetColor2());
    }
    else
    {
        assert("ECardType is None!");
    }
    return result;
}

json SetToJson(const Monopoly::CardSet& set)
{
    json result;
    for (const auto& card : set.GetCards())
    {
        result += CardToJson(card);
    }
    return result;
}

void JSONGame::ShowPublicPlayerData(const int index) const
{
    // Show all game data
    json result;
    result[c_JSON_CurrentPlayerIndex] = Game::GetCurrentPlayerIndex();

    for (int i = 0; i < Game::GetPlayers().size(); ++i)
    {
        const auto& player = Game::GetPlayers()[i];
        json playerData;
        auto addPlayerCards = [&](const CardContainer& cards, const char* jsonKey)
        {
            json cardsData;
            for (const auto& card : cards)
                cardsData += CardToJson(card);
            playerData[jsonKey] = cardsData;
        };

        addPlayerCards(player.GetCardsInHand(), c_JSON_Hand);
        addPlayerCards(player.GetCardsInBank(), c_JSON_Bank);
        {
            auto& sets = player.GetCardSets();
            json setsData;
            for (int j = 0; j < sets.size(); ++j)
                setsData += SetToJson(sets[j]);
            playerData[c_JSON_Sets] = setsData;
        }

        result[c_JSON_Players] += playerData;
    }
    std::cout << result;
}

void JSONGame::ShowPrivatePlayerData(const int index) const
{
    // Not needed
}

void JSONGame::InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container)
{
    json input;
    std::cin >> input;

}

void JSONGame::InputTurn(ETurn& turn, int& cardIndex, int& setIndex) const
{
}

Game::EActionInput JSONGame::GetActionInput() const
{
	return EActionInput();
}

int JSONGame::SelectSetIndex(const std::vector<int>& indices) const
{
	return 0;
}

void JSONGame::InputDealBreaker(int& victimIndex, int& setIndex) const
{
}

void JSONGame::InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet)
{
}

void JSONGame::InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet)
{
}

void JSONGame::InputDebtCollector(int& victimIndex)
{
}

void JSONGame::InputPay(const int notUsed, const int amount, std::vector<int>& moneyIndices, std::unordered_map<int, std::vector<int>>& setIndices)
{
}

void JSONGame::InputRentWild(int& victimIndex, int& setIndex)
{
}

void JSONGame::InputRentTwoColors(int& setIndex)
{
}

void JSONGame::InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes, const std::vector<int>& emptyHotelSetsIndexes, const std::vector<int>& fullSetsWithoutHouseIndexes, const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex)
{
}

bool JSONGame::InputUseJustSayNo(const int victimIndex) const
{
	return false;
}

void JSONGame::InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const
{
}
