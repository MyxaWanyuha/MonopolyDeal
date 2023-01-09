#pragma once

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

    const static std::unordered_map<EActionType, std::string_view> c_ActionTypeEnumToStr =
    {
        {EActionType::None, "None"},
        {EActionType::PassGo, "PassGo"},
        {EActionType::DoubleTheRent, "DoubleTheRent"},
        {EActionType::JustSayNo, "JustSayNo"},
        {EActionType::Hotel, "Hotel"},
        {EActionType::House, "House"},
        {EActionType::DealBreaker, "DealBreaker"},
        {EActionType::SlyDeal, "SlyDeal"},
        {EActionType::ForcedDeal, "ForcedDeal"},
        {EActionType::ItsMyBirthday, "ItsMyBirthday"},
        {EActionType::DebtCollector, "DebtCollector"},
        {EActionType::RentWild, "RentWild"},
        {EActionType::RentLightBlueBrown, "RentLightBlueBrown"},
        {EActionType::RentOrangePink, "RentOrangePink"},
        {EActionType::RentYellowRed, "RentYellowRed"},
        {EActionType::RentUtilityRailroad, "RentUtilityRailroad"},
        {EActionType::RentBlueGreen, "RentBlueGreen"}
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
        ICard(const std::string& name, const std::string& data)
            : m_Name(name), m_Data(data)
        {
        }
        const std::string& GetName() const { return m_Name; }
        const std::string& GetShortData() const { return m_Data; }
        virtual ECardType GetType() const = 0;
        virtual EActionType GetActionType() const { return EActionType::None; }
        virtual uint32_t GetValue() const = 0;
        virtual EColor GetCurrentColor() const { return EColor::None; }
        virtual EColor GetColor2() const { return EColor::None; }
        virtual EColor SwapColor() { return EColor::None; }
    private:
        std::string m_Name;
        std::string m_Data;
    };

    class MoneyCard : public ICard
    {
    public:
        MoneyCard(const std::string& name, const std::string& data, uint32_t value)
            : ICard(name, data), m_Value(value)
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
        PropertyCard(const std::string& name, const std::string& data, uint32_t value, const EColor color1, const EColor color2 = EColor::None)
            : MoneyCard(name, data, value), m_CurrentColor(color1), m_UnusedColor(color2)
        {
        }
        virtual ECardType GetType() const override { return ECardType::Property; }
        virtual EColor GetCurrentColor() const { return m_CurrentColor; }
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
        ActionCard(const std::string& name, const std::string& data, uint32_t value)
            : MoneyCard(name, data, value)
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
        Type_##Card(const std::string& name, const std::string& data, uint32_t value) \
            : ActionCard(name, data, value) {} \
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
#undef ActionCard

#define RentTwoColorsCardClass(Type_, Color1_, Color2_) \
    class Type_ : public ActionCard \
    { \
    public: \
        Type_(const std::string& name, const std::string& data, uint32_t value) \
            : ActionCard(name, data, value) { } \
        virtual EActionType GetActionType() const override { return EActionType::Type_; } \
        virtual EColor GetCurrentColor() const override { return Color1_; } \
        virtual EColor GetColor2() const override { return Color2_; } \
    }; \

    RentTwoColorsCardClass(RentLightBlueBrown, EColor::LightBlue, EColor::Brown);
    RentTwoColorsCardClass(RentOrangePink, EColor::Orange, EColor::Pink);
    RentTwoColorsCardClass(RentYellowRed, EColor::Yellow, EColor::Red);
    RentTwoColorsCardClass(RentUtilityRailroad, EColor::Utility, EColor::Railroad);
    RentTwoColorsCardClass(RentBlueGreen, EColor::Blue, EColor::Green);
#undef RentTwoColorsCardClass

}
