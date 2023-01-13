#pragma once
namespace Monopoly
{

class IGameInput
{
public:

    enum class EActionInput
    {
        ToBank = 0,
        Play = 1
    };
    enum class ETurn : int
    {
        Pass = 1,
        FlipCard = 2,
        PlayCard = 3,
        HouseHotelOnTable = 4
    };

    virtual void ShowPublicPlayerData(const int index) const = 0;
    virtual void ShowPrivatePlayerData(const int index) const = 0;
    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) = 0;
    virtual void InputTurn(ETurn& turn, int& cardIndex, int& setIndexForFlip) const = 0;
    virtual EActionInput GetActionInput() const = 0;
    virtual int SelectSetIndex(const std::vector<int>& indices) const = 0;
    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const = 0;
    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) = 0;
    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet) = 0;
    virtual void InputDebtCollector(int& victimIndex) = 0;
    virtual void InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices, std::unordered_map<int, std::vector<int>>& setIndices) = 0;
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
};

class IController : public IGameInput
{
protected:
    IController(const int index, const class Game& g)
        : m_Index(index), m_Game(g)
    {
    }

    int m_Index;
    const Game& m_Game;
};

}