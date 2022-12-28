#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Monopoly
{
    class Player;
    class Game;
    enum class EActionType
    {
        None = 0,
        PassGo,
        DoubleTheRent,
        JustSayNo,
        Hotel,
        House,
        DealBreaker,
        SlyDeal,
        ForcedDeal,
        ItsMyBirthday,
        DebtCollector,
        RentWild,
        RentLightBlueBrown,
        RentOrangePink,
        RentYellowRed,
        RentUtilityRailroad,
        RentBlueGreen
    };

    enum class EColor
    {
        Yellow = 0,
        Utility,
        Brown,
        Blue,
        Green,
        LightBlue,
        Orange,
        Pink,
        Railroad,
        Red,
        All,
        None
    };

    enum class ECardType
    {
        None = 0,
        Money,
        Property,
        Action
    };

    class ICard
    {
    public:
        ICard(const std::string& name)
            : m_Name(name)
        {
        }
        const std::string& GetName() const { return m_Name; }
        virtual ECardType GetType() const = 0;
        virtual EActionType GetActionType() const = 0;
        virtual uint32_t GetValue() const = 0;
    private:
        std::string m_Name;
    };

    class MoneyCard : public ICard
    {
    public:
        MoneyCard(const std::string& name, uint32_t value)
            : ICard(name), m_Value(value)
        {
        }

        virtual ECardType GetType() const override { return ECardType::Money; }
        virtual EActionType GetActionType() const { return EActionType::None; }
        virtual uint32_t GetValue() const override { return m_Value; }
    private:
        uint32_t m_Value = 0;
    };

    class ActionCard : public MoneyCard
    {
    public:
        ActionCard(const std::string& name, uint32_t value)
            : MoneyCard(name, value)
        {
        }

        virtual ECardType GetType() const override { return ECardType::Action; }
        virtual EActionType GetActionType() const { return EActionType::None; }
    private:
        uint32_t m_Value = 0;
    };

#define ActionCardClass(Type_) \
    class Type_ : public ActionCard \
    { \
    public: \
        virtual EActionType GetActionType() const override { return EActionType::Type_; } \
    }; \

    ActionCardClass(PassGo);
    ActionCardClass(DoubleTheRent);
    ActionCardClass(JustSayNo);
    ActionCardClass(Hotel);
    ActionCardClass(House);
    ActionCardClass(DealBreaker);
    ActionCardClass(SlyDeal);
    ActionCardClass(ForcedDeal);
    ActionCardClass(ItsMyBirthday);
    ActionCardClass(DebtCollector);
    ActionCardClass(RentWild);
    ActionCardClass(RentLightBlueBrown);
    ActionCardClass(RentOrangePink);
    ActionCardClass(RentYellowRed);
    ActionCardClass(RentUtilityRailroad);
    ActionCardClass(RentBlueGreen);
#undef ActionCard(Type_)

}
