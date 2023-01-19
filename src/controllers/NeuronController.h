#pragma once
#include "AIController.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace Monopoly
{

class NeuroController : public AIController
{
    struct Move
    {
        Move() {}
        Move(std::initializer_list<std::string>&& tags, double weight = 0.0)
            : tags(tags), weight(weight)
        {
        }

        std::vector<std::string> tags;
        double weight = 0.0;
    };

public:
    NeuroController(const int index, const Game& g)
        : AIController(index, g)
    {
    }
    virtual ~NeuroController() = default;
    Move PlayCardMove(const json& currentMove) const;
    virtual void SelectTurn() const override;
    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override;
    virtual void InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices,
        std::unordered_map<int, std::vector<int>>& setIndices) override;
    virtual bool InputUseJustSayNo(const int victimIndex) const override;

    static std::unordered_map<std::string, double> s_Neurons;
    mutable std::unordered_map<std::string, double> m_InvolvedNeurons;
};

}
