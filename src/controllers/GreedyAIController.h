#pragma once
#include "AIController.h"

namespace Monopoly
{

class GreedyAIController : public AIController
{
public:
    GreedyAIController(const int index, const Game& g)
        : AIController(index, g)
    {
    }

    virtual ~GreedyAIController() = default;

    virtual void SelectMove() const override;

    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override;
    virtual void InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices, std::unordered_map<int, std::vector<int>>& setIndices) override;
    virtual bool InputUseJustSayNo(const int victimIndex) const override;
};

}
