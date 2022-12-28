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

    const static std::unordered_map<std::string_view, EColor> c_ColorStrToEnum =
    {
        {"Yellow", EColor::Yellow},
        {"Utility", EColor::Utility},
        {"Brown", EColor::Brown},
        {"Blue", EColor::Blue},
        {"Green", EColor::Green},
        {"LightBlue", EColor::LightBlue},
        {"Orange", EColor::Orange},
        {"Pink", EColor::Pink},
        {"Railroad", EColor::Railroad},
        {"Red", EColor::Red},
        {"Wild", EColor::All},
        {"None", EColor::None}
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
        virtual EActionType GetActionType() const { return EActionType::None; }
        virtual uint32_t GetValue() const = 0;
        virtual EColor GetColor1() const { return EColor::None; }
        virtual EColor GetColor2() const { return EColor::None; }
        virtual EColor SwapColor() { return EColor::None; }
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
        virtual uint32_t GetValue() const override { return m_Value; }
    private:
        uint32_t m_Value = 0;
    };

    class PropertyCard : public MoneyCard
    {
    public:
        PropertyCard(const std::string& name, uint32_t value, const EColor color1, const EColor color2 = EColor::None)
            : MoneyCard(name, value), m_CurrentColor(color1), m_UnusedColor(color2)
        {
        }
        virtual ECardType GetType() const override { return ECardType::Property; }
        virtual EColor GetColor1() const { return m_CurrentColor; }
        virtual EColor GetColor2() const { return m_UnusedColor; }
        virtual EColor SwapColor() override
        {
            if (m_UnusedColor != EColor::None)
            {
                std::swap(m_CurrentColor, m_UnusedColor);
            }
            return m_CurrentColor;
        }
    private:
        EColor m_CurrentColor, m_UnusedColor;
    };
    
    class ActionCard : public MoneyCard
    {
    public:
        ActionCard(const std::string& name, uint32_t value)
            : MoneyCard(name, value)
        {
        }

        virtual ECardType GetType() const override { return ECardType::Action; }
    private:
        uint32_t m_Value = 0;
    };

#define ActionCardClass(Type_) \
    class Type_##Card : public ActionCard \
    { \
    public: \
        Type_##Card(const std::string& name, uint32_t value) \
            : ActionCard(name, value) {} \
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
#undef ActionCard(Type_)

#define RentTwoColorsCardClass(Type_, Color1_, Color2_) \
    class Type_ : public ActionCard \
    { \
    public: \
        Type_(const std::string& name, uint32_t value) \
            : ActionCard(name, value) { } \
        virtual EActionType GetActionType() const override { return EActionType::Type_; } \
        virtual EColor GetColor1() const override { return Color1_; } \
        virtual EColor GetColor2() const override { return Color2_; } \
    }; \

    RentTwoColorsCardClass(RentLightBlueBrown, EColor::LightBlue, EColor::Brown);
    RentTwoColorsCardClass(RentOrangePink, EColor::Orange, EColor::Pink);
    RentTwoColorsCardClass(RentYellowRed, EColor::Yellow, EColor::Red);
    RentTwoColorsCardClass(RentUtilityRailroad, EColor::Utility, EColor::Railroad);
    RentTwoColorsCardClass(RentBlueGreen, EColor::Blue, EColor::Green);
#undef RentTwoColorsCardClass(Type_)

}
