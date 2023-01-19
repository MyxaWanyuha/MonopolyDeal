#pragma once
#include "IControllerIO.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Monopoly
{

class AIController : public IControllerIO
{
public:
    AIController(const int index, const Game& g)
        : IControllerIO(index, g)
    {
    }

    virtual ~AIController() = default;

    virtual void SelectTurn() const;
    void ShowPlayerData() const;

    virtual void ShowPublicPlayerData(const int index) const override;
    virtual void ShowPrivatePlayerData(const int index) const override;
    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override;
    virtual void InputTurn(ETurn& turn, int& cardIndex, int& setIndexForFlip) const override;
    virtual EActionInput GetActionInput() const override;
    virtual int SelectSetIndex(const std::vector<int>& indices) const override;
    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const override;
    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) override;
    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet,
        int& playerSetIndex, int& playerPropertyIndexInSet) override;
    virtual void InputDebtCollector(int& victimIndex) override;
    virtual void InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices,
        std::unordered_map<int, std::vector<int>>& setIndices) override;
    virtual void InputRentWild(int& victimIndex, int& setIndex) override;
    virtual void InputRentTwoColors(int& setIndex) override;
    virtual void InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes,
        const std::vector<int>& emptyHotelSetsIndexes, const std::vector<int>& fullSetsWithoutHouseIndexes,
        const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex) override;
    virtual bool InputUseJustSayNo(const int victimIndex) const override;
    virtual void InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const override;

protected:
    json GetAllValidPlayerTurns(int index) const;
    void ValidMoveEnhancementToFullSet(std::vector<int>& emptyIndexes, std::vector<int>& fullSetsIndexes, json& turns) const;
    json ExtraActionInformation(const Monopoly::CardContainerElem& card, const Monopoly::Player& player) const;
    void RentTwoColorAction(const Monopoly::Player& player, Monopoly::EColor color1, Monopoly::EColor color2, json& res) const;
    json GetDoubleTheRentMayUse(const Monopoly::Player& player) const;

    mutable json m_Move;
};

}