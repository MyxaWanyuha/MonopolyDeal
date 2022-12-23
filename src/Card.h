#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Monopoly
{
    const int g_BirthdayGiftAmount = 2;
    const int g_DebtAmount = 5;
    
    template<typename T>
    T FindByValue(const std::unordered_map<T, const char*>& map, const std::string_view& value)
    {
        for (auto& e : map)
        {
            if (e.second == value)
            {
                return e.first;
            }
        }
        assert("FindByValue invalid value!\n");
    }

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

    static const std::unordered_map<EActionType, const char*> ActionTypeStrings =
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

    enum class EColor
    {
        None = 0,
        Yellow,
        Utility,
        Brown,
        Blue,
        Green,
        LightBlue,
        Orange,
        Pink,
        Railroad,
        Red,
        All
    };

    static const std::unordered_map<EColor, const char*> ColorStrings =
    {
        {EColor::None, "None"},
        {EColor::Yellow, "Yellow"},
        {EColor::Utility, "Utility"},
        {EColor::Brown, "Brown"},
        {EColor::Blue, "Blue"},
        {EColor::Green, "Green"},
        {EColor::LightBlue, "LightBlue"},
        {EColor::Orange, "Orange"},
        {EColor::Pink, "Pink"},
        {EColor::Railroad, "Railroad"},
        {EColor::Red, "Red"},
        {EColor::All, "All"}
    };

    enum class ECardType
    {
        None = 0,
        Money,
        Property,
        Action
    };

    static const std::unordered_map<ECardType, const char*> CardTypeStrings =
    {
        {ECardType::None, "None"},
        {ECardType::Money, "Money"},
        {ECardType::Property, "Property"},
        {ECardType::Action, "Action"}
    };

    class Card
    {
    public:
        Card(const std::string& name, ECardType type, int32_t value, EColor color, EColor secondColor, EActionType action)
            : m_Name(name), m_Type(type), m_Value(value), m_MainColor(color), m_SecondColor(secondColor), m_Action(action)
        {
        }
        
        json ToJSON() const;

        ECardType GetType() const { return m_Type; }
        EColor GetColor() const { return m_MainColor; }

    private:
        std::string m_Name;
        ECardType m_Type = ECardType::None;
        int32_t m_Value = -1;
        EColor m_MainColor = EColor::None;
        EColor m_SecondColor = EColor::None;
        EActionType m_Action = EActionType::None;
        bool m_bIsFlipped = false;
    };

}
