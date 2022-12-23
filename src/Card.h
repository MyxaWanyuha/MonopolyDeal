#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Monopoly
{
    const int g_BirthdayGiftAmount = 2;
    const int g_DebtAmount = 5;

    enum class EActionType
    {
        PassGo = 0,
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
        RentBlueGreen,
        None
    };

    enum class EColor : uint32_t
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

    enum class ECardType
    {
        None = 0,
        Money,
        Property,
        Action,
        Enhancement
    };

    class Card
    {
    public:
        Card(std::string&& name, ECardType type, int32_t value, EColor color, EColor secondColor, EActionType action)
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
